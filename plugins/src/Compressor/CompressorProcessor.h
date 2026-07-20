#pragma once
#include "public.sdk/source/vst/vstaudioeffect.h"

using namespace Steinberg;

class CompressorProcessor : public Vst::AudioEffect {
public:
    CompressorProcessor();
    ~CompressorProcessor() override = default;

    static FUnknown* createInstance(void* /*context*/) { return (Vst::IAudioProcessor*)new CompressorProcessor; }

    tresult PLUGIN_API initialize(FUnknown* context) override;
    tresult PLUGIN_API setupProcessing(Vst::ProcessSetup& newSetup) override;
    tresult PLUGIN_API process(Vst::ProcessData& data) override;
    tresult PLUGIN_API setState(IBStream* state) override;
    tresult PLUGIN_API getState(IBStream* state) override;

    static const FUID uid;
private:
    double m_threshold = 0.5;
    double m_ratio = 0.5;
    double m_attack = 0.5;
    double m_release = 0.5;
};
