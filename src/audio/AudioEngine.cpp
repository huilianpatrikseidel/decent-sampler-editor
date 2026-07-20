#include "AudioEngine.h"
#include <cmath>
#include <algorithm>
#include <cstring>

// miniaudio implementation is already compiled in WaveformCache.cpp, so we don't define MINIAUDIO_IMPLEMENTATION here.

AudioEngine::AudioEngine() {
}

AudioEngine::~AudioEngine() {
    stop();
    if (m_isInitialized) {
        ma_device_uninit(&m_device);
        ma_resource_manager_uninit(&m_resourceManager);
    }

    auto fx = m_activeVstEffects.exchange(nullptr);
    if (fx) {
        delete fx;
    }

}

void AudioEngine::pushCommand(const AudioMessage& msg) {
    m_queue.enqueue(msg);
}

void AudioEngine::setMasterEffectsAsync(std::vector<Vst3Host*>* newEffects) {
    std::thread([this, newEffects]() {
        auto oldEffects = m_activeVstEffects.exchange(newEffects, std::memory_order_acq_rel);
        if (oldEffects) {
            std::this_thread::sleep_for(std::chrono::milliseconds(50)); // Wait for audio callback to finish
            // ApplicationController holds the shared_ptr, we only delete the vector, not the contents
            delete oldEffects;
        }
    }).detach();
}




void AudioEngine::data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount) {
    AudioEngine* engine = static_cast<AudioEngine*>(pDevice->pUserData);
    engine->processAudio(static_cast<float*>(pOutput), frameCount);
}

bool AudioEngine::initialize(const QString& deviceId, int sampleRate, int bufferSize) {

    // 1. Initialize Resource Manager (Disk Streaming)
    ma_resource_manager_config rmConfig = ma_resource_manager_config_init();
    rmConfig.decodedFormat     = ma_format_f32;
    rmConfig.decodedChannels   = 2;
    rmConfig.decodedSampleRate = sampleRate;
    
    if (ma_resource_manager_init(&rmConfig, &m_resourceManager) != MA_SUCCESS) {
        return false;
    }

    // 2. Initialize Device
    ma_device_config config = ma_device_config_init(ma_device_type_playback);
    config.playback.format   = ma_format_f32;
    config.playback.channels = 2;
    config.sampleRate        = sampleRate;
    config.dataCallback      = data_callback;
    config.pUserData         = this;
    if (bufferSize > 0) {
        config.periodSizeInFrames = bufferSize;
    }

    // Device selection
    ma_context context;
    ma_device_id* pSelectedDeviceId = nullptr;
    ma_device_id selectedDeviceId;
    bool contextInitialized = false;

    if (!deviceId.isEmpty()) {
        if (ma_context_init(NULL, 0, NULL, &context) == MA_SUCCESS) {
            contextInitialized = true;
            ma_device_info* pPlaybackInfos;
            ma_uint32 playbackCount;
            ma_device_info* pCaptureInfos;
            ma_uint32 captureCount;
            if (ma_context_get_devices(&context, &pPlaybackInfos, &playbackCount, &pCaptureInfos, &captureCount) == MA_SUCCESS) {
                for (ma_uint32 i = 0; i < playbackCount; ++i) {
                    if (QString::fromUtf8(pPlaybackInfos[i].name) == deviceId) {
                        selectedDeviceId = pPlaybackInfos[i].id;
                        pSelectedDeviceId = &selectedDeviceId;
                        break;
                    }
                }
            }
        }
    }

    config.playback.pDeviceID = pSelectedDeviceId;

    if (ma_device_init(NULL, &config, &m_device) != MA_SUCCESS) {
        if (contextInitialized) ma_context_uninit(&context);
        ma_resource_manager_uninit(&m_resourceManager);
        return false;
    }

    m_currentDeviceName = QString::fromUtf8(m_device.playback.name);
    m_currentSampleRate = m_device.sampleRate;

    if (contextInitialized) ma_context_uninit(&context);

    m_isInitialized = true;
    return true;
}

std::vector<AudioDeviceInfo> AudioEngine::enumerateDevices() {
    std::vector<AudioDeviceInfo> result;
    ma_context context;
    if (ma_context_init(NULL, 0, NULL, &context) != MA_SUCCESS) {
        return result;
    }

    ma_device_info* pPlaybackInfos;
    ma_uint32 playbackCount;
    ma_device_info* pCaptureInfos;
    ma_uint32 captureCount;

    if (ma_context_get_devices(&context, &pPlaybackInfos, &playbackCount, &pCaptureInfos, &captureCount) == MA_SUCCESS) {
        for (ma_uint32 i = 0; i < playbackCount; ++i) {
            AudioDeviceInfo info;
            info.name = QString::fromUtf8(pPlaybackInfos[i].name);
            info.id = info.name; // Use name as ID for simplicity
            info.isDefault = pPlaybackInfos[i].isDefault;
            result.push_back(info);
        }
    }

    ma_context_uninit(&context);
    return result;
}

bool AudioEngine::reinitialize(const QString& deviceId, int sampleRate, int bufferSize) {
    // Save current state
    QString oldDeviceName = m_currentDeviceName;
    int oldSampleRate = m_currentSampleRate;

    // Stop and uninit current device
    stop();
    if (m_isInitialized) {
        ma_device_uninit(&m_device);
        ma_resource_manager_uninit(&m_resourceManager);
        m_isInitialized = false;
    }

    // Try to initialize with new settings
    if (!initialize(deviceId, sampleRate, bufferSize)) {
        // Rollback: restore old settings and try to reinitialize
        initialize(oldDeviceName, oldSampleRate, 0);
        start();
        return false;
    }

    start();
    return true;
}

