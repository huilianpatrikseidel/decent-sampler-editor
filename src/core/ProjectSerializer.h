#pragma once
#include <QString>
#include <QJsonObject>
#include <QJsonDocument>

class ProjectManager;

#include "CoreLibExport.h"

class CORELIB_EXPORT ProjectSerializer {
public:
    static bool loadProject(ProjectManager* pm, const QString& projectData);
    static QString saveProject(ProjectManager* pm);
    
    static QJsonObject serializeState(const ProjectManager* pm);
    static void deserializeState(ProjectManager* pm, const QJsonObject& json);
};
