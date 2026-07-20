#include "SettingsDialog.h"
#include "core/ApplicationController.h"
#include "core/ThemeManager.h"
#include "core/AppSettings.h"
#include "audio/AudioEngine.h"
#include "audio/MidiEngine.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QFileDialog>
#include <QLabel>
#include <QFrame>

QWidget* createSeparator(QWidget* parent) {
    QFrame* sep = new QFrame(parent);
    sep->setFrameShape(QFrame::HLine);
    sep->setFixedHeight(1);
    sep->setObjectName("SettingsSeparator");
    return sep;
}

// ─── Constructor ────────────────────────────────────────────────
SettingsDialog::SettingsDialog(ApplicationController* controller, QWidget* parent)
    : NeumorphicDialog(parent), m_controller(controller)
{
    setDialogTitle("Configurações");
    setFixedSize(700, 460);

    // Main horizontal layout: sidebar | content
    QHBoxLayout* mainH = new QHBoxLayout();
    mainH->setContentsMargins(0, 0, 0, 0);
    mainH->setSpacing(0);

    // ── Sidebar ──
    m_categoryList = new QListWidget();
    m_categoryList->setFixedWidth(140);
    m_categoryList->setObjectName("SettingsCategoryList");
    m_categoryList->setFocusPolicy(Qt::NoFocus);
    m_categoryList->setIconSize(QSize(16, 16));

    QListWidgetItem* audioItem = new QListWidgetItem("  Áudio");
    QListWidgetItem* midiItem  = new QListWidgetItem("  MIDI");
    QListWidgetItem* pathsItem = new QListWidgetItem("  Aparência");
    m_categoryList->addItem(audioItem);
    m_categoryList->addItem(midiItem);
    m_categoryList->addItem(pathsItem);
    m_categoryList->setCurrentRow(0);

    // ── Vertical separator ──
    QFrame* vSep = new QFrame();
    vSep->setFrameShape(QFrame::VLine);
    vSep->setFixedWidth(1);
    vSep->setObjectName("SettingsSeparator");

    // ── Stacked pages ──
    m_pages = new QStackedWidget();

    setupAudioPage();
    setupMidiPage();
    setupThemePage();

    connect(m_categoryList, &QListWidget::currentRowChanged, m_pages, &QStackedWidget::setCurrentIndex);

    mainH->addWidget(m_categoryList);
    mainH->addWidget(vSep);
    mainH->addWidget(m_pages, 1);

    contentLayout()->addLayout(mainH, 1);

    // ── Bottom separator + button bar ──
    contentLayout()->addWidget(createSeparator(this));

    QHBoxLayout* btnRow = new QHBoxLayout();
    btnRow->setContentsMargins(0, 8, 0, 0);
    btnRow->addStretch();

    m_cancelBtn = new QPushButton("Cancelar");
    m_cancelBtn->setFixedHeight(32);
    m_cancelBtn->setMinimumWidth(90);
    m_cancelBtn->setObjectName("SettingsSmallBtn");
    connect(m_cancelBtn, &QPushButton::clicked, this, &SettingsDialog::onCancel);
    btnRow->addWidget(m_cancelBtn);

    m_applyBtn = new QPushButton("Aplicar");
    m_applyBtn->setFixedHeight(32);
    m_applyBtn->setMinimumWidth(90);
    m_applyBtn->setObjectName("SettingsApplyBtn");
    connect(m_applyBtn, &QPushButton::clicked, this, &SettingsDialog::onApply);
    btnRow->addWidget(m_applyBtn);

    contentLayout()->addLayout(btnRow);

    // ── Load current values ──
    populateAudioDevices();
    populateMidiPorts();
    loadCurrentSettings();
}

