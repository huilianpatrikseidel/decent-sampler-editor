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

    // Name a sample gets inside a .dsbundle. Encodable WAVs become .flac; anything else
    // keeps its original extension. The preset XML is written before the zip entries, so
    // both sides must derive the name from this one function to stay in agreement.
    static QString getBundleSampleName(const QString& originalPath);
};
