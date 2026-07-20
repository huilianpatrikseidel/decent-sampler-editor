#include "CompressorController.h"
#include "vstgui/plugin-bindings/vst3editor.h"
#include "pluginterfaces/base/ibstream.h"

const FUID CompressorController::uid(INLINE_UID(0x2cb8e54d, 0x03ca433c, 0xacf9321b, 0x94f3590b));

enum CompressorParams {
    kThresholdId = 0,
    kRatioId = 1,
    kAttackId = 2,
    kReleaseId = 3,
};

tresult PLUGIN_API CompressorController::initialize(FUnknown* context) {
    tresult result = EditController::initialize(context);
    if (result != kResultOk) return result;
    
    parameters.addParameter(STR16("Threshold"), STR16(""), 0, 0.5, Vst::ParameterInfo::kCanAutomate, kThresholdId);
    parameters.addParameter(STR16("Ratio"), STR16(""), 0, 0.5, Vst::ParameterInfo::kCanAutomate, kRatioId);
    parameters.addParameter(STR16("Attack"), STR16(""), 0, 0.5, Vst::ParameterInfo::kCanAutomate, kAttackId);
    parameters.addParameter(STR16("Release"), STR16(""), 0, 0.5, Vst::ParameterInfo::kCanAutomate, kReleaseId);
    return kResultOk;
}

tresult PLUGIN_API CompressorController::setComponentState(IBStream* state) { 
    if (!state) return kResultFalse;
    double val = 0.0;
    if (state->read(&val, sizeof(double)) == kResultOk) setParamNormalized(kThresholdId, val);
    if (state->read(&val, sizeof(double)) == kResultOk) setParamNormalized(kRatioId, val);
    if (state->read(&val, sizeof(double)) == kResultOk) setParamNormalized(kAttackId, val);
    if (state->read(&val, sizeof(double)) == kResultOk) setParamNormalized(kReleaseId, val);
    return kResultOk;
}

IPlugView* PLUGIN_API CompressorController::createView(FIDString name) {
    if (FIDStringsEqual(name, Vst::ViewType::kEditor)) {
        return new VSTGUI::VST3Editor(this, "view", "Compressor.uidesc");
    }
    return nullptr;
}
