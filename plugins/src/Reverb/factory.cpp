#include "public.sdk/source/main/pluginfactory.h"
#include "ReverbProcessor.h"
#include "ReverbController.h"
#define stringPluginName "Reverb"
BEGIN_FACTORY_DEF("SamplerEditor", "www.samplereditor.com", "mailto:contact@samplereditor.com")
    DEF_CLASS2(INLINE_UID_FROM_FUID(ReverbProcessor::uid), Steinberg::PClassInfo::kManyInstances, kVstAudioEffectClass, stringPluginName, Steinberg::Vst::kDistributable, Steinberg::Vst::PlugType::kFx, "1.0.0", kVstVersionString, ReverbProcessor::createInstance)
    DEF_CLASS2(INLINE_UID_FROM_FUID(ReverbController::uid), Steinberg::PClassInfo::kManyInstances, kVstComponentControllerClass, stringPluginName " Controller", 0, "", "1.0.0", kVstVersionString, ReverbController::createInstance)
END_FACTORY
