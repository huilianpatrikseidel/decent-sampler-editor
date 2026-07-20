#include "StereoSimulatorController.h"
#include "vstgui/plugin-bindings/vst3editor.h"
#include "pluginterfaces/base/ibstream.h"

const FUID StereoSimulatorController::uid(INLINE_UID(0xeb3b5bc8, 0x1e4a4332, 0xa2ff0fbb, 0x76788a91));

enum StereoSimulatorParams {
    kWidthId = 0,
    kDelayTimeId = 1,
    kModDepthId = 2,
};

tresult PLUGIN_API StereoSimulatorController::initialize(FUnknown* context) {
    tresult result = EditController::initialize(context);
    if (result != kResultOk) return result;
    
    parameters.addParameter(STR16("Width"), STR16(""), 0, 0.5, Vst::ParameterInfo::kCanAutomate, kWidthId);
    parameters.addParameter(STR16("DelayTime"), STR16(""), 0, 0.5, Vst::ParameterInfo::kCanAutomate, kDelayTimeId);
    parameters.addParameter(STR16("ModDepth"), STR16(""), 0, 0.5, Vst::ParameterInfo::kCanAutomate, kModDepthId);
    return kResultOk;
}

tresult PLUGIN_API StereoSimulatorController::setComponentState(IBStream* state) { 
    if (!state) return kResultFalse;
    double val = 0.0;
    if (state->read(&val, sizeof(double)) == kResultOk) setParamNormalized(kWidthId, val);
    if (state->read(&val, sizeof(double)) == kResultOk) setParamNormalized(kDelayTimeId, val);
    if (state->read(&val, sizeof(double)) == kResultOk) setParamNormalized(kModDepthId, val);
    return kResultOk;
}

IPlugView* PLUGIN_API StereoSimulatorController::createView(FIDString name) {
    if (FIDStringsEqual(name, Vst::ViewType::kEditor)) {
        return new VSTGUI::VST3Editor(this, "view", "StereoSimulator.uidesc");
    }
    return nullptr;
}
