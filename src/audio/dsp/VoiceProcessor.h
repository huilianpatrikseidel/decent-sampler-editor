#pragma once
#include "thirdparty/miniaudio/miniaudio.h"
#include "AdsrEnvelope.h"
#include "Oscillator.h"
#include "StateVariableFilter.h"
#include "../AudioMessage.h"
#include "../AudioState.h"
#include "../AudioEngineExport.h"

class AUDIOENGINE_EXPORT VoiceProcessor {
public:
    VoiceProcessor();
    ~VoiceProcessor();

    // Owns m_dataSource, which the destructor uninits. Copying a voice would hand two
    // objects the same source and double-free it, so the copy ops are deleted rather
    // than left implicit (dllexport would happily instantiate them on MSVC).
    VoiceProcessor(const VoiceProcessor&) = delete;
    VoiceProcessor& operator=(const VoiceProcessor&) = delete;

    void initOscillator(const AudioMessage& msg);
    void trigger(const AudioMessage& msg, int rootNote = 60);
    void release();
    void kill();

    // Take ownership of a data source prepared on a non-audio thread and return the
    // previously-held one (or nullptr) for disposal off the audio thread. Allocation-
    // and lock-free: safe to call from the audio callback.
    ma_resource_manager_data_source* adopt(ma_resource_manager_data_source* src);

    // Uninit + free the held source. Only call with the audio device stopped, since
    // uninit touches the resource manager and frees memory.
    void releaseSource();

    // Must be set (from the audio device rate) before any trigger; defaults to 44100.
    void setSampleRate(double sampleRate) { if (sampleRate > 0.0) m_sampleRate = sampleRate; }
    
    void process(const ModInputs& mods, float& outL, float& outR, GlobalAudioState* state);

    bool isActive() const { return m_active; }
    int getPlayingNote() const { return m_playingNote; }
    QUuid getGroupId() const { return m_groupId; }
    bool isReleasing() const { return m_adsr.getState() == AdsrEnvelope::Release; }
    
    void setTargetNote(int note, float glideTime);
    void updateParameters(const AudioMessage& msg);

private:
    bool m_active;
    bool m_isOscillator = false;  // true = oscillator synthesis, false = sample playback
    double m_sampleRate = 44100.0;  // audio device rate, set via setSampleRate()
    
    // Sample playback. The voice borrows this pointer (prepared on another thread);
    // ownership is transferred in/out via adopt()/releaseSource().
    ma_resource_manager_data_source* m_dataSource = nullptr;
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