// ─── Audio Page ─────────────────────────────────────────────────
void SettingsDialog::setupAudioPage() {
    QWidget* page = new QWidget();
    page->setStyleSheet("background: transparent;");
    QVBoxLayout* layout = new QVBoxLayout(page);
    layout->setContentsMargins(20, 12, 12, 12);
    layout->setSpacing(14);

    QLabel* title = new QLabel("Dispositivo de Áudio");
    title->setObjectName("SettingsSectionTitle");
    layout->addWidget(title);

    QFormLayout* form = new QFormLayout();
    form->setSpacing(12);
    form->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);

    // Output Device
    QLabel* devLabel = new QLabel("Output Device:");
    devLabel->setObjectName("SettingsFormLabel");
    m_audioDeviceCombo = new QComboBox();
    m_audioDeviceCombo->setObjectName("SettingsCombo");
    form->addRow(devLabel, m_audioDeviceCombo);

    // Sample Rate
    QLabel* srLabel = new QLabel("Sample Rate:");
    srLabel->setObjectName("SettingsFormLabel");
    m_sampleRateCombo = new QComboBox();
    m_sampleRateCombo->setObjectName("SettingsCombo");
    m_sampleRateCombo->addItem("22050 Hz", 22050);
    m_sampleRateCombo->addItem("44100 Hz", 44100);
    m_sampleRateCombo->addItem("48000 Hz", 48000);
    m_sampleRateCombo->addItem("96000 Hz", 96000);
    form->addRow(srLabel, m_sampleRateCombo);

    // Buffer Size
    QLabel* bsLabel = new QLabel("Buffer Size:");
    bsLabel->setObjectName("SettingsFormLabel");
    m_bufferSizeCombo = new QComboBox();
    m_bufferSizeCombo->setObjectName("SettingsCombo");
    m_bufferSizeCombo->addItem("Auto", 0);
    m_bufferSizeCombo->addItem("64 samples", 64);
    m_bufferSizeCombo->addItem("128 samples", 128);
    m_bufferSizeCombo->addItem("256 samples", 256);
    m_bufferSizeCombo->addItem("512 samples", 512);
    m_bufferSizeCombo->addItem("1024 samples", 1024);
    m_bufferSizeCombo->addItem("2048 samples", 2048);
    form->addRow(bsLabel, m_bufferSizeCombo);

    layout->addLayout(form);

    // Status label
    m_audioStatusLabel = new QLabel("");
    m_audioStatusLabel->setObjectName("SettingsStatusLabel");
    m_audioStatusLabel->setWordWrap(true);
    layout->addWidget(m_audioStatusLabel);

    layout->addStretch();

    m_pages->addWidget(page);
}

// ─── MIDI Page ──────────────────────────────────────────────────
void SettingsDialog::setupMidiPage() {
    QWidget* page = new QWidget();
    page->setStyleSheet("background: transparent;");
    QVBoxLayout* layout = new QVBoxLayout(page);
    layout->setContentsMargins(20, 12, 12, 12);
    layout->setSpacing(14);

    QLabel* title = new QLabel("Controlador MIDI");
    title->setObjectName("SettingsSectionTitle");
    layout->addWidget(title);

    QFormLayout* form = new QFormLayout();
    form->setSpacing(12);
    form->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);

    // MIDI Port
    QLabel* portLabel = new QLabel("Input Device:");
    portLabel->setObjectName("SettingsFormLabel");

    QHBoxLayout* midiRow = new QHBoxLayout();
    m_midiPortCombo = new QComboBox();
    m_midiPortCombo->setObjectName("SettingsCombo");
    midiRow->addWidget(m_midiPortCombo, 1);

    QPushButton* refreshBtn = new QPushButton("↻");
    refreshBtn->setFixedSize(32, 32);
    refreshBtn->setToolTip("Refresh MIDI devices");
    refreshBtn->setObjectName("SettingsSmallBtn");
    connect(refreshBtn, &QPushButton::clicked, this, &SettingsDialog::onRefreshMidi);
    midiRow->addWidget(refreshBtn);

    form->addRow(portLabel, midiRow);

    layout->addLayout(form);

    // Status label
    m_midiStatusLabel = new QLabel("");
    m_midiStatusLabel->setObjectName("SettingsStatusLabel");
    m_midiStatusLabel->setWordWrap(true);
    layout->addWidget(m_midiStatusLabel);

    layout->addStretch();

    m_pages->addWidget(page);
}

