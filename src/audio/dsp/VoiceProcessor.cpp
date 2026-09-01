#include "VoiceProcessor.h"
#include <cmath>
#include <algorithm>

VoiceProcessor::VoiceProcessor() : m_active(false), m_readPointer(0.0), m_rootNote(60), m_playingNote(60), m_volume(1.0f) {
    for (int i=0; i<4; ++i) {
        m_historyL[i] = 0.0f;
        m_historyR[i] = 0.0f;
    }
}

VoiceProcessor::~VoiceProcessor() {
    if (m_dataSource) {
        ma_resource_manager_data_source_uninit(m_dataSource);
        delete m_dataSource;
        m_dataSource = nullptr;
    }
}

ma_resource_manager_data_source* VoiceProcessor::adopt(ma_resource_manager_data_source* src) {
    ma_resource_manager_data_source* old = m_dataSource;
    m_dataSource = src;
    m_readPointer = 0.0;
    return old;
}

void VoiceProcessor::releaseSource() {
    m_active = false;
    if (m_dataSource) {
        ma_resource_manager_data_source_uninit(m_dataSource);
        delete m_dataSource;
        m_dataSource = nullptr;
    }
}

void VoiceProcessor::initOscillator(const AudioMessage& msg) {
    m_isOscillator = true;
    
    // Map integer waveform to enum
    switch (msg.oscWaveform) {
        case 0: m_oscillator.setWaveform(Oscillator::Waveform::Sine); break;
        case 1: m_oscillator.setWaveform(Oscillator::Waveform::Saw); break;
        case 2: m_oscillator.setWaveform(Oscillator::Waveform::Square); break;
        case 3: m_oscillator.setWaveform(Oscillator::Waveform::Triangle); break;
        case 4: m_oscillator.setWaveform(Oscillator::Waveform::Noise); break;
        case 5: m_oscillator.setWaveform(Oscillator::Waveform::Pluck); break;
        default: m_oscillator.setWaveform(Oscillator::Waveform::Saw); break;
    }
    
    m_oscillator.setSampleRate(m_sampleRate);
    m_oscillator.setDamping(msg.oscDamping);
    m_oscillator.noteOn(msg.note, msg.velocity);
}

void VoiceProcessor::trigger(const AudioMessage& msg, int rootNote) {
    m_active = true;
    m_playingNote = msg.note;
    m_rootNote = rootNote;
    m_groupId = msg.groupId;
    m_paramBlockIndex = msg.paramBlockIndex;
    m_channelIndex = msg.channelIndex;
    float velNorm = msg.velocity / 127.0f;
    m_volume = msg.volume * velNorm;
    m_smoothedVolume = 1.0f;
    m_velocity = velNorm;
    m_readPointer = 0.0;
    
    m_currentPitchOffset = static_cast<float>(msg.note - rootNote);
    m_targetPitchOffset = m_currentPitchOffset;
    m_glideRate = 0.0f;
    
    m_numRoutings = msg.numRoutings;
    for (int i = 0; i < msg.numRoutings; ++i) {
        m_routings[i] = msg.routings[i];
    }
    
    m_hasFilter = msg.hasFilter;
    m_filterCutoff = msg.filterCutoff;
    m_filterResonance = msg.filterResonance;
    
    // Setup SVF filters
    if (m_hasFilter) {
        StateVariableFilter::Type svfType = StateVariableFilter::Type::LowPass;
        switch (msg.filterType) {
            case 0: svfType = StateVariableFilter::Type::LowPass; break;
            case 1: svfType = StateVariableFilter::Type::HighPass; break;
            case 2: svfType = StateVariableFilter::Type::BandPass; break;
            case 3: svfType = StateVariableFilter::Type::Notch; break;
        }
        m_svfL.reset(); m_svfR.reset();
        m_svfL.setSampleRate(m_sampleRate); m_svfR.setSampleRate(m_sampleRate);
        m_svfL.setType(svfType); m_svfR.setType(svfType);
        m_svfL.setCutoff(m_filterCutoff); m_svfR.setCutoff(m_filterCutoff);
        m_svfL.setResonance(m_filterResonance); m_svfR.setResonance(m_filterResonance);
    }
    
    // Handle oscillator mode
    m_isOscillator = msg.isOscillator;
    if (m_isOscillator) {
        initOscillator(msg);
    }
    
    for (int i=0; i<4; ++i) {
        m_historyL[i] = 0.0f;
        m_historyR[i] = 0.0f;
    }
    
    m_adsr.trigger(msg.attack, msg.decay, msg.sustain, msg.release, static_cast<float>(m_sampleRate));
    m_modAdsr.trigger(msg.modAttack, msg.modDecay, msg.modSustain, msg.modRelease, static_cast<float>(m_sampleRate));
}

