#include "PresetManager.h"
#include "ProjectManager.h"
#include "ProjectSerializer.h"

PresetManager::PresetManager(ProjectManager* pm) : m_pm(pm) {
    clear();
}

void PresetManager::clear() {
    m_presets.clear();
    Preset p;
    p.name = "Init";
    p.state = QJsonObject();
    m_presets.push_back(p);
    m_activePresetIndex = 0;
}

QString PresetManager::generateUniquePresetName(const QString& baseName) const {
    QString name = baseName;
    int counter = 1;
    bool unique = false;
    while (!unique) {
        unique = true;
        for (const auto& p : m_presets) {
            if (p.name == name) {
                unique = false;
                break;
            }
        }
        if (!unique) {
            name = baseName + " (" + QString::number(counter++) + ")";
        }
    }
    return name;
}

void PresetManager::addPreset(const QString& name) {
    Preset p;
    p.name = generateUniquePresetName(name);
    p.state = QJsonObject(); 
    m_presets.push_back(p);
    m_pm->setDirty(true);
}

bool PresetManager::removePreset(int index) {
    if (m_presets.size() <= 1 || index < 0 || index >= m_presets.size()) return false;
    m_presets.erase(m_presets.begin() + index);
    if (m_activePresetIndex == index) {
        switchPreset(0);
    } else if (m_activePresetIndex > index) {
        m_activePresetIndex--;
    }
    m_pm->setDirty(true);
    return true;
}

bool PresetManager::renamePreset(int index, const QString& newName) {
    if (index < 0 || index >= m_presets.size()) return false;
    for (size_t i=0; i<m_presets.size(); ++i) {
        if (i != index && m_presets[i].name == newName) return false;
    }
    m_presets[index].name = newName;
    m_pm->setDirty(true);
    return true;
}

bool PresetManager::switchPreset(int index) {
    if (index < 0 || index >= m_presets.size()) return false;
    if (m_activePresetIndex == index) return true;
    
    m_presets[m_activePresetIndex].state = ProjectSerializer::serializeState(m_pm);
    
    m_activePresetIndex = index;
    m_pm->clearProjectNodes(); // We need a new function in ProjectManager to clear nodes but keep presets
    
    ProjectSerializer::deserializeState(m_pm, m_presets[m_activePresetIndex].state);
    
    m_pm->setDirty(true);
    return true;
}

int PresetManager::getPresetCount() const {
    return m_presets.size();
}

QString PresetManager::getPresetName(int index) const {
    if (index >= 0 && index < m_presets.size()) return m_presets[index].name;
    return "";
}

QJsonObject PresetManager::getPresetState(int index) const {
    if (index == m_activePresetIndex) return ProjectSerializer::serializeState(m_pm);
    if (index >= 0 && index < m_presets.size()) return m_presets[index].state;
    return QJsonObject();
}
