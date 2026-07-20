#pragma once
#include "public.sdk/source/vst/vsteditcontroller.h"
#include "vstgui/plugin-bindings/vst3editor.h"

using namespace Steinberg;
using namespace Steinberg::Vst;

class EqualizerController : public EditController, public VSTGUI::VST3EditorDelegate {
public:
    EqualizerController() = default;
    ~EqualizerController() override = default;

    static FUnknown* createInstance(void* /*context*/) { return (IEditController*)new EqualizerController; }

    tresult PLUGIN_API initialize(FUnknown* context) override;
    tresult PLUGIN_API setComponentState(IBStream* state) override;
    IPlugView* PLUGIN_API createView(FIDString name) override;

    VSTGUI::CView* verifyView(VSTGUI::CView* view, const VSTGUI::UIAttributes& attributes, const VSTGUI::IUIDescription* description, VSTGUI::VST3Editor* editor) override;
    
    static const FUID uid;
};
