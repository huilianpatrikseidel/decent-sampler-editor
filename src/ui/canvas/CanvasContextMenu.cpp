#include "CanvasContextMenu.h"
#include "InstrumentCanvasView.h"
#include "UiComponentItem.h"
#include "commands/ModifyPropertyCommand.h"
#include "commands/UiCommands.h"
#include <QMenu>
#include <QContextMenuEvent>
#include <QScrollBar>

void CanvasContextMenu::show(InstrumentCanvasView* view, ProjectManager* pm, QGraphicsScene* scene, QContextMenuEvent* event) {
    QMenu menu(view);
    QAction* addKnob = menu.addAction("Add Knob");
    QAction* addSlider = menu.addAction("Add Slider");
    QAction* addButton = menu.addAction("Add Button");
    QAction* addLabel = menu.addAction("Add Label");
    QAction* addRect = menu.addAction("Add Rectangle");
    QAction* addEllipse = menu.addAction("Add Ellipse");
    QAction* addImage = menu.addAction("Add Image");
    menu.addSeparator();
    QAction* resetView = menu.addAction("Fit to Screen (Reset View)");
    menu.addSeparator();
    QAction* bringToFront = menu.addAction("Bring to Front");
    QAction* sendToBack = menu.addAction("Send to Back");
    menu.addSeparator();
    QAction* removeComp = menu.addAction("Remove Selected");
    
    QAction* action = menu.exec(event->globalPos());
    if (action == addKnob) emit view->requestAddComponent("UiKnob", view->mapToScene(event->pos()));
    else if (action == addSlider) emit view->requestAddComponent("UiSlider", view->mapToScene(event->pos()));
    else if (action == addButton) emit view->requestAddComponent("UiButton", view->mapToScene(event->pos()));
    else if (action == addLabel) emit view->requestAddComponent("UiLabel", view->mapToScene(event->pos()));
    else if (action == addRect) emit view->requestAddComponent("UiRect", view->mapToScene(event->pos()));
    else if (action == addEllipse) emit view->requestAddComponent("UiEllipse", view->mapToScene(event->pos()));
    else if (action == addImage) emit view->requestAddComponent("UiImage", view->mapToScene(event->pos()));
    else if (action == bringToFront || action == sendToBack) {
        if (!scene->selectedItems().isEmpty()) {
            auto item = dynamic_cast<UiComponentItem*>(scene->selectedItems().first());
            if (item) {
                int targetZ = 0;
                if (action->text() == "Bring to Front") {
                    for (const auto& pair : pm->getAllUiComponents()) targetZ = std::max(targetZ, pair.second->zIndex);
                    targetZ += 1;
                } else if (action->text() == "Send to Back") {
                    for (const auto& pair : pm->getAllUiComponents()) targetZ = std::min(targetZ, pair.second->zIndex);
                    targetZ -= 1;
                }
                auto comp = pm->getUiComponent(item->getComponentId());
                if (comp) {
                    pm->getUndoStack()->push(new ModifyUiPropertyCommand(pm, comp->id, "zIndex", comp->zIndex, targetZ));
                }
            }
        }
    }
    else if (action == resetView) {
        if (!scene->items().isEmpty()) {
            view->fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatio);
            double zoom = view->transform().m11();
            emit view->viewportChanged(view->horizontalScrollBar()->value(), view->verticalScrollBar()->value(), zoom);
        }
    }
    else if (action == removeComp) {
        if (!scene->selectedItems().isEmpty()) {
            auto item = dynamic_cast<UiComponentItem*>(scene->selectedItems().first());
            if (item) emit view->requestRemoveComponent(item->getComponentId());
        }
    }
}
