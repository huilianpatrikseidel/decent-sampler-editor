#include "InstrumentCanvasView.h"
#include "UiComponentItemFactory.h"
#include <QMenu>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QScrollBar>
#include <cmath>
#include "commands/ModifyPropertyCommand.h"
#include "commands/UiCommands.h"
#include <QMimeData>
#include <QUrl>
#include <QFileInfo>
#include "CanvasContextMenu.h"
#include "CanvasDragDropHandler.h"
#include "../../core/ThemePalette.h"

InstrumentCanvasView::InstrumentCanvasView(ProjectManager* pm, QWidget* parent) 
    : QGraphicsView(parent), m_pm(pm) {
    
    m_scene = new QGraphicsScene(this);
    m_scene->setSceneRect(-2000, -2000, 4000, 4000); // Infinite scrolling area
    setScene(m_scene);
    
    m_headerItem = new CanvasHeaderItem(m_pm);
    m_headerItem->setPos(0, 0);
    m_scene->addItem(m_headerItem);
    
    m_keyboardItem = new CanvasKeyboardItem(m_pm);
    m_keyboardItem->setPos(0, m_pm->getUiState()->getHeight() - 75);
    m_scene->addItem(m_keyboardItem);
    
    connect(m_keyboardItem, &CanvasKeyboardItem::notePressed, this, &InstrumentCanvasView::notePressed);
    connect(m_keyboardItem, &CanvasKeyboardItem::noteReleased, this, &InstrumentCanvasView::noteReleased);
    
    setRenderHint(QPainter::Antialiasing);
    setViewportUpdateMode(QGraphicsView::SmartViewportUpdate); 
    setDragMode(QGraphicsView::RubberBandDrag); 
    setAcceptDrops(true); // Allow image drops
    setResizeAnchor(QGraphicsView::AnchorViewCenter);
    
    connect(m_pm, &ProjectManager::uiComponentAdded, this, &InstrumentCanvasView::onComponentAdded);
    connect(m_pm, &ProjectManager::uiComponentRemoved, this, &InstrumentCanvasView::onComponentRemoved);
    connect(m_pm->getUiState(), &UiStateModel::canvasSizeChanged, this, &InstrumentCanvasView::onCanvasSizeChanged);
    connect(m_pm->getUiState(), &UiStateModel::bgImageChanged, this, [this](const QString&) { m_scene->update(); });
    connect(m_pm->getUiState(), &UiStateModel::bgModeChanged, this, &InstrumentCanvasView::onBgModeChanged);
    connect(m_pm, &ProjectManager::uiComponentModified, this, &InstrumentCanvasView::onComponentModified);
}

void InstrumentCanvasView::onCanvasSizeChanged(int width, int height) {
    m_scene->setSceneRect(0, 0, width, height);
    m_scene->update();
    if (m_headerItem) {
        m_headerItem->setGeometry(QRectF(0, 0, width, 45));
        m_headerItem->setPos(0, 0);
    }
    if (m_keyboardItem) {
        m_keyboardItem->setGeometry(QRectF(0, 0, width, 75));
        m_keyboardItem->setPos(0, height - 75);
    }
    viewport()->update();
}

void InstrumentCanvasView::onBgModeChanged(BgMode mode) {
    m_scene->update();
}

void InstrumentCanvasView::showEvent(QShowEvent* event) {
    QGraphicsView::showEvent(event);
    if (!m_initialized) {
        centerOn(m_pm->getUiState()->getWidth() / 2.0, m_pm->getUiState()->getHeight() / 2.0);
        m_initialized = true;
    }
}

