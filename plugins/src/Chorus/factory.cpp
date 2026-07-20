#include "public.sdk/source/main/pluginfactory.h"
#include "ChorusProcessor.h"
#include "ChorusController.h"
#include "ChorusUIDs.h"
#define stringPluginName "Chorus"
BEGIN_FACTORY_DEF("SamplerEditor", "www.samplereditor.com", "mailto:contact@samplereditor.com")
    DEF_CLASS2(INLINE_UID_FROM_FUID(SamplerEditor::Chorus::ChorusProcessorUID), Steinberg::PClassInfo::kManyInstances, kVstAudioEffectClass, stringPluginName, Steinberg::Vst::kDistributable, Steinberg::Vst::PlugType::kFx, "1.0.0", kVstVersionString, SamplerEditor::Chorus::ChorusProcessor::createInstance)
    DEF_CLASS2(INLINE_UID_FROM_FUID(SamplerEditor::Chorus::ChorusControllerUID), Steinberg::PClassInfo::kManyInstances, kVstComponentControllerClass, stringPluginName " Controller", 0, "", "1.0.0", kVstVersionString, SamplerEditor::Chorus::ChorusController::createInstance)
END_FACTORY
