#pragma once
#include "public.sdk/source/vst/vsteditcontroller.h"

using namespace Steinberg;

class FilterController : public Vst::EditController {
public:
    ~FilterController() override = default;

    static FUnknown* createInstance(void* /*context*/) { return (Vst::IEditController*)new FilterController; }

    tresult PLUGIN_API initialize(FUnknown* context) override;
    tresult PLUGIN_API setComponentState(IBStream* state) override;
    IPlugView* PLUGIN_API createView(FIDString name) override;
    
    static const FUID uid;
};