void InstrumentCanvasView::drawBackground(QPainter *painter, const QRectF &rect) {
    // 1. Draw outer background (Out of bounds)
    painter->fillRect(rect, ThemePalette::color("qss_color_1", QColor(25, 25, 25)));
    
    // 2. Define geometry
    int w = m_pm->getUiState()->getWidth();
    int h = m_pm->getUiState()->getHeight();
    
    QRectF artboard(0, 0, w, h);
    
    // Draw shadow for full plugin
    painter->fillRect(artboard.translated(5, 5), QColor(0, 0, 0, 150));
    
    // --- Draw Artboard ---
    QString bgPath = m_pm->getUiState()->getBgImage();
    if (!bgPath.isEmpty() && QFileInfo(bgPath).exists()) {
        QPixmap pm(bgPath);
        if (!pm.isNull()) {
            if (m_pm->getUiState()->getBgMode() == BgMode::Stretch || m_pm->getUiState()->getBgMode() == BgMode::Absolute) {
                painter->drawPixmap(artboard.toRect(), pm.scaled(artboard.size().toSize(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
            } else if (m_pm->getUiState()->getBgMode() == BgMode::Center) {
                QPixmap scaledPm = pm.scaled(artboard.size().toSize(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
                int px = artboard.x() + (artboard.width() - scaledPm.width()) / 2.0;
                int py = artboard.y() + (artboard.height() - scaledPm.height()) / 2.0;
                painter->fillRect(artboard, ThemePalette::color("qss_color_22", QColor(40, 40, 45))); // Fill background first
                painter->drawPixmap(px, py, scaledPm);
            }
        } else {
            painter->fillRect(artboard, ThemePalette::color("qss_color_22", QColor(40, 40, 45)));
        }
    } else {
        painter->fillRect(artboard, ThemePalette::color("qss_color_22", QColor(40, 40, 45)));
    }
    
    // Draw grid only inside the Artboard
    painter->setClipRect(artboard);
    painter->setPen(ThemePalette::color("qss_color_9", QColor(48, 48, 52)));
    int gridSize = 20;
    int left = int(artboard.left()) - (int(artboard.left()) % gridSize);
    int top = int(artboard.top()) - (int(artboard.top()) % gridSize);
    for (int x = left; x < artboard.right(); x += gridSize) painter->drawLine(x, artboard.top(), x, artboard.bottom());
    for (int y = top; y < artboard.bottom(); y += gridSize) painter->drawLine(artboard.left(), y, artboard.right(), y);
    painter->setClipping(false);
    
    // Draw distinct border around artboard
    painter->setPen(QPen(ThemePalette::color("qss_color_14", QColor(100, 100, 100)), 2));
    painter->drawRect(artboard);
}

void InstrumentCanvasView::drawForeground(QPainter *painter, const QRectF &rect) {
    Q_UNUSED(painter);
    Q_UNUSED(rect);
}



void InstrumentCanvasView::onComponentAdded(const QUuid& id) {
    if (UiComponent* comp = m_pm->getUiComponent(id)) {
        auto item = UiComponentItemFactory::create(comp);
        m_scene->addItem(item);
        m_items[id] = item;
        connect(item, &UiComponentItem::componentResized, this, &InstrumentCanvasView::onComponentResized);
    }
}

void InstrumentCanvasView::onComponentRemoved(const QUuid& id) {
    if (m_items.contains(id)) {
        m_scene->removeItem(m_items[id]);
        delete m_items[id];
        m_items.remove(id);
    }
}

void InstrumentCanvasView::onComponentModified(const QUuid& id, const QString& propertyName) {
    if (m_items.contains(id)) {
        if (UiComponent* comp = m_pm->getUiComponent(id)) {
            m_items[id]->updateFromComponent(comp);
        }
    }
}

void InstrumentCanvasView::onComponentResized(const QUuid& id, double newX, double newY, double newW, double newH) {
    auto comp = m_pm->getUiComponent(id);
    if (!comp) return;
    
    m_pm->getUndoStack()->beginMacro("Resize Component");
    if (comp->x != newX) m_pm->getUndoStack()->push(new ModifyUiPropertyCommand(m_pm, id, "x", comp->x, newX));
    if (comp->y != newY) m_pm->getUndoStack()->push(new ModifyUiPropertyCommand(m_pm, id, "y", comp->y, newY));
    if (comp->width != newW) m_pm->getUndoStack()->push(new ModifyUiPropertyCommand(m_pm, id, "width", comp->width, newW));
    if (comp->height != newH) m_pm->getUndoStack()->push(new ModifyUiPropertyCommand(m_pm, id, "height", comp->height, newH));
    m_pm->getUndoStack()->endMacro();
}

void InstrumentCanvasView::contextMenuEvent(QContextMenuEvent *event) {
    CanvasContextMenu::show(this, m_pm, m_scene, event);
}

void InstrumentCanvasView::mousePressEvent(QMouseEvent *event) {
    QGraphicsView::mousePressEvent(event);
    if (!m_scene->selectedItems().isEmpty()) {
        auto item = dynamic_cast<UiComponentItem*>(m_scene->selectedItems().first());
        if (item) {
            m_draggedCompId = item->getComponentId();
            m_dragStartPos = item->pos();
            emit componentSelected(m_draggedCompId);
        }
    } else {
        emit componentSelected(QUuid());
    }
}

void InstrumentCanvasView::mouseMoveEvent(QMouseEvent *event) {
    QGraphicsView::mouseMoveEvent(event);
}

void InstrumentCanvasView::mouseReleaseEvent(QMouseEvent *event) {
    // Process component movement releases always to keep UI component designer state correct
    QGraphicsView::mouseReleaseEvent(event);
    if (!m_draggedCompId.isNull()) {
        auto item = m_items.value(m_draggedCompId);
        if (item && item->pos() != m_dragStartPos) {
            QPointF newPos = item->pos();
            if (m_snapToGrid) {
                newPos.setX(std::round(newPos.x() / 10.0) * 10.0);
                newPos.setY(std::round(newPos.y() / 10.0) * 10.0);
            }
            item->setPos(m_dragStartPos); // Let undo framework move it, reverting visually first
            emit requestMoveComponent(m_draggedCompId, m_dragStartPos, newPos);
        }
        m_draggedCompId = QUuid();
    }
}



void InstrumentCanvasView::wheelEvent(QWheelEvent *event) {
    // Zoom if Ctrl is pressed
    if (event->modifiers() & Qt::ControlModifier) {
        setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
        double scaleFactor = 1.15;
        if (event->angleDelta().y() > 0) scale(scaleFactor, scaleFactor);
        else scale(1.0 / scaleFactor, 1.0 / scaleFactor);
    } else {
        QGraphicsView::wheelEvent(event);
    }
}

void InstrumentCanvasView::getViewport(double& x, double& y, double& zoom) const {
    x = horizontalScrollBar()->value();
    y = verticalScrollBar()->value();
    zoom = transform().m11();
}

void InstrumentCanvasView::setViewport(double x, double y, double zoom) {
    if (zoom <= 0) zoom = 1.0;
    QTransform t;
    t.scale(zoom, zoom);
    setTransform(t);
    horizontalScrollBar()->setValue(x);
    verticalScrollBar()->setValue(y);
}

void InstrumentCanvasView::alignSelectedItems(const QString& mode) {
    if (m_scene->selectedItems().size() < 2) return;
    
    QRectF bounding = m_scene->selectedItems().first()->sceneBoundingRect();
    for (auto item : m_scene->selectedItems()) {
        bounding = bounding.united(item->sceneBoundingRect());
    }
    
    m_pm->getUndoStack()->beginMacro("Align " + mode);
    for (auto item : m_scene->selectedItems()) {
        auto uiItem = dynamic_cast<UiComponentItem*>(item);
        if (!uiItem) continue;
        auto comp = m_pm->getUiComponent(uiItem->getComponentId());
        if (!comp) continue;
        
        double newX = comp->x;
        double newY = comp->y;
        
        if (mode == "left") newX = bounding.left();
        else if (mode == "right") newX = bounding.right() - comp->width;
        else if (mode == "center") newX = bounding.center().x() - comp->width / 2.0;
        else if (mode == "top") newY = bounding.top();
        else if (mode == "bottom") newY = bounding.bottom() - comp->height;
        
        if (newX != comp->x) m_pm->getUndoStack()->push(new ModifyUiPropertyCommand(m_pm, comp->id, "x", comp->x, newX));
        if (newY != comp->y) m_pm->getUndoStack()->push(new ModifyUiPropertyCommand(m_pm, comp->id, "y", comp->y, newY));
    }
    m_pm->getUndoStack()->endMacro();
}

void InstrumentCanvasView::dragEnterEvent(QDragEnterEvent *event) {
    CanvasDragDropHandler::handleDragEnter(this, event);
    if (!event->isAccepted()) QGraphicsView::dragEnterEvent(event);
}

void InstrumentCanvasView::dragMoveEvent(QDragMoveEvent *event) {
    CanvasDragDropHandler::handleDragMove(this, event);
    if (!event->isAccepted()) QGraphicsView::dragMoveEvent(event);
}

void InstrumentCanvasView::dropEvent(QDropEvent *event) {
    CanvasDragDropHandler::handleDrop(this, m_pm, event, m_snapToGrid);
    if (!event->isAccepted()) QGraphicsView::dropEvent(event);
}

