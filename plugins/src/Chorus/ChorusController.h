#pragma once

#include "public.sdk/source/vst/vsteditcontroller.h"
#include "ChorusUIDs.h"

namespace SamplerEditor {
namespace Chorus {

class ChorusController : public Steinberg::Vst::EditController {
public:
    static Steinberg::FUnknown* createInstance(void* /*context*/) {
        return (Steinberg::Vst::IEditController*)new ChorusController;
    }

    Steinberg::tresult PLUGIN_API initialize(Steinberg::FUnknown* context) override;
    Steinberg::tresult PLUGIN_API setComponentState(Steinberg::IBStream* state) override;
    Steinberg::IPlugView* PLUGIN_API createView(Steinberg::FIDString name) override;
};

}
}
