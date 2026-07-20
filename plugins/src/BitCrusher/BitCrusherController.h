#pragma once
#include "public.sdk/source/vst/vsteditcontroller.h"

using namespace Steinberg;

class BitCrusherController : public Vst::EditController {
public:
    ~BitCrusherController() override = default;

    static FUnknown* createInstance(void* /*context*/) { return (Vst::IEditController*)new BitCrusherController; }

    tresult PLUGIN_API initialize(FUnknown* context) override;
    tresult PLUGIN_API setComponentState(IBStream* state) override;
    IPlugView* PLUGIN_API createView(FIDString name) override;
    
    static const FUID uid;
};
