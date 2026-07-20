#include "Oscillator.h"
#include <cmath>
#include <algorithm>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static constexpr double TWO_PI = 2.0 * M_PI;

Oscillator::Oscillator()
    : m_waveform(Waveform::Saw)
    , m_sampleRate(44100.0)
    , m_frequency(440.0)
    , m_phase(0.0)
    , m_phaseIncrement(0.0)
    , m_pitchMod(0.0)
    , m_noiseSeed(12345)
    , m_ksWritePos(0)
    , m_ksLength(0)
    , m_ksDamping(0.5)
    , m_ksInitialized(false)
{
    updatePhaseIncrement();
}

float Oscillator::nextNoise() {
    // Simple LCG (linear congruential generator) for fast noise
    // Produces uniform float in [-1.0, 1.0]
    m_noiseSeed = m_noiseSeed * 1664525u + 1013904223u;
    return (static_cast<float>(m_noiseSeed) / static_cast<float>(0x7FFFFFFFu)) * 2.0f - 1.0f;
}

void Oscillator::setWaveform(Waveform wf) {
    m_waveform = wf;
}

void Oscillator::setSampleRate(double sampleRate) {
    if (sampleRate > 0.0) {
        m_sampleRate = sampleRate;
        updatePhaseIncrement();
    }
}

void Oscillator::setFrequency(double freq) {
    m_frequency = freq;
    updatePhaseIncrement();
}

void Oscillator::setPitchMod(double semitones) {
    m_pitchMod = semitones;
    updatePhaseIncrement();
}

void Oscillator::setDamping(double damping) {
    m_ksDamping = std::clamp(damping, 0.0, 1.0);
}

void Oscillator::noteOn(int midiNote, int velocity) {
    m_frequency = 440.0 * std::pow(2.0, (midiNote - 69) / 12.0);
    m_phase = 0.0;
    updatePhaseIncrement();

    // Initialize Karplus-Strong buffer if needed
    if (m_waveform == Waveform::Pluck) {
        m_ksLength = static_cast<int>(m_sampleRate / m_frequency);
        if (m_ksLength < 2) m_ksLength = 2;
        if (m_ksLength > KS_BUFFER_SIZE) m_ksLength = KS_BUFFER_SIZE;

        // Scale initial noise by velocity for natural pluck dynamics
        float velScale = velocity / 127.0f;
        m_noiseSeed = static_cast<uint32_t>(midiNote * 7919 + velocity * 104729); // Deterministic seed per note
        for (int i = 0; i < m_ksLength; ++i) {
            m_ksBuffer[i] = nextNoise() * velScale;
        }
        m_ksWritePos = 0;
        m_ksInitialized = true;
    }
}

void Oscillator::noteOff() {
    // Nothing specific for oscillator; envelope handles amplitude fade
}

void Oscillator::reset() {
    m_phase = 0.0;
    m_ksWritePos = 0;
    m_ksInitialized = false;
    for (int i = 0; i < KS_BUFFER_SIZE; ++i) {
        m_ksBuffer[i] = 0.0f;
    }
}

void Oscillator::updatePhaseIncrement() {
    double modFreq = m_frequency * std::pow(2.0, m_pitchMod / 12.0);
    m_phaseIncrement = modFreq / m_sampleRate;
}

double Oscillator::polyBlep(double t) const {
    double dt = m_phaseIncrement;
    if (dt <= 0.0) return 0.0;

    if (t < dt) {
        t /= dt;
        return t + t - t * t - 1.0;
    } else if (t > 1.0 - dt) {
        t = (t - 1.0) / dt;
        return t * t + t + t + 1.0;
    }
    return 0.0;
}

float Oscillator::generateSample() {
    switch (m_waveform) {
        case Waveform::Sine:
            return static_cast<float>(std::sin(TWO_PI * m_phase));

        case Waveform::Saw: {
            double saw = 2.0 * m_phase - 1.0;
            saw -= polyBlep(m_phase);
            return static_cast<float>(saw);
        }

        case Waveform::Square: {
            double sq = (m_phase < 0.5) ? 1.0 : -1.0;
            sq += polyBlep(m_phase);
            sq -= polyBlep(std::fmod(m_phase + 0.5, 1.0));
            return static_cast<float>(sq);
        }

        case Waveform::Triangle: {
            double tri = 4.0 * std::abs(m_phase - 0.5) - 1.0;
            return static_cast<float>(tri);
        }

        case Waveform::Noise:
            return nextNoise();

        case Waveform::Pluck: {
            if (!m_ksInitialized || m_ksLength < 2) return 0.0f;

            float out = m_ksBuffer[m_ksWritePos];
            int nextPos = (m_ksWritePos + 1) % m_ksLength;
            float avg = 0.5f * (m_ksBuffer[m_ksWritePos] + m_ksBuffer[nextPos]);
            m_ksBuffer[m_ksWritePos] = avg * static_cast<float>(m_ksDamping);
            m_ksWritePos = (m_ksWritePos + 1) % m_ksLength;

            return out;
        }
    }
    return 0.0f;
}

float Oscillator::tick() {
    float sample = generateSample();

    m_phase += m_phaseIncrement;
    if (m_phase >= 1.0) {
        m_phase -= 1.0;
    }

    return sample;
}

void Oscillator::process(float* output, int numSamples) {
    for (int i = 0; i < numSamples; ++i) {
        output[i] = tick();
    }
}
