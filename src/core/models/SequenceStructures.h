#pragma once
#include <QString>
#include <QVector>
#include <QJsonObject>
#include <QJsonArray>
#include <QUuid>

struct SequenceStep {
    double position = 0.0;
    double velocity = 1.0;
    int note = 60;
    double length = 1.0;

    QJsonObject toJson() const {
        QJsonObject obj;
        obj["position"] = position;
        obj["velocity"] = velocity;
        obj["note"] = note;
        obj["length"] = length;
        return obj;
    }
    
    static SequenceStep fromJson(const QJsonObject& obj) {
        SequenceStep s;
        s.position = obj["position"].toDouble(0.0);
        s.velocity = obj["velocity"].toDouble(1.0);
        s.note = obj["note"].toInt(60);
        s.length = obj["length"].toDouble(1.0);
        return s;
    }
};

struct NoteSequence {
    QUuid id;
    QString name = "arp";
    int length = 4;
    double rate = 1.0;
    int timeSignatureNumerator = 4;
    int timeSignatureDenominator = 4;
    QVector<SequenceStep> steps;

    NoteSequence() : id(QUuid::createUuid()) {}

    QJsonObject toJson() const {
        QJsonObject obj;
        obj["id"] = id.toString();
        obj["name"] = name;
        obj["length"] = length;
        obj["rate"] = rate;
        obj["tsNum"] = timeSignatureNumerator;
        obj["tsDen"] = timeSignatureDenominator;
        QJsonArray stepsArr;
        for (const auto& step : steps) {
            stepsArr.append(step.toJson());
        }
        obj["steps"] = stepsArr;
        return obj;
    }
    
    static NoteSequence fromJson(const QJsonObject& obj) {
        NoteSequence s;
        if (obj.contains("id")) s.id = QUuid(obj["id"].toString());
        s.name = obj["name"].toString("arp");
        s.length = obj["length"].toInt(4);
        s.rate = obj["rate"].toDouble(1.0);
        s.timeSignatureNumerator = obj["tsNum"].toInt(4);
        s.timeSignatureDenominator = obj["tsDen"].toInt(4);
        
        QJsonArray arr = obj["steps"].toArray();
        for (int i = 0; i < arr.size(); ++i) {
            s.steps.append(SequenceStep::fromJson(arr[i].toObject()));
        }
        return s;
    }
};
