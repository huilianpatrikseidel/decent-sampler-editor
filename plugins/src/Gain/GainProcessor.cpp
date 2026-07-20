#include "GainProcessor.h"
#include "GainController.h"
#include "pluginterfaces/vst/ivstparameterchanges.h"
#include "pluginterfaces/base/ibstream.h"
#include <cmath>

const Steinberg::FUID GainProcessor::uid(INLINE_UID(0x279ab761, 0xa6c94a00, 0xa817049a, 0x621de76d));

GainProcessor::GainProcessor() {
    setControllerClass(GainController::uid);
}

Steinberg::tresult PLUGIN_API GainProcessor::initialize(Steinberg::FUnknown* context) {
    Steinberg::tresult result = AudioEffect::initialize(context);
    if (result != Steinberg::kResultOk) return result;

    addAudioInput(STR16("Stereo In"), Steinberg::Vst::SpeakerArr::kStereo);
    addAudioOutput(STR16("Stereo Out"), Steinberg::Vst::SpeakerArr::kStereo);

    return Steinberg::kResultOk;
}

Steinberg::tresult PLUGIN_API GainProcessor::process(Steinberg::Vst::ProcessData& data) {
    if (data.numInputs == 0 || data.numOutputs == 0)
        return Steinberg::kResultOk;

    if (data.inputParameterChanges) {
        Steinberg::int32 numParamsChanged = data.inputParameterChanges->getParameterCount();
        for (Steinberg::int32 index = 0; index < numParamsChanged; index++) {
            Steinberg::Vst::IParamValueQueue* paramQueue = data.inputParameterChanges->getParameterData(index);
            if (paramQueue) {
                Steinberg::Vst::ParamValue value;
                Steinberg::int32 sampleOffset;
                Steinberg::int32 numPoints = paramQueue->getPointCount();
                if (paramQueue->getPoint(numPoints - 1, sampleOffset, value) == Steinberg::kResultTrue) {
                    if (paramQueue->getParameterId() == 0) { // Gain ID
                        m_gain = value;
                    }
                }
            }
        }
    }

    Steinberg::Vst::SpeakerArrangement arr;
    getBusArrangement(Steinberg::Vst::kOutput, 0, arr);
    int numChannels = Steinberg::Vst::SpeakerArr::getChannelCount(arr);

    for (int ch = 0; ch < numChannels; ch++) {
        float* inChannel = data.inputs[0].channelBuffers32[ch];
        float* outChannel = data.outputs[0].channelBuffers32[ch];
        
        for (int i = 0; i < data.numSamples; i++) {
            outChannel[i] = inChannel[i] * m_gain;
        }
    }

    return Steinberg::kResultOk;
}

Steinberg::tresult PLUGIN_API GainProcessor::setState(Steinberg::IBStream* state) {
    if (!state)
        return Steinberg::kResultFalse;

    double savedGain = 0.0;
    if (state->read(&savedGain, sizeof(double)) != Steinberg::kResultOk)
        return Steinberg::kResultFalse;

    m_gain = savedGain;
    return Steinberg::kResultOk;
}

Steinberg::tresult PLUGIN_API GainProcessor::getState(Steinberg::IBStream* state) {
    if (!state)
        return Steinberg::kResultFalse;

    state->write(&m_gain, sizeof(double));
    return Steinberg::kResultOk;
}

Steinberg::tresult PLUGIN_API GainProcessor::setupProcessing(Steinberg::Vst::ProcessSetup& newSetup) {
    return AudioEffect::setupProcessing(newSetup);
}
