#pragma once
#include <QString>
#include <QUuid>
#include <QJsonObject>
#include <QHash>

namespace std {
    template<> struct hash<QUuid> {
        size_t operator()(const QUuid& uid) const {
            return qHash(uid);
        }
    };
}

// Connections (Wires/Edges) for Node Graph
struct Connection {
    QUuid sourceId;
    QString sourcePort;
    QUuid targetId;
    QString targetPort;
    
    bool operator==(const Connection& other) const {
        return sourceId == other.sourceId && sourcePort == other.sourcePort &&
               targetId == other.targetId && targetPort == other.targetPort;
    }

    QJsonObject toJson() const {
        QJsonObject obj;
        obj["sourceId"] = sourceId.toString();
        obj["sourcePort"] = sourcePort;
        obj["targetId"] = targetId.toString();
        obj["targetPort"] = targetPort;
        return obj;
    }
};

// Definições de Portas para Node Graph (WYSIWYG)
struct PortDefinition {
    QString name;
    enum Direction { Input, Output } direction;
    enum Type { Audio, Midi, Control } type;
};

// Mod Matrix Routing
struct ModRouting {
    QUuid sourceId;
    QString source;      // e.g. "LFO1", "Env2", "Velocity", "ModWheel"
    QString destination; // e.g. "Pitch", "Volume", "Pan", "Cutoff"
    double amount = 0.0; // -1.0 to 1.0
    bool bipolar = false;
    
    // Novas propriedades de mapeamento avançado (MISS-11 e MISS-12)
    QString translation = "linear"; // "linear", "table", "fixed_value"
    QString translationTable = "";  // "0,33;0.5,1100;1.0,22000"
    double translationValue = 1.0;
    QString modBehavior = "add"; // add, multiply, set
    
    QJsonObject toJson() const {
        QJsonObject obj;
        obj["sourceId"] = sourceId.toString();
        obj["source"] = source;
        obj["destination"] = destination;
        obj["amount"] = amount;
        obj["bipolar"] = bipolar;
        obj["translation"] = translation;
        obj["translationTable"] = translationTable;
        obj["translationValue"] = translationValue;
        obj["modBehavior"] = modBehavior;
        return obj;
    }

    static ModRouting fromJson(const QJsonObject& obj) {
        ModRouting r;
        r.sourceId = QUuid(obj["sourceId"].toString());
        r.source = obj["source"].toString();
        r.destination = obj["destination"].toString();
        r.amount = obj["amount"].toDouble();
        r.bipolar = obj["bipolar"].toBool();
        r.translation = obj["translation"].toString("linear");
        r.translationTable = obj["translationTable"].toString();
        r.translationValue = obj["translationValue"].toDouble(1.0);
        r.modBehavior = obj["modBehavior"].toString("add");
        return r;
    }
};
