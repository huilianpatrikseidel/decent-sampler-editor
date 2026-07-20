#include "public.sdk/source/main/pluginfactory.h"
#include "DelayProcessor.h"
#include "DelayController.h"
#define stringPluginName "Delay"
BEGIN_FACTORY_DEF("SamplerEditor", "www.samplereditor.com", "mailto:contact@samplereditor.com")
    DEF_CLASS2(INLINE_UID_FROM_FUID(DelayProcessor::uid), Steinberg::PClassInfo::kManyInstances, kVstAudioEffectClass, stringPluginName, Steinberg::Vst::kDistributable, Steinberg::Vst::PlugType::kFx, "1.0.0", kVstVersionString, DelayProcessor::createInstance)
    DEF_CLASS2(INLINE_UID_FROM_FUID(DelayController::uid), Steinberg::PClassInfo::kManyInstances, kVstComponentControllerClass, stringPluginName " Controller", 0, "", "1.0.0", kVstVersionString, DelayController::createInstance)
END_FACTORY
