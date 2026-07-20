#include "public.sdk/source/main/pluginfactory.h"
#include "StereoSimulatorProcessor.h"
#include "StereoSimulatorController.h"
#define stringPluginName "StereoSimulator"
BEGIN_FACTORY_DEF("SamplerEditor", "www.samplereditor.com", "mailto:contact@samplereditor.com")
    DEF_CLASS2(INLINE_UID_FROM_FUID(StereoSimulatorProcessor::uid), Steinberg::PClassInfo::kManyInstances, kVstAudioEffectClass, stringPluginName, Steinberg::Vst::kDistributable, Steinberg::Vst::PlugType::kFx, "1.0.0", kVstVersionString, StereoSimulatorProcessor::createInstance)
    DEF_CLASS2(INLINE_UID_FROM_FUID(StereoSimulatorController::uid), Steinberg::PClassInfo::kManyInstances, kVstComponentControllerClass, stringPluginName " Controller", 0, "", "1.0.0", kVstVersionString, StereoSimulatorController::createInstance)
END_FACTORY
