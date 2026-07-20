#pragma once
#include <QString>
#include "core/ProjectManager.h"

#include <functional>
#include <QImage>

class BundleExporter {
public:
    using BackgroundRasterizer = std::function<QImage(const ProjectManager*)>;
    static void setRasterizer(BackgroundRasterizer r);
    // Returns true on success, false on failure, populates errorMsg if failed.
    static bool exportToDecentSampler(const ProjectManager* pm, const QString& outputPath, QString& errorMsg);
    static bool exportToSfz(const ProjectManager* pm, const QString& outputPath, QString& errorMsg);
    
    static QString getSafeExportName(const QString& originalPath, bool asFlac);
};
