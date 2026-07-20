#pragma once

#include "public.sdk/source/vst/vstaudioeffect.h"
#include "ChorusUIDs.h"

namespace SamplerEditor {
namespace Chorus {

class ChorusProcessor : public Steinberg::Vst::AudioEffect {
public:
    ChorusProcessor();
    ~ChorusProcessor() override;

    static Steinberg::FUnknown* createInstance(void* /*context*/) {
        return (Steinberg::Vst::IAudioProcessor*)new ChorusProcessor;
    }

    Steinberg::tresult PLUGIN_API initialize(Steinberg::FUnknown* context) override;
    Steinberg::tresult PLUGIN_API terminate() override;
    
    Steinberg::tresult PLUGIN_API setBusArrangements(Steinberg::Vst::SpeakerArrangement* inputs,
                                                     Steinberg::int32 numIns,
                                                     Steinberg::Vst::SpeakerArrangement* outputs,
                                                     Steinberg::int32 numOuts) override;

    Steinberg::tresult PLUGIN_API setActive(Steinberg::TBool state) override;
    Steinberg::tresult PLUGIN_API process(Steinberg::Vst::ProcessData& data) override;

    Steinberg::tresult PLUGIN_API setState(Steinberg::IBStream* state) override;
    Steinberg::tresult PLUGIN_API getState(Steinberg::IBStream* state) override;

private:
    float m_rate = 1.0f;
    float m_depth = 0.5f;
    float m_mix = 0.5f;
    
    // Simple DSP state for Chorus
    float m_phase = 0.0f;
    int m_writePos = 0;
    float* m_bufferL = nullptr;
    float* m_bufferR = nullptr;
    int m_bufferSize = 0;
};

}
}
