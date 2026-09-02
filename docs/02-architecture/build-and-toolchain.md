# Build and toolchain

CMake with `FetchContent`. **No vcpkg** — earlier documentation claimed otherwise and was
wrong. Dependencies: Qt 6, miniaudio and FFTConvolver (vendored in `src/thirdparty`),
RtMidi, miniz, and the Steinberg VST3 SDK.

Commands are in [../../CLAUDE.md](../../CLAUDE.md).

## Target layout

Twelve libraries plus the executable, listed in [../../CLAUDE.md](../../CLAUDE.md). Each
shared library sets `WINDOWS_EXPORT_ALL_SYMBOLS ON` and, where it has an export header,
a matching `*_BUILD` compile definition.

## Symbol export

Each library has an export header (`CoreLibExport.h`, `AudioEngineExport.h`, …) defining
a `*_EXPORT` macro that expands to `Q_DECL_EXPORT` while building the library and
`Q_DECL_IMPORT` elsewhere.

Two rules, both learned from build failures:

**On MinGW, auto-export is all-or-nothing.** The linker exports every symbol only while
*no* symbol is explicitly exported. Adding `__declspec(dllexport)` to a single class in a
library silently unexports everything else in it, producing a wall of undefined
references from unrelated code. If one public class in a library needs an explicit macro,
they all do.

**On MSVC, `WINDOWS_EXPORT_ALL_SYMBOLS` skips data symbols.** A `Q_OBJECT` class used
across a library boundary fails to link on its `staticMetaObject`, which is data. Moving
the class into its only consumer is usually cheaper than exporting the whole library
explicitly — that is what was done for `ExportWorker`.

## MSVC vs GCC

**Class-key mangling.** MSVC encodes `struct` and `class` differently in mangled names;
GCC does not. A type defined as `struct Node` but forward-declared `class Node;` links on
MinGW and fails on MSVC with `LNK2019`. Forward declarations must match the definition.

**`dllexport` instantiates everything.** MSVC instantiates every member of a dllexported
class, including implicit copy operations. A class holding a container of move-only values
(`std::vector<std::unique_ptr<T>>`) then fails with `C2280`. Deleting the copy operations
fixes it — and is usually correct anyway. Plain `unique_ptr` or `atomic` members are safe,
because those already delete the copy implicitly.

**Missing transitive includes.** GCC pulls in `<cstring>` and defines `M_PI` more freely.
MSVC needs `<cstring>` for `memset`/`strncpy` explicitly, and `M_PI` guarded, since it is
not exposed from `<cmath>` without `_USE_MATH_DEFINES`.

## CI

`.github/workflows/build.yml`, matrix over Ubuntu, Windows and macOS.

**Linux is the required gate.** Windows and macOS run with `continue-on-error` because
both are known-red; the run passes regardless of their result.

Note the workflow triggers only on pushes to `master` and pull requests targeting it —
pushing a branch alone does **not** start a run.

Current state and open failures: [../04-planning/backlog.md](../04-planning/backlog.md).

## Sanitizers

Opt-in ASan/UBSan through CMake options, off by default:

```bash
cmake -B build-asan -G Ninja -DCMAKE_BUILD_TYPE=Debug -DENABLE_ASAN=ON
cmake --build build-asan
ctest --test-dir build-asan --output-on-failure
```

Add `-DENABLE_UBSAN=ON` for UBSan. The configure step self-checks the toolchain and
aborts early with an actionable message if it cannot link a sanitized binary.

The most valuable target is `SamplerEditorTests`, which exercises the voice-lifecycle
paths (`adopt` / `releaseSource` / `prepareSampleSource` / `reinitialize`) the sanitizer
is meant to harden.

*Last verified: 2026-08-18*
