#include "GraphSerializer.h"
#include "../ProjectManager.h"
#include "../factories/NodeFactory.h"
#include <QJsonArray>

QJsonObject GraphSerializer::serialize(const ProjectManager* pm) {
    QJsonObject state;
    
    QJsonArray nodesArray;
    for (const auto& pair : pm->getAllNodes()) {
        QJsonObject nodeObj = pair.second->toJson();
        QPointF pos = pm->getNodePosition(pair.first);
        nodeObj["ui_x"] = pos.x();
        nodeObj["ui_y"] = pos.y();
        nodesArray.append(nodeObj);
    }
    state["nodes"] = nodesArray;
    
    QJsonArray connectionsArray;
    for (const auto& conn : pm->getAllConnections()) {
        connectionsArray.append(conn.toJson());
    }
    state["connections"] = connectionsArray;
    
    return state;
}

void GraphSerializer::deserialize(ProjectManager* pm, const QJsonObject& json) {
    if (json.contains("nodes")) {
        QJsonArray nodesArray = json["nodes"].toArray();
        for (int i = 0; i < nodesArray.size(); ++i) {
            QJsonObject nodeObj = nodesArray[i].toObject();
            auto node = NodeFactory::fromJson(nodeObj);
            if (node) {
                QUuid id = node->id;
                pm->addNode(std::move(node));
                if (nodeObj.contains("ui_x") && nodeObj.contains("ui_y")) {
                    pm->setNodePosition(id, nodeObj["ui_x"].toDouble(), nodeObj["ui_y"].toDouble());
                }
            }
        }
    }
    
    if (json.contains("connections")) {
        QJsonArray connectionsArray = json["connections"].toArray();
        for (int i = 0; i < connectionsArray.size(); ++i) {
            QJsonObject connObj = connectionsArray[i].toObject();
            Connection c;
            c.sourceId = QUuid(connObj["sourceId"].toString());
            c.sourcePort = connObj["sourcePort"].toString();
            c.targetId = QUuid(connObj["targetId"].toString());
            c.targetPort = connObj["targetPort"].toString();
            pm->addConnection(c);
        }
    }
}
