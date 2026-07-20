#include "ExportWorker.h"
#include "BundleExporter.h"

ExportWorker::ExportWorker(ExportType type, const ProjectManager* pm, const QString& outputPath, QObject* parent)
    : QObject(parent), m_type(type), m_pm(pm), m_outputPath(outputPath) {}

void ExportWorker::process() {
    bool success = false;
    QString err;
    
    if (m_type == ExportType::DecentSampler) {
        success = BundleExporter::exportToDecentSampler(m_pm, m_outputPath, err);
    } else if (m_type == ExportType::SFZ) {
        success = BundleExporter::exportToSfz(m_pm, m_outputPath, err);
    }
    
    emit finished(success, err);
}
