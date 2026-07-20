#include "WaveFolderController.h"
#include "vstgui/plugin-bindings/vst3editor.h"
#include "pluginterfaces/base/ibstream.h"

const FUID WaveFolderController::uid(INLINE_UID(0xd8c75d45, 0xf37d485c, 0xaab3dbbf, 0x307ef5e3));

enum WaveFolderParams {
    kDriveId = 0,
    kThresholdId = 1,
};

tresult PLUGIN_API WaveFolderController::initialize(FUnknown* context) {
    tresult result = EditController::initialize(context);
    if (result != kResultOk) return result;
    
    parameters.addParameter(STR16("Drive"), STR16(""), 0, 0.5, Vst::ParameterInfo::kCanAutomate, kDriveId);
    parameters.addParameter(STR16("Threshold"), STR16(""), 0, 0.5, Vst::ParameterInfo::kCanAutomate, kThresholdId);
    return kResultOk;
}

tresult PLUGIN_API WaveFolderController::setComponentState(IBStream* state) { 
    if (!state) return kResultFalse;
    double val = 0.0;
    if (state->read(&val, sizeof(double)) == kResultOk) setParamNormalized(kDriveId, val);
    if (state->read(&val, sizeof(double)) == kResultOk) setParamNormalized(kThresholdId, val);
    return kResultOk;
}

IPlugView* PLUGIN_API WaveFolderController::createView(FIDString name) {
    if (FIDStringsEqual(name, Vst::ViewType::kEditor)) {
        return new VSTGUI::VST3Editor(this, "view", "WaveFolder.uidesc");
    }
    return nullptr;
}
