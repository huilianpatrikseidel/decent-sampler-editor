#pragma once
#include "public.sdk/source/vst/vsteditcontroller.h"

using namespace Steinberg;

class StereoSimulatorController : public Vst::EditController {
public:
    ~StereoSimulatorController() override = default;

    static FUnknown* createInstance(void* /*context*/) { return (Vst::IEditController*)new StereoSimulatorController; }

    tresult PLUGIN_API initialize(FUnknown* context) override;
    tresult PLUGIN_API setComponentState(IBStream* state) override;
    IPlugView* PLUGIN_API createView(FIDString name) override;
    
    static const FUID uid;
};