// ─── Theme Page ─────────────────────────────────────────────────
void SettingsDialog::setupThemePage() {
    QWidget* page = new QWidget();
    page->setStyleSheet("background: transparent;");
    QVBoxLayout* layout = new QVBoxLayout(page);
    layout->setContentsMargins(20, 12, 12, 12);
    layout->setSpacing(14);

    QLabel* title = new QLabel("Aparência");
    title->setObjectName("SettingsSectionTitle");
    layout->addWidget(title);

    QFormLayout* form = new QFormLayout();
    form->setSpacing(12);
    form->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);

    // Theme
    QLabel* themeLabel = new QLabel("Tema:");
    themeLabel->setObjectName("SettingsFormLabel");

    m_themeCombo = new QComboBox();
    m_themeCombo->setObjectName("SettingsCombo");
    m_themeCombo->addItem("Dark", "Dark");
    m_themeCombo->addItem("Light", "Light");

    form->addRow(themeLabel, m_themeCombo);

    layout->addLayout(form);

    m_themeStatusLabel = new QLabel("");
    m_themeStatusLabel->setObjectName("SettingsStatusLabel");
    m_themeStatusLabel->setWordWrap(true);
    layout->addWidget(m_themeStatusLabel);

    layout->addStretch();

    m_pages->addWidget(page);
}

// ─── Data population ────────────────────────────────────────────
void SettingsDialog::populateAudioDevices() {
    m_audioDeviceCombo->clear();
    m_audioDeviceCombo->addItem("Default (Sistema)", QString(""));

    auto devices = AudioEngine::enumerateDevices();
    for (const auto& dev : devices) {
        QString label = dev.name;
        if (dev.isDefault) label += "  ★";
        m_audioDeviceCombo->addItem(label, dev.id);
    }
}

void SettingsDialog::populateMidiPorts() {
    m_midiPortCombo->clear();

    auto ports = m_controller->getMidiEngine()->enumeratePorts();
    if (ports.empty()) {
        m_midiPortCombo->addItem("Nenhum dispositivo MIDI encontrado", -1);
        m_midiPortCombo->setEnabled(false);
        m_midiStatusLabel->setText("Nenhuma porta MIDI detectada. Conecte um controlador e clique ↻.");
    } else {
        m_midiPortCombo->setEnabled(true);
        for (int i = 0; i < static_cast<int>(ports.size()); ++i) {
            m_midiPortCombo->addItem(ports[i], i);
        }
        m_midiStatusLabel->setText("");
    }
}

void SettingsDialog::loadCurrentSettings() {
    AppSettings& settings = AppSettings::instance();

    // Audio device
    QString savedDevice = settings.audioDeviceId();
    if (savedDevice.isEmpty()) {
        m_audioDeviceCombo->setCurrentIndex(0); // "Default"
    } else {
        int idx = m_audioDeviceCombo->findData(savedDevice);
        if (idx >= 0) m_audioDeviceCombo->setCurrentIndex(idx);
        else m_audioDeviceCombo->setCurrentIndex(0);
    }

    // Sample rate
    int savedRate = settings.sampleRate();
    int rateIdx = m_sampleRateCombo->findData(savedRate);
    if (rateIdx >= 0) m_sampleRateCombo->setCurrentIndex(rateIdx);

    // Buffer size
    int savedBuf = settings.bufferSize();
    int bufIdx = m_bufferSizeCombo->findData(savedBuf);
    if (bufIdx >= 0) m_bufferSizeCombo->setCurrentIndex(bufIdx);

    // MIDI port
    int savedPort = m_controller->getMidiEngine()->currentPort();
    if (savedPort >= 0 && savedPort < m_midiPortCombo->count()) {
        m_midiPortCombo->setCurrentIndex(savedPort);
    }

    // Active info
    AudioEngine* audio = m_controller->getAudioEngine();
    QString deviceInfo = audio->currentDeviceName();
    if (!deviceInfo.isEmpty()) {
        m_audioStatusLabel->setText(
            QString("Ativo: %1  •  %2 Hz").arg(deviceInfo).arg(audio->currentSampleRate()));
    }

    MidiEngine* midi = m_controller->getMidiEngine();
    QString midiInfo = midi->currentPortName();
    if (!midiInfo.isEmpty() && m_midiStatusLabel->text().isEmpty()) {
        m_midiStatusLabel->setText(QString("Ativo: %1").arg(midiInfo));
    }

    // Theme
    QString savedTheme = settings.theme();
    int themeIdx = m_themeCombo->findData(savedTheme);
    if (themeIdx >= 0) m_themeCombo->setCurrentIndex(themeIdx);
}

