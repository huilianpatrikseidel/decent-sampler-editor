#include "EqualizerController.h"
#include "vstgui/plugin-bindings/vst3editor.h"
#include "pluginterfaces/base/ibstream.h"
#include "pluginterfaces/base/ustring.h"
#include "SpectrumCurveView.h"
#include <cstdio>
#include <cmath>

const FUID EqualizerController::uid(INLINE_UID(0x30da32a9, 0xe4174632, 0xa6c9e14f, 0x72693ddf));

tresult PLUGIN_API EqualizerController::initialize(FUnknown* context) {
    tresult result = EditController::initialize(context);
    if (result != kResultOk) return result;
    
    char asciiBuf[128];
    for (int i = 0; i < 24; ++i) {
        int baseId = i * 10;
        Vst::String128 nameBuf;

        snprintf(asciiBuf, 128, "Band %d Enabled", i + 1);
        Steinberg::UString(nameBuf, 128).fromAscii(asciiBuf);
        parameters.addParameter(nameBuf, STR16(""), 1, 0.0, Vst::ParameterInfo::kCanAutomate, baseId + 0);

        snprintf(asciiBuf, 128, "Band %d Freq", i + 1);
        Steinberg::UString(nameBuf, 128).fromAscii(asciiBuf);
        parameters.addParameter(nameBuf, STR16("Hz"), 0, 0.5, Vst::ParameterInfo::kCanAutomate, baseId + 1);

        snprintf(asciiBuf, 128, "Band %d Gain", i + 1);
        Steinberg::UString(nameBuf, 128).fromAscii(asciiBuf);
        parameters.addParameter(nameBuf, STR16("dB"), 0, 0.5, Vst::ParameterInfo::kCanAutomate, baseId + 2);

        snprintf(asciiBuf, 128, "Band %d Q", i + 1);
        Steinberg::UString(nameBuf, 128).fromAscii(asciiBuf);
        parameters.addParameter(nameBuf, STR16(""), 0, 0.5, Vst::ParameterInfo::kCanAutomate, baseId + 3);

        snprintf(asciiBuf, 128, "Band %d Type", i + 1);
        Steinberg::UString(nameBuf, 128).fromAscii(asciiBuf);
        parameters.addParameter(nameBuf, STR16(""), 6, 0.0, Vst::ParameterInfo::kCanAutomate, baseId + 4);
    }
    return kResultOk;
}

tresult PLUGIN_API EqualizerController::setComponentState(IBStream* state) { 
    if (!state) return kResultFalse;
    
    for (int i = 0; i < 24; ++i) {
        int baseId = i * 10;
        
        int enabledInt = 0;
        if (state->read(&enabledInt, sizeof(int)) != kResultOk) return kResultFalse;
        setParamNormalized(baseId + 0, enabledInt ? 1.0 : 0.0);

        double val = 0.0;
        if (state->read(&val, sizeof(double)) != kResultOk) return kResultFalse;
        double normFreq = std::log(val / 20.0) / std::log(1000.0);
        setParamNormalized(baseId + 1, std::fmax(0.0, std::fmin(1.0, normFreq)));

        if (state->read(&val, sizeof(double)) != kResultOk) return kResultFalse;
        setParamNormalized(baseId + 2, (val + 30.0) / 60.0);

        if (state->read(&val, sizeof(double)) != kResultOk) return kResultFalse;
        double normQ = std::log(val / 0.1) / std::log(400.0);
        setParamNormalized(baseId + 3, std::fmax(0.0, std::fmin(1.0, normQ)));

        int typeInt = 0;
        if (state->read(&typeInt, sizeof(int)) != kResultOk) return kResultFalse;
        setParamNormalized(baseId + 4, typeInt / 6.0);
    }
    return kResultOk;
}

IPlugView* PLUGIN_API EqualizerController::createView(FIDString name) {
    if (FIDStringsEqual(name, Vst::ViewType::kEditor)) {
        return new VSTGUI::VST3Editor(this, "view", "Equalizer.uidesc");
    }
    return nullptr;
}

VSTGUI::CView* EqualizerController::verifyView(VSTGUI::CView* view, const VSTGUI::UIAttributes& attributes, const VSTGUI::IUIDescription* description, VSTGUI::VST3Editor* editor) {
    if (auto* curveView = dynamic_cast<VSTGUI::SpectrumCurveView*>(view)) {
        curveView->setController(this);
    }
    return view;
}
