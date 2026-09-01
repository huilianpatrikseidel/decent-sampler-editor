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
        // Dispose voice sources (and any queued ones) while the resource manager is
        // still alive — data-source uninit references it.
        releaseAllVoiceSources();
        ma_device_uninit(&m_device);
        ma_resource_manager_uninit(&m_resourceManager);
    }

    auto fx = m_activeVstEffects.exchange(nullptr);
    if (fx) {
        delete fx;
    }

    auto graph = m_activeMixGraph.exchange(nullptr);
    if (graph) {
        delete graph;
    }

}

void AudioEngine::pushCommand(const AudioMessage& msg) {
    m_queue.enqueue(msg);
}

void* AudioEngine::prepareSampleSource(const char* filepath) {
    // Runs on the note-trigger thread (MIDI/UI), never the audio callback.
    drainFreeQueue(); // opportunistically reclaim sources finished by the audio thread
    if (!m_isInitialized || !filepath || filepath[0] == '\0') {
        return nullptr;
    }

    auto* src = new ma_resource_manager_data_source();
    ma_result res = ma_resource_manager_data_source_init(
        &m_resourceManager,
        filepath,
        MA_RESOURCE_MANAGER_DATA_SOURCE_FLAG_STREAM | MA_RESOURCE_MANAGER_DATA_SOURCE_FLAG_ASYNC,
        NULL,
        src);
    if (res != MA_SUCCESS) {
        delete src;
        return nullptr;
    }
    return src;
}

void AudioEngine::disposePreparedSource(void* handle) {
    if (handle) {
        m_freeQueue.enqueue(static_cast<ma_resource_manager_data_source*>(handle));
    }
    drainFreeQueue();
}

void AudioEngine::drainFreeQueue() {
    ma_resource_manager_data_source* src = nullptr;
    while (m_freeQueue.try_dequeue(src)) {
        if (src) {
            ma_resource_manager_data_source_uninit(src);
            delete src;
        }
    }
}

