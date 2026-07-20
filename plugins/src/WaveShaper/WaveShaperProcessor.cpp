#include "WaveShaperProcessor.h"
#include "WaveShaperController.h"
#include "pluginterfaces/vst/ivstparameterchanges.h"
#include "pluginterfaces/base/ibstream.h"

const FUID WaveShaperProcessor::uid(INLINE_UID(0x1c39cc42, 0x8277478e, 0x85c1a220, 0x331413e8));

WaveShaperProcessor::WaveShaperProcessor() {
    setControllerClass(WaveShaperController::uid);
}

tresult PLUGIN_API WaveShaperProcessor::initialize(FUnknown* context) {
    tresult result = AudioEffect::initialize(context);
    if (result != kResultOk) return result;
    addAudioInput(STR16("Stereo In"), Steinberg::Vst::SpeakerArr::kStereo);
    addAudioOutput(STR16("Stereo Out"), Steinberg::Vst::SpeakerArr::kStereo);
    return kResultOk;
}

tresult PLUGIN_API WaveShaperProcessor::setupProcessing(Vst::ProcessSetup& newSetup) {
    return AudioEffect::setupProcessing(newSetup);
}

tresult PLUGIN_API WaveShaperProcessor::process(Vst::ProcessData& data) {
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
                    if (paramQueue->getParameterId() == 0) { m_drive = value; }
                    if (paramQueue->getParameterId() == 1) { m_outputlevel = value; }

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

tresult PLUGIN_API WaveShaperProcessor::setState(IBStream* state) { 
    if (!state) return kResultFalse;
    double val = 0.0;
    if (state->read(&val, sizeof(double)) == kResultOk) m_drive = val;
    if (state->read(&val, sizeof(double)) == kResultOk) m_outputlevel = val;
    return kResultOk;
}
tresult PLUGIN_API WaveShaperProcessor::getState(IBStream* state) { 
    if (!state) return kResultFalse;
    state->write(&m_drive, sizeof(double));
    state->write(&m_outputlevel, sizeof(double));
    return kResultOk;
}
