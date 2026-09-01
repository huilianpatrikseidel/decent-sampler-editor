# Audio engine

`AudioEngineLib`, driven by miniaudio. The rules here are not style preferences: a lock
or an allocation inside the callback produces audible dropouts.

## Real-time rules

Inside `AudioEngine::processAudio` and everything it calls:

- **No allocation.** Buffers are sized in `AudioEngine::initialize`.
- **No locks.** Communication is a `moodycamel::ConcurrentQueue` of `AudioMessage`.
- **No file I/O.** Sample sources are opened elsewhere; see below.

## Sample ownership

Opening a file is slow, so it never happens on the audio thread:

1. The note-triggering thread calls `AudioEngine::prepareSampleSource`, which opens a
   streaming miniaudio data source and returns an opaque handle.
2. The handle rides along on the `AudioMessage`.
3. The audio thread calls `VoiceProcessor::adopt`, which installs the new source and
   **returns the previous one** so it can be pushed onto a free-queue.
4. Off-thread, `drainFreeQueue` uninitialises and frees them.

`VoiceProcessor::releaseSource` is the only path that frees on the spot, and it requires
the device to be stopped. `VoiceProcessor` and `Vst3Host` have deleted copy operations
precisely because they own these resources.

## Render path

Per buffer:

1. Drain the command queue.
2. Per frame, compute the shared modulation sources and run every active voice, summing
   each voice into **its own channel's buffer**. A voice whose channel index is -1 — no
   channel, or one dropped past `MAX_CHANNELS` — lands on master instead, so its audio is
   never lost.
3. Walk the channels **front to back**, applying each one's insert chain and gain, then
   summing it into its destination channel. The ordering guarantees a channel is always
   rendered before whatever it feeds, so one pass suffices.
4. Apply master gain, then the master chain, then clip to the output.

The graph is a `MixGraph` — per-channel chains, gains and destination indices — built on
the UI thread by `VstPluginManager` and published into an atomic by `setMixGraphAsync`,
so the callback never walks the project model. A null graph means every voice goes to
master, which is the state before a project loads.

Channel indices come from `MixerTopology`, which both `VstPluginManager` and
`AudioGraphBuilder` call. They must agree on what an index means; deriving it
independently would route voices through the wrong chain.

Buffers for all `MAX_CHANNELS` channels are allocated in `initialize`. The old
buffers are swapped without locking, and the delay before disposing them is timing-based
rather than proven — recorded in
[../04-planning/technical-debt.md](../04-planning/technical-debt.md).

## Modulation

Modulation sources that are global to the engine — the two LFOs plus the live MIDI
controllers — are computed **once per frame** and handed to every voice as a `ModInputs`
struct. Per-voice sources (envelopes, velocity) are computed inside the voice.

MIDI controllers arrive on the MIDI thread and are stored as normalised atomics in
`GlobalAudioState`: `modWheel` (CC 1), `pitchBend` (14-bit, centred) and `aftertouch`
(channel or poly). The engine reads them once per buffer and applies a one-pole smoother
per frame, because raw 7-bit steps would zipper on pitch or gain.

Smoothing centrally rather than per voice is deliberate: a note triggered in the middle
of a mod-wheel sweep starts at the **current** controller position instead of ramping up
to it.

`VoiceProcessor` maps `ModSource` to `ModDest` through the routing array copied in at
trigger time.

## Audio Analysis (`AudioAnalyzer`)

Analysis tasks execute asynchronously on dedicated worker threads (`std::thread`), completely isolated from the real-time audio thread:

- **YIN Pitch Detection (`AudioAnalyzer::detectPitchAsync`):** Analyzes audio files using the YIN autocorrelation algorithm to extract fundamental frequency (\(F_0\)) and deduce the matching MIDI note number for automatic root note mapping.
- **SAD Auto-Loop Detection (`AudioAnalyzer::findBestLoopAsync`):** Employs Sum of Absolute Differences (SAD) across sliding sample windows to detect seamless, phase-coherent loop start and loop end points.

## Playback Sequencer (`PlaybackSequencer`)

`PlaybackSequencer` drives timed audition playback of note sequences independently of the GUI thread, dispatching `PlayNote` and `StopNote` commands into `AudioEngine`'s lock-free queue at the configured BPM, time signature, and playback rate.

## Volume

Everything in the model is decibels. `AudioGraphBuilder` converts with
`DecibelUtils::dbToLinear` before anything reaches a voice or `GlobalAudioState`. Getting
this wrong renders unity-gain channels silent — see [../../CLAUDE.md](../../CLAUDE.md).

## Sample rate

The device's actual rate is propagated to every DSP stage in `AudioEngine::initialize`.
Envelopes, filters, oscillators and LFOs would otherwise run as if the device were
44.1 kHz.

---

*Last verified: 2026-08-19*
