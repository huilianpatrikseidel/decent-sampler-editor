# Sample Editor

The **Sample Editor** tab in the bottom dock (`SampleEditorContainer`) provides detailed waveform inspection, marker adjustments, loop editing, and per-zone envelope overrides for individual samples.

---

## Dual Waveform Views

The editor hosts two specialized waveform views:

### 1. Zone Waveform (`WaveformView`)
- Visual display of the selected zone's audio waveform loaded asynchronously via `WaveformAudioLoader`.
- Interactive marker boundaries for playback start (`sampleStart`) and end (`sampleEnd`).
- Visual loop region highlights showing active loop boundaries.

### 2. Audio Analyzer & Auto-Loop (`WaveformEditorView`)
- Waveform inspection with automated loop point detection using the **Sum of Absolute Differences (SAD)** algorithm (`AudioAnalyzer::findBestLoopAsync`).
- Draggable loop start and end handles for continuous sample looping with real-time frame position updates.

---

## Zone Parameters & Looping Controls

The parameter panel on the right of the sample editor edits:

- **Looping Controls:**
  - **Enable Loop (`loopEnabled`):** Toggles continuous playback looping.
  - **Loop Start (`loopStart`):** Starting sample offset for the loop segment.
  - **Loop End (`loopEnd`):** Ending sample offset for the loop segment.
  - **Loop Crossfade (`loopCrossfade`):** Length of crossfade blending between loop boundaries to prevent audible clicking.
- **Playback Bounds:**
  - **Sample Start (`sampleStart`):** Offset in sample frames from the beginning of the file.
  - **Sample End (`sampleEnd`):** Playback termination offset (0 means end of file).

---

## Local Envelope Override (`useLocalAmpEnv`)

While zones inherit their amplifier envelope from their parent group by default, the Sample Editor allows per-zone customization:

- **Override Group Env (`useLocalAmpEnv`):** Checkbox enabling an independent local ADSR envelope.
- **Local ADSR Editor (`AdsrEditorView`):** Configure Attack, Decay, Sustain, and Release specifically for the selected zone without modifying other samples in the same group.

---

*Last verified: 2026-08-19*
