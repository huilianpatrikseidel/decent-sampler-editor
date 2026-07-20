#include "CompressorProcessor.h"
#include "CompressorController.h"
#include "pluginterfaces/vst/ivstparameterchanges.h"
#include "pluginterfaces/base/ibstream.h"

const FUID CompressorProcessor::uid(INLINE_UID(0xdbc5d08f, 0x3cf645c4, 0x9030fbc5, 0x660ae9ff));

CompressorProcessor::CompressorProcessor() {
    setControllerClass(CompressorController::uid);
}

tresult PLUGIN_API CompressorProcessor::initialize(FUnknown* context) {
    tresult result = AudioEffect::initialize(context);
    if (result != kResultOk) return result;
    addAudioInput(STR16("Stereo In"), Steinberg::Vst::SpeakerArr::kStereo);
    addAudioOutput(STR16("Stereo Out"), Steinberg::Vst::SpeakerArr::kStereo);
    return kResultOk;
}

tresult PLUGIN_API CompressorProcessor::setupProcessing(Vst::ProcessSetup& newSetup) {
    return AudioEffect::setupProcessing(newSetup);
}

tresult PLUGIN_API CompressorProcessor::process(Vst::ProcessData& data) {
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
                    if (paramQueue->getParameterId() == 0) { m_threshold = value; }
                    if (paramQueue->getParameterId() == 1) { m_ratio = value; }
                    if (paramQueue->getParameterId() == 2) { m_attack = value; }
                    if (paramQueue->getParameterId() == 3) { m_release = value; }

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

tresult PLUGIN_API CompressorProcessor::setState(IBStream* state) { 
    if (!state) return kResultFalse;
    double val = 0.0;
    if (state->read(&val, sizeof(double)) == kResultOk) m_threshold = val;
    if (state->read(&val, sizeof(double)) == kResultOk) m_ratio = val;
    if (state->read(&val, sizeof(double)) == kResultOk) m_attack = val;
    if (state->read(&val, sizeof(double)) == kResultOk) m_release = val;
    return kResultOk;
}
tresult PLUGIN_API CompressorProcessor::getState(IBStream* state) { 
    if (!state) return kResultFalse;
    state->write(&m_threshold, sizeof(double));
    state->write(&m_ratio, sizeof(double));
    state->write(&m_attack, sizeof(double));
    state->write(&m_release, sizeof(double));
    return kResultOk;
}