// ─── Slots ──────────────────────────────────────────────────────
void SettingsDialog::onApply() {
    AppSettings& settings = AppSettings::instance();

    // ── Audio ──
    QString newDeviceId = m_audioDeviceCombo->currentData().toString();
    int newSampleRate = m_sampleRateCombo->currentData().toInt();
    int newBufferSize = m_bufferSizeCombo->currentData().toInt();

    bool audioChanged = (newDeviceId != settings.audioDeviceId()) ||
                        (newSampleRate != settings.sampleRate()) ||
                        (newBufferSize != settings.bufferSize());

    if (audioChanged) {
        AudioEngine* engine = m_controller->getAudioEngine();
        bool ok = engine->reinitialize(newDeviceId, newSampleRate, newBufferSize);
        if (ok) {
            m_audioStatusLabel->setText(
                QString("✓ Aplicado: %1  •  %2 Hz").arg(engine->currentDeviceName()).arg(engine->currentSampleRate()));
            m_audioStatusLabel->setStyleSheet("color: #66cc66;");
        } else {
            m_audioStatusLabel->setText("✗ Falha ao aplicar. Restaurado para configuração anterior.");
            m_audioStatusLabel->setStyleSheet("color: #ff4444;");
        }
    }

    // ── MIDI ──
    int newMidiPort = m_midiPortCombo->currentData().toInt();
    MidiEngine* midi = m_controller->getMidiEngine();
    if (newMidiPort >= 0 && newMidiPort != midi->currentPort()) {
        bool ok = midi->switchPort(newMidiPort);
        if (ok) {
            m_midiStatusLabel->setText(QString("✓ Conectado: %1").arg(midi->currentPortName()));
            m_midiStatusLabel->setStyleSheet("color: #66cc66;");
        } else {
            m_midiStatusLabel->setText("✗ Falha ao trocar a porta MIDI.");
            m_midiStatusLabel->setStyleSheet("color: #ff4444;");
        }
    }

    // ── Theme ──
    QString newTheme = m_themeCombo->currentData().toString();
    if (newTheme != settings.theme()) {
        settings.setTheme(newTheme);
        ThemeManager::applyTheme(qobject_cast<QMainWindow*>(parent()));
        m_themeStatusLabel->setText("✓ Tema aplicado.");
        m_themeStatusLabel->setStyleSheet("color: #66cc66;");
    }

    // Don't close — let the user see feedback
}

void SettingsDialog::onCancel() {
    reject();
}

void SettingsDialog::onRefreshMidi() {
    int currentPort = m_midiPortCombo->currentData().toInt();
    populateMidiPorts();
    if (currentPort >= 0 && currentPort < m_midiPortCombo->count()) {
        m_midiPortCombo->setCurrentIndex(currentPort);
    }
    m_midiStatusLabel->setText("Lista atualizada.");
    m_midiStatusLabel->setStyleSheet("");
}
