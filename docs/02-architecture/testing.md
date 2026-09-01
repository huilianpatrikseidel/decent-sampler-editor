# Testing

One QtTest binary, `SamplerEditorTests`, built from `tests/main_test.cpp` and registered
as a single `ctest` entry.

## Running

```bash
export PATH="/d/Development/Softwares/QT/6.11.1/mingw_64/bin:/d/Development/Softwares/QT/Tools/mingw1310_64/bin:/d/Development/projects/SamplerEditor/build:$PATH"
QT_QPA_PLATFORM=offscreen ctest --test-dir build --output-on-failure
```

**The build directory must be on `PATH`.** The binary links a dozen shared libraries;
without them every run exits `0xc0000135` ("DLL not found"), which reads like a test
failure and is not. `QT_QPA_PLATFORM=offscreen` lets the GUI tests run headless.

## Harness limitations

Two things the harness cannot do, both of which have already cost real time:

**It cannot tell you which case failed.** Everything runs under one `ctest` entry, so a
failure or a hang is reported against the suite as a whole.

**Its output is discarded.** The test binary is a GUI-subsystem executable with no
console, so QtTest's per-case output never reaches the CI log. When the Windows job hung,
the responsible test could not be identified at all.

Resolution is tracked in [../04-planning/technical-debt.md](../04-planning/technical-debt.md).

## Proving a test actually runs

Because a passing suite says nothing about whether your new case executed, **invert its
assertion and confirm the suite turns red**, then restore it. This is the only reliable
check available today, and it is worth doing for any test guarding non-obvious behaviour
— hand-written codecs, export formats, audio maths.

## What is covered

Model and undo commands, serialisation round-trips, the lock-free queue, voice source
lifecycle, transpiler output, decibel conversion and volume emission, FLAC round-trip
bit-exactness (verified against an independent decoder), and bundle contents.

## Conventions

- Audio tests that need a real device call `QSKIP` when initialisation fails, so headless
  CI does not report a false failure.
- Tests that need audio files build them in a `QTemporaryDir` rather than committing
  fixtures.

*Last verified: 2026-08-18*
