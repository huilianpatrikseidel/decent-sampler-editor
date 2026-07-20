#pragma once
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QMouseEvent>
#include <QUuid>
#include "core/ProjectManager.h"
#include "UiComponentItem.h"
#include "CanvasHeaderItem.h"
#include "CanvasKeyboardItem.h"

#include "../designer/UiDesignerExport.h"

class UIDESIGNER_EXPORT InstrumentCanvasView : public QGraphicsView {
    Q_OBJECT
public:
    explicit InstrumentCanvasView(ProjectManager* pm, QWidget* parent = nullptr);

signals:
    void componentSelected(const QUuid& id); 
    void requestMoveComponent(const QUuid& id, const QPointF& oldPos, const QPointF& newPos);
    void requestAddComponent(const QString& type, const QPointF& pos);
    void requestRemoveComponent(const QUuid& id);
    void viewportChanged(double x, double y, double zoom);
    void notePressed(int midiNote, int velocity);
    void noteReleased(int midiNote);

public slots:
    void onComponentAdded(const QUuid& id);
    void onComponentRemoved(const QUuid& id);
    void onComponentModified(const QUuid& id, const QString& propertyName);
    void onComponentResized(const QUuid& id, double newX, double newY, double newW, double newH);
    void onCanvasSizeChanged(int width, int height);
    void onBgModeChanged(BgMode mode);

    void getViewport(double& x, double& y, double& zoom) const;
    void setViewport(double x, double y, double zoom);
    
    void alignSelectedItems(const QString& mode);
    void setSnapToGrid(bool snap) { m_snapToGrid = snap; }

protected:
    void contextMenuEvent(QContextMenuEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void drawBackground(QPainter *painter, const QRectF &rect) override;
    void drawForeground(QPainter *painter, const QRectF &rect) override;
    void showEvent(QShowEvent *event) override;

private:
    ProjectManager* m_pm;
    QGraphicsScene* m_scene;
    QMap<QUuid, UiComponentItem*> m_items;
    
    CanvasHeaderItem* m_headerItem;
    CanvasKeyboardItem* m_keyboardItem;
    
    QUuid m_draggedCompId;
    QPointF m_dragStartPos;
    bool m_snapToGrid = true;
    bool m_initialized = false;
};
