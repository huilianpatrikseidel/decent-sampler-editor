#include "WaveShaperController.h"
#include "vstgui/plugin-bindings/vst3editor.h"
#include "pluginterfaces/base/ibstream.h"

const FUID WaveShaperController::uid(INLINE_UID(0x89bce743, 0xe4eb42df, 0x92ccfb46, 0xa9f08466));

enum WaveShaperParams {
    kDriveId = 0,
    kOutputLevelId = 1,
};

tresult PLUGIN_API WaveShaperController::initialize(FUnknown* context) {
    tresult result = EditController::initialize(context);
    if (result != kResultOk) return result;
    
    parameters.addParameter(STR16("Drive"), STR16(""), 0, 0.5, Vst::ParameterInfo::kCanAutomate, kDriveId);
    parameters.addParameter(STR16("OutputLevel"), STR16(""), 0, 0.5, Vst::ParameterInfo::kCanAutomate, kOutputLevelId);
    return kResultOk;
}

tresult PLUGIN_API WaveShaperController::setComponentState(IBStream* state) { 
    if (!state) return kResultFalse;
    double val = 0.0;
    if (state->read(&val, sizeof(double)) == kResultOk) setParamNormalized(kDriveId, val);
    if (state->read(&val, sizeof(double)) == kResultOk) setParamNormalized(kOutputLevelId, val);
    return kResultOk;
}

IPlugView* PLUGIN_API WaveShaperController::createView(FIDString name) {
    if (FIDStringsEqual(name, Vst::ViewType::kEditor)) {
        return new VSTGUI::VST3Editor(this, "view", "WaveShaper.uidesc");
    }
    return nullptr;
}
