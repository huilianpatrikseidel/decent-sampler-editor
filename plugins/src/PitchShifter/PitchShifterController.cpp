#include "PitchShifterController.h"
#include "vstgui/plugin-bindings/vst3editor.h"
#include "pluginterfaces/base/ibstream.h"

const FUID PitchShifterController::uid(INLINE_UID(0x05c630b8, 0x2550459d, 0xb07086c6, 0xd82eb7ea));

enum PitchShifterParams {
    kPitchShiftId = 0,
    kMixId = 1,
};

tresult PLUGIN_API PitchShifterController::initialize(FUnknown* context) {
    tresult result = EditController::initialize(context);
    if (result != kResultOk) return result;
    
    parameters.addParameter(STR16("PitchShift"), STR16(""), 0, 0.5, Vst::ParameterInfo::kCanAutomate, kPitchShiftId);
    parameters.addParameter(STR16("Mix"), STR16(""), 0, 0.5, Vst::ParameterInfo::kCanAutomate, kMixId);
    return kResultOk;
}

tresult PLUGIN_API PitchShifterController::setComponentState(IBStream* state) { 
    if (!state) return kResultFalse;
    double val = 0.0;
    if (state->read(&val, sizeof(double)) == kResultOk) setParamNormalized(kPitchShiftId, val);
    if (state->read(&val, sizeof(double)) == kResultOk) setParamNormalized(kMixId, val);
    return kResultOk;
}

IPlugView* PLUGIN_API PitchShifterController::createView(FIDString name) {
    if (FIDStringsEqual(name, Vst::ViewType::kEditor)) {
        return new VSTGUI::VST3Editor(this, "view", "PitchShifter.uidesc");
    }
    return nullptr;
}
