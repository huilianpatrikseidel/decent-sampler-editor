# Overview

A Qt 6 / C++17 desktop application, split into shared libraries by concern, with a hard
boundary between the GUI thread and the audio callback thread.

## Layers

```
                    DecentSamplerEditor (executable)
                    MainWindow, DockManager, inspector, ExportWorker
                                     |
   +----------------+----------------+----------------+----------------+
   |                |                |                |                |
 MapperLib      MixerLib      UiDesignerLib     NodeMapLib      UiComponentsLib
 mapping        mixer,        instrument UI     node graph      shared widgets
 editor         FX rack       canvas                            (knobs, faders)
   |                |                |                |                |
   +----------------+--------+-------+----------------+----------------+
                             |
                          CoreLib
        data model, ProjectManager, undo stack, serialisation,
        AudioGraphBuilder, VstPluginManager
                             |
            +----------------+----------------+
            |                                 |
      AudioEngineLib                    TranspilersLib
      engine, voices, DSP, MIDI         Decent Sampler / SFZ export,
            |                           bundle writer, FLAC encoder
      Vst3HostLib
      VST3 plugin hosting

   Vendored: MiniaudioLib (audio I/O + decoding), FFTConvolverLib (convolution)
```

`SamplerEditorLib` is a static library that links the rest together for the executable
and the test binary.

## The thread boundary

Two threads matter, and they never share mutable state directly.

**GUI thread** owns the model. Every edit goes through `ProjectManager` and a
`QUndoCommand`, so `Ctrl+Z` works everywhere. After an edit, `ApplicationController`
rebuilds the render graph (`AudioGraphBuilder::buildRenderGraph`) and publishes it
atomically.

**Audio thread** owns rendering. It consumes `AudioMessage` values from a lock-free
queue, reads continuous parameters from atomics in `GlobalAudioState`, and never
allocates, locks or touches the disk. See [audio-engine.md](audio-engine.md).

**MIDI thread** decodes incoming events in `ApplicationController`'s callback and either
enqueues note messages or stores controller values into `GlobalAudioState` atomics.

## What flows where

- **Editing** — UI → `QUndoCommand` → `ProjectManager` → signals → views refresh.
- **Auditioning** — model → `AudioGraphBuilder` → `RenderZone` list → `AudioMessage` →
  voice.
- **Exporting** — model → `DecentSamplerTranspiler` / `SfzTranspiler` → `DsNode` tree →
  XML → `BundleExporter` → `.dsbundle`.

The export path and the audition path read the **same model** but are otherwise
independent, which is exactly why they can drift apart. Keeping them in agreement is a
recurring theme — see [workarounds.md](workarounds.md).

*Last verified: 2026-08-18*
