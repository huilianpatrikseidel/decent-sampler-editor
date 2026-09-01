# Decisions

Settled trade-offs and the reasoning behind them, so they are not re-argued. Each entry
records the alternatives that were actually considered, not just the winner.

---

## Volume is stored in decibels

**Decision.** `SampleGroup::volume`, `BusNode::volume` and `Zone::volume` hold decibels,
`0.0` = unity, composed by addition.

**Alternative.** Store a linear 0..1 multiplier, matching the fader widget as it
originally was.

**Why decibels.** The exporter already composed parent and child gains additively, which
is dB arithmetic, and the model defaults of `0.0` only make sense as 0 dB. Users think in
dB when balancing a mix, and the requirement that "−6 dB on the group plus +6 dB on the
bus yields 0 dB in the XML" is dB arithmetic by definition. Linear also cannot express
boost above unity within a 0..1 range.

**Consequence.** The fader was converted to a dB scale, and every path into DSP passes
through `DecibelUtils::dbToLinear`.

---

## FLAC encoding is in-house, not libFLAC

**Decision.** Ship a self-contained encoder in `src/export/FlacEncoder`.

**Alternatives.** Vendor libFLAC; or drop the feature and document WAV passthrough.

**Why in-house.** The codebase deliberately avoided the dependency once already (the old
comment read "no FLAC transcoding to avoid libsndfile dependency"), and the project's
existing style is single-file vendored libraries. A fixed-predictor encoder with Rice
coding is tractable and produces ordinary FLAC that any decoder reads. Crucially it is
verifiable: the round-trip test decodes with an independent decoder (dr_flac via
miniaudio) and asserts bit-exactness.

**Consequence.** No LPC coefficient search, so a few percent worse ratio than the
reference encoder. Accepted.

---

## Non-encodable audio is stored unchanged

**Decision.** Only integer PCM at 8/16/24 bits is encoded; everything else goes into the
bundle byte-for-byte.

**Why.** A "lossless compression" feature that quietly truncates float WAVs to 24 bits
would be worse than no feature. The naming decision is centralised in
`BundleExporter::getBundleSampleName` so the preset XML and the archive can never
disagree about a file's extension.

---

## Buses are editor-only, flattened on export

**Decision.** Keep buses as an authoring convenience and replicate their effects onto
every contributing group at export, folding the gains.

**Alternatives.** Drop buses entirely, since the format has no submix; or render them in
the editor and accept that the export sounds different.

**Why flatten.** Buses genuinely help authoring, and the same pattern already exists and
works for the synth container. Rendering them without flattening would make the audition
preview a lie — worse than the current problem, which is only that effects are inaudible.

**Consequence.** A shared reverb tail becomes N independent tails after export. Linear
effects are unaffected. Documented in
[`../02-architecture/workarounds.md`](../02-architecture/workarounds.md).

---

## `insertEffects` is the canonical FX chain

**Decision.** The ordered `insertEffects` list on each channel is the source of truth;
the exporter reads it.

**Alternative.** Keep deriving effects from node-graph connections, as the exporter did.

**Why.** Chain order matters and connections cannot express it. The mixer already writes
`insertEffects`, so this also ends the situation where the UI wrote to a list nothing
read while the exporter read from somewhere else entirely.

---

## Linux is the CI gate

**Decision.** Only the Linux leg blocks; Windows and macOS run `continue-on-error`.

**Why.** Both were red for reasons unrelated to any given change, and blocking on them
would have blocked everything. Making them green is tracked in [backlog.md](backlog.md).

**Consequence.** A Windows- or macOS-only regression can land unnoticed. Accepted while
those legs are known-red anyway.

*Last verified: 2026-08-18*
