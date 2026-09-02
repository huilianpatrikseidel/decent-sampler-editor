# Backlog & Fix Guide — Decent Sampler Editor

> Everything the audit, cleanup, audio-correctness, cross-platform and UX work shipped is already on **`master`** — the Linux CI build + tests are the required gate and pass. This document is **what remains**. Every item has its exact location, root cause, and a concrete fix so it can be picked up cold.

**Status:** 7 open items · 0 known bugs · 0 feature gaps · Linux CI **green** (gate).

| Priority | Meaning | Effort | Meaning |
|---|---|---|---|
| 🔴 **Critical** | breaks a build/ships broken | **S** | hours |
| 🟠 **High** | real bug or major gap | **M** | ~1 day |
| 🟡 **Medium** | worth doing, contained | **L** | multi-day |
| ⚪ **Low** | polish / platform WIP | | |

---

## 1. CI: full green on Windows & macOS

*Linux is the required gate and passes. Windows & macOS run as `continue-on-error` (WIP) — they don't fail the build. Config: `.github/workflows/build.yml`.*

### 1.1 · Windows CI build (MSVC)
**Priority:** 🟡 Medium · **Effort:** M

**Situation.** The CI Windows runner uses **MSVC**, but the project is developed/shipped with **MinGW**. Two rounds of blockers are fixed (see §4): the `C2280` on the exported `DsNode`, and a static sweep for constructs GCC accepts but MSVC rejects — `M_PI` without a guard, and `<cstring>` arriving only transitively.

**This has still never been compiled with MSVC.** The fixes above are the known-hostile constructs a static audit can find; only a CI run can say what is left. Push and read the log — that is the next action, not more local auditing.

**Fix — pick a lane.**
1. **Keep fixing MSVC (recommended):** re-run and iterate on any remaining MSVC-strictness errors. Bonus: the code gets more standards-correct.
2. **Or match the toolchain:** switch the Windows leg to MinGW (`install-qt-action` `arch: win64_mingw` + matching MinGW toolchain + Ninja). Faithful to what actually ships; sidesteps MSVC-only issues.

### 1.2 · macOS CI build — `ld: framework 'AGL' not found`
**Priority:** 🟡 Medium · **Effort:** M

**Situation.** Configure passes; the link step fails resolving the deprecated `AGL` framework pulled in transitively by `Qt6::Gui` on the macOS runner. (Removing the unused `svg2ico` tool cleared its instance, but the app link still hits it.)

**Fix — investigate.**
1. Try a Qt patch/arch whose CMake config doesn't reference AGL, or the official Qt online installer package.
2. As a workaround, provide a link path / empty AGL shim, or drop the transitive OpenGL link if the app doesn't need it.
3. Confirm via `aqt list-qt mac desktop --arch` which build the runner should use (arm64 vs x86_64).

---

## 2. Cross-platform runtime

*The CMake now targets all three OSes, but several runtime features are still Windows-only behind `#ifdef Q_OS_WIN` guards (they compile out elsewhere).*

### 2.1 · VST3 plugin editor embedding is HWND-only
**Priority:** 🟡 Medium · **Effort:** M
**Location:** `src/audio/Vst3Host.cpp:204`, `src/ui/mixer/Vst3EditorWidget.cpp`

**Fix.** Pick the platform type per OS — `kPlatformTypeHWND` (Win, done), `kPlatformTypeNSView` (macOS), `kPlatformTypeX11EmbedWindowID` (Linux, plus the Steinberg `IRunLoop` plumbing — the fiddly part). `QWidget::winId()` already returns the right native handle per platform.

### 2.2 · Frameless custom title bar is Windows-native
**Priority:** ⚪ Low · **Effort:** M
**Location:** `src/ui/MainWindow.cpp:146` (`nativeEvent`), `src/ui/core/ThemeManager.cpp` (DWM)

**Fix.** Already isolated behind `Q_OS_WIN`, so it compiles elsewhere but has no resize/snap. Add per-OS frameless handling, or fall back to the native title bar on macOS/Linux.

### 2.3 · Bundled VST3 plugins are Windows-only
**Priority:** ⚪ Low · **Effort:** L
**Location:** `plugins/CMakeLists.txt` (guarded to `WIN32`)

**Fix.** The 13 built-in effects link VSTGUI with Windows-specific libraries. Port the VSTGUI link setup to macOS/Linux to re-enable `add_subdirectory(plugins)` off-Windows.

### 2.4 · Packaging: only Windows deploy exists
**Priority:** ⚪ Low · **Effort:** M
**Location:** `CMakeLists.txt` (install / `windeployqt`), `installer.iss`

