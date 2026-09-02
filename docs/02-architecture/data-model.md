# Data model

Everything lives in `CoreLib`. `ProjectManager` owns the node graph, the UI component
tree, the preset snapshots, and the undo stack.

## Nodes

`Node` (`src/core/models/nodes/AudioNodeBase.h`) is the base for everything in the graph:
a UUID, a name, a position, and a `type` string used for dispatch. Note it is a
**`struct`** — forward declarations must say `struct`, or MSVC fails to link. Subtypes:

| Type | Purpose |
|---|---|
| `SampleGroup` | A playable sound group: zones, envelopes, filter settings, voice tags, modulation routings, insert effects, and optional oscillator/synth parameters |
| `BusNode` | A submix channel: volume and insert effects |
| Effect nodes | `Delay`, `Reverb`, `Filter`, `Chorus`, `Gain`, `Phaser`, `PitchShifter`, `WaveFolder`, `WaveShaper`, `StereoSimulator`, `BitCrusher`, `Equalizer` |

A `Zone` is a mapped sample inside a group: path, key and velocity range, root note,
playback bounds (`sampleStart`, `sampleEnd`), loop points (`loopStart`, `loopEnd`, `loopCrossfade`, `loopEnabled`), round-robin position (`seqPosition`), keyswitch note (`keySwitchNote`), mic layer, volume/tuning offsets, and an optional local ADSR override (`useLocalAmpEnv`).

## Project States (`ProjectStates.h`)

Global state models companion to the node graph:

- **`AudioStateModel`:** Manages global volume/pan/tuning, global LFOs (`LFO`), master insert effects, MIDI CC bindings (`MidiBinding`), note sequences (`NoteSequence`), tag polyphony limits (`tagPolyphony`), keyboard color zones (`KeyboardColor`), and global macro definitions (`GlobalMacro`).
- **`UiStateModel`:** Manages instrument canvas dimensions (`width`, `height`), background artwork path (`bgImage`), background rendering mode (`BgMode`), and canvas viewport coordinates (`viewportX`, `viewportY`, `zoom`).

## Connections

`Connection` records a typed edge between node ports. `ProjectManager::canConnect`
validates direction, type compatibility, duplicates, and runs a DFS to reject cycles — so
the graph is always a DAG.

Connections express **routing**. They do not express **ordering**, which is why insert
chains are stored as ordered `insertEffects` lists on the channel instead. See
[../04-planning/decisions.md](../04-planning/decisions.md).

## Channels

A "channel" in the mixer is a `SampleGroup` or a `BusNode`; the master channel is
represented by a null UUID. Each channel carries an `outputBusId` naming the bus it feeds,
null meaning master. `ProjectManager::canRouteToBus` validates an assignment and rejects
anything that would close a loop, because a cycle would stall the topological ordering the
renderer depends on. `ProjectManager` resolves a channel id to its effects list,
falling back to `AudioStateModel::getMasterEffects` for the null case.

## Undo & Commands

Every mutation goes through a `QUndoCommand` subclass in `src/commands`. Commands capture
the node's JSON before and after, so undo restores state rather than replaying inverse
operations. Subclasses exist for nodes (`AddNodeCommand`, `RemoveNodeCommand`, `MoveNodeCommand`, `ModifyPropertyCommand`), zones (`ModifyZonePropertyCommand`, `ZoneCommands`), UI components (`AddUiComponentCommand`, `RemoveUiComponentCommand`, `MoveUiComponentCommand`, `ModifyUiPropertyCommand`), canvas modifications, and graph connections.

## Serialisation & Database

- **Project Snapshots (`ProjectSerializer`):** Writes the full project as structured JSON. Each node and component implements `toJson` and `loadFromJson`. Presets are snapshots of the entire state held by `PresetManager`.
- **Database Storage (`DatabaseManager`):** Manages an SQLite database (`Databases/Libraries/libraries.db`) recording project IDs, names, creation/modification timestamps, thumbnail paths, and serialized project data payloads.

When adding a field, give `loadFromJson` a sensible default for projects saved before the
field existed — `obj["x"].toDouble()` silently yields `0.0` for a missing key, which is
correct for a decibel value and wrong for almost anything else.

## Continuous parameters

Values that change while audio runs do not travel as messages. `GlobalAudioState` holds a
fixed array of `NodeParamBlock` slots of atomics; `AudioGraphBuilder` allocates a slot per
group and stores its index on the render zone, so a voice can read its group's live volume
without touching the model.

---

*Last verified: 2026-08-19*