void VoiceProcessor::setTargetNote(int note, float glideTime) {
    m_playingNote = note;
    m_targetPitchOffset = static_cast<float>(note - m_rootNote);
    if (glideTime > 0.001f) {
        float samples = glideTime * static_cast<float>(m_sampleRate);
        m_glideRate = (m_targetPitchOffset - m_currentPitchOffset) / samples;
    } else {
        m_currentPitchOffset = m_targetPitchOffset;
        m_glideRate = 0.0f;
    }
    
    // For oscillator, update the frequency target
    if (m_isOscillator) {
        double freq = 440.0 * std::pow(2.0, (note - 69) / 12.0);
        m_oscillator.setFrequency(freq);
    }
}

void VoiceProcessor::release() {
    m_adsr.release();
    if (m_isOscillator) {
        m_oscillator.noteOff();
    }
}

void VoiceProcessor::kill() {
    m_active = false;
    m_adsr.reset();
    if (m_isOscillator) {
        m_oscillator.reset();
    }
}

float VoiceProcessor::hermite(float frac, float xm1, float x0, float x1, float x2) {
    const float c = (x1 - xm1) * 0.5f;
    const float v = x0 - x1;
    const float w = c + v;
    const float a = w + v + (x2 - x0) * 0.5f;
    const float b_neg = w + a;
    return ((((a * frac) - b_neg) * frac + c) * frac + x0);
}

void VoiceProcessor::process(const ModInputs& mods, float& outL, float& outR, GlobalAudioState* state) {
    if (!m_active) {
        outL = 0.0f;
        outR = 0.0f;
        return;
    }

    // A sample voice with no data source (e.g. slot reclaimed) produces silence.
    if (!m_isOscillator && !m_dataSource) {
        m_active = false;
        outL = 0.0f;
        outR = 0.0f;
        return;
    }

    // Process Envelopes
    float envAmp = m_adsr.process();
    float modEnvAmp = m_modAdsr.process();
    
    if (m_adsr.getState() == AdsrEnvelope::Off) {
        m_active = false;
        outL = 0.0f;
        outR = 0.0f;
        return;
    }
    
    // Process Glide
    if (m_glideRate != 0.0f) {
        m_currentPitchOffset += m_glideRate;
        if ((m_glideRate > 0.0f && m_currentPitchOffset >= m_targetPitchOffset) ||
            (m_glideRate < 0.0f && m_currentPitchOffset <= m_targetPitchOffset)) {
            m_currentPitchOffset = m_targetPitchOffset;
            m_glideRate = 0.0f;
        }
    }
    
    // Calculate Routing Modulations
    float pitchMod = 0.0f;
    float volMod = 0.0f;
    float cutoffMod = 0.0f;
    float panMod = 0.0f;
    float resMod = 0.0f;
    
    for (int i = 0; i < m_numRoutings; ++i) {
        float sourceVal = 0.0f;
        switch (m_routings[i].source) {
            case ModSource::LFO1: sourceVal = mods.lfo1; break;
            case ModSource::LFO2: sourceVal = mods.lfo2; break;
            case ModSource::Env1: sourceVal = envAmp; break;
            case ModSource::Env2: sourceVal = modEnvAmp; break;
            case ModSource::Velocity: sourceVal = m_velocity; break;
            case ModSource::ModWheel: sourceVal = mods.modWheel; break;
            case ModSource::PitchBend: sourceVal = mods.pitchBend; break;
            case ModSource::Aftertouch: sourceVal = mods.aftertouch; break;
            default: break;
        }
        
        switch (m_routings[i].dest) {
            case ModDest::Pitch: pitchMod += sourceVal * m_routings[i].amount; break;
            case ModDest::Volume: volMod += sourceVal * m_routings[i].amount; break;
            case ModDest::Cutoff: cutoffMod += sourceVal * m_routings[i].amount; break;
            case ModDest::Pan: panMod += sourceVal * m_routings[i].amount; break;
            case ModDest::Resonance: resMod += sourceVal * m_routings[i].amount; break;
            default: break;
        }
    }
    
    float sampleL, sampleR;
    
    if (m_isOscillator) {
        // ── OSCILLATOR SYNTHESIS MODE ──
        // Apply pitch modulation in semitones
        m_oscillator.setPitchMod(pitchMod * 12.0f + m_currentPitchOffset);
        
        float oscSample = m_oscillator.tick();
        sampleL = oscSample;
        sampleR = oscSample;
    } else {
        // ── SAMPLE PLAYBACK MODE ──
        // Pitch ratio with routing modulations
        float routingPitchRatio = std::pow(2.0f, pitchMod);
        float finalRatio = routingPitchRatio * std::pow(2.0f, m_currentPitchOffset / 12.0f);
        m_readPointer += finalRatio;
        
        while (m_readPointer >= 1.0) {
            m_historyL[0] = m_historyL[1]; m_historyL[1] = m_historyL[2]; m_historyL[2] = m_historyL[3];
            m_historyR[0] = m_historyR[1]; m_historyR[1] = m_historyR[2]; m_historyR[2] = m_historyR[3];
            
            float frame[2] = {0.0f, 0.0f};
            ma_uint64 framesRead = 0;
            ma_data_source_read_pcm_frames(m_dataSource, frame, 1, &framesRead);
            
            if (framesRead == 0) {
                if (m_adsr.getState() != AdsrEnvelope::Release) m_adsr.release();
                m_historyL[3] = 0.0f;
                m_historyR[3] = 0.0f;
            } else {
                m_historyL[3] = frame[0];
                m_historyR[3] = frame[1];
            }
            m_readPointer -= 1.0;
        }
        
        sampleL = hermite(static_cast<float>(m_readPointer), m_historyL[0], m_historyL[1], m_historyL[2], m_historyL[3]);
        sampleR = hermite(static_cast<float>(m_readPointer), m_historyR[0], m_historyR[1], m_historyR[2], m_historyR[3]);
    }
    
    // Read Shared Atomic State with Smoothing (One-Pole Low-Pass)
    if (state && m_paramBlockIndex >= 0 && m_paramBlockIndex < GlobalAudioState::MAX_NODES) {
        float targetVol = state->nodeParams[m_paramBlockIndex].volume.load(std::memory_order_relaxed);
        // One-Pole Low Pass Filter for Smoothing
        // alpha approx 0.005 for a nice smooth transition at audio rate (fs=44100)
        m_smoothedVolume += 0.005f * (targetVol - m_smoothedVolume);
    }

    float baseVol = m_volume * m_smoothedVolume;

    // Apply volume with modulation
    float finalVol = baseVol * (1.0f + volMod);
    if (finalVol < 0.0f) finalVol = 0.0f;
    
    outL = sampleL * envAmp * finalVol;
    outR = sampleR * envAmp * finalVol;
    
    // Apply pan modulation
    if (std::abs(panMod) > 0.001f) {
        float panL = std::min(1.0f, 1.0f - panMod);
        float panR = std::min(1.0f, 1.0f + panMod);
        outL *= panL;
        outR *= panR;
    }
    
    // Apply SVF filter (replaces naive 1-pole)
    if (m_hasFilter) {
        m_svfL.setCutoffMod(cutoffMod);
        m_svfR.setCutoffMod(cutoffMod);
        
        // Apply resonance modulation
        if (std::abs(resMod) > 0.001f) {
            float modRes = m_filterResonance * (1.0f + resMod);
            if (modRes < 0.1f) modRes = 0.1f;
            if (modRes > 10.0f) modRes = 10.0f;
            m_svfL.setResonance(modRes);
            m_svfR.setResonance(modRes);
        }
        
        outL = m_svfL.process(outL);
        outR = m_svfR.process(outR);
    }
}

