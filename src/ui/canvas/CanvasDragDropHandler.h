#pragma once

class InstrumentCanvasView;
class ProjectManager;
class QDragEnterEvent;
class QDragMoveEvent;
class QDropEvent;

#include "../designer/UiDesignerExport.h"

class UIDESIGNER_EXPORT CanvasDragDropHandler {
public:
    static void handleDragEnter(InstrumentCanvasView* view, QDragEnterEvent* event);
    static void handleDragMove(InstrumentCanvasView* view, QDragMoveEvent* event);
    static void handleDrop(InstrumentCanvasView* view, ProjectManager* pm, QDropEvent* event, bool snapToGrid);
};
