#pragma once

#include <cmath>

// Channel and zone volumes are stored in decibels throughout the model: 0.0 means unity,
// which is why the defaults are 0.0 and why the exporter composes parent and child gains
// by adding them. The audio engine needs a linear multiplier, so every read that reaches
// DSP has to pass through here first.
//
// Decent Sampler uses the same convention on the other side, but only when the value
// carries a "dB" suffix in the XML; a bare number there is a linear 0..1 multiplier.
namespace DecibelUtils {

// Below this the result is inaudible, and returning a hard zero keeps a fader at the
// bottom of its travel from leaking a denormal-sized signal.
constexpr double kSilenceDb = -96.0;

inline float dbToLinear(double db) {
    if (db <= kSilenceDb) return 0.0f;
    return static_cast<float>(std::pow(10.0, db / 20.0));
}

inline double linearToDb(double linear) {
    if (linear <= 0.0) return kSilenceDb;
    return 20.0 * std::log10(linear);
}

} // namespace DecibelUtils
