#pragma once
#include <QString>
#include <QUuid>
#include <QJsonObject>

// Modulators
struct ADSR {
    double attack = 0.0;
    double decay = 0.0;
    double sustain = 1.0;
    double release = 0.1;
    
    QJsonObject toJson() const {
        QJsonObject obj;
        obj["attack"] = attack; obj["decay"] = decay;
        obj["sustain"] = sustain; obj["release"] = release;
        return obj;
    }
};

struct LFO {
    QUuid id;
    QString name = "LFO";
    QString shape = "sine";
    double frequency = 1.0;
    double amount = 0.0;
    QString scope = "global"; // "global" or "voice"
    double delayTime = 0.0;
    bool enabled = true;

    LFO() : id(QUuid::createUuid()) {}

    QJsonObject toJson() const {
        QJsonObject obj;
        obj["id"] = id.toString(QUuid::WithoutBraces);
        obj["name"] = name;
        obj["shape"] = shape;
        obj["frequency"] = frequency;
        obj["amount"] = amount;
        obj["scope"] = scope;
        obj["delayTime"] = delayTime;
        obj["enabled"] = enabled;
        return obj;
    }

    static LFO fromJson(const QJsonObject& obj) {
        LFO lfo;
        if (obj.contains("id")) lfo.id = QUuid(obj["id"].toString());
        if (obj.contains("name")) lfo.name = obj["name"].toString();
        lfo.shape = obj["shape"].toString("sine");
        lfo.frequency = obj["frequency"].toDouble(1.0);
        lfo.amount = obj["amount"].toDouble(0.0);
        lfo.scope = obj["scope"].toString("global");
        lfo.delayTime = obj["delayTime"].toDouble(0.0);
        lfo.enabled = obj["enabled"].toBool(true);
        return lfo;
    }
};
