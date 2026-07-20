#include "CanvasDragDropHandler.h"
#include "InstrumentCanvasView.h"
#include "commands/UiCommands.h"
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QUrl>
#include <QFileInfo>
#include <cmath>

void CanvasDragDropHandler::handleDragEnter(InstrumentCanvasView* view, QDragEnterEvent* event) {
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    } else {
        // Pass back to view base class logic if needed, but since we intercepted, we do nothing or view->dragEnterEvent base call
        // In our case, we can just ignore.
    }
}

void CanvasDragDropHandler::handleDragMove(InstrumentCanvasView* view, QDragMoveEvent* event) {
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
}

void CanvasDragDropHandler::handleDrop(InstrumentCanvasView* view, ProjectManager* pm, QDropEvent* event, bool snapToGrid) {
    if (!event->mimeData()->hasUrls()) {
        return;
    }
    
    QList<QUrl> urls = event->mimeData()->urls();
    if (urls.isEmpty()) return;
    
    QString filePath = urls.first().toLocalFile();
    QFileInfo fi(filePath);
    QString ext = fi.suffix().toLower();
    
    if (ext == "png" || ext == "jpg" || ext == "jpeg") {
        QPointF scenePos = view->mapToScene(event->position().toPoint());
        
        if (snapToGrid) {
            scenePos.setX(std::round(scenePos.x() / 10.0) * 10.0);
            scenePos.setY(std::round(scenePos.y() / 10.0) * 10.0);
        }
        
        auto img = std::make_unique<UiImage>();
        img->imagePath = filePath;
        QImage pixmap(filePath);
        img->width = pixmap.isNull() ? 200 : pixmap.width();
        img->height = pixmap.isNull() ? 150 : pixmap.height();
        img->x = scenePos.x();
        img->y = scenePos.y();
        
        pm->getUndoStack()->push(new AddUiComponentCommand(pm, std::move(img)));
        event->acceptProposedAction();
    }
}
