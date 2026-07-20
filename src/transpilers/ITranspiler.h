#pragma once
#include <QString>
#include "core/ProjectManager.h"

class ITranspiler {
public:
    virtual ~ITranspiler() = default;
    virtual QString generate(const ProjectManager* pm, bool isBundle = false, const QString& sampleDirPrefix = "", const QString& bgName = "") = 0;
};
