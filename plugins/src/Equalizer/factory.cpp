#include "public.sdk/source/main/pluginfactory.h"
#include "EqualizerProcessor.h"
#include "EqualizerController.h"
#define stringPluginName "Equalizer"
BEGIN_FACTORY_DEF("SamplerEditor", "www.samplereditor.com", "mailto:contact@samplereditor.com")
    DEF_CLASS2(INLINE_UID_FROM_FUID(EqualizerProcessor::uid), Steinberg::PClassInfo::kManyInstances, kVstAudioEffectClass, stringPluginName, Steinberg::Vst::kDistributable, Steinberg::Vst::PlugType::kFx, "1.0.0", kVstVersionString, EqualizerProcessor::createInstance)
    DEF_CLASS2(INLINE_UID_FROM_FUID(EqualizerController::uid), Steinberg::PClassInfo::kManyInstances, kVstComponentControllerClass, stringPluginName " Controller", 0, "", "1.0.0", kVstVersionString, EqualizerController::createInstance)
END_FACTORY
