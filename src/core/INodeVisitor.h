#pragma once

class SampleGroup;
class BusNode;
class DelayNode;
class ReverbNode;
class FilterNode;
class ChorusNode;
class CompressorNode;
class GainNode;
class PhaserNode;
class PitchShifterNode;
class WaveFolderNode;
class WaveShaperNode;
class StereoSimulatorNode;
class BitCrusherNode;
class EqualizerNode;

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
