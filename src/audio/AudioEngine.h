#pragma once
#include "thirdparty/miniaudio/miniaudio.h"
#include <concurrentqueue.h> 
#include <atomic>
#include <memory>
#include <vector>
#include <string>
#include <map>
#include <QUuid>
#include "AudioMessage.h"
#include "core/models/AudioNodes.h"
#include "core/models/GraphStructures.h"
#include "thirdparty/FFTConvolver/FFTConvolver.h"
#include "AudioState.h"
#include <thread>
#include <mutex>
#include <condition_variable>

#include "dsp/LfoOscillator.h"
#include "dsp/VoiceProcessor.h"
#include "Vst3Host.h"
#include "AudioEngineExport.h"

#include <QString>

struct AudioDeviceInfo {
    QString id;
    QString name;
    bool isDefault = false;
};

class AUDIOENGINE_EXPORT AudioEngine {
public:
    AudioEngine();
    ~AudioEngine();

    bool initialize(const QString& deviceId = QString(), int sampleRate = 44100, int bufferSize = 0);
    void start();
    void stop();

    void pushCommand(const AudioMessage& msg);
    
    void setMasterEffectsAsync(std::vector<Vst3Host*>* newEffects);
    
    GlobalAudioState* getAudioState() { return &m_audioState; }

    static std::vector<AudioDeviceInfo> enumerateDevices();
    bool reinitialize(const QString& deviceId, int sampleRate, int bufferSize);
    QString currentDeviceName() const { return m_currentDeviceName; }
    int currentSampleRate() const { return m_currentSampleRate; }

private:
    ma_device m_device;
    ma_resource_manager m_resourceManager;
    bool m_isInitialized = false;
    moodycamel::ConcurrentQueue<AudioMessage> m_queue;

    GlobalAudioState m_audioState;

    // Cache de Samples Removido! Agora usamos Disk Streaming via m_resourceManager.
    
    // Matriz de Vozes Polifônicas
    static const int MAX_VOICES = 32;
    VoiceProcessor m_voices[MAX_VOICES];

    // LFO Global
    
    LfoOscillator m_lfo1;
    LfoOscillator m_lfo2;
    


    static void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount);
    void processAudio(float* outputBuffer, ma_uint32 frameCount);

    std::atomic<std::vector<Vst3Host*>*> m_activeVstEffects{nullptr};

    QString m_currentDeviceName;
    int m_currentSampleRate = 44100;
};
