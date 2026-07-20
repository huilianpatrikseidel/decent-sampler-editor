#include "public.sdk/source/main/pluginfactory.h"
#include "WaveShaperProcessor.h"
#include "WaveShaperController.h"
#define stringPluginName "WaveShaper"
BEGIN_FACTORY_DEF("SamplerEditor", "www.samplereditor.com", "mailto:contact@samplereditor.com")
    DEF_CLASS2(INLINE_UID_FROM_FUID(WaveShaperProcessor::uid), Steinberg::PClassInfo::kManyInstances, kVstAudioEffectClass, stringPluginName, Steinberg::Vst::kDistributable, Steinberg::Vst::PlugType::kFx, "1.0.0", kVstVersionString, WaveShaperProcessor::createInstance)
    DEF_CLASS2(INLINE_UID_FROM_FUID(WaveShaperController::uid), Steinberg::PClassInfo::kManyInstances, kVstComponentControllerClass, stringPluginName " Controller", 0, "", "1.0.0", kVstVersionString, WaveShaperController::createInstance)
END_FACTORY
