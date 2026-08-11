# Sanitizers (ASan / UBSan)

Opt-in AddressSanitizer / UndefinedBehaviorSanitizer support, wired through CMake
options in `CMakeLists.txt`. Off by default — normal builds are unaffected.

## Enabling

```bash
cmake -B build-asan -G Ninja -DCMAKE_BUILD_TYPE=Debug -DENABLE_ASAN=ON   # add -DENABLE_UBSAN=ON for UBSan
cmake --build build-asan
ctest --test-dir build-asan --output-on-failure
```

The most valuable target is **`SamplerEditorTests`** — it exercises the audio
voice-lifecycle paths (`adopt` / `releaseSource` / `prepareSampleSource` /
`reinitialize`) that the sanitizer is meant to harden.

The configure step self-checks the toolchain: if the compiler can't link a
sanitized binary it aborts immediately with an actionable message (no wasted
FetchContent/build time).

## Toolchain support

| Platform / compiler | ASan | Notes |
|---|:--:|---|
| Linux / macOS — GCC or Clang | ✅ | Reliable. The recommended way to run sanitized tests. |
| Windows — MSVC (`cl`) | ✅ | `/fsanitize=address`; needs a Qt-for-MSVC build and the ASan runtime DLL on `PATH`. |
| Windows — MinGW GCC *(current default toolchain)* | ❌ | mingw-w64 GCC ships no `libasan`; configure aborts by design. |
| Windows — llvm-mingw (Clang) | ⚠️ | ASan runtime is present, but the Win32 interceptor is unstable here (`interception_win: unhandled instruction` → crash), and its libc++ clashes with the GCC-built Qt's libstdc++. Not recommended. |

Because the project currently builds **Windows-only with MinGW**, the practical
home for sanitized runs is the **Linux/macOS CI legs** that come online with the
cross-platform work. Once those build, add a job such as:

```yaml
  asan:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
      - name: Install deps
        run: sudo apt-get update && sudo apt-get install -y qt6-base-dev qt6-svg-dev ninja-build
      - name: Build + run tests under ASan/UBSan
        run: |
          cmake -B build-asan -G Ninja -DCMAKE_BUILD_TYPE=Debug -DENABLE_ASAN=ON -DENABLE_UBSAN=ON
          cmake --build build-asan
          ctest --test-dir build-asan --output-on-failure
        env:
          ASAN_OPTIONS: detect_leaks=1:abort_on_error=1
          UBSAN_OPTIONS: halt_on_error=1:print_stacktrace=1
```

## Windows-now alternative (no recompile)

To memory-check the existing MinGW build on Windows today, use **Dr. Memory**
(a Valgrind-style tool that instruments GCC binaries without ASan):

```bash
drmemory -- build/tests/SamplerEditorTests.exe
```
