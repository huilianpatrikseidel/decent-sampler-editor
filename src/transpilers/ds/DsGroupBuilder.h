#pragma once
#include "../../core/DsDom.h"
#include "../../core/ProjectManager.h"

class DsGroupBuilder {
public:
    static void buildGroups(DsNode* rootGroups, const ProjectManager* pm, bool isBundle);
};
