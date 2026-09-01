# Getting started

## The main window

A custom title bar carries the menus, the window controls, and the tab strip; the native
window frame is replaced on Windows with custom DWM-integrated resizing and snapping.

Five main tabs are freely navigable in any order:

| Tab | Purpose |
|---|---|
| **Libraries** | Project browser and entry point |
| **Sampling** | The main workspace: mapping, editing, mixing |
| **Node Map** | Signal routing as a node graph |
| **UI Designer** | The instrument's own interface |
| **Code Editor** | Real-time generated preset source (`.dspreset` / `.sfz`) |

The custom title bar also includes:
- **Project Dirty Indicator:** An asterisk (`*`) is appended to the window and title bar when unsaved modifications exist. Closing the application with unsaved changes prompts to Save, Discard, or Cancel.
- **Settings Dialog (`SettingsDialog`):** Accessible via the gear icon on the title bar.
- **About Dialog (`AboutDialog`):** Accessible via the info icon, displaying software version, copyright, GPLv2 license, and the local SQLite database path.

---

## Settings & Preferences

The **Settings** dialog (`SettingsDialog`) configures runtime hardware and visual preferences:

- **Audio:** Select the active output device (enumerated via `AudioEngine::enumerateDevices`), sample rate (22050 Hz, 44100 Hz, 48000 Hz, 96000 Hz), and audio buffer size (`Auto`, 64 to 2048 samples).
- **MIDI:** Select the active MIDI input port (enumerated via `MidiEngine::enumeratePorts`) with a live refresh button (`↻`) to detect newly plugged controllers without restarting.
- **Appearance (Themes):** Switch between **Dark** and **Light** themes. Themes are loaded dynamically from theme definitions (`dark.json`, `light.json`, `style_template.qss`) and update `ThemePalette` and Qt stylesheets application-wide.

Settings are persisted automatically across sessions via `AppSettings`.

---

## Libraries

The project browser. Each project is presented as a card showing a preview thumbnail, the project name, and its last-modified date.

- **New Project:** Prompts for a project name, creates its dedicated media-pool directory under `Databases/Libraries/Projects/<ProjectName>`, adds a record in SQLite (`DatabaseManager`), and switches directly to the Sampling workspace.
- **Search:** Real-time filter searching by project name.
- **Sort Selector:** Reorders libraries by *Last Modified*, *Name*, or *Date Created*.
- **Card Context Menu:** Right-click any project card to *Rename Project* or *Delete Project* (which recursively cleans up the project's media folder and database entry).
- **Opening a Project:** Double-click a card to load the project state snapshot into `ProjectManager`.

Cards without a custom preview render a generated gradient thumbnail derived deterministically from a hash of the project name.

> [!NOTE]
> The **Import** button on the Libraries header is currently reserved for future standalone instrument import workflows. Use **Import Samples** inside the Sampling workspace to import audio files into your instrument groups.

---

## The Sampling workspace

The primary authoring environment, organised into four regions:

- **Left (Presets Dock):** The preset list (`PresetListWidget`) for the instrument. Add (`+`), remove (`-`), rename, or switch between snapshot presets. Switching presets restores canvas viewport coordinates and instrument parameters.
- **Centre (Editor):** The sample mapping grid (`MappingEditorView`) for regular sample groups, or the modular synth generator stack (`SynthEditorView`) for synth containers.
- **Right (Tabified Sidebars):**
  - **Properties Panel (`PropertiesInspector`):** An interactive editing form that inspects the current selection (nodes, individual zones, UI components, or canvas background/dimensions) and pushes property changes to the undo stack (`QUndoStack`).
  - **Project Explorer (`ProjectTreeWidget`):** A hierarchical tree view (`Instrument -> Groups -> Zones`) that synchronises selection with the mapping editor and properties inspector, and supports inline group renaming.
  - **UI Assets (`AssetManagerView`):** A visual image asset browser with thumbnail previews. Features a quick jump to the current project's asset directory and **supports drag-and-drop of images directly onto the UI Designer canvas**.
- **Bottom (Dock):** A multi-tab drawer of specialised editors.

The centre editor and the bottom dock are separated by a **draggable vertical splitter**. The dock can be collapsed to just its tab bar by clicking the collapse chevron or by clicking the active tab header.

### The bottom dock

| Tab | Purpose |
|---|---|
| **Group Settings** | Volume (dB), pan, trigger modes, playback/round-robin modes, portamento/glide, pedal CC64 ranges, voice tags/silencing, filter settings, and ADSR envelopes |
| **Sample Editor** | Waveform viewer, sample start/end, loop points, crossfade, per-zone local envelope overrides, and audio loop analyzer |
| **Mixer & FX** | Channel strips, dB faders, mute/solo, insert effect racks with drag-and-drop reordering, and native VST3 effect editors |
| **Modulators** | Global Modulation Matrix table linking sources (LFOs, Envelopes, MIDI CCs) to destinations |
| **Macros** | Multi-parameter global macro knobs and macro routing targets |
| **Sequencer** | Audition step-sequencer with interactive note/velocity/gate editing, saved into the instrument definition |

**Focus Mode**, located at the bottom-right corner of the dock (`FocusModeOverlay`), expands the bottom drawer to fill the entire window for distraction-free editing.

---

## Auditioning

A virtual keyboard plays the current instrument in real time, and any connected MIDI device drives playback through `MidiEngine`. Note on/off, CC 1 (mod wheel), 14-bit pitch bend, and aftertouch are all handled.

The virtual keyboard is embedded directly at the bottom of the Mapping Grid (60px height) and at the bottom of the UI Designer canvas (75px height).

> [!NOTE]
> Insert effects placed on group or bus channels are processed during export, but during live auditioning only the master effect chain is applied. See [../04-planning/per-bus-fx-plan.md](../04-planning/per-bus-fx-plan.md).

---

*Last verified: 2026-08-19*
