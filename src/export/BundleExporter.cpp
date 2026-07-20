#include "BundleExporter.h"
#include "transpilers/DecentSamplerTranspiler.h"
#include "transpilers/SfzTranspiler.h"
#include <miniz.h>
#include <QFile>
#include <QFileInfo>
#include "../core/ProjectSerializer.h"
#include <QDir>
#include <QSet>
#include <QDebug>
#include <QTemporaryFile>
#include <QCryptographicHash>
// UI rasterization has been moved to a callback to decouple from the GUI layer
#include <QBuffer>

BundleExporter::BackgroundRasterizer s_rasterizer = nullptr;

void BundleExporter::setRasterizer(BackgroundRasterizer r) {
    s_rasterizer = r;
}

QString BundleExporter::getSafeExportName(const QString& originalPath, bool asFlac) {
    QFileInfo info(originalPath);
    QString hash = QString(QCryptographicHash::hash(originalPath.toUtf8(), QCryptographicHash::Md5).toHex()).left(8);
    QString ext = info.suffix().toLower();
    if (asFlac) ext = "flac";
    if (ext.isEmpty()) ext = "wav";
    return info.completeBaseName() + "_" + hash + "." + ext;
}

bool BundleExporter::exportToDecentSampler(const ProjectManager* pm, const QString& outputPath, QString& errorMsg) {
    // Create zip archive
    mz_zip_archive zip_archive;
    memset(&zip_archive, 0, sizeof(zip_archive));
    
    if (!mz_zip_writer_init_file(&zip_archive, outputPath.toUtf8().constData(), 0)) {
        errorMsg = "Failed to create .dslibrary file.";
        return false;
    }
    
    QSet<QString> allSamplePaths;
    QSet<QString> allFilmstripPaths;
    
    int numPresets = pm->getPresetManager()->getPresetCount();
    for (int i = 0; i < numPresets; ++i) {
        ProjectManager pmTemp;
        pmTemp.blockSignals(true);
        ProjectSerializer::deserializeState(&pmTemp, pm->getPresetManager()->getPresetState(i));
        
        QString bgName = pm->getPresetManager()->getPresetName(i);
        if (bgName.isEmpty()) bgName = "background";
        bgName += ".png";
        
        DecentSamplerTranspiler transpiler;
        QString xmlContent = transpiler.generate(&pmTemp, true, "", bgName);
        
        QImage bgImage;
        if (s_rasterizer) {
            bgImage = s_rasterizer(&pmTemp);
        } else {
            bgImage = QImage(812, 375, QImage::Format_ARGB32);
            bgImage.fill(QColor("#2B2B2B"));
        }
        
        QByteArray bgData;
        QBuffer bgBuffer(&bgData);
        bgBuffer.open(QIODevice::WriteOnly);
        bgImage.save(&bgBuffer, "PNG");
        
        mz_zip_writer_add_mem(&zip_archive, bgName.toUtf8().constData(), 
                              bgData.constData(), bgData.size(), MZ_DEFAULT_COMPRESSION);
        
        QString dspresetName = pm->getPresetManager()->getPresetName(i) + ".dspreset";
        if (dspresetName == ".dspreset") dspresetName = "instrument.dspreset";
        
        if (!mz_zip_writer_add_mem(&zip_archive, dspresetName.toUtf8().constData(), 
                                   xmlContent.toUtf8().constData(), xmlContent.toUtf8().size(), MZ_DEFAULT_COMPRESSION)) {
            errorMsg = "Failed to add preset XML to bundle.";
            mz_zip_writer_end(&zip_archive);
            return false;
        }
        
        // Collect unique sample paths
        for (const auto& pair : pmTemp.getAllNodes()) {
            const auto& node = pair.second;
            if (node->type == "SampleGroup") {
                SampleGroup* sg = static_cast<SampleGroup*>(node.get());
                for (const Zone& z : sg->zones) {
                    allSamplePaths.insert(z.samplePath);
                }
            }
        }
        
        // Collect UI Filmstrips
        for (const auto& pair : pmTemp.getAllUiComponents()) {
            const auto& comp = pair.second;
            if (comp->type == "UiKnob") {
                UiKnob* k = static_cast<UiKnob*>(comp.get());
                if (!k->filmstripPath.isEmpty()) {
                    allFilmstripPaths.insert(k->filmstripPath);
                }
            }
        }
    }
    
    // Add audio files (Direct copy, no FLAC transcoding to avoid libsndfile dependency)
    for (const QString& path : allSamplePaths) {
        QFile file(path);
        if (file.open(QIODevice::ReadOnly)) {
            QString safeName = getSafeExportName(path, false);
            QByteArray data = file.readAll();
            if (!mz_zip_writer_add_mem(&zip_archive, safeName.toUtf8().constData(), 
                                        data.constData(), data.size(), MZ_DEFAULT_COMPRESSION)) {
                errorMsg = "Failed to add audio file " + safeName + " to bundle.";
                mz_zip_writer_end(&zip_archive);
                return false;
            }
        } else {
            errorMsg = "Error: Could not read audio file " + path;
            mz_zip_writer_end(&zip_archive);
            return false;
        }
    }
    
    // Add UI Filmstrips
    for (const QString& path : allFilmstripPaths) {
        QFile file(path);
        if (file.open(QIODevice::ReadOnly)) {
            QByteArray data = file.readAll();
            QFileInfo info(path);
            QString destName = info.fileName();
            mz_zip_writer_add_mem(&zip_archive, destName.toUtf8().constData(), 
                                  data.constData(), data.size(), MZ_DEFAULT_COMPRESSION);
        }
    }
    
    mz_zip_writer_finalize_archive(&zip_archive);
    mz_zip_writer_end(&zip_archive);
    
    return true;
}

