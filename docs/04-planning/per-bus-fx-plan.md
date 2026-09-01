# Plan: per-channel FX rendering and bus flattening

Approved plan for backlog item "per-bus / per-channel FX are not rendered live". Stage 1
is **done**; the rest is pending.

## Problem

The backlog described this as "insert effects on a channel are saved and exported, but you
cannot hear them". Investigation showed the diagnosis was half wrong and the underlying
situation worse:

1. **Two disconnected sources of truth.** The mixer's FX rack writes to
   `SampleGroup::insertEffects` / `BusNode::insertEffects` — a list **nothing reads**.
   The exporter derives group effects from **node-graph connections** instead. So mixer
   inserts are neither exported nor rendered.
2. **Group volume was exported as silence.** The model stores decibels, but the group
   emitter wrote a bare number, which Decent Sampler reads as a linear multiplier — so a
   0 dB group exported as `volume="0"`, total silence.
3. **The audition engine multiplied decibels as if linear**, silencing 0 dB groups there
   too.
4. **The fader was linear 0..1**, so a bus at +6 dB was not even representable.
5. **Group-to-bus routing does not exist** in the interface.

## Decisions

- **Buses stay editor-only** and are flattened on export: bus effects replicated into
  every contributing group, gains folded (−6 dB group + 6 dB bus → 0 dB). Same pattern the
  synth container already uses.
- **`insertEffects` becomes canonical** — it is ordered, and chain order matters.
- **The fader speaks decibels.**

Reasoning in [decisions.md](decisions.md); the export-side consequences in
[../02-architecture/workarounds.md](../02-architecture/workarounds.md).

---

## Stage 1 — decibel coherence — **done**

The model was already right (dB, `0.0` = unity, additive composition). Its consumers were
not.

- Added `DecibelUtils::dbToLinear` (`src/core/DecibelUtils.h`).
- `DsGroupBuilder::setVolumeDb` emits group and zone volume with a `dB` suffix, omitting
  the attribute at 0 dB — matching how global volume was already written.
- `AudioGraphBuilder` converts to linear before feeding voices or `GlobalAudioState`.
- `FaderWidget` operates in decibels, with unity at 80% of travel and headroom to +6 dB.
- Fixed a separate bug: the master fader wrote `msg.volume` while the engine reads
  `msg.value`, so moving it silenced everything.

Covered by `testDecibelConversion` and `testGroupVolumeExportedAsDecibels`, both confirmed
to fail without the fix.

## Stage 2 — `insertEffects` as the export source — **done**

- `DsGroupBuilder` reads `sg->insertEffects` in order instead of scanning connections.
- `DsEffectBuilder::buildEffects` takes instrument-level effects from
  `AudioStateModel::getMasterEffects` instead of the "no group connects into it"
  heuristic.
- `DsEffectBuilder::getEffectPosition` reindexes over the new ordering — it feeds UI
  bindings via `DsUiBuilder`, so a wrong index repoints knobs in the exported instrument.

`getEffectPosition` now counts what is actually emitted rather than assuming one element
per node: a bypassed effect emits nothing and has no position, and an equalizer emits one
element per band, so anything after one would otherwise bind to the wrong effect.

Covered by `testGroupInsertChainOrderExported` and
`testEffectPositionSkipsBypassedAndCountsEqBands`, both confirmed to fail without the fix.

## Stage 3 — group-to-bus routing — **done**

- `outputBusId` (null = master) on `SampleGroup` and `BusNode`, serialised.
- Cycle rejection on assignment, mirroring the DFS in `ProjectManager::canConnect`.
- An output selector on the mixer channel strip.

Cycle rejection lives in `ProjectManager::canRouteToBus`, which walks the destination's
own chain rather than the connection graph, and also refuses a chain that is already
cyclic instead of looping forever on it. The mixer only offers destinations that pass the
check, so an illegal route cannot be selected in the first place.

Covered by `testBusRoutingPersists` and `testBusRoutingRejectsCycles`; the cycle test was
confirmed to fail with the guard removed.

## Stage 4 — per-channel audition

- New `MixerTopology` in `CoreLib`: enumerates channels, resolves destinations, orders
  them topologically. **The single source of channel indices** — `AudioGraphBuilder` and
  `VstPluginManager` both derive from it, and if they disagree the effects land on the
  wrong channel.
- `channelIndex` threaded through `RenderZone` → `AudioMessage` → `VoiceProcessor`,
  mirroring how `paramBlockIndex` already travels.
- `AudioEngine` gains a `MixGraph` of per-channel chains, gains and destinations,
  published with the same atomic swap as the master chain, plus pre-allocated per-channel
  buffers sized in `initialize`. `processAudio` sums voices per channel, applies each
  chain in topological order, routes into the destination, then the master chain. The
  real-time rules in [../02-architecture/audio-engine.md](../02-architecture/audio-engine.md)
  still apply.
- Unmapped channels route straight to master rather than disappearing.

*Verify:* a unit test builds group → bus → master with known gains and asserts the
composed amplitude; then a reverb on a bus is audible in the running application.

## Stage 5 — bus flattening on export

For each group, walk the `outputBusId` chain and compose:

- `<effects>` = the group's own inserts, then each ancestor bus's, in order.
- `volume` = the group's decibels plus every ancestor bus's.

Must compose with the synth case, where a child already adds its container's volume.

*Verify:* a group at −6 dB into a bus at +6 dB exports with no `volume` attribute and the
bus effect replicated; the same holds for every group a synth container fans out into.

## Risks

- **Diverging channel indices** between the two producers would put effects on the wrong
  channel. Mitigated by `MixerTopology` being the only source.
- **A bus cycle** would stall the topological sort. Rejected at assignment.
- **`Vst3Host` lifetime** is timing-based today and this multiplies the number of live
  hosts. Tracked in [technical-debt.md](technical-debt.md).

*Last verified: 2026-08-18*
