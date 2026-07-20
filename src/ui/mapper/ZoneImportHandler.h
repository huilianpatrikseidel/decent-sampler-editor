#pragma once

#include <QUuid>
#include <QPointF>


#include "ui/mapper/MapperLibExport.h"
class ProjectManager;
class AudioAnalyzer;
class QDropEvent;
class MappingEditorView;

class MAPPERLIB_EXPORT ZoneImportHandler {
public:
    static void handleDrop(MappingEditorView* view, ProjectManager* pm, AudioAnalyzer* analyzer, QUuid& currentSgId, QDropEvent* event, double noteWidth, double velocityHeight);
};
