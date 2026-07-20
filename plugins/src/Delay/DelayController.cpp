#include "DelayController.h"
#include "vstgui/plugin-bindings/vst3editor.h"
#include "pluginterfaces/base/ibstream.h"

const FUID DelayController::uid(INLINE_UID(0x84384de3, 0x173e49a9, 0xae6adf28, 0x5542e4d3));

enum DelayParams {
    kTimeId = 0,
    kFeedbackId = 1,
};

tresult PLUGIN_API DelayController::initialize(FUnknown* context) {
    tresult result = EditController::initialize(context);
    if (result != kResultOk) return result;
    
    parameters.addParameter(STR16("Time"), STR16(""), 0, 0.5, Vst::ParameterInfo::kCanAutomate, kTimeId);
    parameters.addParameter(STR16("Feedback"), STR16(""), 0, 0.5, Vst::ParameterInfo::kCanAutomate, kFeedbackId);
    return kResultOk;
}

tresult PLUGIN_API DelayController::setComponentState(IBStream* state) { 
    if (!state) return kResultFalse;
    double val = 0.0;
    if (state->read(&val, sizeof(double)) == kResultOk) setParamNormalized(kTimeId, val);
    if (state->read(&val, sizeof(double)) == kResultOk) setParamNormalized(kFeedbackId, val);
    return kResultOk;
}

IPlugView* PLUGIN_API DelayController::createView(FIDString name) {
    if (FIDStringsEqual(name, Vst::ViewType::kEditor)) {
        return new VSTGUI::VST3Editor(this, "view", "Delay.uidesc");
    }
    return nullptr;
}
