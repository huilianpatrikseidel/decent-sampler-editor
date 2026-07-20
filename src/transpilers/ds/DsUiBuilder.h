#pragma once
#include "../../core/DsDom.h"
#include "../../core/ProjectManager.h"

class DsUiBuilder {
public:
    static void buildUi(DsNode* rootUi, const ProjectManager* pm, const QString& bgImageName);
};
