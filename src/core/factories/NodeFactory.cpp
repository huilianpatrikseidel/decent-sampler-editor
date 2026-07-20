#include "NodeFactory.h"
#include <QJsonArray>

std::unique_ptr<Node> NodeFactory::fromJson(const QJsonObject& nodeObj) {
    QString type = nodeObj["type"].toString();
    std::unique_ptr<Node> node;
    
    if (type == "SampleGroup") node = std::make_unique<SampleGroup>();
    else if (type == "Bus") node = std::make_unique<BusNode>();
    else if (type == "Delay") node = std::make_unique<DelayNode>();
    else if (type == "Reverb") node = std::make_unique<ReverbNode>();
    else if (type == "Filter") node = std::make_unique<FilterNode>();
    else if (type == "Chorus") node = std::make_unique<ChorusNode>();
    else if (type == "Gain") node = std::make_unique<GainNode>();
    else if (type == "Phaser") node = std::make_unique<PhaserNode>();
    else if (type == "PitchShifter") node = std::make_unique<PitchShifterNode>();
    else if (type == "WaveFolder") node = std::make_unique<WaveFolderNode>();
    else if (type == "WaveShaper") node = std::make_unique<WaveShaperNode>();
    else if (type == "StereoSimulator") node = std::make_unique<StereoSimulatorNode>();
    else if (type == "BitCrusher") node = std::make_unique<BitCrusherNode>();
    else if (type == "Equalizer") node = std::make_unique<EqualizerNode>();
    
    if (node) {
        node->loadFromJson(nodeObj);
    }
    
    return node;
}
