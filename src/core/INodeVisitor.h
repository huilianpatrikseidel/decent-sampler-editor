#pragma once

struct SampleGroup;
struct BusNode;
struct DelayNode;
struct ReverbNode;
struct FilterNode;
struct ChorusNode;
class CompressorNode;
struct GainNode;
struct PhaserNode;
struct PitchShifterNode;
struct WaveFolderNode;
struct WaveShaperNode;
struct StereoSimulatorNode;
struct BitCrusherNode;
struct EqualizerNode;

class INodeVisitor {
public:
    virtual ~INodeVisitor() = default;
    virtual void visit(const SampleGroup* sg) = 0;
    virtual void visit(const BusNode* bus) = 0;
    virtual void visit(const DelayNode* delay) = 0;
    virtual void visit(const ReverbNode* reverb) = 0;
    virtual void visit(const FilterNode* filter) = 0;
    virtual void visit(const ChorusNode* chorus) = 0;
    virtual void visit(const GainNode* gain) = 0;
    virtual void visit(const PhaserNode* phaser) = 0;
    virtual void visit(const PitchShifterNode* pitch) = 0;
    virtual void visit(const WaveFolderNode* folder) = 0;
    virtual void visit(const WaveShaperNode* shaper) = 0;
    virtual void visit(const StereoSimulatorNode* stereo) = 0;
    virtual void visit(const BitCrusherNode* bit) = 0;
    virtual void visit(const EqualizerNode* eq) = 0;
};