void AudioEngine::start() {
    if (m_isInitialized) {
        ma_device_start(&m_device);
    }
}

void AudioEngine::stop() {
    if (m_isInitialized) {
        ma_device_stop(&m_device);
    }
}

void AudioEngine::processAudio(float* outputBuffer, ma_uint32 frameCount) {
    // 1. Processar Fila de Comandos (Lock-Free)
    AudioMessage msg;
    while (m_queue.try_dequeue(msg)) {
        if (msg.type == AudioCommandType::PlayNote) {
            bool handledLegato = false;
            
            if (msg.isLegato) {
                // Find if there is an active voice for this group
                for (int i = 0; i < MAX_VOICES; ++i) {
                    if (m_voices[i].isActive() && !m_voices[i].isReleasing() && m_voices[i].getGroupId() == msg.groupId) {
                        m_voices[i].setTargetNote(msg.note, msg.glideTime);
                        handledLegato = true;
                        break;
                    }
                }
            }
            
            if (!handledLegato) {
                for (int i = 0; i < MAX_VOICES; ++i) {
                    if (!m_voices[i].isActive() || m_voices[i].isReleasing()) {
                        if (msg.isOscillator) {
                            // Oscillator voice: no sample to load
                            m_voices[i].trigger(msg, msg.note);
                        } else if (m_voices[i].init(&m_resourceManager, msg.sampleId.c_str())) {
                            m_voices[i].trigger(msg);
                        }
                        break;
                    }
                }
            }
        } else if (msg.type == AudioCommandType::StopNote) {
            for (int i = 0; i < MAX_VOICES; ++i) {
                if (m_voices[i].isActive() && m_voices[i].getPlayingNote() == msg.note && !m_voices[i].isReleasing()) {
                    m_voices[i].release();
                }
            }
        } else if (msg.type == AudioCommandType::SetMasterVolume) {
            m_audioState.masterVolume.store(msg.value, std::memory_order_relaxed);
        } else if (msg.type == AudioCommandType::SetLFO) {
            LfoOscillator* targetLfo = (msg.lfoIndex == 1) ? &m_lfo1 : &m_lfo2;
            targetLfo->setShape(static_cast<LfoShape>(msg.lfoShape));
            targetLfo->setFrequency(msg.lfoFreq);
            targetLfo->setAmount(msg.lfoAmount);
        }
    }

    float masterVol = m_audioState.masterVolume.load(std::memory_order_relaxed);

    for (ma_uint32 i = 0; i < frameCount * 2; ++i) {
        outputBuffer[i] = 0.0f;
    }

    std::vector<float> dryL(frameCount, 0.0f);
    std::vector<float> dryR(frameCount, 0.0f);

    for (int frame = 0; frame < frameCount; ++frame) {
        float mixL = 0.0f;
        float mixR = 0.0f;
        
        float lfo1Val = m_lfo1.process();
        float lfo2Val = m_lfo2.process();
        
        for (int i = 0; i < MAX_VOICES; ++i) {
            if (m_voices[i].isActive()) {
                float vL = 0.0f, vR = 0.0f;
                m_voices[i].process(lfo1Val, lfo2Val, vL, vR, &m_audioState);
                mixL += vL;
                mixR += vR;
            }
        }
        
        dryL[frame] = mixL * masterVol;
        dryR[frame] = mixR * masterVol;
    }
    
    // Hard clipping before writing to output
    for (int frame = 0; frame < frameCount; ++frame) {
        float outL = dryL[frame];
        float outR = dryR[frame];
        if (outL > 1.0f) outL = 1.0f;
        if (outL < -1.0f) outL = -1.0f;
        if (outR > 1.0f) outR = 1.0f;
        if (outR < -1.0f) outR = -1.0f;
        
        outputBuffer[frame * 2]     = outL;
        outputBuffer[frame * 2 + 1] = outR;
    }

    // Now apply VST3 host processing over the final interleaved output buffer
    std::vector<Vst3Host*>* activeFx = m_activeVstEffects.load(std::memory_order_acquire);
    if (activeFx && !activeFx->empty()) {
        std::vector<float> vstOutL(frameCount);
        std::vector<float> vstOutR(frameCount);
        
        float* inputs[2] = { dryL.data(), dryR.data() };
        float* outputs[2] = { vstOutL.data(), vstOutR.data() };
        
        for (Vst3Host* host : *activeFx) {
            if (host) {
                host->processAudio(inputs, 2, outputs, 2, frameCount);
                // Copy outputs back to inputs for the next effect in the chain
                std::copy(outputs[0], outputs[0] + frameCount, inputs[0]);
                std::copy(outputs[1], outputs[1] + frameCount, inputs[1]);
            }
        }
        
        // Interleave back from the final inputs (which now contain the final output of the chain)
        for (int frame = 0; frame < frameCount; ++frame) {
            float outL = inputs[0][frame];
            float outR = inputs[1][frame];
            if (outL > 1.0f) outL = 1.0f;
            if (outL < -1.0f) outL = -1.0f;
            if (outR > 1.0f) outR = 1.0f;
            if (outR < -1.0f) outR = -1.0f;
            outputBuffer[frame * 2]     = outL;
            outputBuffer[frame * 2 + 1] = outR;
        }
    }
}
