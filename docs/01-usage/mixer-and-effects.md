# Mixer and effects

**Mixer & FX** in the bottom dock. One channel strip per group and per bus, plus the
master strip.

## Channel strips

Each strip has a fader, a pan control, mute and solo, the channel name, and an FX rack.

The fader is calibrated in **decibels**. Unity (0 dB) sits at 80% of the travel, leaving
headroom up to +6 dB above it; below unity the scale runs down to −60 dB and then to
silence at the bottom. Double-clicking returns it to 0 dB.

Because 0 dB means "unchanged", a channel you have not touched is already at unity.

## Insert effects

The FX rack on a strip holds that channel's insert chain, in order. Available effects:
delay, reverb (convolution when given an impulse response), filter, chorus, gain, phaser,
pitch shifter, wave folder, wave shaper, stereo simulator, bit crusher and equalizer.

**Insert effects are not audible during audition yet.** Only the master chain is applied
by the built-in engine. They are part of the project and reach the exported instrument.
See [../04-planning/per-bus-fx-plan.md](../04-planning/per-bus-fx-plan.md).

Effects can be reordered by dragging vertically within a rack and can also be moved across channels by dragging an effect slot onto a different channel's rack (using `application/x-sampler-fx` drag and drop).

## Buses

A bus is a submix channel: route several groups into it and treat them as one — a single
reverb across a whole string section, for instance.

**Buses exist only in the editor.** The Decent Sampler format has no submix concept, so
on export a bus is flattened: its effects are copied onto every group feeding it, and its
gain is folded into each group's volume. A group at −6 dB into a bus at +6 dB exports at
0 dB.

The practical consequence: a shared reverb becomes several independent reverbs in the
exported instrument. For gain, EQ and filtering the result is identical; for reverb and
delay tails it is an approximation. Full reasoning in
[../02-architecture/workarounds.md](../02-architecture/workarounds.md).

Buses are created in the **Node Map** tab by right-clicking the canvas and choosing **Add Bus**. Signal routing between Groups, Buses, and the Master output is managed graphically in the **Node Map** tab signal graph.

## Master

The master strip carries the instrument-wide chain. Its fader currently drives the
audition output but is **not saved with the project** — recorded in
[../04-planning/technical-debt.md](../04-planning/technical-debt.md).

*Last verified: 2026-08-18*
