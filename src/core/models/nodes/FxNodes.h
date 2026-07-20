#pragma once
#include <QString>
#include <QVector>
#include <QJsonObject>
#include <QJsonArray>
#include "AudioNodeBase.h"

struct DelayNode : public Node {
    double time = 0.5; // seconds
    double feedback = 0.3;
    double wetLevel = 0.5;
    double stereoOffset = 0.0;
    void accept(INodeVisitor* v) const override { v->visit(this); }
    DelayNode() { type = "Delay"; }
    QVector<PortDefinition> getPorts() const override {
        return { {"audio_in", PortDefinition::Input, PortDefinition::Audio}, {"audio_out", PortDefinition::Output, PortDefinition::Audio} };
    }
    QJsonObject toJson() const override {
        QJsonObject obj = Node::toJson(); obj["time"] = time; obj["feedback"] = feedback; 
        obj["wetLevel"] = wetLevel; obj["stereoOffset"] = stereoOffset; return obj;
    }
    void loadFromJson(const QJsonObject& obj) override {
        Node::loadFromJson(obj);
        time = obj["time"].toDouble();
        feedback = obj["feedback"].toDouble();
        wetLevel = obj["wetLevel"].toDouble();
        stereoOffset = obj["stereoOffset"].toDouble();
    }
};

struct ReverbNode : public Node {
    double size = 0.5;
    double damp = 0.5;
    double wetLevel = 0.5;
    QString irPath; // For Convolution Reverb
    
    void accept(INodeVisitor* v) const override { v->visit(this); }
    ReverbNode() { type = "Reverb"; }
    QVector<PortDefinition> getPorts() const override {
        return { {"audio_in", PortDefinition::Input, PortDefinition::Audio}, {"audio_out", PortDefinition::Output, PortDefinition::Audio} };
    }
    QJsonObject toJson() const override {
        QJsonObject obj = Node::toJson(); 
        obj["size"] = size; 
        obj["damp"] = damp; 
        obj["wetLevel"] = wetLevel;
        obj["irPath"] = irPath;
        return obj;
    }
    void loadFromJson(const QJsonObject& obj) override {
        Node::loadFromJson(obj);
        size = obj["size"].toDouble();
        damp = obj["damp"].toDouble();
        wetLevel = obj["wetLevel"].toDouble();
        irPath = obj["irPath"].toString();
    }
};

struct FilterNode : public Node {
    QString filterType = "LowPass";
    double cutoff = 1000.0;
    double resonance = 1.0;
    void accept(INodeVisitor* v) const override { v->visit(this); }
    FilterNode() { type = "Filter"; }
    QVector<PortDefinition> getPorts() const override {
        return { {"audio_in", PortDefinition::Input, PortDefinition::Audio}, {"audio_out", PortDefinition::Output, PortDefinition::Audio} };
    }
    QJsonObject toJson() const override {
        QJsonObject obj = Node::toJson(); obj["filterType"] = filterType; obj["cutoff"] = cutoff; obj["resonance"] = resonance; return obj;
    }
    void loadFromJson(const QJsonObject& obj) override {
        Node::loadFromJson(obj);
        filterType = obj["filterType"].toString();
        cutoff = obj["cutoff"].toDouble();
        resonance = obj["resonance"].toDouble();
    }
};

struct ChorusNode : public Node {
    double rate = 1.0;
    double depth = 0.5;
    double mix = 0.5;
    void accept(INodeVisitor* v) const override { v->visit(this); }
    ChorusNode() { type = "Chorus"; }
    QVector<PortDefinition> getPorts() const override {
        return { {"audio_in", PortDefinition::Input, PortDefinition::Audio}, {"audio_out", PortDefinition::Output, PortDefinition::Audio} };
    }
    QJsonObject toJson() const override {
        QJsonObject obj = Node::toJson(); obj["rate"] = rate; obj["depth"] = depth; obj["mix"] = mix; return obj;
    }
    void loadFromJson(const QJsonObject& obj) override {
        Node::loadFromJson(obj);
        rate = obj["rate"].toDouble();
        depth = obj["depth"].toDouble();
        mix = obj["mix"].toDouble();
    }
};

