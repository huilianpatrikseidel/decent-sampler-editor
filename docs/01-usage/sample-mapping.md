# Sample mapping

## Importing

Samples can be loaded into an instrument in two ways:
1. Clicking **Import Samples** in the Sampling tab toolbar.
2. Dragging and dropping audio files directly onto the mapping grid (`MappingEditorView`).

### Automatic Token Parsing (`AutoMapper`)

When audio files have standard naming conventions, the auto-mapper parses metadata tokens from filenames to automatically position zones:
- **Pitch / Note:** Standard note names (e.g., `C4`, `D#3`, `Bb2`) or MIDI note numbers.
- **Velocity Layers:** Ranges such as `v001-v127`, `vel01-vel100`, or single dynamic boundaries like `v127`.
- **Round Robin:** Tokens like `rr1`, `rr02`, `RR3`, or trailing numbers like `_1`, `_2`.
- **Mic Positions:** Layer names such as `Close`, `Room`, `Overhead`.

### Interactive Import Modal (`ZoneImportOverlay`)

When importing through the modal interface, you can configure:
- **Mapping Mode:** Choose between **Tonal** (pitched across the keyboard) and **Percussive** (one note per sample).
- **Velocity Buckets:** Distribute multiple velocity layers across dynamic ranges automatically.
- **Default Root Note:** Base key for percussive or unpitched samples.
- **Target Group:** Assign incoming samples to an existing group or create a new one.

### Automatic Pitch Detection via YIN (`AudioAnalyzer`)

When audio files are dropped onto the grid without recognisable pitch tokens in their filenames:
- The editor runs an asynchronous background pitch analysis using the **YIN Pitch Detection Algorithm** (`AudioAnalyzer::detectPitchAsync`).
- Upon detecting the fundamental frequency, the editor calculates the exact MIDI note and assigns the zone's root note and key placement automatically.

---

## Zones

A **Zone** represents a single sample mapping inside a group with the following attributes:

- **Key Range & Root Note:** Low note (`loNote`), high note (`hiNote`), and recorded root note (`rootNote`).
- **Velocity Range:** Dynamic trigger window (`loVel` to `hiVel`).
- **Sample Bounds:** Start offset (`sampleStart`) and end offset (`sampleEnd`).
- **Loop Points:** `loopStart`, `loopEnd`, `loopCrossfade` (crossfade length), and `loopEnabled` flag.
- **Round-Robin Position (`seqPosition`):** Stacked alternate index for round-robin cycling.
- **Keyswitch Note (`keySwitchNote`):** Dedicated MIDI note triggering this articulation (-1 if unused).
- **Mic Layer (`micLayer`):** Microphone channel name for multi-mic instruments.
- **Offsets:** Volume offset (in **decibels**), tuning offset (semitones/cents), and stereo pan.
- **Local Envelope Override (`useLocalAmpEnv`):** Allows a zone to override its group's amplifier envelope with its own dedicated per-zone ADSR parameters.

---

## Groups & Group Settings

A **SampleGroup** holds a collection of related zones. All group properties can be edited in the **Group Settings** tab of the bottom dock (`GroupEditorView`):

### 1. Group Parameters
- **Volume:** Calibrated in decibels (`0.0 dB` = unity gain).
- **Pan:** Stereo balance (-1.0 to +1.0).
- **Trigger:** Playback trigger event (`attack`, `release`, `first`, `legato`).
- **Playback Mode (`seqMode`):** Sequence playback logic (`normal`, `round_robin`, `random`, `true_random`).
- **Portamento:** Enable legato and adjust glide time in seconds (`glideTime`).
- **Pedal CC64 Range:** Define minimum (`loCC64`) and maximum (`hiCC64`) sustain pedal values.

### 2. Voice Control
- **Custom Tags (`customTags`):** Assign group tags for polyphony and choking control.
- **Silenced By (`silencedByTags`):** Define tags that silence/choke this group when triggered.
- **Silencing Mode (`silencingMode`):** Choke behavior (`fast` or `normal`).

### 3. Filter Section
- **Filter Type:** `LowPass`, `HighPass`, `BandPass`, `Notch`.
- **Controls:** Cutoff frequency (Hz), Resonance, Envelope Depth (`filterEnvDepth`), and Keyboard Tracking (`filterKeyTrack`). Can be enabled/disabled with the power toggle.

### 4. Envelopes
- **Amp Envelope (Amp Env):** Dedicated Attack, Decay, Sustain, and Release controls with real-time visual envelope curve preview (`EnvelopeGraphWidget`).
- **Modulation Envelope (Mod Env / Env2):** Secondary ADSR envelope routable to pitch, filter cutoff, or volume in the modulation matrix.

---

## Synth Groups & Modular Generators (`SynthEditorView`)

A **Synth Container** generates sound programmatically instead of playing sampled audio:

1. Create a synth group via **+ Add Group** and enable the **Synth Container** checkbox in `CreateGroupDialog`.
2. Select the synth group to open the modular **Synth Editor** (`SynthEditorView`).
3. Add sound generators using **+ Add Oscillator**. Each oscillator contains:
   - **Oscillator Card (`OscillatorCardWidget`):** Waveform selection (**Sine**, **Saw**, **Square**, **Triangle**, **Noise**, **Pluck** [Karplus-Strong with damping control], and **Wavetable** [supporting frame interpolation and wavetable position index]).
   - **Filter Card (`FilterCardWidget`):** Independent per-oscillator SVF filter.
   - **Envelope Card (`EnvelopeCardWidget`):** Independent per-oscillator ADSR amplifier envelope.
   - **Legato Card (`LegatoCardWidget`):** Per-oscillator portamento and glide smoothing.
4. **Global LFOs:** Managed directly on the right column of `SynthEditorView` via `GlobalLfoCardWidget` cards and the **+ Add LFO** button.

---

*Last verified: 2026-08-19*
