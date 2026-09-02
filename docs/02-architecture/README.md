# Architecture

| Document | Answers |
|---|---|
| [overview.md](overview.md) | What the targets are, how the layers split, where the thread boundary sits |
| [audio-engine.md](audio-engine.md) | How audio is rendered and how the UI talks to the audio thread safely |
| [data-model.md](data-model.md) | What a project is made of and how it is stored |
| [decent-sampler-export.md](decent-sampler-export.md) | How the model becomes a `.dspreset` / `.dsbundle` |
| [workarounds.md](workarounds.md) | Where the editor's concepts do not exist in the format, and how export bridges the gap |
| [build-and-toolchain.md](build-and-toolchain.md) | CMake layout, export macros, compiler traps, CI |
| [testing.md](testing.md) | Running the suite, sanitizers, and what the harness cannot tell you |
| [ui-automation.md](ui-automation.md) | Driving the interface from outside the process |
| [ui-automation.md](ui-automation.md) | Driving the running app from outside via Windows UI Automation, and the gotchas that cost time |

*Last verified: 2026-09-01*
