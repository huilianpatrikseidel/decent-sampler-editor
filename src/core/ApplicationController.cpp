#include "ApplicationController.h"
#include "AppSettings.h"
#include <QDebug>
#include <QTimer>
#include <QCoreApplication>

ApplicationController::ApplicationController(QObject *parent) : QObject(parent) {
    m_vstPluginManager = std::make_unique<VstPluginManager>(&m_projectManager, &m_audioEngine, this);
    m_playbackSequencer = std::make_unique<PlaybackSequencer>(&m_audioEngine, this);

}

ApplicationController::~ApplicationController() {
    shutdownSubsystems();
}

bool ApplicationController::initializeSubsystems(std::function<void(int, const QString&)> progressCallback, std::function<bool()> cancelCheck) {
    if (cancelCheck && cancelCheck()) return false;
    
    if (progressCallback) progressCallback(10, "Initializing Database...");
    if (!m_databaseManager.initialize()) {
        qCritical() << "Failed to initialize Database Manager!";
    }
    
    if (cancelCheck && cancelCheck()) return false;
    
    if (progressCallback) progressCallback(40, "Initializing Audio Engine...");
    AppSettings& settings = AppSettings::instance();
    bool audioOk = m_audioEngine.initialize(settings.audioDeviceId(), settings.sampleRate(), settings.bufferSize());
    if (audioOk) {
        m_audioEngine.start();
        qDebug() << "AudioEngine started.";
    }

    if (cancelCheck && cancelCheck()) return false;
    
    if (progressCallback) progressCallback(70, "Initializing MIDI Engine...");
    bool midiOk = m_midiEngine.initialize(settings.midiPortIndex());
    if (midiOk) {
        qDebug() << "MidiEngine started and connected.";
    } else {
        qDebug() << "MidiEngine failed to initialize (no MIDI ports?), but continuing.";
    }
    
    if (cancelCheck && cancelCheck()) return false;
    
    if (progressCallback) progressCallback(90, "Setting up internal connections...");
    setupConnections();
    
    return audioOk; // Retorna true se o core vital (áudio) inicializou
}

void ApplicationController::shutdownSubsystems() {
    // 1. Para MIDI primeiro para não enfileirar mais eventos
    m_midiEngine.stop();
    
    // 2. Envia Shutdown graceful para a Audio Thread
    m_audioEngine.pushCommand({AudioCommandType::Shutdown, 0.0f});
    m_audioEngine.stop();
}

void ApplicationController::setupConnections() {
    m_midiEngine.setCallback([this](unsigned char status, unsigned char data1, unsigned char data2) {
        if ((status & 0xF0) == 0x90 && data2 > 0) {
            playNote(data1, data2);
        } else if ((status & 0xF0) == 0x80 || ((status & 0xF0) == 0x90 && data2 == 0)) {
            stopNote(data1);
        }
    });
    
    connect(&m_projectManager, &ProjectManager::nodeAdded, this, &ApplicationController::updateRenderGraph);
    connect(&m_projectManager, &ProjectManager::nodeRemoved, this, &ApplicationController::updateRenderGraph);
    connect(&m_projectManager, &ProjectManager::nodeModified, this, &ApplicationController::updateRenderGraph);
    connect(&m_projectManager, &ProjectManager::projectLoaded, this, &ApplicationController::updateRenderGraph);
    connect(m_projectManager.getAudioState(), &AudioStateModel::globalLfoChanged, this, &ApplicationController::updateLfos);
    connect(&m_projectManager, &ProjectManager::projectLoaded, this, &ApplicationController::updateLfos);
    
    connect(&m_projectManager, &ProjectManager::nodeAdded, m_vstPluginManager.get(), &VstPluginManager::updateMasterEffects);
    connect(&m_projectManager, &ProjectManager::nodeRemoved, m_vstPluginManager.get(), &VstPluginManager::updateMasterEffects);
    connect(&m_projectManager, &ProjectManager::nodeModified, m_vstPluginManager.get(), &VstPluginManager::updateMasterEffects);
    connect(&m_projectManager, &ProjectManager::projectLoaded, m_vstPluginManager.get(), &VstPluginManager::updateMasterEffects);
    
    connect(&m_projectManager, &ProjectManager::projectSaved, this, [this]() {
        int id = m_projectManager.getCurrentProjectId();
        if (id != -1) {
            m_databaseManager.updateProjectModifiedAt(id);
        }
    });
    
    connect(&m_projectManager, &ProjectManager::projectSavedWithData, this, [this](int id, const QString& data) {
        if (id != -1) {
            m_databaseManager.updateProjectData(id, data);
        }
    });
}

