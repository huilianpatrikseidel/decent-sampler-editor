#include "EqualizerProcessor.h"
#include "EqualizerController.h"
#include "pluginterfaces/vst/ivstparameterchanges.h"
#include "pluginterfaces/base/ibstream.h"
#include <cmath>

const FUID EqualizerProcessor::uid(INLINE_UID(0xa7a9aea6, 0x7a2c4fa2, 0x855c85e7, 0xfddf8985));

EqualizerProcessor::EqualizerProcessor() {
    setControllerClass(EqualizerController::uid);
}

tresult PLUGIN_API EqualizerProcessor::initialize(FUnknown* context) {
    tresult result = AudioEffect::initialize(context);
    if (result != kResultOk) return result;
    addAudioInput(STR16("Stereo In"), Steinberg::Vst::SpeakerArr::kStereo);
    addAudioOutput(STR16("Stereo Out"), Steinberg::Vst::SpeakerArr::kStereo);
    return kResultOk;
}

tresult PLUGIN_API EqualizerProcessor::setupProcessing(Vst::ProcessSetup& newSetup) {
    tresult result = AudioEffect::setupProcessing(newSetup);
    if (result == kResultOk) {
        m_sampleRate = newSetup.sampleRate;
        updateFilters();
    }
    return result;
}

tresult PLUGIN_API EqualizerProcessor::setActive(TBool state) {
    if (state) {
        for (int i = 0; i < MAX_EQ_BANDS; ++i) {
            m_filters[i][0].reset();
            m_filters[i][1].reset();
        }
    }
    return AudioEffect::setActive(state);
}

void EqualizerProcessor::updateFilters() {
    for (int i = 0; i < MAX_EQ_BANDS; ++i) {
        if (m_bands[i].enabled) {
            m_filters[i][0].calculateCoefficients(m_bands[i].type, m_sampleRate, m_bands[i].freq, m_bands[i].q, m_bands[i].gain);
            m_filters[i][1].calculateCoefficients(m_bands[i].type, m_sampleRate, m_bands[i].freq, m_bands[i].q, m_bands[i].gain);
        }
    }
}

tresult PLUGIN_API EqualizerProcessor::process(Vst::ProcessData& data) {
    if (data.numInputs == 0 || data.numOutputs == 0) return kResultOk;
    
    bool filtersChanged = false;

    if (data.inputParameterChanges) {
        int32 numParamsChanged = data.inputParameterChanges->getParameterCount();
        for (int32 index = 0; index < numParamsChanged; index++) {
            Vst::IParamValueQueue* paramQueue = data.inputParameterChanges->getParameterData(index);
            if (paramQueue) {
                Vst::ParamValue value;
                int32 sampleOffset;
                int32 numPoints = paramQueue->getPointCount();
                if (paramQueue->getPoint(numPoints - 1, sampleOffset, value) == kResultTrue) {
                    uint32 paramId = paramQueue->getParameterId();
                    
                    int bandIndex = paramId / 10;
                    int paramType = paramId % 10;

                    if (bandIndex >= 0 && bandIndex < MAX_EQ_BANDS) {
                        filtersChanged = true;
                        if (paramType == 0) m_bands[bandIndex].enabled = (value > 0.5);
                        else if (paramType == 1) { // Freq: 20 to 20000 log scaled
                            m_bands[bandIndex].freq = 20.0 * std::pow(1000.0, value);
                        }
                        else if (paramType == 2) { // Gain: -30 to 30
                            m_bands[bandIndex].gain = (value * 60.0) - 30.0;
                        }
                        else if (paramType == 3) { // Q: 0.1 to 40.0
                            m_bands[bandIndex].q = 0.1 * std::pow(400.0, value);
                        }
                        else if (paramType == 4) { // Type
                            int typeVal = std::round(value * 6.0);
                            m_bands[bandIndex].type = static_cast<FilterType>(typeVal);
                        }
                    }
                }
            }
        }
    }

    if (filtersChanged) {
        updateFilters();
    }
    
    int32 numChannels = data.inputs[0].numChannels;
    int32 numSamples = data.numSamples;

    for (int32 ch = 0; ch < numChannels; ++ch) {
        float* inChannel = data.inputs[0].channelBuffers32[ch];
        float* outChannel = data.outputs[0].channelBuffers32[ch];

        for (int32 s = 0; s < numSamples; ++s) {
            double sample = inChannel[s];
            
            for (int b = 0; b < MAX_EQ_BANDS; ++b) {
                if (m_bands[b].enabled) {
                    sample = m_filters[b][ch].process(sample);
                }
            }
            outChannel[s] = static_cast<float>(sample);
        }
    }
    return kResultOk;
}

tresult PLUGIN_API EqualizerProcessor::setState(IBStream* state) { 
    if (!state) return kResultFalse;
    
    for (int i = 0; i < MAX_EQ_BANDS; ++i) {
        int enabledInt = 0;
        if (state->read(&enabledInt, sizeof(int)) != kResultOk) return kResultFalse;
        m_bands[i].enabled = (enabledInt != 0);

        if (state->read(&m_bands[i].freq, sizeof(double)) != kResultOk) return kResultFalse;
        if (state->read(&m_bands[i].gain, sizeof(double)) != kResultOk) return kResultFalse;
        if (state->read(&m_bands[i].q, sizeof(double)) != kResultOk) return kResultFalse;
        
        int typeInt = 0;
        if (state->read(&typeInt, sizeof(int)) != kResultOk) return kResultFalse;
        m_bands[i].type = static_cast<FilterType>(typeInt);
    }
    updateFilters();
    return kResultOk;
}

tresult PLUGIN_API EqualizerProcessor::getState(IBStream* state) { 
    if (!state) return kResultFalse;

    for (int i = 0; i < MAX_EQ_BANDS; ++i) {
        int enabledInt = m_bands[i].enabled ? 1 : 0;
        state->write(&enabledInt, sizeof(int));
        state->write(&m_bands[i].freq, sizeof(double));
        state->write(&m_bands[i].gain, sizeof(double));
        state->write(&m_bands[i].q, sizeof(double));
        
        int typeInt = static_cast<int>(m_bands[i].type);
        state->write(&typeInt, sizeof(int));
    }
    return kResultOk;
}
