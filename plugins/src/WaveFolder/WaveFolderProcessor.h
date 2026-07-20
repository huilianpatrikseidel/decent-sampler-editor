#pragma once
#include "public.sdk/source/vst/vstaudioeffect.h"

using namespace Steinberg;

class WaveFolderProcessor : public Vst::AudioEffect {
public:
    WaveFolderProcessor();
    ~WaveFolderProcessor() override = default;

    static FUnknown* createInstance(void* /*context*/) { return (Vst::IAudioProcessor*)new WaveFolderProcessor; }

    tresult PLUGIN_API initialize(FUnknown* context) override;
    tresult PLUGIN_API setupProcessing(Vst::ProcessSetup& newSetup) override;
    tresult PLUGIN_API process(Vst::ProcessData& data) override;
    tresult PLUGIN_API setState(IBStream* state) override;
    tresult PLUGIN_API getState(IBStream* state) override;

    static const FUID uid;
private:
    double m_drive = 0.5;
    double m_threshold = 0.5;
};
