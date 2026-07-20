#pragma once

class InstrumentCanvasView;
class QContextMenuEvent;
class ProjectManager;
class QGraphicsScene;

#include "../designer/UiDesignerExport.h"

class UIDESIGNER_EXPORT CanvasContextMenu {
public:
    static void show(InstrumentCanvasView* view, ProjectManager* pm, QGraphicsScene* scene, QContextMenuEvent* event);
};
