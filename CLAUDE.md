# Decent Sampler Editor — agent and contributor guide

A Qt 6 / C++17 desktop IDE for authoring Decent Sampler instruments: map samples,
design the instrument UI, audition it with a built-in audio engine, and export
`.dspreset` / `.dsbundle` / `.sfz`.

**Read this file before searching the source.** It exists so that facts which cost a
full investigation to rediscover are already in context. If something here is wrong,
fixing it is more valuable than working around it.

---

## Build and test

The toolchain is **MinGW + Ninja + Qt 6**, driven by CMake. Neither Ninja nor Qt is on
`PATH` by default on the dev machine.

```bash
export PATH="/d/Development/Softwares/QT/6.11.1/mingw_64/bin:/d/Development/Softwares/QT/Tools/Ninja:/d/Development/Softwares/QT/Tools/mingw1310_64/bin:$PATH"
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel 8
```

Running the tests needs **the build directory on `PATH` as well** — the test binary
links a dozen shared libraries, and without them every run dies with exit code
`0xc0000135` (Windows "DLL not found"), which looks like a test failure but is not:

```bash
export PATH="/d/Development/projects/SamplerEditor/build:$PATH"
QT_QPA_PLATFORM=offscreen ctest --test-dir build --output-on-failure
```

See [docs/02-architecture/testing.md](docs/02-architecture/testing.md) for the harness
limitations, and [docs/02-architecture/build-and-toolchain.md](docs/02-architecture/build-and-toolchain.md)
for the CI matrix and the compiler traps.

---

## Repo map

Twelve libraries plus the executable. Knowing which one owns a concern is usually
enough to avoid a repo-wide grep.

| Target | Owns |
|---|---|
| `CoreLib` | Data model, `ProjectManager`, undo stack, serialisation, `AudioGraphBuilder`, `VstPluginManager` (`src/core`, `src/commands`) |
| `AudioEngineLib` | Audio engine, voices, DSP, MIDI (`src/audio`) |
| `Vst3HostLib` | VST3 plugin hosting (`src/audio/Vst3Host.cpp`) |
| `TranspilersLib` | Export: Decent Sampler and SFZ transpilers, bundle writer, FLAC encoder (`src/transpilers`, `src/export`) |
| `MapperLib` | Sample mapping editor, waveform views (`src/ui/mapper`) |
| `MixerLib` | Mixer, channel strips, FX rack (`src/ui/mixer`) |
| `UiDesignerLib` | Instrument-UI canvas (`src/ui/designer`, `src/ui/canvas`) |
| `UiComponentsLib` | Shared widgets: knobs, faders, title bar (`src/ui/components`) |
| `NodeMapLib` | Node-graph view (`src/ui/nodemap`) |
| `MiniaudioLib`, `FFTConvolverLib` | Vendored third party (`src/thirdparty`) |
| `SamplerEditorLib` | Static glue library |
| `DecentSamplerEditor` | The executable; owns `src/ui` top level, inspector, dock, and `ExportWorker` |

---

## Invariants

Violating any of these produces a bug that is expensive to trace back. Each was learned
the hard way; the linked document explains the full reasoning.

### Volume is decibels

`SampleGroup::volume`, `BusNode::volume` and `Zone::volume` are **decibels**. `0.0`
means unity, which is why the defaults are `0.0` and why parent and child gains compose
by **addition**.

Decent Sampler only reads a volume as decibels when the value carries a `dB` suffix —
a bare number there is a **linear 0..1 multiplier**, where `0` is total silence. Export
must therefore write `-6dB`, never `-6`. See `DsGroupBuilder::setVolumeDb`.

Anything feeding DSP needs `DecibelUtils::dbToLinear` first (`src/core/DecibelUtils.h`).

### The audio callback is real-time

No allocation, no locks, no file I/O inside `AudioEngine::processAudio` or anything it
calls. Sample sources are opened on the calling thread via
`AudioEngine::prepareSampleSource` and handed to a voice with `VoiceProcessor::adopt`;
finished sources go back through a lock-free free-queue to be released off-thread.
Parameter changes travel as `AudioMessage` values on a lock-free queue.

See [docs/02-architecture/audio-engine.md](docs/02-architecture/audio-engine.md).

### Export macros: one explicit export disables all the implicit ones

Shared libraries rely on CMake's `WINDOWS_EXPORT_ALL_SYMBOLS`. On **MinGW**, auto-export
applies only while *no* symbol is explicitly exported — adding `__declspec(dllexport)`
to a single class silently unexports everything else in that library. This already broke
the build once. If a class needs an explicit macro, every public class in that library
needs one too.

MSVC has the mirror-image problem: `WINDOWS_EXPORT_ALL_SYMBOLS` never covers **data**
symbols, so a `Q_OBJECT` class used across a library boundary fails to link on its
`staticMetaObject`. The cheaper fix is usually to move the class into the consumer.

### MSVC encodes `class` versus `struct`

MSVC puts the class-key in the mangled name (`U` for struct, `V` for class); GCC does
not. A type defined as `struct Node` but forward-declared as `class Node;` links fine on
MinGW and fails on MSVC with `LNK2019`. Forward declarations must match the definition.

### The test suite reports as a single case

All of QtTest runs under **one** `ctest` entry, and the binary is a GUI-subsystem
executable whose stdout is discarded. A hanging or failing case therefore cannot be
identified from CI logs today. To prove a new test actually runs, invert its assertion
and confirm the suite turns red.

---

## Documentation

| Looking for | Read |
|---|---|
| How to use the app | [docs/01-usage/](docs/01-usage/) |
| How it is built, threads, data model | [docs/02-architecture/](docs/02-architecture/) |
| Why the editor and the format disagree | [docs/02-architecture/workarounds.md](docs/02-architecture/workarounds.md) |
| What the `.dspreset` format supports | [docs/03-decent-sampler-reference/](docs/03-decent-sampler-reference/) |
| What is open, planned or deferred | [docs/04-planning/](docs/04-planning/) |

### Maintenance rules

The previous documentation rotted because nothing tied it to the code. These rules exist
to prevent a repeat, and apply to humans and agents equally.

1. **Cite file paths and symbol names, never line numbers.** Line numbers go stale
   within a single commit; `DsGroupBuilder::setVolumeDb` stays findable.
2. **Every document carries a `Last verified:` line** with a date and commit sha. If you
   read a document while working and it was still accurate, bump it.
3. **Anything deferred goes into
   [docs/04-planning/technical-debt.md](docs/04-planning/technical-debt.md) in the same
   change that defers it**, with the reason. "I will remember" is how the current debt
   became invisible.
4. **A change that alters an invariant above must update this file in the same commit.**
   The invariants list is a contract, not a summary.
5. **Mark unverified claims** with `> **To confirm**` instead of asserting them. A
   confident wrong statement costs more than an admitted gap.
6. **Record decisions, not just outcomes**, in
   [docs/04-planning/decisions.md](docs/04-planning/decisions.md), so settled trade-offs
   are not re-litigated.

---

*Last verified: 2026-08-18*
