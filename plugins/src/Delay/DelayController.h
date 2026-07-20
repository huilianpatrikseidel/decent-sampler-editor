#pragma once
#include "public.sdk/source/vst/vsteditcontroller.h"

using namespace Steinberg;

class DelayController : public Vst::EditController {
public:
    ~DelayController() override = default;

    static FUnknown* createInstance(void* /*context*/) { return (Vst::IEditController*)new DelayController; }

    tresult PLUGIN_API initialize(FUnknown* context) override;
    tresult PLUGIN_API setComponentState(IBStream* state) override;
    IPlugView* PLUGIN_API createView(FIDString name) override;
    
    static const FUID uid;
};
