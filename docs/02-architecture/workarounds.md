# Workarounds: editor concepts vs. the Decent Sampler format

The editor deliberately offers concepts the `.dspreset` format does not have, because
they make authoring tractable. Each one is only legitimate because **export collapses it
into something the format does support**.

Every entry below states three things: what the editor pretends, what the format
actually offers, and how export bridges them. If you change one side, change the other —
a workaround that stops being bridged turns the audition preview into a lie.

---

## Synth container: one editor identity, many exported groups

**Editor.** A synthesizer is a single object with one name, one volume, one set of
envelopes and modulation routings.

**Format.** There is no such grouping. Each wave generator has to be its own `<group>`.

**Bridge.** The container is marked `isSynthContainer` and is **skipped entirely** on
export (`DsGroupBuilder`'s visitor returns immediately for it). Each child carries
`synthParentId`, and when a child is exported the parent's parameters are composed into
it: `volume`, `pan` and `tuning` are **added**, while `trigger`, `seqMode`,
`silencedByTags`, `silencingMode`, `loCC64`/`hiCC64` and legato settings are **taken
from the parent**, and the parent's modulation routings are appended to the child's.

The consequence to remember: **anything applied to the container must be replicated onto
every child**, because after export the container no longer exists.

---

## Buses: submixing that the format has no concept of

**Editor.** Groups can be routed into a bus, and a bus carries its own insert chain and
fader — the usual mixer idea of treating a section as one thing.

**Format.** There is no submix. There are groups, and there is one instrument-level
`<effects>` block. Both export visitors implement `visit(const BusNode*)` as an explicit
no-op.

**Bridge.** On export the bus is flattened: its insert chain is **replicated into every
group that feeds it**, and its gain is **folded into each group's volume**. A group at
−6 dB feeding a bus at +6 dB exports as a group at 0 dB — which, per the volume rule
below, means the attribute is omitted entirely.

**Accepted inaccuracy.** A bus effect runs *once* over the summed groups during
audition, but becomes *N independent instances* after export. For linear effects (gain,
EQ, filter) the result is equivalent. For reverb and delay it is not: a shared tail is
not what N separate reverbs produce. This was chosen knowingly — the alternative was to
drop buses, and the format offers no third option.

> **To confirm** — whether replicating a heavy effect across many groups is acceptable
> in practice for large instruments, or whether the UI should warn above some count.

---

## FLAC: an in-house encoder instead of a dependency

**Editor.** `.dsbundle` samples are compressed losslessly.

**Format.** Decent Sampler reads FLAC.

**Bridge.** `src/export/FlacEncoder` is a self-contained encoder — fixed predictors,
Rice-coded residuals, stereo decorrelation, CRC-8/16 and the STREAMINFO MD5. It exists
so the project does not take on libFLAC.

Only integer PCM at 8, 16 or 24 bits is encoded. Float and 32-bit WAVs, and anything
else the encoder cannot reproduce bit-exactly, are **stored unchanged** — the lossless
guarantee is never traded for compression. `FlacEncoder::canEncode` makes that decision,
and `BundleExporter::getBundleSampleName` is the single place the resulting filename is
derived, because the preset XML is written before the archive entries and the two must
agree on the extension.

Wavetables are deliberately **not** encoded: they are stored as-is, since FLAC wavetable
support is unconfirmed and a broken wavetable would break the preset.

---

## Round robin: three editor modes, one format mode

**Editor.** `seqMode` can be `normal`, `random` or `true_random`.

**Format.** The useful distinction is `round_robin`.

**Bridge.** Both `random` and `true_random` are emitted as `round_robin`. A group whose
zones use `seqPosition > 1` while still set to `normal` is also promoted to
`round_robin`, since stacked positions are meaningless otherwise.

---

## Convolution reverb: one editor node, two format types

**Editor.** A single reverb node, which may or may not carry an impulse response.

**Format.** `type="reverb"` and `type="convolution"` are different effects.

**Bridge.** A reverb with an IR path set exports as `type="convolution"` with `irFile`
and `mix`; without one it exports as `type="reverb"` with `roomSize`, `damping` and
`wetLevel`. Same node, different emission.

---

## Equalizer: one node, one effect per band

**Editor.** An equalizer with a list of bands.

**Format.** No multi-band EQ element.

**Bridge.** Each band is emitted as its own `<effect>`, typed by the band's filter type.
`gain` is only written for band types where it is meaningful (peak and shelves).

---

## Volume: the same number means two different things

Covered fully in [`../03-decent-sampler-reference/verified-format-notes.md`](../03-decent-sampler-reference/verified-format-notes.md),
and summarised in [`../../CLAUDE.md`](../../CLAUDE.md) because it is the single most
expensive fact in this codebase to get wrong.

The model stores decibels. The format reads a bare number as a **linear multiplier** and
only treats it as decibels when it carries a `dB` suffix. Export therefore has to write
the suffix, and the audition engine has to convert to linear before any multiplication.

*Last verified: 2026-08-18*
