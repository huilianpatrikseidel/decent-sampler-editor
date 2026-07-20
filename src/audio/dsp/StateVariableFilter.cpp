#include "StateVariableFilter.h"
#include <cmath>
#include <algorithm>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

StateVariableFilter::StateVariableFilter()
    : m_type(Type::LowPass)
    , m_sampleRate(44100.0)
    , m_cutoff(1000.0)
    , m_resonance(1.0)
    , m_cutoffMod(0.0)
    , m_ic1eq(0.0)
    , m_ic2eq(0.0)
    , m_g(0.0)
    , m_k(0.0)
    , m_a1(0.0)
    , m_a2(0.0)
    , m_a3(0.0)
{
    updateCoefficients();
}

void StateVariableFilter::setType(Type type) {
    m_type = type;
}

void StateVariableFilter::setSampleRate(double sampleRate) {
    if (sampleRate > 0.0) {
        m_sampleRate = sampleRate;
        updateCoefficients();
    }
}

void StateVariableFilter::setCutoff(double freq) {
    m_cutoff = std::clamp(freq, 20.0, 20000.0);
    updateCoefficients();
}

void StateVariableFilter::setResonance(double q) {
    m_resonance = std::clamp(q, 0.1, 10.0);
    updateCoefficients();
}

void StateVariableFilter::setCutoffMod(double mod) {
    m_cutoffMod = std::clamp(mod, -1.0, 1.0);
    updateCoefficients();
}

void StateVariableFilter::reset() {
    m_ic1eq = 0.0;
    m_ic2eq = 0.0;
}

void StateVariableFilter::updateCoefficients() {
    // Apply cutoff modulation: 4 octaves of range in each direction
    double effectiveCutoff = m_cutoff * std::pow(2.0, m_cutoffMod * 4.0);
    effectiveCutoff = std::clamp(effectiveCutoff, 20.0, 20000.0);

    // Cytomic SVF coefficients (trapezoidal integration, unconditionally stable)
    m_g = std::tan(M_PI * effectiveCutoff / m_sampleRate);
    m_k = 1.0 / m_resonance;  // resonance 0.1-10 maps to k 10-0.1
    m_a1 = 1.0 / (1.0 + m_g * (m_g + m_k));
    m_a2 = m_g * m_a1;
    m_a3 = m_g * m_a2;
}

float StateVariableFilter::process(float input) {
    double v0 = static_cast<double>(input);

    double v3 = v0 - m_ic2eq;
    double v1 = m_a1 * m_ic1eq + m_a2 * v3;
    double v2 = m_ic2eq + m_a2 * m_ic1eq + m_a3 * v3;

    m_ic1eq = 2.0 * v1 - m_ic1eq;
    m_ic2eq = 2.0 * v2 - m_ic2eq;

    double out = 0.0;
    switch (m_type) {
        case Type::LowPass:
            out = v2;
            break;
        case Type::HighPass:
            out = v0 - m_k * v1 - v2;
            break;
        case Type::BandPass:
            out = v1;
            break;
        case Type::Notch:
            out = v0 - m_k * v1;
            break;
    }

    return static_cast<float>(out);
}
