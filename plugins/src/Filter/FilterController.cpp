#include "FilterController.h"
#include "vstgui/plugin-bindings/vst3editor.h"
#include "pluginterfaces/base/ibstream.h"

const FUID FilterController::uid(INLINE_UID(0x9d1d038c, 0x047444ff, 0x8195e2c3, 0xebe08b12));

enum FilterParams {
    kCutoffId = 0,
    kResonanceId = 1,
};

tresult PLUGIN_API FilterController::initialize(FUnknown* context) {
    tresult result = EditController::initialize(context);
    if (result != kResultOk) return result;
    
    parameters.addParameter(STR16("Cutoff"), STR16(""), 0, 0.5, Vst::ParameterInfo::kCanAutomate, kCutoffId);
    parameters.addParameter(STR16("Resonance"), STR16(""), 0, 0.5, Vst::ParameterInfo::kCanAutomate, kResonanceId);
    return kResultOk;
}

tresult PLUGIN_API FilterController::setComponentState(IBStream* state) { 
    if (!state) return kResultFalse;
    double val = 0.0;
    if (state->read(&val, sizeof(double)) == kResultOk) setParamNormalized(kCutoffId, val);
    if (state->read(&val, sizeof(double)) == kResultOk) setParamNormalized(kResonanceId, val);
    return kResultOk;
}

IPlugView* PLUGIN_API FilterController::createView(FIDString name) {
    if (FIDStringsEqual(name, Vst::ViewType::kEditor)) {
        return new VSTGUI::VST3Editor(this, "view", "Filter.uidesc");
    }
    return nullptr;
}
