#pragma once
#include "thirdparty/miniaudio/miniaudio.h"
#include "AdsrEnvelope.h"
#include "Oscillator.h"
#include "StateVariableFilter.h"
#include "../AudioMessage.h"
#include "../AudioState.h"

class VoiceProcessor {
public:
    VoiceProcessor();
    ~VoiceProcessor();
    
    bool init(ma_resource_manager* rm, const char* filepath);
    void initOscillator(const AudioMessage& msg);
    void trigger(const AudioMessage& msg, int rootNote = 60);
    void release();
    void kill();
    
    void process(float lfo1Val, float lfo2Val, float& outL, float& outR, GlobalAudioState* state);

    bool isActive() const { return m_active; }
    int getPlayingNote() const { return m_playingNote; }
    QUuid getGroupId() const { return m_groupId; }
    bool isReleasing() const { return m_adsr.getState() == AdsrEnvelope::Release; }
    
    void setTargetNote(int note, float glideTime);

private:
    bool m_active;
    bool m_isOscillator = false;  // true = oscillator synthesis, false = sample playback
    
    // Sample playback
    ma_resource_manager_data_source m_dataSource;
    bool m_hasDataSource;
    float m_historyL[4];
    float m_historyR[4];
    double m_readPointer;
    
    // Oscillator synthesis
    Oscillator m_oscillator;
    
    int m_playingNote;
    int m_rootNote;
    // Modulation Sources
    float m_velocity;
    float m_volume;
    float m_smoothedVolume;
    int m_paramBlockIndex = -1;
    QUuid m_groupId;
    
    // Modulation Routingamento
    float m_currentPitchOffset;
    float m_targetPitchOffset;
    float m_glideRate;
    
    AdsrEnvelope m_adsr;
    AdsrEnvelope m_modAdsr;
    
    RenderRouting m_routings[8];
    int m_numRoutings;
    
    // Voice Filter State (SVF replaces naive 1-pole)
    bool m_hasFilter;
    StateVariableFilter m_svfL;  // Left channel SVF
    StateVariableFilter m_svfR;  // Right channel SVF
    float m_filterCutoff;
    float m_filterResonance;
    
    float hermite(float frac, float xm1, float x0, float x1, float x2);
};
