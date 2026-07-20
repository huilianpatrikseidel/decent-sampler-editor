#include "ChorusController.h"
#include "base/source/fstreamer.h"
#include "pluginterfaces/base/ibstream.h"
#include "vstgui/plugin-bindings/vst3editor.h"

namespace SamplerEditor {
namespace Chorus {

Steinberg::tresult PLUGIN_API ChorusController::initialize(Steinberg::FUnknown* context) {
    Steinberg::tresult result = EditController::initialize(context);
    if (result != Steinberg::kResultOk) return result;

    parameters.addParameter(STR16("Rate"), STR16("Hz"), 0, 0.5, Steinberg::Vst::ParameterInfo::kCanAutomate, kRateId);
    parameters.addParameter(STR16("Depth"), STR16("%"), 0, 0.5, Steinberg::Vst::ParameterInfo::kCanAutomate, kDepthId);
    parameters.addParameter(STR16("Mix"), STR16("%"), 0, 0.5, Steinberg::Vst::ParameterInfo::kCanAutomate, kMixId);

    return Steinberg::kResultOk;
}

Steinberg::tresult PLUGIN_API ChorusController::setComponentState(Steinberg::IBStream* state) {
    if (!state) return Steinberg::kResultFalse;
    Steinberg::IBStreamer streamer(state, kLittleEndian);
    
    float rate = 0.0f, depth = 0.0f, mix = 0.0f;
    if (streamer.readFloat(rate) == false) return Steinberg::kResultFalse;
    if (streamer.readFloat(depth) == false) return Steinberg::kResultFalse;
    if (streamer.readFloat(mix) == false) return Steinberg::kResultFalse;
    
    setParamNormalized(kRateId, (rate - 0.1f) / 9.9f);
    setParamNormalized(kDepthId, depth);
    setParamNormalized(kMixId, mix);

    return Steinberg::kResultOk;
}

Steinberg::IPlugView* PLUGIN_API ChorusController::createView(Steinberg::FIDString name) {
    if (Steinberg::FIDStringsEqual(name, Steinberg::Vst::ViewType::kEditor)) {
        return new VSTGUI::VST3Editor(this, "view", "Chorus.uidesc");
    }
    return nullptr;
}

}
}
