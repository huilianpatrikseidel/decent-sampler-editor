#include "NodeGraphView.h"
#include <QMenu>
#include <QGraphicsSceneMouseEvent>
#include "../../core/ThemePalette.h"

NodeGraphView::NodeGraphView(ProjectManager* pm, QWidget* parent) 
    : QGraphicsView(parent), m_pm(pm) {
    
    m_scene = new QGraphicsScene(this);
    m_scene->setSceneRect(-5000, -5000, 10000, 10000);
    setScene(m_scene);
    
    setRenderHint(QPainter::Antialiasing);
    setViewportUpdateMode(QGraphicsView::SmartViewportUpdate); 
    setDragMode(QGraphicsView::ScrollHandDrag); 
    setBackgroundBrush(ThemePalette::color("qss_color_1", QColor(30, 30, 30)));
}

void NodeGraphView::onNodeAdded(const QUuid& id) {
    if (Node* node = m_pm->getNode(id)) {
        if (node->type == "SampleGroup" || node->type == "Bus") {
            GraphNodeItem* item = new GraphNodeItem(node, m_pm->getNodePosition(id));
            m_scene->addItem(item);
            m_nodeItems[id] = item;
        }
    }
}

void NodeGraphView::onNodeRemoved(const QUuid& id) {
    if (m_nodeItems.contains(id)) {
        m_scene->removeItem(m_nodeItems[id]);
        delete m_nodeItems[id];
        m_nodeItems.remove(id);
    }
}

void NodeGraphView::onNodeModified(const QUuid& id, const QString& propertyName) {
    if (m_nodeItems.contains(id)) {
        if (Node* node = m_pm->getNode(id)) {
            m_nodeItems[id]->updateFromNode(node, m_pm->getNodePosition(id));
            updateConnections();
        }
    }
}

void NodeGraphView::onConnectionAdded(const Connection& conn) {
    if (m_nodeItems.contains(conn.sourceId) && m_nodeItems.contains(conn.targetId)) {
        auto item = new GraphConnectionItem(m_nodeItems[conn.sourceId], conn.sourcePort,
                                            m_nodeItems[conn.targetId], conn.targetPort, conn);
        m_scene->addItem(item);
        m_connItems.append(item);
    }
}

void NodeGraphView::onConnectionRemoved(const Connection& conn) {
    for (int i = 0; i < m_connItems.size(); ++i) {
        if (m_connItems[i]->getConnection() == conn) {
            m_scene->removeItem(m_connItems[i]);
            delete m_connItems[i];
            m_connItems.removeAt(i);
            break;
        }
    }
}

void NodeGraphView::updateConnections() {
    for (auto c : m_connItems) {
        c->updatePosition();
    }
}

void NodeGraphView::contextMenuEvent(QContextMenuEvent *event) {
    QMenu menu(this);
    QAction* addSg = menu.addAction("Add SampleGroup");
    QAction* addBus = menu.addAction("Add Bus");
    menu.addSeparator();
    QAction* autoRR = menu.addAction("Auto-Distribute Round Robins");
    menu.addSeparator();
    QAction* resetView = menu.addAction("Fit to Screen (Reset View)");
    menu.addSeparator();
    QAction* removeNode = menu.addAction("Remove Selected Node");
    QAction* removeConn = menu.addAction("Remove Selected Connection");
    
    QAction* selected = menu.exec(event->globalPos());
    if (selected == addSg) emit requestAddNode("SampleGroup", mapToScene(event->pos()));
    else if (selected == addBus) emit requestAddNode("Bus", mapToScene(event->pos()));
    else if (selected == autoRR) {
        if (!m_scene->selectedItems().isEmpty()) {
            GraphNodeItem* item = dynamic_cast<GraphNodeItem*>(m_scene->selectedItems().first());
            if (item) emit requestAutoRR(item->getNodeId());
        }
    }
    else if (selected == resetView) {
        if (!m_scene->items().isEmpty()) {
            fitInView(m_scene->itemsBoundingRect(), Qt::KeepAspectRatio);
        }
    }
    else if (selected == removeNode) {
        if (!m_scene->selectedItems().isEmpty()) {
            GraphNodeItem* item = dynamic_cast<GraphNodeItem*>(m_scene->selectedItems().first());
            if (item) emit requestRemoveNode(item->getNodeId());
        }
    }
    else if (selected == removeConn) {
        if (!m_scene->selectedItems().isEmpty()) {
            GraphConnectionItem* item = dynamic_cast<GraphConnectionItem*>(m_scene->selectedItems().first());
            if (item) emit requestRemoveConnection(item->getConnection());
        }
    }
}

