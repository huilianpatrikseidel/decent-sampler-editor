#pragma once
#include <QVector>
#include <QUuid>
#include <QJsonObject>
#include <QJsonArray>
#include "AudioNodeBase.h"

// Represents a submix bus
struct BusNode : public Node {
    double volume = 0.0;
    QVector<QUuid> insertEffects;
    
    void accept(INodeVisitor* v) const override { v->visit(this); }
    BusNode() { type = "Bus"; }
    
    QVector<PortDefinition> getPorts() const override {
        return {
            {"audio_in", PortDefinition::Input, PortDefinition::Audio},
            {"audio_out", PortDefinition::Output, PortDefinition::Audio}
        };
    }
    
    QJsonObject toJson() const override {
        QJsonObject obj = Node::toJson();
        obj["volume"] = volume;
        QJsonArray effectsArray;
        for (const auto& fx : insertEffects) {
            effectsArray.append(fx.toString());
        }
        obj["insertEffects"] = effectsArray;
        return obj;
    }
    
    void loadFromJson(const QJsonObject& obj) override {
        Node::loadFromJson(obj);
        volume = obj["volume"].toDouble();
        if (obj.contains("insertEffects")) {
            QJsonArray effectsArray = obj["insertEffects"].toArray();
            for (int i = 0; i < effectsArray.size(); ++i) {
                insertEffects.push_back(QUuid(effectsArray[i].toString()));
            }
        }
    }
};
