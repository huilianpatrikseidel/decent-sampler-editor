#pragma once
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QMouseEvent>
#include <QUuid>
#include "core/ProjectManager.h"
#include "GraphNodeItem.h"
#include "GraphConnectionItem.h"

class NodeGraphView : public QGraphicsView {
    Q_OBJECT
public:
    explicit NodeGraphView(ProjectManager* pm, QWidget* parent = nullptr);

signals:
    void nodeSelected(const QUuid& id); 
    void requestMoveNode(const QUuid& id, const QPointF& oldPos, const QPointF& newPos);
    void requestAutoRR(const QUuid& sgId);
    void requestAddNode(const QString& type, const QPointF& pos);
    void requestRemoveNode(const QUuid& id);
    void requestAddConnection(const Connection& conn);
    void requestRemoveConnection(const Connection& conn);

public slots:
    void onNodeAdded(const QUuid& id);
    void onNodeRemoved(const QUuid& id);
    void onNodeModified(const QUuid& id, const QString& propertyName);
    void onConnectionAdded(const Connection& conn);
    void onConnectionRemoved(const Connection& conn);

protected:
    void contextMenuEvent(QContextMenuEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

private:
    ProjectManager* m_pm;
    QGraphicsScene* m_scene;
    QMap<QUuid, GraphNodeItem*> m_nodeItems;
    QList<GraphConnectionItem*> m_connItems;
    
    QUuid m_draggedNodeId;
    QPointF m_dragStartPos;
    
    QUuid m_dragConnSourceId;
    QString m_dragConnSourcePort;
    QGraphicsPathItem* m_tempConnItem = nullptr;
    
    void updateConnections();
};
