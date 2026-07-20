#pragma once
#include <RtMidi.h>
#include <memory>
#include <functional>
#include <QObject>
#include <QString>
#include <vector>
#include <atomic>
#include "AudioEngineExport.h"

struct MidiDeviceConfig {
    int portIndex = 0;
};

class AUDIOENGINE_EXPORT MidiEngine {
public:
    MidiEngine();
    ~MidiEngine();
    
    bool initialize(int portIndex = 0);
    void stop();
    void setCallback(std::function<void(unsigned char, unsigned char, unsigned char)> callback);

    std::vector<QString> enumeratePorts() const;
    bool switchPort(int portIndex);
    int currentPort() const { return m_currentPort; }
    QString currentPortName() const;

private:
    std::unique_ptr<RtMidiIn> m_midiIn;
    // Ponteiro atômico evita race condition entre UI Thread (setCallback) e MIDI Thread
    std::atomic<std::function<void(unsigned char, unsigned char, unsigned char)>*> m_callback{nullptr};
    int m_currentPort = -1;
    
    static void midiCallback(double timeStamp, std::vector<unsigned char> *message, void *userData);
};
