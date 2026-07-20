#pragma once
#include <QString>
#include <QJsonObject>
#include <vector>

class ProjectManager;

#include "CoreLibExport.h"

class CORELIB_EXPORT PresetManager {
public:
    explicit PresetManager(ProjectManager* pm);
    
    struct Preset {
        QString name;
        QJsonObject state;
    };
    
    void clear();
    void addPreset(const QString& name);
    bool removePreset(int index);
    bool renamePreset(int index, const QString& newName);
    bool switchPreset(int index);
    
    int getActivePresetIndex() const { return m_activePresetIndex; }
    void setActivePresetIndex(int index) { m_activePresetIndex = index; }
    
    QString getPresetName(int index) const;
    int getPresetCount() const;
    QJsonObject getPresetState(int index) const;
    
    std::vector<Preset>& getPresets() { return m_presets; }
    const std::vector<Preset>& getPresets() const { return m_presets; }

private:
    QString generateUniquePresetName(const QString& baseName) const;
    
    ProjectManager* m_pm;
    std::vector<Preset> m_presets;
    int m_activePresetIndex = 0;
};
