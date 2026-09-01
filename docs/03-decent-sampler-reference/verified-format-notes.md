# Verified format notes

Behaviour we have **confirmed**, as opposed to assumed. Each entry says how it was
established. When you learn something new about the format, add it here — this file is
what stops the next person repeating the investigation.

---

## Volume accepts two different formats

**Confirmed** against the Decent Sampler documentation.

A volume value is read in one of two ways depending on how it is written:

| Written as | Interpreted as |
|---|---|
| `0.5` (bare number) | **Linear scale**, `0.0` = silence, `1.0` = unity. A direct multiplier. |
| `-6dB` (with suffix) | **Decibels**, logarithmic. |

This is the single most consequential fact in the export path. The editor's model stores
decibels with `0.0` meaning unity — so emitting the raw model value as a bare number
turns every unity-gain group into **total silence**. Export writes the `dB` suffix, and
omits the attribute entirely at 0 dB (`DsGroupBuilder::setVolumeDb`).

UI bindings can state the math explicitly with a `translation` attribute
(`translation="linear"`), which controls how a knob scales across its range.

---

## Group-level effects are supported

**Confirmed** by the exporter emitting them and the format accepting the nesting.

An `<effects>` element can be nested inside a `<group>`, so per-group insert chains are
natively representable. Instrument-wide effects live in the top-level `<effects>` block.

There is **no submix or bus concept** at any level — which is why the editor's buses have
to be flattened on export. See
[`../02-architecture/workarounds.md`](../02-architecture/workarounds.md).

---

## `<groups>` container defaults

The `<groups>` element accepts `globalVolume`, `globalTuning` and `globalPan`, applied
across all groups. The editor writes `globalVolume` with a `dB` suffix.

> **To confirm** — whether `globalVolume` composes multiplicatively with each group's own
> volume, or overrides it. The editor currently assumes composition.

---

## Effect types used by the exporter

`delay`, `reverb`, `convolution`, `chorus`, `gain`, `phaser`, `pitch_shift`,
`wave_folder`, `wave_shaper`, `stereo_simulator`, `bit_crusher`, plus filter types
(`lowpass`, `highpass`, `bandpass`, `notch`) which also serve as equalizer bands.

The `gain` effect carries an explicit `levelUnit` attribute (`decibels` or `linear`) —
evidence that the format's dual-unit handling is not limited to group volume.

*Last verified: 2026-08-18*
