#pragma once
#include <QString>
#include <QVector>
#include <QUuid>
#include <QJsonObject>
#include <QJsonArray>
#include "../../INodeVisitor.h"
#include "../GraphStructures.h"

#include "../../CoreLibExport.h"

// Agnostic Base Node for Node Graph
struct CORELIB_EXPORT Node {
    QUuid id;
    QString name;
    QString type;
    bool bypassed = false;
    bool muted = false;
    bool soloed = false;
    QString tags = "";
    
    Node() : id(QUuid::createUuid()) {}
    virtual ~Node() = default;
    
    virtual QVector<PortDefinition> getPorts() const { return {}; }
    
    virtual void accept(INodeVisitor* v) const {}
    
    virtual QJsonObject toJson() const {
        QJsonObject obj;
        obj["id"] = id.toString();
        obj["name"] = name;
        obj["type"] = type;
        obj["bypassed"] = bypassed;
        obj["muted"] = muted;
        obj["soloed"] = soloed;
        obj["tags"] = tags;
        return obj;
    }
    
    virtual void loadFromJson(const QJsonObject& obj) {
        id = QUuid(obj["id"].toString());
        name = obj["name"].toString();
        bypassed = obj["bypassed"].toBool(false);
        muted = obj["muted"].toBool(false);
        soloed = obj["soloed"].toBool(false);
        tags = obj["tags"].toString("");
    }
};