void VoiceProcessor::updateParameters(const AudioMessage& msg) {
    // Update ADSR
    m_adsr.setParameters(msg.attack, msg.decay, msg.sustain, msg.release, static_cast<float>(m_sampleRate));
    m_modAdsr.setParameters(msg.modAttack, msg.modDecay, msg.modSustain, msg.modRelease, static_cast<float>(m_sampleRate));
    
    // Update Routing
    m_numRoutings = msg.numRoutings;
    for (int i = 0; i < msg.numRoutings; ++i) {
        m_routings[i] = msg.routings[i];
    }
    
    // Update Filter
    m_hasFilter = msg.hasFilter;
    m_filterCutoff = msg.filterCutoff;
    m_filterResonance = msg.filterResonance;
    
    if (m_hasFilter) {
        StateVariableFilter::Type svfType = StateVariableFilter::Type::LowPass;
        switch (msg.filterType) {
            case 0: svfType = StateVariableFilter::Type::LowPass; break;
            case 1: svfType = StateVariableFilter::Type::HighPass; break;
            case 2: svfType = StateVariableFilter::Type::BandPass; break;
            case 3: svfType = StateVariableFilter::Type::Notch; break;
        }
        m_svfL.setType(svfType); m_svfR.setType(svfType);
        m_svfL.setCutoff(m_filterCutoff); m_svfR.setCutoff(m_filterCutoff);
        m_svfL.setResonance(m_filterResonance); m_svfR.setResonance(m_filterResonance);
    }
}
