#include "AdsrEnvelope.h"

AdsrEnvelope::AdsrEnvelope() : m_state(Off), m_currentAmp(0.0f), m_attackRate(0.0f), m_decayRate(0.0f), m_sustainLevel(1.0f), m_releaseRate(0.0f) {}

void AdsrEnvelope::trigger(float attackTime, float decayTime, float sustainLevel, float releaseTime, float sampleRate) {
    m_state = Attack;
    m_currentAmp = 0.0f;
    m_sustainLevel = sustainLevel;
    
    m_attackRate = (attackTime > 0.0f) ? (1.0f / (attackTime * sampleRate)) : 1.0f;
    m_decayRate = (decayTime > 0.0f) ? ((1.0f - sustainLevel) / (decayTime * sampleRate)) : 1.0f;
    m_releaseRate = (releaseTime > 0.0f) ? (1.0f / (releaseTime * sampleRate)) : 1.0f;
}

void AdsrEnvelope::release() {
    if (m_state != Off) {
        m_state = Release;
    }
}

void AdsrEnvelope::reset() {
    m_state = Off;
    m_currentAmp = 0.0f;
}

float AdsrEnvelope::process() {
    switch (m_state) {
        case Attack:
            m_currentAmp += m_attackRate;
            if (m_currentAmp >= 1.0f) {
                m_currentAmp = 1.0f;
                m_state = Decay;
            }
            break;
            
        case Decay:
            m_currentAmp -= m_decayRate;
            if (m_currentAmp <= m_sustainLevel) {
                m_currentAmp = m_sustainLevel;
                m_state = Sustain;
            }
            break;
            
        case Sustain:
            // stays at sustainLevel
            break;
            
        case Release:
            m_currentAmp -= m_releaseRate;
            if (m_currentAmp <= 0.0f) {
                m_currentAmp = 0.0f;
                m_state = Off;
            }
            break;
            
        case Off:
            m_currentAmp = 0.0f;
            break;
    }
    return m_currentAmp;
}
