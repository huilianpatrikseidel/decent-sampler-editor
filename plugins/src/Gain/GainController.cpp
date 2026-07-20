#include "GainController.h"
#include "vstgui/plugin-bindings/vst3editor.h"

const FUID GainController::uid(INLINE_UID(0xd023184f, 0x04ce47fd, 0x9d1b244e, 0x36888903));

tresult PLUGIN_API GainController::initialize(FUnknown* context) {
    tresult result = EditController::initialize(context);
    if (result != kResultOk) return result;
    
    // Add parameters here
    // parameters.addParameter(STR16("Bypass"), nullptr, 1, 0, Vst::ParameterInfo::kCanAutomate | Vst::ParameterInfo::kIsBypass, 0);

    return kResultOk;
}

tresult PLUGIN_API GainController::setComponentState(IBStream* state) { return kResultOk; }

IPlugView* PLUGIN_API GainController::createView(FIDString name) {
    if (FIDStringsEqual(name, Vst::ViewType::kEditor)) {
        return new VSTGUI::VST3Editor(this, "view", "Gain.uidesc");
    }
    return nullptr;
}
