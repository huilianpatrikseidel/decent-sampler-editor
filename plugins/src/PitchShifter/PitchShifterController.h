#pragma once
#include "public.sdk/source/vst/vsteditcontroller.h"

using namespace Steinberg;

class PitchShifterController : public Vst::EditController {
public:
    ~PitchShifterController() override = default;

    static FUnknown* createInstance(void* /*context*/) { return (Vst::IEditController*)new PitchShifterController; }

    tresult PLUGIN_API initialize(FUnknown* context) override;
    tresult PLUGIN_API setComponentState(IBStream* state) override;
    IPlugView* PLUGIN_API createView(FIDString name) override;
    
    static const FUID uid;
};
