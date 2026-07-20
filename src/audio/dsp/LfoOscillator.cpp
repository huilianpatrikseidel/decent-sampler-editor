#include "LfoOscillator.h"
#include <cmath>

LfoOscillator::LfoOscillator() : m_phase(0.0f), m_freq(1.0f), m_amount(0.0f), m_dt(1.0f / 44100.0f), m_shape(LfoShape::Sine) {}

void LfoOscillator::setFrequency(float freq) {
    m_freq = freq;
}

void LfoOscillator::setAmount(float amount) {
    m_amount = amount;
}

void LfoOscillator::setSampleRate(float sampleRate) {
    if (sampleRate > 0.0f) {
        m_dt = 1.0f / sampleRate;
    }
}

void LfoOscillator::setShape(LfoShape shape) {
    m_shape = shape;
}

float LfoOscillator::process() {
    m_phase += m_freq * m_dt;
    if (m_phase > 1.0f) {
        m_phase -= 1.0f;
    }
    
    float out = 0.0f;
    switch (m_shape) {
        case LfoShape::Sine:
            out = std::sin(m_phase * 2.0f * 3.1415926535f);
            break;
        case LfoShape::Triangle:
            out = 2.0f * std::abs(2.0f * m_phase - 1.0f) - 1.0f;
            break;
        case LfoShape::Square:
            out = (m_phase < 0.5f) ? 1.0f : -1.0f;
            break;
        case LfoShape::Saw:
            out = 2.0f * m_phase - 1.0f;
            break;
    }
    
    return out * m_amount;
}