struct GainNode : public Node {
    double level = 0.0;
    QString levelUnit = "decibels"; // or "linear"
    void accept(INodeVisitor* v) const override { v->visit(this); }
    GainNode() { type = "Gain"; }
    QVector<PortDefinition> getPorts() const override { return { {"audio_in", PortDefinition::Input, PortDefinition::Audio}, {"audio_out", PortDefinition::Output, PortDefinition::Audio} }; }
    QJsonObject toJson() const override { QJsonObject obj = Node::toJson(); obj["level"] = level; obj["levelUnit"] = levelUnit; return obj; }
    void loadFromJson(const QJsonObject& obj) override {
        Node::loadFromJson(obj);
        level = obj["level"].toDouble();
        levelUnit = obj["levelUnit"].toString("decibels");
    }
};

struct PhaserNode : public Node {
    double mix = 0.5;
    double modDepth = 0.2;
    double modRate = 0.2;
    double centerFrequency = 400.0;
    double feedback = 0.7;
    void accept(INodeVisitor* v) const override { v->visit(this); }
    PhaserNode() { type = "Phaser"; }
    QVector<PortDefinition> getPorts() const override { return { {"audio_in", PortDefinition::Input, PortDefinition::Audio}, {"audio_out", PortDefinition::Output, PortDefinition::Audio} }; }
    QJsonObject toJson() const override { QJsonObject obj = Node::toJson(); obj["mix"] = mix; obj["modDepth"] = modDepth; obj["modRate"] = modRate; obj["centerFrequency"] = centerFrequency; obj["feedback"] = feedback; return obj; }
    void loadFromJson(const QJsonObject& obj) override {
        Node::loadFromJson(obj);
        mix = obj["mix"].toDouble();
        modDepth = obj["modDepth"].toDouble();
        modRate = obj["modRate"].toDouble();
        centerFrequency = obj["centerFrequency"].toDouble();
        feedback = obj["feedback"].toDouble();
    }
};

struct PitchShifterNode : public Node {
    double pitchShift = 0.0; // -24 to 24
    double mix = 0.5;
    void accept(INodeVisitor* v) const override { v->visit(this); }
    PitchShifterNode() { type = "PitchShifter"; }
    QVector<PortDefinition> getPorts() const override { return { {"audio_in", PortDefinition::Input, PortDefinition::Audio}, {"audio_out", PortDefinition::Output, PortDefinition::Audio} }; }
    QJsonObject toJson() const override { QJsonObject obj = Node::toJson(); obj["pitchShift"] = pitchShift; obj["mix"] = mix; return obj; }
    void loadFromJson(const QJsonObject& obj) override {
        Node::loadFromJson(obj);
        pitchShift = obj["pitchShift"].toDouble();
        mix = obj["mix"].toDouble();
    }
};

struct WaveFolderNode : public Node {
    double drive = 1.0;
    double threshold = 0.25;
    void accept(INodeVisitor* v) const override { v->visit(this); }
    WaveFolderNode() { type = "WaveFolder"; }
    QVector<PortDefinition> getPorts() const override { return { {"audio_in", PortDefinition::Input, PortDefinition::Audio}, {"audio_out", PortDefinition::Output, PortDefinition::Audio} }; }
    QJsonObject toJson() const override { QJsonObject obj = Node::toJson(); obj["drive"] = drive; obj["threshold"] = threshold; return obj; }
    void loadFromJson(const QJsonObject& obj) override {
        Node::loadFromJson(obj);
        drive = obj["drive"].toDouble();
        threshold = obj["threshold"].toDouble();
    }
};

struct WaveShaperNode : public Node {
    double drive = 1.0;
    double driveBoost = 1.0;
    double outputLevel = 0.1;
    bool highQuality = true;
    void accept(INodeVisitor* v) const override { v->visit(this); }
    WaveShaperNode() { type = "WaveShaper"; }
    QVector<PortDefinition> getPorts() const override { return { {"audio_in", PortDefinition::Input, PortDefinition::Audio}, {"audio_out", PortDefinition::Output, PortDefinition::Audio} }; }
    QJsonObject toJson() const override { QJsonObject obj = Node::toJson(); obj["drive"] = drive; obj["driveBoost"] = driveBoost; obj["outputLevel"] = outputLevel; obj["highQuality"] = highQuality; return obj; }
    void loadFromJson(const QJsonObject& obj) override {
        Node::loadFromJson(obj);
        drive = obj["drive"].toDouble();
        driveBoost = obj["driveBoost"].toDouble();
        outputLevel = obj["outputLevel"].toDouble();
        highQuality = obj["highQuality"].toBool(true);
    }
};

