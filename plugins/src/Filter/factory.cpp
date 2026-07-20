#include "public.sdk/source/main/pluginfactory.h"
#include "FilterProcessor.h"
#include "FilterController.h"
#define stringPluginName "Filter"
BEGIN_FACTORY_DEF("SamplerEditor", "www.samplereditor.com", "mailto:contact@samplereditor.com")
    DEF_CLASS2(INLINE_UID_FROM_FUID(FilterProcessor::uid), Steinberg::PClassInfo::kManyInstances, kVstAudioEffectClass, stringPluginName, Steinberg::Vst::kDistributable, Steinberg::Vst::PlugType::kFx, "1.0.0", kVstVersionString, FilterProcessor::createInstance)
    DEF_CLASS2(INLINE_UID_FROM_FUID(FilterController::uid), Steinberg::PClassInfo::kManyInstances, kVstComponentControllerClass, stringPluginName " Controller", 0, "", "1.0.0", kVstVersionString, FilterController::createInstance)
END_FACTORY
