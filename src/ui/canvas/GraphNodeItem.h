#pragma once
#include <QGraphicsItem>
#include <QUuid>
#include "core/models/AudioNodes.h"
#include "core/models/UiComponents.h"
#include "core/models/GraphStructures.h"

class GraphNodeItem : public QGraphicsItem {
public:
    GraphNodeItem(const Node* node, const QPointF& pos, QGraphicsItem* parent = nullptr);
    
    enum { Type = UserType + 1 };
    int type() const override { return Type; }
    
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    
    QUuid getNodeId() const { return m_nodeId; }
    QPointF getPortPosition(const QString& portName) const;
    QString getPortAt(const QPointF& localPos) const;
    void updateFromNode(const Node* node, const QPointF& pos);

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

private:
    QUuid m_nodeId;
    QString m_name;
    QString m_nodeType;
    QVector<PortDefinition> m_ports;
    
    int m_width = 120;
    int m_height = 80;
};
