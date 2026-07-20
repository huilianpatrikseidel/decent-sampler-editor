#pragma once
#include <QObject>
#include <memory>
#include "core/ProjectManager.h"
#include "core/DatabaseManager.h"
#include "audio/AudioEngine.h"
#include "audio/MidiEngine.h"
#include "AudioGraphBuilder.h"
#include "VstPluginManager.h"
#include "audio/PlaybackSequencer.h"
#include <vector>
#include <atomic>
#include <string>

#include "CoreLibExport.h"

// Controller Central para evitar a MainWindow de se tornar um God Object
class CORELIB_EXPORT ApplicationController : public QObject {
    Q_OBJECT
public:
    explicit ApplicationController(QObject *parent = nullptr);
    ~ApplicationController() override;

    bool initializeSubsystems(std::function<void(int, const QString&)> progressCallback = nullptr, std::function<bool()> cancelCheck = nullptr);
    void shutdownSubsystems();

    ProjectManager* getProjectManager() { return &m_projectManager; }
    DatabaseManager* getDatabaseManager() { return &m_databaseManager; }
    AudioEngine* getAudioEngine() { return &m_audioEngine; }
    MidiEngine* getMidiEngine() { return &m_midiEngine; }
    VstPluginManager* getVstPluginManager() { return m_vstPluginManager.get(); }
    PlaybackSequencer* getPlaybackSequencer() { return m_playbackSequencer.get(); }

public slots:
    void updateRenderGraph();
    void updateLfos();
    void playNote(int midiNote, int velocity);
    void stopNote(int midiNote);
    
private:
    ProjectManager m_projectManager;
    DatabaseManager m_databaseManager;
    AudioEngine m_audioEngine;
    MidiEngine m_midiEngine;
    
    std::unique_ptr<VstPluginManager> m_vstPluginManager;
    std::unique_ptr<PlaybackSequencer> m_playbackSequencer;
    
    std::shared_ptr<const std::vector<RenderZone>> m_renderZones;
    
    void setupConnections();
};
