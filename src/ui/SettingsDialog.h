#pragma once
#include "components/NeumorphicDialog.h"
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QListWidget>
#include <QStackedWidget>
#include <QLabel>

class ApplicationController;

class SettingsDialog : public NeumorphicDialog {
    Q_OBJECT
public:
    explicit SettingsDialog(ApplicationController* controller, QWidget* parent = nullptr);

private slots:
    void onApply();
    void onCancel();
    void onRefreshMidi();

private:
    ApplicationController* m_controller;

    QListWidget* m_categoryList;
    QStackedWidget* m_pages;

    // Audio page
    QComboBox* m_audioDeviceCombo;
    QComboBox* m_sampleRateCombo;
    QComboBox* m_bufferSizeCombo;
    QLabel* m_audioStatusLabel;

    // MIDI page
    QComboBox* m_midiPortCombo;
    QLabel* m_midiStatusLabel;

    // Theme page
    QComboBox* m_themeCombo;
    QLabel* m_themeStatusLabel;

    // Buttons
    QPushButton* m_applyBtn;
    QPushButton* m_cancelBtn;

    void setupAudioPage();
    void setupMidiPage();
    void setupThemePage();
    void populateAudioDevices();
    void populateMidiPorts();
    void loadCurrentSettings();
};
