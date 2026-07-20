#pragma once
#include <QString>
#include <QVector>
#include <QUuid>
#include <QJsonObject>
#include <QJsonArray>
#include "AudioNodeBase.h"
#include "ZoneNode.h"
#include "Modulators.h"

struct OscillatorParams {
    QString waveform = "saw"; // sine, saw, square, triangle, noise, pluck, wavetable
    QString wavetableFile = "";
    double wavetablePosition = 0.0;
    bool wavetableFrameInterpolation = true;
    QString pluckType = "karplus_strong";
    double damping = 0.5;
    
    QJsonObject toJson() const {
        QJsonObject obj;
        obj["waveform"] = waveform;
        obj["wavetableFile"] = wavetableFile;
        obj["wavetablePosition"] = wavetablePosition;
        obj["wavetableFrameInterpolation"] = wavetableFrameInterpolation;
        obj["pluckType"] = pluckType;
        obj["damping"] = damping;
        return obj;
    }
};

// Represents a group of zones
struct SampleGroup : public Node {
    QVector<Zone> zones;
    QVector<ModRouting> routings;
    double volume = 0.0;
    double pan = 0.0;
    double tuning = 0.0;
    ADSR ampEnv;
    ADSR modEnv;
    
    bool isOscillator = false;
    bool isSynthContainer = false;
    QUuid synthParentId; // Null if it's a normal group or synth container
    bool oscEnabled = true;
    OscillatorParams oscParams;
    
    bool filterEnabled = true;
    QString filterType = "LowPass";
    double filterCutoff = 1000.0;
    double filterResonance = 1.0;
    double filterEnvDepth = 0.0;
    double filterKeyTrack = 0.0;
    
    QVector<QUuid> insertEffects;
    
    // Decent Sampler Muting & Triggers
    QString trigger = "attack"; // "attack", "release", "first", "legato"
    QString seqMode = "normal"; // "normal", "round_robin", "random", "true_random"
    int loCC64 = 0;
    int hiCC64 = 127;
    
    // Legato / Portamento
    bool legatoEnabled = false;
    double glideTime = 0.05; // seconds
    
    QString customTags = "";
    QString silencedByTags = "";
    QString silencingMode = "fast"; // "fast", "normal"
    
    void accept(INodeVisitor* v) const override { v->visit(this); }
    SampleGroup() { type = "SampleGroup"; }
    
    QVector<PortDefinition> getPorts() const override {
        return {
            {"audio_out", PortDefinition::Output, PortDefinition::Audio}
        };
    }
    
    QJsonObject toJson() const override {
        QJsonObject obj = Node::toJson();
        obj["volume"] = volume;
        obj["pan"] = pan;
        obj["tuning"] = tuning;
        obj["ampEnv"] = ampEnv.toJson();
        obj["modEnv"] = modEnv.toJson();
        obj["isOscillator"] = isOscillator;
        obj["isSynthContainer"] = isSynthContainer;
        obj["synthParentId"] = synthParentId.toString(QUuid::WithoutBraces);
        obj["oscEnabled"] = oscEnabled;
        obj["oscParams"] = oscParams.toJson();
        
        obj["filterEnabled"] = filterEnabled;
        obj["filterType"] = filterType;
        obj["filterCutoff"] = filterCutoff;
        obj["filterResonance"] = filterResonance;
        obj["filterEnvDepth"] = filterEnvDepth;
        obj["filterKeyTrack"] = filterKeyTrack;
        
        obj["trigger"] = trigger;
        obj["seqMode"] = seqMode;
        obj["loCC64"] = loCC64;
        obj["hiCC64"] = hiCC64;
        obj["legatoEnabled"] = legatoEnabled;
        obj["glideTime"] = glideTime;
        obj["customTags"] = customTags;
        obj["silencedByTags"] = silencedByTags;
        obj["silencingMode"] = silencingMode;
        
        QJsonArray zonesArray;
        for (const auto& z : zones) {
            zonesArray.append(z.toJson());
        }
        obj["zones"] = zonesArray;
        
        QJsonArray routingsArray;
        for (const auto& r : routings) {
            routingsArray.append(r.toJson());
        }
        obj["routings"] = routingsArray;
        
        QJsonArray effectsArray;
        for (const auto& fx : insertEffects) {
            effectsArray.append(fx.toString());
        }
        obj["insertEffects"] = effectsArray;
        
        return obj;
    }
    