void ApplicationController::updateRenderGraph() {
    auto newZones = std::make_shared<const std::vector<RenderZone>>(AudioGraphBuilder::buildRenderGraph(&m_projectManager, &m_audioEngine));
    std::atomic_store(&m_renderZones, newZones);
}

void ApplicationController::playNote(int midiNote, int velocity) {
    bool found = false;
    
    auto zones = std::atomic_load(&m_renderZones);
    if (zones) {
        for (const auto& z : *zones) {
            if (midiNote >= z.loNote && midiNote <= z.hiNote && velocity >= z.loVel && velocity <= z.hiVel) {
                AudioMessage msg;
                msg.type = AudioCommandType::PlayNote;
                msg.note = midiNote;
                msg.velocity = velocity;
                msg.sampleId = z.sampleId;
                msg.attack = z.attack;
                msg.decay = z.decay;
                msg.sustain = z.sustain;
                msg.release = z.release;
                msg.volume = z.volume;
                
                msg.modAttack = z.modAttack;
                msg.modDecay = z.modDecay;
                msg.modSustain = z.modSustain;
                msg.modRelease = z.modRelease;
                
                msg.isLegato = z.legatoEnabled;
                msg.glideTime = z.glideTime;
                msg.groupId = z.groupId;
                msg.paramBlockIndex = z.paramBlockIndex;
                
                msg.hasFilter = z.hasFilter;
                msg.filterCutoff = z.filterCutoff;
                msg.filterResonance = z.filterResonance;
                msg.filterType = z.filterType;
                
                // Oscillator data
                msg.isOscillator = z.isOscillator;
                msg.oscWaveform = z.oscWaveform;
                msg.oscDamping = z.oscDamping;
                
                msg.numRoutings = z.numRoutings;
                for (int i = 0; i < z.numRoutings; ++i) {
                    msg.routings[i] = z.routings[i];
                }
                
                m_audioEngine.pushCommand(msg);
                found = true;
            }
        }
    }
    
    if (!found) { // Synth Fallback removed for architectural cleanliness.
    }
}

void ApplicationController::updateLfos() {
    auto sendLfoMsg = [this](const LFO& lfo, int idx) {
        AudioMessage msg;
        msg.type = AudioCommandType::SetLFO;
        msg.lfoIndex = idx;
        msg.lfoFreq = lfo.frequency;
        msg.lfoAmount = 1.0f; // Amount handled per routing
        
        if (lfo.shape == "sine") msg.lfoShape = 0;
        else if (lfo.shape == "triangle") msg.lfoShape = 1;
        else if (lfo.shape == "square") msg.lfoShape = 2;
        else if (lfo.shape == "saw") msg.lfoShape = 3;
        
        m_audioEngine.pushCommand(msg);
    };
    auto lfos = m_projectManager.getAudioState()->getGlobalLfos();
    for (int i = 0; i < lfos.size(); ++i) {
        sendLfoMsg(lfos[i], i + 1);
    }
}



void ApplicationController::stopNote(int midiNote) {
    AudioMessage msg;
    msg.type = AudioCommandType::StopNote;
    msg.note = midiNote;
    m_audioEngine.pushCommand(msg);
}
