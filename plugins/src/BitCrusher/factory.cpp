#include "public.sdk/source/main/pluginfactory.h"
#include "BitCrusherProcessor.h"
#include "BitCrusherController.h"
#define stringPluginName "BitCrusher"
BEGIN_FACTORY_DEF("SamplerEditor", "www.samplereditor.com", "mailto:contact@samplereditor.com")
    DEF_CLASS2(INLINE_UID_FROM_FUID(BitCrusherProcessor::uid), Steinberg::PClassInfo::kManyInstances, kVstAudioEffectClass, stringPluginName, Steinberg::Vst::kDistributable, Steinberg::Vst::PlugType::kFx, "1.0.0", kVstVersionString, BitCrusherProcessor::createInstance)
    DEF_CLASS2(INLINE_UID_FROM_FUID(BitCrusherController::uid), Steinberg::PClassInfo::kManyInstances, kVstComponentControllerClass, stringPluginName " Controller", 0, "", "1.0.0", kVstVersionString, BitCrusherController::createInstance)
END_FACTORY
