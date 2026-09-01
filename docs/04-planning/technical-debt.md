# Technical debt

Compromises deliberately left in place. Each entry records **what**, **why it was
acceptable at the time**, and **what it would take to resolve**. Anything deferred during
a change belongs here in that same change — the point is that debt stays visible.

Items severe enough to be scheduled work live in [backlog.md](backlog.md) instead; this
file is for things we chose to live with.

---

## Master fader does not persist

`MixerView` sends the master fader's value straight to the audio engine and never calls
`AudioStateModel::setGlobalVolume`. The exported `globalVolume` therefore ignores
whatever the user set on the master strip, and the setting is lost on reload.

*Why:* fixing it needs its own undo command and a decision about whether master gain is
a project property or a session preference.

*Resolution:* route the fader through a `ModifyPropertyCommand` like the other channels.

---

## VST3 host lifetime is timing-based

`VstPluginManager` swaps effect chains by publishing a new vector, sleeping 50 ms in a
detached thread, then deleting the old one, with a further `QTimer::singleShot(100)`
holding the old hosts alive. Nothing actually proves the audio thread has finished with
them.

*Why:* it predates this work and has not misbehaved in practice.

*Resolution:* a reference count or a generation counter the audio thread acknowledges.
This gets more pressing once per-channel chains multiply the number of live hosts.

---

## Bundle export holds every sample in memory

Each sample is read whole, decoded whole and encoded whole before reaching the archive
writer. A multi-gigabyte library could exhaust memory during export.

*Why:* the previous code also called `readAll()`, so this is a pre-existing scaling
limit rather than a regression.

*Resolution:* stream in blocks and feed `mz_zip_writer_add_*` incrementally.

---

## The test suite cannot name a failing case

The whole QtTest binary is one `ctest` entry, and it is a GUI-subsystem executable whose
stdout is discarded. When the Windows CI job hung, the responsible test could not be
identified from the logs at all.

*Why:* it has been adequate while Linux was the only green platform.

*Resolution:* register each test slot as its own `add_test`, and set `timeout-minutes`
on the CI job so a hang fails in minutes instead of hours.

---

## SFZ export ignores oscillators and wavetables

`SfzTranspiler` emits sample regions only. Oscillator groups and wavetable files are
silently absent from an SFZ export.

*Why:* SFZ support is secondary, and the format's synthesis support varies by player.

*Resolution:* decide the target player, then map what it supports.

---

## Windows and macOS runtime gaps

VST3 editor embedding is HWND-only, the frameless title bar is Windows-native, the
bundled VST3 effects build only on Windows, and packaging exists only as Inno Setup.
Tracked in [backlog.md](backlog.md); listed here because they are why the CI matrix is
not green.

*Last verified: 2026-08-18*
