#pragma once
#include <QMap>
#include <QUuid>
#include <memory>
#include <vector>
#include "core/ProjectManager.h"
#include "audio/AudioEngine.h"
#include "audio/Vst3Host.h"

#include "CoreLibExport.h"

class CORELIB_EXPORT VstPluginManager : public QObject {
    Q_OBJECT
public:
    VstPluginManager(ProjectManager* pm, AudioEngine* audioEngine, QObject* parent = nullptr);
    ~VstPluginManager() override = default;

    std::shared_ptr<void> getVstHost(const QUuid& fxId) const;

public slots:
    void updateMasterEffects();

private:
    ProjectManager* m_pm;
    AudioEngine* m_audioEngine;
    QMap<QUuid, std::shared_ptr<Vst3Host>> m_vstHosts;
};
