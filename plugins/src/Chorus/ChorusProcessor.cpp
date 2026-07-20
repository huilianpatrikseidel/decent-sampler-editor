#include "ChorusProcessor.h"
#include "ChorusController.h"
#include "pluginterfaces/vst/ivstparameterchanges.h"
#include "pluginterfaces/base/ibstream.h"
#include <cmath>

namespace SamplerEditor {
namespace Chorus {

ChorusProcessor::ChorusProcessor() {
    setControllerClass(ChorusControllerUID);
}

ChorusProcessor::~ChorusProcessor() {
    if (m_bufferL) delete[] m_bufferL;
    if (m_bufferR) delete[] m_bufferR;
}

Steinberg::tresult PLUGIN_API ChorusProcessor::initialize(Steinberg::FUnknown* context) {
    Steinberg::tresult result = AudioEffect::initialize(context);
    if (result != Steinberg::kResultOk) return result;

    addAudioInput(STR16("Stereo In"), Steinberg::Vst::SpeakerArr::kStereo);
    addAudioOutput(STR16("Stereo Out"), Steinberg::Vst::SpeakerArr::kStereo);

    return Steinberg::kResultOk;
}

Steinberg::tresult PLUGIN_API ChorusProcessor::terminate() {
    return AudioEffect::terminate();
}

Steinberg::tresult PLUGIN_API ChorusProcessor::setBusArrangements(Steinberg::Vst::SpeakerArrangement* inputs,
                                                                  Steinberg::int32 numIns,
                                                                  Steinberg::Vst::SpeakerArrangement* outputs,
                                                                  Steinberg::int32 numOuts) {
    return AudioEffect::setBusArrangements(inputs, numIns, outputs, numOuts);
}

Steinberg::tresult PLUGIN_API ChorusProcessor::setActive(Steinberg::TBool state) {
    if (state) {
        m_bufferSize = (int)(processSetup.sampleRate * 2.0); // 2 seconds max
        m_bufferL = new float[m_bufferSize];
        m_bufferR = new float[m_bufferSize];
        for (int i=0; i<m_bufferSize; i++) {
            m_bufferL[i] = 0.0f;
            m_bufferR[i] = 0.0f;
        }
        m_writePos = 0;
        m_phase = 0.0f;
    } else {
        if (m_bufferL) delete[] m_bufferL;
        if (m_bufferR) delete[] m_bufferR;
        m_bufferL = nullptr;
        m_bufferR = nullptr;
    }
    return AudioEffect::setActive(state);
}

Steinberg::tresult PLUGIN_API ChorusProcessor::process(Steinberg::Vst::ProcessData& data) {
    if (data.numInputs == 0 || data.numOutputs == 0) return Steinberg::kResultOk;

    if (data.inputParameterChanges) {
        Steinberg::int32 numParamsChanged = data.inputParameterChanges->getParameterCount();
        for (Steinberg::int32 index = 0; index < numParamsChanged; index++) {
            Steinberg::Vst::IParamValueQueue* paramQueue = data.inputParameterChanges->getParameterData(index);
            if (paramQueue) {
                Steinberg::Vst::ParamValue value;
                Steinberg::int32 sampleOffset;
                Steinberg::int32 numPoints = paramQueue->getPointCount();
                if (paramQueue->getPoint(numPoints - 1, sampleOffset, value) == Steinberg::kResultTrue) {
                    if (paramQueue->getParameterId() == 0) m_rate = value;
                    if (paramQueue->getParameterId() == 1) m_depth = value;
                    if (paramQueue->getParameterId() == 2) m_mix = value;
                }
            }
        }
    }

    float* inL = data.inputs[0].channelBuffers32[0];
    float* inR = data.inputs[0].channelBuffers32[1];
    float* outL = data.outputs[0].channelBuffers32[0];
    float* outR = data.outputs[0].channelBuffers32[1];

    if (!m_bufferL || !m_bufferR) return Steinberg::kResultOk;

    for (int i = 0; i < data.numSamples; i++) {
        m_bufferL[m_writePos] = inL[i];
        m_bufferR[m_writePos] = inR[i];

        // LFO
        float lfo = sin(m_phase * 2.0f * 3.1415926535f);
        m_phase += (m_rate * 5.0f) / processSetup.sampleRate;
        if (m_phase > 1.0f) m_phase -= 1.0f;

        float delayTime = 0.02f + (lfo * m_depth * 0.01f);
        int delaySamples = (int)(delayTime * processSetup.sampleRate);

        int readPos = m_writePos - delaySamples;
        if (readPos < 0) readPos += m_bufferSize;

        outL[i] = inL[i] * (1.0f - m_mix) + m_bufferL[readPos] * m_mix;
        outR[i] = inR[i] * (1.0f - m_mix) + m_bufferR[readPos] * m_mix;

        m_writePos++;
        if (m_writePos >= m_bufferSize) m_writePos = 0;
    }

    return Steinberg::kResultOk;
}

Steinberg::tresult PLUGIN_API ChorusProcessor::setState(Steinberg::IBStream* state) {
    if (!state) return Steinberg::kResultFalse;
    double val;
    if (state->read(&val, sizeof(double)) == Steinberg::kResultOk) m_rate = val;
    if (state->read(&val, sizeof(double)) == Steinberg::kResultOk) m_depth = val;
    if (state->read(&val, sizeof(double)) == Steinberg::kResultOk) m_mix = val;
    return Steinberg::kResultOk;
}

Steinberg::tresult PLUGIN_API ChorusProcessor::getState(Steinberg::IBStream* state) {
    if (!state) return Steinberg::kResultFalse;
    double val;
    val = m_rate; state->write(&val, sizeof(double));
    val = m_depth; state->write(&val, sizeof(double));
    val = m_mix; state->write(&val, sizeof(double));
    return Steinberg::kResultOk;
}

} // namespace Chorus
} // namespace SamplerEditor

