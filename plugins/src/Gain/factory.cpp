#include "public.sdk/source/main/pluginfactory.h"
#include "GainProcessor.h"
#include "GainController.h"
#define stringPluginName "Gain"
BEGIN_FACTORY_DEF("SamplerEditor", "www.samplereditor.com", "mailto:contact@samplereditor.com")
    DEF_CLASS2(INLINE_UID_FROM_FUID(GainProcessor::uid), Steinberg::PClassInfo::kManyInstances, kVstAudioEffectClass, stringPluginName, Steinberg::Vst::kDistributable, Steinberg::Vst::PlugType::kFx, "1.0.0", kVstVersionString, GainProcessor::createInstance)
    DEF_CLASS2(INLINE_UID_FROM_FUID(GainController::uid), Steinberg::PClassInfo::kManyInstances, kVstComponentControllerClass, stringPluginName " Controller", 0, "", "1.0.0", kVstVersionString, GainController::createInstance)
END_FACTORY