struct StereoSimulatorNode : public Node {
    QString algorithm = "adt"; // lauridsen, schroeder, adt
    double width = 0.5;
    double delayTime = 0.005;
    double modRate = 0.5;
    double modDepth = 0.3;
    void accept(INodeVisitor* v) const override { v->visit(this); }
    StereoSimulatorNode() { type = "StereoSimulator"; }
    QVector<PortDefinition> getPorts() const override { return { {"audio_in", PortDefinition::Input, PortDefinition::Audio}, {"audio_out", PortDefinition::Output, PortDefinition::Audio} }; }
    QJsonObject toJson() const override { QJsonObject obj = Node::toJson(); obj["algorithm"] = algorithm; obj["width"] = width; obj["delayTime"] = delayTime; obj["modRate"] = modRate; obj["modDepth"] = modDepth; return obj; }
    void loadFromJson(const QJsonObject& obj) override {
        Node::loadFromJson(obj);
        algorithm = obj["algorithm"].toString("adt");
        width = obj["width"].toDouble();
        delayTime = obj["delayTime"].toDouble();
        modRate = obj["modRate"].toDouble();
        modDepth = obj["modDepth"].toDouble();
    }
};

struct BitCrusherNode : public Node {
    int bitDepth = 24;
    int sampleRateReduction = 1;
    double mix = 1.0;
    void accept(INodeVisitor* v) const override { v->visit(this); }
    BitCrusherNode() { type = "BitCrusher"; }
    QVector<PortDefinition> getPorts() const override { return { {"audio_in", PortDefinition::Input, PortDefinition::Audio}, {"audio_out", PortDefinition::Output, PortDefinition::Audio} }; }
    QJsonObject toJson() const override { QJsonObject obj = Node::toJson(); obj["bitDepth"] = bitDepth; obj["sampleRateReduction"] = sampleRateReduction; obj["mix"] = mix; return obj; }
    void loadFromJson(const QJsonObject& obj) override {
        Node::loadFromJson(obj);
        bitDepth = obj["bitDepth"].toInt(24);
        sampleRateReduction = obj["sampleRateReduction"].toInt(1);
        mix = obj["mix"].toDouble(1.0);
    }
};

struct EqBand {
    QString type = "Peak"; // Peak, LowShelf, HighShelf, LowPass, HighPass, Notch, BandPass
    double freq = 1000.0;
    double gain = 0.0;
    double q = 1.0;
    
    QJsonObject toJson() const {
        QJsonObject obj;
        obj["type"] = type; obj["freq"] = freq; obj["gain"] = gain; obj["q"] = q; return obj;
    }
    static EqBand fromJson(const QJsonObject& obj) {
        EqBand b;
        b.type = obj["type"].toString("Peak");
        b.freq = obj["freq"].toDouble(1000.0);
        b.gain = obj["gain"].toDouble(0.0);
        b.q = obj["q"].toDouble(1.0);
        return b;
    }
};

struct EqualizerNode : public Node {
    QVector<EqBand> bands;
    void accept(INodeVisitor* v) const override { v->visit(this); }
    EqualizerNode() { 
        type = "Equalizer"; 
        // Default with 4 bands
        bands.append({"LowShelf", 100.0, 0.0, 1.0});
        bands.append({"Peak", 500.0, 0.0, 1.0});
        bands.append({"Peak", 2000.0, 0.0, 1.0});
        bands.append({"HighShelf", 8000.0, 0.0, 1.0});
    }
    QVector<PortDefinition> getPorts() const override {
        return { {"audio_in", PortDefinition::Input, PortDefinition::Audio}, {"audio_out", PortDefinition::Output, PortDefinition::Audio} };
    }
    QJsonObject toJson() const override {
        QJsonObject obj = Node::toJson();
        QJsonArray arr;
        for (const auto& b : bands) arr.append(b.toJson());
        obj["bands"] = arr;
        return obj;
    }
    void loadFromJson(const QJsonObject& obj) override {
        Node::loadFromJson(obj);
        if (obj.contains("bands")) {
            bands.clear();
            QJsonArray bandsArr = obj["bands"].toArray();
            for (int i = 0; i < bandsArr.size(); ++i) {
                bands.append(EqBand::fromJson(bandsArr[i].toObject()));
            }
        }
    }
};