    void loadFromJson(const QJsonObject& nodeObj) override {
        Node::loadFromJson(nodeObj);
        volume = nodeObj["volume"].toDouble();
        pan = nodeObj["pan"].toDouble();
        tuning = nodeObj["tuning"].toDouble();
        
        if (nodeObj.contains("ampEnv")) {
            QJsonObject env = nodeObj["ampEnv"].toObject();
            ampEnv.attack = env["attack"].toDouble();
            ampEnv.decay = env["decay"].toDouble();
            ampEnv.sustain = env["sustain"].toDouble();
            ampEnv.release = env["release"].toDouble();
        }
        
        if (nodeObj.contains("modEnv")) {
            QJsonObject mEnvObj = nodeObj["modEnv"].toObject();
            modEnv.attack = mEnvObj["attack"].toDouble();
            modEnv.decay = mEnvObj["decay"].toDouble();
            modEnv.sustain = mEnvObj["sustain"].toDouble();
            modEnv.release = mEnvObj["release"].toDouble();
        }
        
        isOscillator = nodeObj["isOscillator"].toBool(false);
        isSynthContainer = nodeObj["isSynthContainer"].toBool(false);
        QString parentIdStr = nodeObj["synthParentId"].toString();
        if (!parentIdStr.isEmpty()) synthParentId = QUuid(parentIdStr);
        oscEnabled = nodeObj["oscEnabled"].toBool(true);
        
        if (nodeObj.contains("oscParams")) {
            QJsonObject oObj = nodeObj["oscParams"].toObject();
            oscParams.waveform = oObj["waveform"].toString();
            oscParams.wavetableFile = oObj["wavetableFile"].toString();
            oscParams.wavetablePosition = oObj["wavetablePosition"].toDouble();
            oscParams.wavetableFrameInterpolation = oObj["wavetableFrameInterpolation"].toBool(true);
            oscParams.pluckType = oObj["pluckType"].toString();
            oscParams.damping = oObj["damping"].toDouble();
        }
        
        filterEnabled = nodeObj["filterEnabled"].toBool(true);
        filterType = nodeObj["filterType"].toString("LowPass");
        filterCutoff = nodeObj["filterCutoff"].toDouble(1000.0);
        filterResonance = nodeObj["filterResonance"].toDouble(1.0);
        filterEnvDepth = nodeObj["filterEnvDepth"].toDouble(0.0);
        filterKeyTrack = nodeObj["filterKeyTrack"].toDouble(0.0);
        
        if (nodeObj.contains("trigger")) trigger = nodeObj["trigger"].toString();
        if (nodeObj.contains("seqMode")) seqMode = nodeObj["seqMode"].toString();
        if (nodeObj.contains("loCC64")) loCC64 = nodeObj["loCC64"].toInt();
        if (nodeObj.contains("hiCC64")) hiCC64 = nodeObj["hiCC64"].toInt();
        if (nodeObj.contains("legatoEnabled")) legatoEnabled = nodeObj["legatoEnabled"].toBool();
        if (nodeObj.contains("glideTime")) glideTime = nodeObj["glideTime"].toDouble();
        if (nodeObj.contains("customTags")) customTags = nodeObj["customTags"].toString();
        if (nodeObj.contains("silencedByTags")) silencedByTags = nodeObj["silencedByTags"].toString();
        if (nodeObj.contains("silencingMode")) silencingMode = nodeObj["silencingMode"].toString();
        
        if (nodeObj.contains("zones")) {
            zones.clear();
            QJsonArray zonesArray = nodeObj["zones"].toArray();
            for (int z = 0; z < zonesArray.size(); ++z) {
                QJsonObject zObj = zonesArray[z].toObject();
                Zone zone;
                zone.samplePath = zObj["samplePath"].toString();
                zone.loNote = zObj["loNote"].toInt();
                zone.hiNote = zObj["hiNote"].toInt();
                zone.rootNote = zObj["rootNote"].toInt();
                zone.loVel = zObj["loVel"].toInt();
                zone.hiVel = zObj["hiVel"].toInt();
                zone.seqPosition = zObj["seqPosition"].toInt();
                zone.keySwitchNote = zObj["keySwitchNote"].toInt();
                zone.micLayer = zObj["micLayer"].toString();
                zone.loopEnabled = zObj["loopEnabled"].toBool();
                zone.loopStart = zObj["loopStart"].toDouble();
                zone.loopEnd = zObj["loopEnd"].toDouble();
                zone.loopCrossfade = zObj["loopCrossfade"].toDouble();
                zone.sampleStart = zObj["sampleStart"].toDouble();
                zone.sampleEnd = zObj["sampleEnd"].toDouble();
                zone.useLocalAmpEnv = zObj["useLocalAmpEnv"].toBool();
                if (zone.useLocalAmpEnv) {
                    QJsonObject zEnv = zObj["ampEnv"].toObject();
                    zone.ampEnv.attack = zEnv["attack"].toDouble();
                    zone.ampEnv.decay = zEnv["decay"].toDouble();
                    zone.ampEnv.sustain = zEnv["sustain"].toDouble();
                    zone.ampEnv.release = zEnv["release"].toDouble();
                }
                zones.push_back(zone);
            }
        }
        
        if (nodeObj.contains("routings")) {
            routings.clear();
            QJsonArray routingsArray = nodeObj["routings"].toArray();
            for (int r = 0; r < routingsArray.size(); ++r) {
                QJsonObject rObj = routingsArray[r].toObject();
                ModRouting routing;
                routing.source = rObj["source"].toString();
                routing.destination = rObj["destination"].toString();
                routing.amount = rObj["amount"].toDouble();
                routings.push_back(routing);
            }
        }
        
        if (nodeObj.contains("insertEffects")) {
            insertEffects.clear();
            QJsonArray effectsArray = nodeObj["insertEffects"].toArray();
            for (int i = 0; i < effectsArray.size(); ++i) {
                insertEffects.push_back(QUuid(effectsArray[i].toString()));
            }
        }
    }
};
