#include "GraphNodeItem.h"
#include <QPainter>
#include <QGraphicsScene>
#include "../../core/ThemePalette.h"

GraphNodeItem::GraphNodeItem(const Node* node, const QPointF& pos, QGraphicsItem* parent) 
    : QGraphicsItem(parent), m_nodeId(node->id), m_name(node->name), 
      m_nodeType(node->type), m_ports(node->getPorts()) {
    setFlag(ItemIsMovable);
    setFlag(ItemIsSelectable);
    setFlag(ItemSendsGeometryChanges);
    setCacheMode(DeviceCoordinateCache); // Performance optimization
    setPos(pos);
}

void GraphNodeItem::updateFromNode(const Node* node, const QPointF& pos) {
    if (node->id == m_nodeId) {
        m_name = node->name;
        setPos(pos);
        update();
    }
}

QRectF GraphNodeItem::boundingRect() const {
    return QRectF(0, 0, m_width, m_height);
}

void GraphNodeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    Q_UNUSED(option);
    Q_UNUSED(widget);
    
    // Draw Background
    painter->setBrush(isSelected() ? ThemePalette::color("qss_color_23", QColor(80, 100, 150)) : ThemePalette::color("qss_color_22", QColor(50, 50, 50)));
    painter->setPen(QPen(ThemePalette::color("qss_color_9", Qt::black), 2));
    painter->drawRoundedRect(0, 0, m_width, m_height, 5, 5);
    
    // Draw Title Header
    painter->setBrush(m_nodeType == "SampleGroup" ? QColor(0, 150, 136) : QColor(255, 152, 0));
    painter->drawRoundedRect(0, 0, m_width, 25, 5, 5);
    
    painter->setPen(ThemePalette::color("qss_color_15", Qt::white));
    painter->drawText(QRectF(0, 0, m_width, 25), Qt::AlignCenter, m_name.isEmpty() ? m_nodeType : m_name);
    
    // Draw Ports
    painter->setBrush(ThemePalette::color("qss_color_35", Qt::darkGray));
    for (int i = 0; i < m_ports.size(); ++i) {
        QPointF pos = getPortPosition(m_ports[i].name);
        painter->setPen(ThemePalette::color("qss_color_9", Qt::black));
        painter->drawEllipse(pos - QPointF(5, 5), 10, 10);
        painter->setPen(ThemePalette::color("qss_color_8", Qt::white));
        painter->drawText(QRectF(pos.x() > 50 ? pos.x() - 100 : pos.x() + 15, pos.y() - 10, 80, 20), 
                          m_ports[i].direction == PortDefinition::Input ? Qt::AlignLeft : Qt::AlignRight, 
                          m_ports[i].name);
    }
}

QPointF GraphNodeItem::getPortPosition(const QString& portName) const {
    int inCount = 0, outCount = 0;
    for (const auto& p : m_ports) {
        if (p.name == portName) {
            if (p.direction == PortDefinition::Input) return QPointF(0, 45 + inCount * 20);
            else return QPointF(m_width, 45 + outCount * 20);
        }
        if (p.direction == PortDefinition::Input) inCount++;
        else outCount++;
    }
    return QPointF(0,0);
}

QString GraphNodeItem::getPortAt(const QPointF& localPos) const {
    for (const auto& p : m_ports) {
        if (QLineF(localPos, getPortPosition(p.name)).length() < 12.0) {
            return p.name;
        }
    }
    return QString();
}

QVariant GraphNodeItem::itemChange(GraphicsItemChange change, const QVariant &value) {
    return QGraphicsItem::itemChange(change, value);
}
