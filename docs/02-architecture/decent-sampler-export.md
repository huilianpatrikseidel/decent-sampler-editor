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

Both instrument-level and group-level chains come from ordered `insertEffects` lists —
the master list on `AudioStateModel`, and each group's own. Connections are not consulted:
they carry no ordering, and chain order changes the sound. Group effects are emitted as an
`<effects>` element nested inside the `<group>`.

`DsEffectBuilder::getEffectPosition` gives UI bindings the positional index of a master
effect, counting what is actually emitted: a bypassed node contributes nothing, and an
equalizer contributes one element per band.

## Buses

Buses do not survive export. Each group folds the gain of every bus it feeds into its own
volume, and appends those buses' insert chains after its own. `busChain` in
`DsGroupBuilder` resolves the chain, bounded so a hand-edited cycle cannot hang the export.

## Where the model and the format disagree

Synth containers, buses, round-robin modes, convolution reverb and multi-band EQ all need
translating. Each is documented in [workarounds.md](workarounds.md).

## SFZ

`SfzTranspiler` emits opcodes for sample regions and the sound logic, ignoring the UI
layer. Oscillators and wavetables are not exported.

*Last verified: 2026-08-18*