void NodeGraphView::mousePressEvent(QMouseEvent *event) {
    QPointF scenePos = mapToScene(event->pos());
    QGraphicsItem* item = m_scene->itemAt(scenePos, transform());
    if (GraphNodeItem* nodeItem = dynamic_cast<GraphNodeItem*>(item)) {
        QString port = nodeItem->getPortAt(nodeItem->mapFromScene(scenePos));
        if (!port.isEmpty()) {
            m_dragConnSourceId = nodeItem->getNodeId();
            m_dragConnSourcePort = port;
            m_tempConnItem = new QGraphicsPathItem();
            m_tempConnItem->setPen(QPen(QColor(255, 255, 0), 3, Qt::DashLine));
            m_tempConnItem->setZValue(100);
            m_scene->addItem(m_tempConnItem);
            return;
        }
    }

    QGraphicsView::mousePressEvent(event);
    if (!m_scene->selectedItems().isEmpty()) {
        GraphNodeItem* nodeItem = dynamic_cast<GraphNodeItem*>(m_scene->selectedItems().first());
        if (nodeItem) {
            m_draggedNodeId = nodeItem->getNodeId();
            m_dragStartPos = nodeItem->pos();
            emit nodeSelected(m_draggedNodeId);
        }
    } else {
        emit nodeSelected(QUuid()); // Deselect
    }
}

void NodeGraphView::mouseMoveEvent(QMouseEvent *event) {
    if (m_tempConnItem && !m_dragConnSourceId.isNull()) {
        QPointF scenePos = mapToScene(event->pos());
        GraphNodeItem* sourceItem = m_nodeItems.value(m_dragConnSourceId);
        if (sourceItem) {
            QPointF start = sourceItem->mapToScene(sourceItem->getPortPosition(m_dragConnSourcePort));
            QPainterPath path;
            path.moveTo(start);
            qreal dx = scenePos.x() - start.x();
            QPointF ctrl1 = start + QPointF(qMax(qreal(50.0), dx/2.0), 0);
            QPointF ctrl2 = scenePos - QPointF(qMax(qreal(50.0), dx/2.0), 0);
            path.cubicTo(ctrl1, ctrl2, scenePos);
            
            // Highlight color based on connection validity
            QColor connColor(255, 255, 0); // Yellow while dragging freely
            QGraphicsItem* targetItem = m_scene->itemAt(scenePos, transform());
            if (GraphNodeItem* nodeTarget = dynamic_cast<GraphNodeItem*>(targetItem)) {
                QString targetPort = nodeTarget->getPortAt(nodeTarget->mapFromScene(scenePos));
                if (!targetPort.isEmpty()) {
                    Connection testConn{m_dragConnSourceId, m_dragConnSourcePort, nodeTarget->getNodeId(), targetPort};
                    if (m_pm->canConnect(testConn)) {
                        connColor = QColor(0, 255, 0); // Green (Valid)
                    } else {
                        connColor = QColor(255, 0, 0); // Red (Invalid)
                    }
                }
            }
            m_tempConnItem->setPen(QPen(connColor, 3, Qt::DashLine));
            m_tempConnItem->setPath(path);
        }
        return;
    }
    QGraphicsView::mouseMoveEvent(event);
}

void NodeGraphView::mouseReleaseEvent(QMouseEvent *event) {
    if (m_tempConnItem) {
        QPointF scenePos = mapToScene(event->pos());
        QGraphicsItem* targetItem = m_scene->itemAt(scenePos, transform());
        if (GraphNodeItem* nodeTarget = dynamic_cast<GraphNodeItem*>(targetItem)) {
            QString targetPort = nodeTarget->getPortAt(nodeTarget->mapFromScene(scenePos));
            if (!targetPort.isEmpty()) {
                Connection conn{m_dragConnSourceId, m_dragConnSourcePort, nodeTarget->getNodeId(), targetPort};
                if (m_pm->canConnect(conn)) {
                    m_pm->addConnection(conn);
                }
            }
        }
        
        m_scene->removeItem(m_tempConnItem);
        delete m_tempConnItem;
        m_tempConnItem = nullptr;
        m_dragConnSourceId = QUuid();
        m_dragConnSourcePort = QString();
        return;
    }

    QGraphicsView::mouseReleaseEvent(event);
    if (!m_draggedNodeId.isNull()) {
        GraphNodeItem* item = m_nodeItems.value(m_draggedNodeId);
        if (item && item->pos() != m_dragStartPos) {
            emit requestMoveNode(m_draggedNodeId, m_dragStartPos, item->pos());
            // Revert local position to allow UndoCommand to handle the actual logic and trigger onNodeModified
            item->setPos(m_dragStartPos); 
        }
        m_draggedNodeId = QUuid();
    }
    updateConnections(); 
}

void NodeGraphView::wheelEvent(QWheelEvent *event) {
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    double scaleFactor = 1.15;
    if (event->angleDelta().y() > 0) {
        scale(scaleFactor, scaleFactor);
    } else {
        scale(1.0 / scaleFactor, 1.0 / scaleFactor);
    }
}
