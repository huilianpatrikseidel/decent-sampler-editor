#include "StereoSimulatorProcessor.h"
#include "StereoSimulatorController.h"
#include "pluginterfaces/vst/ivstparameterchanges.h"
#include "pluginterfaces/base/ibstream.h"

const FUID StereoSimulatorProcessor::uid(INLINE_UID(0x25328168, 0x19874b75, 0xbe7d122e, 0x2074e334));

StereoSimulatorProcessor::StereoSimulatorProcessor() {
    setControllerClass(StereoSimulatorController::uid);
}

tresult PLUGIN_API StereoSimulatorProcessor::initialize(FUnknown* context) {
    tresult result = AudioEffect::initialize(context);
    if (result != kResultOk) return result;
    addAudioInput(STR16("Stereo In"), Steinberg::Vst::SpeakerArr::kStereo);
    addAudioOutput(STR16("Stereo Out"), Steinberg::Vst::SpeakerArr::kStereo);
    return kResultOk;
}

tresult PLUGIN_API StereoSimulatorProcessor::setupProcessing(Vst::ProcessSetup& newSetup) {
    return AudioEffect::setupProcessing(newSetup);
}

tresult PLUGIN_API StereoSimulatorProcessor::process(Vst::ProcessData& data) {
    if (data.numInputs == 0 || data.numOutputs == 0) return kResultOk;
    
    if (data.inputParameterChanges) {
        int32 numParamsChanged = data.inputParameterChanges->getParameterCount();
        for (int32 index = 0; index < numParamsChanged; index++) {
            Vst::IParamValueQueue* paramQueue = data.inputParameterChanges->getParameterData(index);
            if (paramQueue) {
                Vst::ParamValue value;
                int32 sampleOffset;
                int32 numPoints = paramQueue->getPointCount();
                if (paramQueue->getPoint(numPoints - 1, sampleOffset, value) == kResultTrue) {
                    if (paramQueue->getParameterId() == 0) { m_width = value; }
                    if (paramQueue->getParameterId() == 1) { m_delaytime = value; }
                    if (paramQueue->getParameterId() == 2) { m_moddepth = value; }

                }
            }
        }
    }
    
    for (int32 i = 0; i < data.numOutputs; ++i) {
        for (int32 j = 0; j < data.numSamples; ++j) {
            data.outputs[i].channelBuffers32[0][j] = data.inputs[i].channelBuffers32[0][j];
            data.outputs[i].channelBuffers32[1][j] = data.inputs[i].channelBuffers32[1][j];
        }
    }
    return kResultOk;
}

tresult PLUGIN_API StereoSimulatorProcessor::setState(IBStream* state) { 
    if (!state) return kResultFalse;
    double val = 0.0;
    if (state->read(&val, sizeof(double)) == kResultOk) m_width = val;
    if (state->read(&val, sizeof(double)) == kResultOk) m_delaytime = val;
    if (state->read(&val, sizeof(double)) == kResultOk) m_moddepth = val;
    return kResultOk;
}
tresult PLUGIN_API StereoSimulatorProcessor::getState(IBStream* state) { 
    if (!state) return kResultFalse;
    state->write(&m_width, sizeof(double));
    state->write(&m_delaytime, sizeof(double));
    state->write(&m_moddepth, sizeof(double));
    return kResultOk;
}
