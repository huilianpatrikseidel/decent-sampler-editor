#pragma once
#include <QGraphicsPathItem>
#include "GraphNodeItem.h"
#include "core/models/AudioNodes.h"
#include "core/models/UiComponents.h"
#include "core/models/GraphStructures.h"

class GraphConnectionItem : public QGraphicsPathItem {
public:
    GraphConnectionItem(GraphNodeItem* source, const QString& srcPort, 
                        GraphNodeItem* target, const QString& tgtPort, 
                        const Connection& conn, QGraphicsItem* parent = nullptr);
    
    void updatePosition();
    Connection getConnection() const { return m_conn; }

private:
    GraphNodeItem* m_source;
    GraphNodeItem* m_target;
    QString m_srcPort;
    QString m_tgtPort;
    Connection m_conn;
};
