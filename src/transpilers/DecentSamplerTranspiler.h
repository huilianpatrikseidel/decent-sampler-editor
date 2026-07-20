#pragma once
#include "ITranspiler.h"
#include "../core/DsDom.h"

class DecentSamplerTranspiler : public ITranspiler {
public:
    QString generate(const ProjectManager* pm, bool isBundle = false, const QString& sampleDirPrefix = "", const QString& bgImageName = "");
};
