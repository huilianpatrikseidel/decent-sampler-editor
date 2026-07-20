#include "BitCrusherController.h"
#include "vstgui/plugin-bindings/vst3editor.h"
#include "pluginterfaces/base/ibstream.h"

const FUID BitCrusherController::uid(INLINE_UID(0x42ff6391, 0x67f34531, 0x929dbbe0, 0xecb35322));

enum BitCrusherParams {
    kBitDepthId = 0,
    kReductionId = 1,
    kMixId = 2,
};

tresult PLUGIN_API BitCrusherController::initialize(FUnknown* context) {
    tresult result = EditController::initialize(context);
    if (result != kResultOk) return result;
    
    parameters.addParameter(STR16("BitDepth"), STR16(""), 0, 0.5, Vst::ParameterInfo::kCanAutomate, kBitDepthId);
    parameters.addParameter(STR16("Reduction"), STR16(""), 0, 0.5, Vst::ParameterInfo::kCanAutomate, kReductionId);
    parameters.addParameter(STR16("Mix"), STR16(""), 0, 0.5, Vst::ParameterInfo::kCanAutomate, kMixId);
    return kResultOk;
}

tresult PLUGIN_API BitCrusherController::setComponentState(IBStream* state) { 
    if (!state) return kResultFalse;
    double val = 0.0;
    if (state->read(&val, sizeof(double)) == kResultOk) setParamNormalized(kBitDepthId, val);
    if (state->read(&val, sizeof(double)) == kResultOk) setParamNormalized(kReductionId, val);
    if (state->read(&val, sizeof(double)) == kResultOk) setParamNormalized(kMixId, val);
    return kResultOk;
}

IPlugView* PLUGIN_API BitCrusherController::createView(FIDString name) {
    if (FIDStringsEqual(name, Vst::ViewType::kEditor)) {
        return new VSTGUI::VST3Editor(this, "view", "BitCrusher.uidesc");
    }
    return nullptr;
}