void AudioEngine::releaseAllVoiceSources() {
    // Caller guarantees the device is stopped (no concurrent audio callback).
    for (int i = 0; i < MAX_VOICES; ++i) {
        m_voices[i].releaseSource();
    }
    drainFreeQueue();
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




void AudioEngine::setMixGraphAsync(MixGraph* newGraph) {
    std::thread([this, newGraph]() {
        auto oldGraph = m_activeMixGraph.exchange(newGraph, std::memory_order_acq_rel);
        if (oldGraph) {
            std::this_thread::sleep_for(std::chrono::milliseconds(50)); // let the callback finish
            // VstPluginManager owns the hosts; only the graph itself is ours to delete.
            delete oldGraph;
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

    // Propagate the actual device rate to every DSP stage. Without this the
    // envelopes, filters, oscillators and LFOs run as if the device were 44.1 kHz,
    // producing wrong timing/frequency at any other rate (e.g. 48 kHz).
    const double deviceRate = static_cast<double>(m_currentSampleRate);
    for (int i = 0; i < MAX_VOICES; ++i) {
        m_voices[i].setSampleRate(deviceRate);
    }
    m_lfo1.setSampleRate(static_cast<float>(deviceRate));
    m_lfo2.setSampleRate(static_cast<float>(deviceRate));

    if (contextInitialized) ma_context_uninit(&context);

    // Pre-allocate DSP buffers for audio callback
    size_t maxFrames = (bufferSize > 0) ? std::max(bufferSize, 8192) : 8192;
    m_dryL.resize(maxFrames);
    m_dryR.resize(maxFrames);
    m_vstOutL.resize(maxFrames);
    m_vstOutR.resize(maxFrames);

    // Allocated up front: the callback may not allocate, so every channel that could
    // ever exist has its buffer ready before audio starts.
    m_chanL.assign(MAX_CHANNELS, std::vector<float>(maxFrames, 0.0f));
    m_chanR.assign(MAX_CHANNELS, std::vector<float>(maxFrames, 0.0f));

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
        // Voice sources reference the current resource manager — dispose them before
        // it is torn down, or their pointers dangle across the reinit.
        releaseAllVoiceSources();
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
            // Source was opened off the audio thread; here we only adopt pointers and
            // hand finished ones to the free queue — no file I/O, alloc or free.
            auto* prepared = static_cast<ma_resource_manager_data_source*>(msg.preparedSource);
            bool preparedAdopted = false;
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
                        ma_resource_manager_data_source* old = nullptr;
                        if (msg.isOscillator) {
                            // Oscillator voice: drop any stale sample source the slot held.
                            old = m_voices[i].adopt(nullptr);
                            m_voices[i].trigger(msg, msg.note);
                        } else if (prepared) {
                            old = m_voices[i].adopt(prepared);
                            preparedAdopted = true;
                            m_voices[i].trigger(msg);
                        }
                        if (old) m_freeQueue.enqueue(old);
                        break;
                    }
                }
            }

            // Prepared source no voice took (legato handled, pool full): dispose off-thread.
            if (prepared && !preparedAdopted) {
                m_freeQueue.enqueue(prepared);
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
        } else if (msg.type == AudioCommandType::SetParameter || msg.type == AudioCommandType::RebuildGraph) {
            for (int i = 0; i < MAX_VOICES; ++i) {
                if (m_voices[i].isActive()) {
                    m_voices[i].updateParameters(msg);
                }
            }
        }
    }

    // Fallback for a host asking for a larger buffer than we sized for. This allocates on
    // the audio thread, which the rules forbid -- but the alternative is indexing past the
    // end of a channel buffer, which is a crash. It has never been observed to fire; see
    // docs/04-planning/technical-debt.md.
    if (frameCount > m_dryL.size()) {
        m_dryL.resize(frameCount);
        m_dryR.resize(frameCount);
        m_vstOutL.resize(frameCount);
        m_vstOutR.resize(frameCount);
        for (auto& buffer : m_chanL) buffer.resize(frameCount);
        for (auto& buffer : m_chanR) buffer.resize(frameCount);
    }

    // Latest MIDI controller positions, sampled once per buffer. The per-frame one-pole
    // below turns their 7-bit steps into a continuous ramp.
    const float modWheelTarget = m_audioState.modWheel.load(std::memory_order_relaxed);
    const float pitchBendTarget = m_audioState.pitchBend.load(std::memory_order_relaxed);
    const float aftertouchTarget = m_audioState.aftertouch.load(std::memory_order_relaxed);
    constexpr float kCtrlSmoothing = 0.005f; // same coefficient as the per-voice volume ramp

    // The graph is published whole and swapped atomically; a null one means every voice
    // goes straight to master, which is what happens before any project is loaded.
    MixGraph* graph = m_activeMixGraph.load(std::memory_order_acquire);
    const int channelCount = graph
        ? std::min(static_cast<int>(graph->channels.size()), MAX_CHANNELS)
        : 0;

    for (int c = 0; c < channelCount; ++c) {
        std::fill(m_chanL[c].begin(), m_chanL[c].begin() + frameCount, 0.0f);
        std::fill(m_chanR[c].begin(), m_chanR[c].begin() + frameCount, 0.0f);
    }

    ModInputs mods;

    for (ma_uint32 frame = 0; frame < frameCount; ++frame) {
        m_smoothedModWheel += kCtrlSmoothing * (modWheelTarget - m_smoothedModWheel);
        m_smoothedPitchBend += kCtrlSmoothing * (pitchBendTarget - m_smoothedPitchBend);
        m_smoothedAftertouch += kCtrlSmoothing * (aftertouchTarget - m_smoothedAftertouch);

        mods.lfo1 = m_lfo1.process();
        mods.lfo2 = m_lfo2.process();
        mods.modWheel = m_smoothedModWheel;
        mods.pitchBend = m_smoothedPitchBend;
        mods.aftertouch = m_smoothedAftertouch;

        float masterL = 0.0f;
        float masterR = 0.0f;

        for (int i = 0; i < MAX_VOICES; ++i) {
            if (!m_voices[i].isActive()) continue;

            float vL = 0.0f, vR = 0.0f;
            m_voices[i].process(mods, vL, vR, &m_audioState);

            // A voice whose channel was dropped (or never assigned) still has to be
            // heard, so it lands on master rather than being discarded.
            const int channel = m_voices[i].getChannelIndex();
            if (channel >= 0 && channel < channelCount) {
                m_chanL[channel][frame] += vL;
                m_chanR[channel][frame] += vR;
            } else {
                masterL += vL;
                masterR += vR;
            }
        }

        m_dryL[frame] = masterL;
        m_dryR[frame] = masterR;
    }

    // Channels are ordered so a channel always precedes the one it feeds, which lets this
    // run front to back: apply the channel's own inserts, then sum it into its destination.
    for (int c = 0; c < channelCount; ++c) {
        const MixChannel& channel = graph->channels[c];
        float* chL = m_chanL[c].data();
        float* chR = m_chanR[c].data();

        if (!channel.effects.empty()) {
            float* inputs[2] = { chL, chR };
            float* outputs[2] = { m_vstOutL.data(), m_vstOutR.data() };
            for (Vst3Host* host : channel.effects) {
                if (!host) continue;
                host->processAudio(inputs, 2, outputs, 2, frameCount);
                std::copy(outputs[0], outputs[0] + frameCount, inputs[0]);
                std::copy(outputs[1], outputs[1] + frameCount, inputs[1]);
            }
        }

        const float gain = channel.gain;
        const int dest = channel.destination;
        float* dstL = (dest >= 0 && dest < channelCount) ? m_chanL[dest].data() : m_dryL.data();
        float* dstR = (dest >= 0 && dest < channelCount) ? m_chanR[dest].data() : m_dryR.data();

        for (ma_uint32 frame = 0; frame < frameCount; ++frame) {
            dstL[frame] += chL[frame] * gain;
            dstR[frame] += chR[frame] * gain;
        }
    }

    const float masterVol = m_audioState.masterVolume.load(std::memory_order_relaxed);
    for (ma_uint32 frame = 0; frame < frameCount; ++frame) {
        m_dryL[frame] *= masterVol;
        m_dryR[frame] *= masterVol;
    }

    // Master chain: prefer the graph's list, falling back to the standalone one so an
    // engine driven without a graph still applies master effects.
    const std::vector<Vst3Host*>* masterFx = nullptr;
    if (graph && !graph->masterEffects.empty()) {
        masterFx = &graph->masterEffects;
    } else if (auto* legacy = m_activeVstEffects.load(std::memory_order_acquire)) {
        if (!legacy->empty()) masterFx = legacy;
    }

    float* finalL = m_dryL.data();
    float* finalR = m_dryR.data();
    if (masterFx) {
        float* inputs[2] = { finalL, finalR };
        float* outputs[2] = { m_vstOutL.data(), m_vstOutR.data() };
        for (Vst3Host* host : *masterFx) {
            if (!host) continue;
            host->processAudio(inputs, 2, outputs, 2, frameCount);
            std::copy(outputs[0], outputs[0] + frameCount, inputs[0]);
            std::copy(outputs[1], outputs[1] + frameCount, inputs[1]);
        }
    }

    for (ma_uint32 frame = 0; frame < frameCount; ++frame) {
        float outL = finalL[frame];
        float outR = finalR[frame];
        if (outL > 1.0f) outL = 1.0f;
        if (outL < -1.0f) outL = -1.0f;
        if (outR > 1.0f) outR = 1.0f;
        if (outR < -1.0f) outR = -1.0f;
        outputBuffer[frame * 2]     = outL;
        outputBuffer[frame * 2 + 1] = outR;
    }
}
