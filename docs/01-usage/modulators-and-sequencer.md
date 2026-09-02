# Modulators and Sequencer

Modulation and pattern playback systems in Decent Sampler Editor.

---

## Global Modulation Matrix (`ModulatorsView`)

Located in the **Modulators** tab of the bottom dock, the Global Modulation Matrix displays and manages all active modulation routings across all groups in the instrument.

- **Grouped Layout:** Routings are organized clearly by synthesizer / sample group.
- **Modulation Sources:**
  - **LFO 1 / LFO 2:** Global low-frequency oscillators.
  - **AmpEnv:** The group's primary amplifier envelope.
  - **ModEnv (Env2):** The secondary modulation envelope.
  - **Velocity:** Key strike dynamics.
  - **ModWheel (CC 1):** Continuous modulation controller.
  - **PitchBend:** 14-bit pitch wheel.
  - **Aftertouch:** Channel or polyphonic key pressure.
- **Modulation Destinations:**
  - `Volume`, `Pan`, `Tuning` (Pitch), `Filter Cutoff`, `Filter Resonance`.
- **Parameters & Math:**
  - **Amount:** Depth slider from `-1.0` to `+1.0`.
  - **Translation:** Scaling response (`linear`, `table`, `fixed_value`).
  - **Modulation Behavior:** `add` (default), `multiply`, or `set`.

---

## Global LFO Generators (`GlobalLfoCardWidget`)

Global LFOs are configured in the modular synth workspace (`SynthEditorView`) and shared globally across the entire audio graph:

- **Shapes:** **Sine**, **Triangle** (exported as `saw` for Decent Sampler format compatibility), **Square**, and **Saw**.
- **Parameters:**
  - **Rate:** Frequency in Hertz (`0.1 Hz` – `20.0 Hz`).
  - **Depth / Amount:** Modulation intensity (`0.0` – `1.0`).
  - **Scope:** `global` or `voice`.
  - **Delay Time:** Delay before modulation onset.
- **Waveform Graph (`LFOGraphWidget`):** Live animated visual waveform preview.
- **Power Toggle:** Enable or disable individual LFOs.
- **Adding LFOs:** Click **+ Add LFO** to spawn additional modulators.

---

## Audition & Pattern Sequencer (`NoteSequenceEditorView`)

The **Sequencer** tab in the bottom dock provides an interactive step-sequencer for real-time auditioning and instrument pattern authoring:

### Sequence Configuration
- **Sequence Manager:** Create multiple named patterns using **+ New Sequence** and save updates with **Save Changes**.
- **Target Group:** Route pattern playback to a specific sound group or audition across the whole instrument.
- **Playback Controls:** Start/Stop audition playback (`PlaybackSequencer`) synchronized with the audio engine.
- **Tempo & Timing:**
  - **BPM:** Tempo adjustable from `20` to `300 BPM`.
  - **Time Signature:** Numerator (`1` to `16`) and Denominator (`1` to `32`).
  - **Playback Rate:** `1/1` (Whole), `1/2` (Half), `1/4` (Quarter), `1/8` (Eighth), or `1/16` (Sixteenth).

### Interactive Step Grid Gestures (`SequenceGraphWidget`)

The step graph supports direct mouse interactions:
- **Click / Drag:** Adjust step velocity.
- **Shift + Drag:** Transpose step pitch (note value).
- **Ctrl + Drag:** Adjust step gate length.
- **Mouse Wheel:** Scroll over any step to transpose pitch up or down.
- **+ Add Step / - Remove Step:** Dynamically extend or shorten pattern length.

### Decent Sampler Format Export
Unlike basic audition tools, note sequences created here are persisted in project files and exported directly into the `.dspreset` XML definition under `<noteSequences><sequence name="..." length="..." rate="..."><note ... /></sequence></noteSequences>`.

---

*Last verified: 2026-08-19*
