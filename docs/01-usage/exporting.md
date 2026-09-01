# Exporting

Instruments are exported from the **File** menu. Export tasks execute asynchronously on a background worker thread (`ExportWorker`), keeping the UI responsive during compression and I/O.

---

## Formats & Menu Actions

| Menu Action | Output | Description |
|---|---|---|
| **Export to DecentSampler (.dslibrary)** | `.dslibrary` | A self-contained ZIP bundle containing `.dspreset` files for all presets, losslessly encoded FLAC samples, UI backgrounds, and filmstrips. |
| **Export to SFZ (.sfz)** | `.sfz` + `<Name>_Samples/` | Standard SFZ instrument file accompanied by a directory of organized sample files. |

> [!TIP]
> To obtain raw, unbundled `.dspreset` XML code referencing samples in place, switch to the **Code Editor** tab and click **Copy to Clipboard**.

---

## What Happens to Your Samples

When exporting a Decent Sampler library (`.dslibrary`):

- **Lossless FLAC Compression:** Integer PCM WAV files at 8, 16, or 24 bits are compressed to **FLAC** using the in-house `FlacEncoder` — reducing library size by roughly 50% with zero quality loss.
- **Byte-for-Byte Preservation:** Float, 32-bit, or already-compressed audio formats that cannot be bit-exactly reproduced by fixed integer predictors are **stored unchanged** byte-for-byte.
- **Wavetable Files:** Kept in their original uncompressed format to guarantee compatibility with Decent Sampler's wavetable engine.
- **Deterministic Collision-Free Naming:** Sample files are renamed using path-based MD5 hashes (`BundleExporter::getSafeExportName`), ensuring samples with identical filenames from different folders never collide.

---

## Multi-Preset Bundling

Every preset in `PresetManager` is serialized as an independent `.dspreset` file inside the bundle, along with its own rendered UI background image (`<PresetName>.png`). All presets within the same library share a single set of sample assets, avoiding duplicate audio files.

---

## What Gets Translated

Several editor features without a 1:1 match in the format are flattened or bridged at export time:

- **Synth Containers:** Expanded into individual `<group>` elements per generator, copying parent volume, pan, tuning, triggers, and modulations.
- **Buses:** Submix buses are flattened into the groups feeding them; bus insert effects are replicated onto contributing groups, and gains are folded additively.
- **Equalizer:** Multi-band EQ nodes are decomposed into individual filter effects (`<effect type="lowpass|hipass|peak|..." />`).
- **Sequence Modes:** `random` and `true_random` modes are mapped to `round_robin`.

See [../02-architecture/workarounds.md](../02-architecture/workarounds.md) for full technical reasoning.

---

## SFZ Limitations

The SFZ exporter focuses on sample mappings, velocity zones, and core filter/envelope sound parameters. It generates `<control>` blocks for UI knobs mapped via the UI Designer's Sforzando mode. Oscillators, wavetables, and insert effect chains are not supported by standard SFZ export.

---

*Last verified: 2026-08-19*
