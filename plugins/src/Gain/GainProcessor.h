#pragma once
#include "public.sdk/source/vst/vstaudioeffect.h"

using namespace Steinberg;

class GainProcessor : public Vst::AudioEffect {
public:
    GainProcessor();
    ~GainProcessor() override = default;

    static FUnknown* createInstance(void* /*context*/) { return (Vst::IAudioProcessor*)new GainProcessor; }

    tresult PLUGIN_API initialize(FUnknown* context) override;
    tresult PLUGIN_API setupProcessing(Vst::ProcessSetup& newSetup) override;
    tresult PLUGIN_API process(Vst::ProcessData& data) override;
    tresult PLUGIN_API setState(IBStream* state) override;
    tresult PLUGIN_API getState(IBStream* state) override;

    static const FUID uid;
private:
    double m_gain = 1.0;
};
