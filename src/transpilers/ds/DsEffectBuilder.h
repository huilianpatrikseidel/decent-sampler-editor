#pragma once
#include "../../core/DsDom.h"
#include "../../core/models/AudioNodes.h"
#include "../../core/ProjectManager.h"

class DsEffectBuilder {
public:
    static void buildEffects(DsNode* rootEffects, const ProjectManager* pm);
    static void buildSingleEffect(DsNode* parent, const Node* node);
    static int getEffectPosition(const ProjectManager* pm, const Node* effectNode);
};
