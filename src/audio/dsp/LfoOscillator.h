#pragma once

enum class LfoShape { Sine, Triangle, Square, Saw };

class LfoOscillator {
public:
    LfoOscillator();

    void setFrequency(float freq);
    void setAmount(float amount);
    void setSampleRate(float sampleRate);
    void setShape(LfoShape shape);
    
    float process();
    
private:
    float m_phase;
    float m_freq;
    float m_amount;
    float m_dt;
    LfoShape m_shape;
};
