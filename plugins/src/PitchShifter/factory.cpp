#include "public.sdk/source/main/pluginfactory.h"
#include "PitchShifterProcessor.h"
#include "PitchShifterController.h"
#define stringPluginName "PitchShifter"
BEGIN_FACTORY_DEF("SamplerEditor", "www.samplereditor.com", "mailto:contact@samplereditor.com")
    DEF_CLASS2(INLINE_UID_FROM_FUID(PitchShifterProcessor::uid), Steinberg::PClassInfo::kManyInstances, kVstAudioEffectClass, stringPluginName, Steinberg::Vst::kDistributable, Steinberg::Vst::PlugType::kFx, "1.0.0", kVstVersionString, PitchShifterProcessor::createInstance)
    DEF_CLASS2(INLINE_UID_FROM_FUID(PitchShifterController::uid), Steinberg::PClassInfo::kManyInstances, kVstComponentControllerClass, stringPluginName " Controller", 0, "", "1.0.0", kVstVersionString, PitchShifterController::createInstance)
END_FACTORY
