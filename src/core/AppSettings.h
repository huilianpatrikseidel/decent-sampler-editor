#pragma once
#include <QString>
#include "CoreLibExport.h"

class CORELIB_EXPORT AppSettings {
public:
    static AppSettings& instance();

    // Audio
    QString audioDeviceId() const;
    void setAudioDeviceId(const QString& id);
    
    int sampleRate() const;
    void setSampleRate(int rate);
    
    int bufferSize() const;
    void setBufferSize(int size);

    // MIDI
    int midiPortIndex() const;
    void setMidiPortIndex(int index);

    // Theme
    QString theme() const;
    void setTheme(const QString& themeName);

private:
    AppSettings() = default;
    AppSettings(const AppSettings&) = delete;
    AppSettings& operator=(const AppSettings&) = delete;
};
