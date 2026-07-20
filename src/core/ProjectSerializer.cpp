#include "ProjectSerializer.h"
#include "ProjectManager.h"
#include "serializers/AudioStateSerializer.h"
#include "serializers/UiStateSerializer.h"
#include "serializers/GraphSerializer.h"
#include <QFile>
#include <QJsonArray>

bool ProjectSerializer::loadProject(ProjectManager* pm, const QString &projectData) {
    PresetManager* presetMgr = pm->getPresetManager();
    if (projectData.isEmpty()) {
        presetMgr->clear();
        pm->setDirty(false);
        return true;
    }
    
    QByteArray data = projectData.toUtf8();
    QJsonDocument doc(QJsonDocument::fromJson(data));
    QJsonObject root = doc.object();
    
    QString version = root["version"].toString();
    pm->setCurrentProjectName(root["projectName"].toString());
    
    presetMgr->getPresets().clear();
    
#include <QDebug>
    
    qDebug() << "ProjectSerializer::loadProject called. Data length:" << projectData.length();
    if (version == "2.0.0") {
        presetMgr->setActivePresetIndex(root["activePresetIndex"].toInt());
        QJsonArray presetsArray = root["presets"].toArray();
        qDebug() << "Found 2.0.0 project with" << presetsArray.size() << "presets.";
        for (int i = 0; i < presetsArray.size(); ++i) {
            QJsonObject pObj = presetsArray[i].toObject();
            PresetManager::Preset p;
            p.name = pObj["name"].toString();
            p.state = pObj["state"].toObject();
            presetMgr->getPresets().push_back(p);
        }
        int activeIdx = presetMgr->getActivePresetIndex();
        if (activeIdx >= 0 && activeIdx < presetMgr->getPresetCount()) {
            deserializeState(pm, presetMgr->getPresets()[activeIdx].state);
            qDebug() << "Successfully deserialized state for active preset.";
        } else {
            qDebug() << "Active preset index out of bounds!";
        }
    } else {
        qDebug() << "Unknown or legacy project version:" << version;
    }
    
    pm->setDirty(false);
    return true;
}

QString ProjectSerializer::saveProject(ProjectManager* pm) {
    PresetManager* presetMgr = pm->getPresetManager();
    int activeIdx = presetMgr->getActivePresetIndex();
    if (activeIdx >= 0 && activeIdx < presetMgr->getPresetCount()) {
        presetMgr->getPresets()[activeIdx].state = serializeState(pm);
    }
    
    QJsonObject root;
    root["version"] = "2.0.0";
    root["projectName"] = pm->getCurrentProjectName();
    root["activePresetIndex"] = activeIdx;
    
    QJsonArray presetsArray;
    for (const auto& p : presetMgr->getPresets()) {
        QJsonObject pObj;
        pObj["name"] = p.name;
        pObj["state"] = p.state;
        presetsArray.append(pObj);
    }
    root["presets"] = presetsArray;

    QJsonDocument doc(root);
    return QString(doc.toJson(QJsonDocument::Indented));
}

QJsonObject ProjectSerializer::serializeState(const ProjectManager* pm) {
    QJsonObject state;
    state["audio"] = AudioStateSerializer::serialize(pm->getAudioState());
    state["ui"] = UiStateSerializer::serialize(pm);
    state["graph"] = GraphSerializer::serialize(pm);
    return state;
}

void ProjectSerializer::deserializeState(ProjectManager* pm, const QJsonObject &json) {
    if (json.contains("audio")) {
        AudioStateSerializer::deserialize(pm->getAudioState(), json["audio"].toObject());
    }
    if (json.contains("ui")) {
        UiStateSerializer::deserialize(pm, json["ui"].toObject());
    }
    if (json.contains("graph")) {
        GraphSerializer::deserialize(pm, json["graph"].toObject());
    }
}
