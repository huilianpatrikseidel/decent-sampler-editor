#pragma once

class StateVariableFilter {
public:
    enum class Type {
        LowPass,
        HighPass,
        BandPass,
        Notch
    };

    StateVariableFilter();

    void setType(Type type);
    void setSampleRate(double sampleRate);
    void setCutoff(double freq);
    void setResonance(double q);
    void setCutoffMod(double mod);

    float process(float input);
    void reset();

private:
    Type m_type = Type::LowPass;
    double m_sampleRate = 44100.0;
    double m_cutoff = 1000.0;
    double m_resonance = 1.0;
    double m_cutoffMod = 0.0;

    // SVF state
    double m_ic1eq = 0.0;
    double m_ic2eq = 0.0;

    // Coefficients
    double m_g = 0.0;
    double m_k = 0.0;
    double m_a1 = 0.0;
    double m_a2 = 0.0;
    double m_a3 = 0.0;

    void updateCoefficients();
};
