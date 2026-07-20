#pragma once

#include <QObject>
#include <QString>
#include "core/ProjectManager.h"

class ExportWorker : public QObject {
    Q_OBJECT
public:
    enum class ExportType {
        DecentSampler,
        SFZ
    };

    explicit ExportWorker(ExportType type, const ProjectManager* pm, const QString& outputPath, QObject* parent = nullptr);

public slots:
    void process();

signals:
    void finished(bool success, const QString& errorMsg);

private:
    ExportType m_type;
    const ProjectManager* m_pm;
    QString m_outputPath;
};