**Fix.** Install rules + Inno Setup are Windows-shaped. Add `macdeployqt` → `.dmg` (notarization) for macOS and linuxdeployqt/AppImage for Linux.

---

## 3. Smaller known gaps

### 3.1 · Bundle export holds every sample in memory
**Priority:** ⚪ Low · **Effort:** M
**Location:** `src/export/BundleExporter.cpp`, `src/export/FlacEncoder.cpp` (`encodeFile`)

**Symptom.** Each sample is read whole, decoded whole, and encoded whole before being handed to miniz. A multi-GB library could exhaust memory during export.

**Fix.** Stream: decode/encode in blocks and feed `mz_zip_writer_add_*` incrementally. Pre-existing behaviour (the old code also called `readAll()`), so this is a scaling improvement, not a regression.

---

## 4. Shipped in this round

*Recorded so the next reader doesn't re-investigate them.*

| Item | What landed |
|---|---|
| **MSVC `C2280`** | Copy ops deleted on `DsNode`/`DsDocument` (`src/core/DsDom.h`). Audited every other `*_EXPORT` class: only these two hold a container of move-only values, which is the shape that trips dllexport — `unique_ptr`/`atomic` members already delete the copy implicitly. Also deleted the copy ops on `VoiceProcessor` and `Vst3Host`, whose implicit copies were well-formed and would have double-freed owned resources. |
| **Live MIDI modulation** | The MIDI callback now decodes CC 1, 14-bit Pitch Bend, and channel/poly Aftertouch into lock-free atomics on `GlobalAudioState`. `AudioEngine` smooths them once per frame into a `ModInputs` struct shared by every voice — so a note triggered mid-gesture starts at the current controller position instead of ramping up to it — and `VoiceProcessor` has the three missing mod-source cases. Covered by `testMidiModulationSources`. |
| **FLAC bundle compression** | `src/export/FlacEncoder` is a self-contained encoder (fixed predictors, Rice-coded residuals, stereo decorrelation, CRC-8/16, MD5 in STREAMINFO) — no new third-party dependency. Integer PCM WAVs at 8/16/24 bits are encoded; anything else is stored unchanged so nothing is ever degraded. `getBundleSampleName()` is the single naming decision shared by the transpiler and the zip writer. `testFlacEncoderRoundTrip` verifies bit-exactness by decoding through an independent decoder (dr_flac via miniaudio). `docs/03` now matches the implementation. |
| **Dock scroll** | Group Settings and Sequencer are wrapped in `QScrollArea` in `DockManager`; the other four tabs already scroll internally, so they were left alone to avoid nested scrollbars. The envelope knob block no longer pins an 85px height it overflowed. |
| **Libraries page** | Cards render a generated thumbnail (name-hashed gradient + initials) instead of a black rectangle, with a stored preview taking precedence when one exists. Added an empty state, a search icon, and wired the search field and sort selector, which were both inert. |
| **UX nits** | Envelope knobs 36px → 44px, value readout 10px → 11px, "Trigger Event" → "Trigger" to match the inspector and the `.dspreset` attribute, and the main tabs dropped their `1.`–`5.` prefixes (they are freely navigable, so the numbering promised a wizard flow that does not exist). |
| **Wavetable bundling** | Wavetable oscillators referenced a file the zone sweep never collected, so the preset pointed at a missing entry. Collected into a set of its own and stored unencoded, since the transpiler names it with `getSafeExportName(path, false)` and Decent Sampler is not confirmed to read FLAC wavetables. `testBundleIncludesWavetable` opens the produced zip and asserts the entry exists; it was confirmed to fail without the fix. |
| **MSVC hardening** | `M_PI` guarded in the three files that used it with only `<cmath>` (`Oscillator`/`StateVariableFilter` already had the guard), and explicit `<cstring>` where `memset`/`strncpy` were arriving only transitively. Static audit found no VLAs, `__attribute__`, alternative tokens, or `windows.h` min/max clashes. |
| **Per-bus FX** | Delivered across five stages: decibel coherence, `insertEffects` as the canonical chain, group-to-bus routing, per-channel rendering in the engine, and bus flattening on export. Detail in [per-bus-fx-plan.md](per-bus-fx-plan.md). |
| **Docs** | The three stale audit reports moved to `docs/04-planning/history/` behind a README explaining they are superseded. |

---

## Recommended sequence

1. **Confirm bus effects by ear** — the render path is in place and tested, but nobody has yet listened to a reverb on a bus with a real instrument loaded.
2. **Drive the Windows CI leg to green** — the build now links; the tests hang for reasons still unidentified.
3. **Cross-platform runtime + packaging** — tackle once a target platform is actually being shipped.

---

*Line numbers reference the state at the last pushed commit on `master`; re-check after edits.*
