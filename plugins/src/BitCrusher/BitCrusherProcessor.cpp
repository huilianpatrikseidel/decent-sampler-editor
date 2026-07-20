#include "BitCrusherProcessor.h"
#include "BitCrusherController.h"
#include "pluginterfaces/vst/ivstparameterchanges.h"
#include "pluginterfaces/base/ibstream.h"

const FUID BitCrusherProcessor::uid(INLINE_UID(0x6206b732, 0x95c344aa, 0xa4e566fe, 0xc40796d8));

BitCrusherProcessor::BitCrusherProcessor() {
    setControllerClass(BitCrusherController::uid);
}

tresult PLUGIN_API BitCrusherProcessor::initialize(FUnknown* context) {
    tresult result = AudioEffect::initialize(context);
    if (result != kResultOk) return result;
    addAudioInput(STR16("Stereo In"), Steinberg::Vst::SpeakerArr::kStereo);
    addAudioOutput(STR16("Stereo Out"), Steinberg::Vst::SpeakerArr::kStereo);
    return kResultOk;
}

tresult PLUGIN_API BitCrusherProcessor::setupProcessing(Vst::ProcessSetup& newSetup) {
    return AudioEffect::setupProcessing(newSetup);
}

tresult PLUGIN_API BitCrusherProcessor::process(Vst::ProcessData& data) {
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
                    if (paramQueue->getParameterId() == 0) { m_bitdepth = value; }
                    if (paramQueue->getParameterId() == 1) { m_reduction = value; }
                    if (paramQueue->getParameterId() == 2) { m_mix = value; }

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

tresult PLUGIN_API BitCrusherProcessor::setState(IBStream* state) { 
    if (!state) return kResultFalse;
    double val = 0.0;
    if (state->read(&val, sizeof(double)) == kResultOk) m_bitdepth = val;
    if (state->read(&val, sizeof(double)) == kResultOk) m_reduction = val;
    if (state->read(&val, sizeof(double)) == kResultOk) m_mix = val;
    return kResultOk;
}
tresult PLUGIN_API BitCrusherProcessor::getState(IBStream* state) { 
    if (!state) return kResultFalse;
    state->write(&m_bitdepth, sizeof(double));
    state->write(&m_reduction, sizeof(double));
    state->write(&m_mix, sizeof(double));
    return kResultOk;
}
