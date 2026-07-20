#include "public.sdk/source/main/pluginfactory.h"
#include "PhaserProcessor.h"
#include "PhaserController.h"
#define stringPluginName "Phaser"
BEGIN_FACTORY_DEF("SamplerEditor", "www.samplereditor.com", "mailto:contact@samplereditor.com")
    DEF_CLASS2(INLINE_UID_FROM_FUID(PhaserProcessor::uid), Steinberg::PClassInfo::kManyInstances, kVstAudioEffectClass, stringPluginName, Steinberg::Vst::kDistributable, Steinberg::Vst::PlugType::kFx, "1.0.0", kVstVersionString, PhaserProcessor::createInstance)
    DEF_CLASS2(INLINE_UID_FROM_FUID(PhaserController::uid), Steinberg::PClassInfo::kManyInstances, kVstComponentControllerClass, stringPluginName " Controller", 0, "", "1.0.0", kVstVersionString, PhaserController::createInstance)
END_FACTORY