bool BundleExporter::exportToSfz(const ProjectManager* pm, const QString& outputPath, QString& errorMsg) {
    QFileInfo outInfo(outputPath);
    QString baseName = outInfo.completeBaseName();
    QString sampleDirName = baseName + "_Samples";
    QString sampleDirPath = outInfo.absoluteDir().absoluteFilePath(sampleDirName);
    
    QDir dir;
    if (!dir.mkpath(sampleDirPath)) {
        errorMsg = "Failed to create samples directory: " + sampleDirPath;
        return false;
    }
    
    QSet<QString> allSamplePaths;
    int numPresets = pm->getPresetManager()->getPresetCount();
    
    for (int i = 0; i < numPresets; ++i) {
        ProjectManager pmTemp;
        pmTemp.blockSignals(true);
        ProjectSerializer::deserializeState(&pmTemp, pm->getPresetManager()->getPresetState(i));
        
        SfzTranspiler transpiler;
        QString sfzContent = transpiler.generate(&pmTemp, false, sampleDirName + "/");
        
        QString sfzName = pm->getPresetManager()->getPresetName(i) + ".sfz";
        if (sfzName == ".sfz") sfzName = baseName + ".sfz";
        
        QFile file(outInfo.absoluteDir().absoluteFilePath(sfzName));
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            qint64 written = file.write(sfzContent.toUtf8());
            file.close();
            if (written == -1) {
                errorMsg = "Failed to write SFZ file to disk: " + file.errorString();
                return false;
            }
        } else {
            errorMsg = "Failed to create SFZ file: " + file.errorString();
            return false;
        }
        
        for (const auto& pair : pmTemp.getAllNodes()) {
            const auto& node = pair.second;
            if (node->type == "SampleGroup") {
                SampleGroup* sg = static_cast<SampleGroup*>(node.get());
                for (const Zone& z : sg->zones) {
                    allSamplePaths.insert(z.samplePath);
                }
            }
        }
    }
    
    // Copy unique samples
    for (const QString& path : allSamplePaths) {
        QString safeName = getSafeExportName(path, false);
        QString destPath = sampleDirPath + "/" + safeName;
        if (!QFile::exists(destPath)) {
            if (!QFile::copy(path, destPath)) {
                errorMsg = "Failed to copy sample " + path + " to " + destPath;
                return false;
            }
        }
    }
    
    return true;
}
