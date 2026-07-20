#include "ReverbController.h"
#include "vstgui/plugin-bindings/vst3editor.h"
#include "pluginterfaces/base/ibstream.h"

const FUID ReverbController::uid(INLINE_UID(0x68697b01, 0xcf5846d5, 0x92ff7357, 0xc74f7639));

enum ReverbParams {
    kSizeId = 0,
    kDampId = 1,
};

tresult PLUGIN_API ReverbController::initialize(FUnknown* context) {
    tresult result = EditController::initialize(context);
    if (result != kResultOk) return result;
    
    parameters.addParameter(STR16("Size"), STR16(""), 0, 0.5, Vst::ParameterInfo::kCanAutomate, kSizeId);
    parameters.addParameter(STR16("Damp"), STR16(""), 0, 0.5, Vst::ParameterInfo::kCanAutomate, kDampId);
    return kResultOk;
}

tresult PLUGIN_API ReverbController::setComponentState(IBStream* state) { 
    if (!state) return kResultFalse;
    double val = 0.0;
    if (state->read(&val, sizeof(double)) == kResultOk) setParamNormalized(kSizeId, val);
    if (state->read(&val, sizeof(double)) == kResultOk) setParamNormalized(kDampId, val);
    return kResultOk;
}

IPlugView* PLUGIN_API ReverbController::createView(FIDString name) {
    if (FIDStringsEqual(name, Vst::ViewType::kEditor)) {
        return new VSTGUI::VST3Editor(this, "view", "Reverb.uidesc");
    }
    return nullptr;
}
