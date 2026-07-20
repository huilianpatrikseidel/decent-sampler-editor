#pragma once
#include "public.sdk/source/vst/vstaudioeffect.h"

using namespace Steinberg;

class DelayProcessor : public Vst::AudioEffect {
public:
    DelayProcessor();
    ~DelayProcessor() override = default;

    static FUnknown* createInstance(void* /*context*/) { return (Vst::IAudioProcessor*)new DelayProcessor; }

    tresult PLUGIN_API initialize(FUnknown* context) override;
    tresult PLUGIN_API setupProcessing(Vst::ProcessSetup& newSetup) override;
    tresult PLUGIN_API process(Vst::ProcessData& data) override;
    tresult PLUGIN_API setState(IBStream* state) override;
    tresult PLUGIN_API getState(IBStream* state) override;

    static const FUID uid;
private:
    double m_time = 0.5;
    double m_feedback = 0.5;
};
