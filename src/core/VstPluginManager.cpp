#include "VstPluginManager.h"
#include <QCoreApplication>
#include <QJsonObject>
#include <QTimer>
#include <QDebug>

VstPluginManager::VstPluginManager(ProjectManager* pm, AudioEngine* audioEngine, QObject* parent)
    : QObject(parent), m_pm(pm), m_audioEngine(audioEngine) {
}

std::shared_ptr<void> VstPluginManager::getVstHost(const QUuid& fxId) const {
    return m_vstHosts.value(fxId, nullptr);
}

void VstPluginManager::updateMasterEffects() {
    auto newFx = new std::vector<Vst3Host*>();
    
    // Create a new map of hosts so we don't destroy the old ones yet
    QMap<QUuid, std::shared_ptr<Vst3Host>> newHosts;
    
    // 1. Load VST instances for ALL effects in the project (so their UI can open)
    for (const auto& pair : m_pm->getAllNodes()) {
        const QUuid& fxId = pair.first;
        Node* n = pair.second.get();
        QString t = n->type;
        
        if (t == "Delay" || t == "Reverb" || t == "Filter" || t == "Chorus" || 
            t == "Gain" || t == "Phaser" || t == "PitchShifter" || t == "WaveFolder" || 
            t == "WaveShaper" || t == "StereoSimulator" || t == "BitCrusher" || 
            t == "Equalizer") 
        {
            QJsonObject obj = n->toJson();
            QString pluginPath = QCoreApplication::applicationDirPath() + "/VST3/" + t + ".vst3";
            
            std::shared_ptr<Vst3Host> host = std::make_shared<Vst3Host>();
            if (host->loadPlugin(pluginPath)) {
                if (t == "Delay") {
                    host->setParameter(0, obj["time"].toDouble());
                    host->setParameter(1, obj["feedback"].toDouble());
                } else if (t == "Reverb") {
                    host->setParameter(0, obj["size"].toDouble());
                    host->setParameter(1, obj["damp"].toDouble());
                } else if (t == "Filter") {
                    host->setParameter(1, obj["cutoff"].toDouble());
                    host->setParameter(2, obj["resonance"].toDouble());
                } else if (t == "Chorus") {
                    host->setParameter(0, obj["rate"].toDouble());
                    host->setParameter(1, obj["depth"].toDouble());
                    host->setParameter(2, obj["mix"].toDouble());
                } else if (t == "Gain") {
                    host->setParameter(0, obj["level"].toDouble());
                } else if (t == "Phaser") {
                    host->setParameter(0, obj["mix"].toDouble());
                    host->setParameter(1, obj["modDepth"].toDouble());
                    host->setParameter(2, obj["modRate"].toDouble());
                    host->setParameter(3, obj["centerFrequency"].toDouble());
                    host->setParameter(4, obj["feedback"].toDouble());
                } else if (t == "PitchShifter") {
                    host->setParameter(0, obj["pitchShift"].toDouble());
                    host->setParameter(1, obj["mix"].toDouble());
                } else if (t == "WaveFolder") {
                    host->setParameter(0, obj["drive"].toDouble());
                    host->setParameter(1, obj["threshold"].toDouble());
                } else if (t == "WaveShaper") {
                    host->setParameter(0, obj["drive"].toDouble());
                    host->setParameter(1, obj["driveBoost"].toDouble());
                    host->setParameter(2, obj["outputLevel"].toDouble());
                } else if (t == "StereoSimulator") {
                    host->setParameter(1, obj["width"].toDouble());
                    host->setParameter(2, obj["delayTime"].toDouble());
                    host->setParameter(3, obj["modRate"].toDouble());
                    host->setParameter(4, obj["modDepth"].toDouble());
                } else if (t == "BitCrusher") {
                    host->setParameter(0, obj["bitDepth"].toDouble());
                    host->setParameter(1, obj["sampleRateReduction"].toDouble());
                    host->setParameter(2, obj["mix"].toDouble());
                }
                
                newHosts[fxId] = host;
            } else {
                qWarning() << "Failed to load VST3 plugin:" << pluginPath;
            }
        }
    }
    
    // 2. Assemble Master FX Chain for the AudioEngine
    for (const QUuid& fxId : m_pm->getAudioState()->getMasterEffects()) {
        if (newHosts.contains(fxId)) {
            newFx->push_back(newHosts[fxId].get());
        }
    }
    
    // Preserve old hosts until audio engine completes swap
    auto oldHosts = m_vstHosts;
    m_vstHosts = newHosts;
    
    m_audioEngine->setMasterEffectsAsync(newFx);
    
    // Safely delete old hosts after 100ms
    QTimer::singleShot(100, [oldHosts]() {
        // oldHosts falls out of scope here
    });
}
