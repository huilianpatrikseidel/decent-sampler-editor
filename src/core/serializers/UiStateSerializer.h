#pragma once
#include <QJsonObject>

class ProjectManager;

class UiStateSerializer {
public:
    static QJsonObject serialize(const ProjectManager* pm);
    static void deserialize(ProjectManager* pm, const QJsonObject& json);
};
