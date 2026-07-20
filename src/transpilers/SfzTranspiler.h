#pragma once
#include "ITranspiler.h"

class SfzTranspiler : public ITranspiler {
public:
    QString generate(const ProjectManager* pm, bool isBundle = false, const QString& sampleDirPrefix = "", const QString& bgName = "") override;
};
