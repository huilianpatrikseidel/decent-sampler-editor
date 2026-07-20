#pragma once

class AdsrEnvelope {
public:
    enum State { Off = 0, Attack = 1, Decay = 2, Sustain = 3, Release = 4 };

    AdsrEnvelope();

    void trigger(float attackTime, float decayTime, float sustainLevel, float releaseTime, float sampleRate = 44100.0f);
    void release();
    void reset();

    float process();
    
    bool isActive() const { return m_state != Off; }
    State getState() const { return m_state; }
    float getCurrentAmp() const { return m_currentAmp; }

private:
    State m_state;
    float m_currentAmp;
    
    float m_attackRate;
    float m_decayRate;
    float m_sustainLevel;
    float m_releaseRate;
};
