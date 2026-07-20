#include "public.sdk/source/main/pluginfactory.h"
#include "CompressorProcessor.h"
#include "CompressorController.h"
#define stringPluginName "Compressor"
BEGIN_FACTORY_DEF("SamplerEditor", "www.samplereditor.com", "mailto:contact@samplereditor.com")
    DEF_CLASS2(INLINE_UID_FROM_FUID(CompressorProcessor::uid), Steinberg::PClassInfo::kManyInstances, kVstAudioEffectClass, stringPluginName, Steinberg::Vst::kDistributable, Steinberg::Vst::PlugType::kFx, "1.0.0", kVstVersionString, CompressorProcessor::createInstance)
    DEF_CLASS2(INLINE_UID_FROM_FUID(CompressorController::uid), Steinberg::PClassInfo::kManyInstances, kVstComponentControllerClass, stringPluginName " Controller", 0, "", "1.0.0", kVstVersionString, CompressorController::createInstance)
END_FACTORY
