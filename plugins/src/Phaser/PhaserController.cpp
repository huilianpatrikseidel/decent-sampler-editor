#include "PhaserController.h"
#include "vstgui/plugin-bindings/vst3editor.h"
#include "pluginterfaces/base/ibstream.h"

const FUID PhaserController::uid(INLINE_UID(0x792da2f1, 0x74a74850, 0xa4f38398, 0xb937dbdd));

enum PhaserParams {
    kMixId = 0,
    kModDepthId = 1,
    kModRateId = 2,
};

tresult PLUGIN_API PhaserController::initialize(FUnknown* context) {
    tresult result = EditController::initialize(context);
    if (result != kResultOk) return result;
    
    parameters.addParameter(STR16("Mix"), STR16(""), 0, 0.5, Vst::ParameterInfo::kCanAutomate, kMixId);
    parameters.addParameter(STR16("ModDepth"), STR16(""), 0, 0.5, Vst::ParameterInfo::kCanAutomate, kModDepthId);
    parameters.addParameter(STR16("ModRate"), STR16(""), 0, 0.5, Vst::ParameterInfo::kCanAutomate, kModRateId);
    return kResultOk;
}

tresult PLUGIN_API PhaserController::setComponentState(IBStream* state) { 
    if (!state) return kResultFalse;
    double val = 0.0;
    if (state->read(&val, sizeof(double)) == kResultOk) setParamNormalized(kMixId, val);
    if (state->read(&val, sizeof(double)) == kResultOk) setParamNormalized(kModDepthId, val);
    if (state->read(&val, sizeof(double)) == kResultOk) setParamNormalized(kModRateId, val);
    return kResultOk;
}

IPlugView* PLUGIN_API PhaserController::createView(FIDString name) {
    if (FIDStringsEqual(name, Vst::ViewType::kEditor)) {
        return new VSTGUI::VST3Editor(this, "view", "Phaser.uidesc");
    }
    return nullptr;
}
