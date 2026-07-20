#pragma once
#include <cstdint>

class Oscillator {
public:
    enum class Waveform {
        Sine,
        Saw,
        Square,
        Triangle,
        Noise,
        Pluck  // Karplus-Strong
    };

    Oscillator();

    void setWaveform(Waveform wf);
    void setSampleRate(double sampleRate);
    void setFrequency(double freq);
    void setPitchMod(double semitones);

    // Process a block of samples (mono)
    void process(float* output, int numSamples);

    // Get a single sample and advance phase
    float tick();

    void noteOn(int midiNote, int velocity);
    void noteOff();
    void reset();

    // Karplus-Strong specific
    void setDamping(double damping);

private:
    Waveform m_waveform = Waveform::Saw;
    double m_sampleRate = 44100.0;
    double m_frequency = 440.0;
    double m_phase = 0.0;
    double m_phaseIncrement = 0.0;
    double m_pitchMod = 0.0;

    // Simple LCG noise state (avoids <random> header SSE issues with MinGW)
    uint32_t m_noiseSeed = 12345;
    float nextNoise();

    // Karplus-Strong state
    static constexpr int KS_BUFFER_SIZE = 4096;
    float m_ksBuffer[KS_BUFFER_SIZE] = {};
    int m_ksWritePos = 0;
    int m_ksLength = 0;
    double m_ksDamping = 0.5;
    bool m_ksInitialized = false;

    void updatePhaseIncrement();
    float generateSample();

    // PolyBLEP for anti-aliased saw/square
    double polyBlep(double t) const;
};
