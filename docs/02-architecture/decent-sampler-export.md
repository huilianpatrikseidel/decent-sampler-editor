# Export to Decent Sampler

`TranspilersLib`. The model becomes a `DsNode` tree (`src/core/DsDom.h`), which serialises
to XML.

## Pipeline

```
ProjectManager
   -> DecentSamplerTranspiler::generate
        -> DsGroupBuilder::buildGroups     <groups> / <group> / <sample> / <effects>
        -> DsEffectBuilder::buildEffects   instrument-level <effects>
        -> DsUiBuilder                     <ui> and control bindings
   -> DsDocument::toXmlString
   -> BundleExporter                       .dspreset, samples, background -> .dsbundle
```

`BundleExporter::exportToDecentSampler` iterates every preset, emitting a `.dspreset` and
a rendered background per preset, then writes the shared sample set once.

## Samples in a bundle

Sample filenames inside the archive come from `BundleExporter::getBundleSampleName`, and
**only** from there. The preset XML is written before the archive entries, so if the
transpiler and the writer derived names independently they could disagree and the preset
would reference a file the bundle does not contain.

Encodable WAVs become `.flac`; everything else keeps its extension. FLAC entries are
stored without deflate, since recompressing them only costs time.

Wavetable files are collected separately and stored unencoded.

## Volume

Emitted with a `dB` suffix, and omitted entirely at 0 dB, via `DsGroupBuilder`'s
`setVolumeDb`. A bare number would be read as a linear multiplier — see
[../03-decent-sampler-reference/verified-format-notes.md](../03-decent-sampler-reference/verified-format-notes.md).

## Effects

Instrument-level effects come from the master effects list. Group-level effects are
emitted as an `<effects>` element nested inside the `<group>`.

`DsEffectBuilder::getEffectPosition` computes the positional index that UI bindings use to
address an effect. It is fragile — see
[../04-planning/technical-debt.md](../04-planning/technical-debt.md).

## Where the model and the format disagree

Synth containers, buses, round-robin modes, convolution reverb and multi-band EQ all need
translating. Each is documented in [workarounds.md](workarounds.md).

## SFZ

`SfzTranspiler` emits opcodes for sample regions and the sound logic, ignoring the UI
layer. Oscillators and wavetables are not exported.

*Last verified: 2026-08-18*
