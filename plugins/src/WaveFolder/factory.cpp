#include "public.sdk/source/main/pluginfactory.h"
#include "WaveFolderProcessor.h"
#include "WaveFolderController.h"
#define stringPluginName "WaveFolder"
BEGIN_FACTORY_DEF("SamplerEditor", "www.samplereditor.com", "mailto:contact@samplereditor.com")
    DEF_CLASS2(INLINE_UID_FROM_FUID(WaveFolderProcessor::uid), Steinberg::PClassInfo::kManyInstances, kVstAudioEffectClass, stringPluginName, Steinberg::Vst::kDistributable, Steinberg::Vst::PlugType::kFx, "1.0.0", kVstVersionString, WaveFolderProcessor::createInstance)
    DEF_CLASS2(INLINE_UID_FROM_FUID(WaveFolderController::uid), Steinberg::PClassInfo::kManyInstances, kVstComponentControllerClass, stringPluginName " Controller", 0, "", "1.0.0", kVstVersionString, WaveFolderController::createInstance)
END_FACTORY
