#pragma once
#include "public.sdk/source/vst/vstaudioeffect.h"

using namespace Steinberg;
#include "Biquad.h"

constexpr int MAX_EQ_BANDS = 24;

struct EqBandParams {
    bool enabled = false;
    double freq = 1000.0;
    double gain = 0.0;
    double q = 1.0;
    FilterType type = FilterType::Peak;
};

class EqualizerProcessor : public Vst::AudioEffect {
public:
    EqualizerProcessor();
    ~EqualizerProcessor() override = default;

    static FUnknown* createInstance(void* /*context*/) { return (Vst::IAudioProcessor*)new EqualizerProcessor; }

    tresult PLUGIN_API initialize(FUnknown* context) override;
    tresult PLUGIN_API setupProcessing(Vst::ProcessSetup& newSetup) override;
    tresult PLUGIN_API process(Vst::ProcessData& data) override;
    tresult PLUGIN_API setState(IBStream* state) override;
    tresult PLUGIN_API getState(IBStream* state) override;
    
    tresult PLUGIN_API setActive(TBool state) override;

    static const FUID uid;
private:
    void updateFilters();

    EqBandParams m_bands[MAX_EQ_BANDS];
    BiquadFilter m_filters[MAX_EQ_BANDS][2]; // [band][channel]
    
    double m_sampleRate = 44100.0;
};
