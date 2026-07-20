#include "AppSettings.h"
#include <QSettings>
#include <QCoreApplication>

AppSettings& AppSettings::instance() {
    static AppSettings s;
    return s;
}

// --- Audio ---

QString AppSettings::audioDeviceId() const {
    QSettings settings("HuilianMusic", "DecentSamplerEditor");
    return settings.value("audio/deviceId", QString()).toString();
}

void AppSettings::setAudioDeviceId(const QString& id) {
    QSettings settings("HuilianMusic", "DecentSamplerEditor");
    settings.setValue("audio/deviceId", id);
    settings.sync();
}

int AppSettings::sampleRate() const {
    QSettings settings("HuilianMusic", "DecentSamplerEditor");
    return settings.value("audio/sampleRate", 44100).toInt();
}

void AppSettings::setSampleRate(int rate) {
    QSettings settings("HuilianMusic", "DecentSamplerEditor");
    settings.setValue("audio/sampleRate", rate);
    settings.sync();
}

int AppSettings::bufferSize() const {
    QSettings settings("HuilianMusic", "DecentSamplerEditor");
    return settings.value("audio/bufferSize", 0).toInt();
}

void AppSettings::setBufferSize(int size) {
    QSettings settings("HuilianMusic", "DecentSamplerEditor");
    settings.setValue("audio/bufferSize", size);
    settings.sync();
}

// --- MIDI ---

int AppSettings::midiPortIndex() const {
    QSettings settings("HuilianMusic", "DecentSamplerEditor");
    return settings.value("midi/portIndex", 0).toInt();
}

void AppSettings::setMidiPortIndex(int index) {
    QSettings settings("HuilianMusic", "DecentSamplerEditor");
    settings.setValue("midi/portIndex", index);
    settings.sync();
}

// --- Theme ---

QString AppSettings::theme() const {
    QSettings settings("HuilianMusic", "DecentSamplerEditor");
    return settings.value("appearance/theme", "Dark").toString();
}

void AppSettings::setTheme(const QString& themeName) {
    QSettings settings("HuilianMusic", "DecentSamplerEditor");
    settings.setValue("appearance/theme", themeName);
    settings.sync();
}
