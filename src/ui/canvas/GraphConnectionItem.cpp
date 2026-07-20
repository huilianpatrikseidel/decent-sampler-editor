#include "GraphConnectionItem.h"
#include <QPen>
#include "../../core/ThemePalette.h"

GraphConnectionItem::GraphConnectionItem(GraphNodeItem* source, const QString& srcPort, 
                                         GraphNodeItem* target, const QString& tgtPort, 
                                         const Connection& conn, QGraphicsItem* parent)
    : QGraphicsPathItem(parent), m_source(source), m_target(target), 
      m_srcPort(srcPort), m_tgtPort(tgtPort), m_conn(conn) {
    
    setPen(QPen(ThemePalette::color("qss_color_17", QColor(150, 150, 150)), 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    setZValue(-1); // Draw behind nodes
    setFlag(QGraphicsItem::ItemIsSelectable);
    updatePosition();
}

void GraphConnectionItem::updatePosition() {
    if (!m_source || !m_target) return;
    
    QPointF start = m_source->mapToScene(m_source->getPortPosition(m_srcPort));
    QPointF end = m_target->mapToScene(m_target->getPortPosition(m_tgtPort));
    
    QPainterPath path;
    path.moveTo(start);
    // Bezier control points for smooth wire rendering
    qreal dx = end.x() - start.x();
    QPointF ctrl1 = start + QPointF(qMax(qreal(50.0), dx/2.0), 0);
    QPointF ctrl2 = end - QPointF(qMax(qreal(50.0), dx/2.0), 0);
    path.cubicTo(ctrl1, ctrl2, end);
    
    setPath(path);
}
