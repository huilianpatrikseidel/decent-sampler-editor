#include "AudioAnalyzer.h"
#include "../thirdparty/miniaudio/miniaudio.h"
#include <cmath>
#include <algorithm>
#include <QMetaObject>

AudioAnalyzer::AudioAnalyzer(QObject* parent) : QObject(parent) {}

AudioAnalyzer::~AudioAnalyzer() {
    m_cancelFlag = true;
    if (m_workerThread.joinable()) {
        m_workerThread.join();
    }
}

void AudioAnalyzer::detectPitchAsync(const QString& filepath) {
    if (m_workerThread.joinable()) {
        m_cancelFlag = true;
        m_workerThread.join();
    }
    m_cancelFlag = false;
    m_workerThread = std::thread(&AudioAnalyzer::detectPitchWorker, this, filepath);
}

void AudioAnalyzer::findBestLoopAsync(const QString& filepath, double startPct, double endPct) {
    if (m_workerThread.joinable()) {
        m_cancelFlag = true;
        m_workerThread.join();
    }
    m_cancelFlag = false;
    m_workerThread = std::thread(&AudioAnalyzer::findLoopWorker, this, filepath, startPct, endPct);
}

void AudioAnalyzer::detectPitchWorker(QString filepath) {
    ma_decoder decoder;
    ma_decoder_config config = ma_decoder_config_init(ma_format_f32, 1, 0); // Force mono for analysis
    
#ifdef _WIN32
    ma_result result = ma_decoder_init_file_w(filepath.toStdWString().c_str(), &config, &decoder);
#else
    ma_result result = ma_decoder_init_file(filepath.toUtf8().constData(), &config, &decoder);
#endif

    if (result != MA_SUCCESS) {
        QMetaObject::invokeMethod(this, [this, filepath](){ emit pitchDetectionFailed(filepath); }, Qt::QueuedConnection);
        return;
    }
    
    ma_uint64 totalFrames = 0;
    ma_decoder_get_length_in_pcm_frames(&decoder, &totalFrames);
    
    if (totalFrames == 0) {
        ma_decoder_uninit(&decoder);
        QMetaObject::invokeMethod(this, [this, filepath](){ emit pitchDetectionFailed(filepath); }, Qt::QueuedConnection);
        return;
    }
    
    std::vector<float> buffer(totalFrames);
    ma_uint64 framesRead = 0;
    ma_decoder_read_pcm_frames(&decoder, buffer.data(), totalFrames, &framesRead);
    ma_decoder_uninit(&decoder);
    
    // 1. Find Peak Amplitude (QA Requirement)
    size_t peakIndex = 0;
    float maxAmp = 0.0f;
    for (size_t i = 0; i < buffer.size(); ++i) {
        float amp = std::abs(buffer[i]);
        if (amp > maxAmp) {
            maxAmp = amp;
            peakIndex = i;
        }
    }
    
    // 2. Extract 8192 sample window around/after peak
    const size_t WINDOW_SIZE = 8192;
    std::vector<float> analysisWindow;
    analysisWindow.reserve(WINDOW_SIZE);
    
    size_t startIndex = peakIndex;
    // Step back slightly if possible to catch the true onset of the sustained wave
    if (startIndex > 512) startIndex -= 512;
    
    for (size_t i = 0; i < WINDOW_SIZE && (startIndex + i) < buffer.size(); ++i) {
        analysisWindow.push_back(buffer[startIndex + i]);
    }
    
    if (analysisWindow.size() < 1024) {
        QMetaObject::invokeMethod(this, [this, filepath](){ emit pitchDetectionFailed(filepath); }, Qt::QueuedConnection);
        return;
    }
    
    // 3. YIN Algorithm
    double freq = computeYin(analysisWindow, decoder.outputSampleRate);
    
    if (freq > 0.0) {
        int midiNote = static_cast<int>(std::round(69.0 + 12.0 * std::log2(freq / 440.0)));
        QMetaObject::invokeMethod(this, [this, filepath, midiNote, freq](){ 
            emit pitchDetected(filepath, midiNote, freq); 
        }, Qt::QueuedConnection);
    } else {
        QMetaObject::invokeMethod(this, [this, filepath](){ emit pitchDetectionFailed(filepath); }, Qt::QueuedConnection);
    }
}

double AudioAnalyzer::computeYin(const std::vector<float>& buffer, int sampleRate) {
    int yinBufferLength = buffer.size() / 2;
    std::vector<double> yinBuffer(yinBufferLength, 0.0);
    
    // Step 1: Difference Function
    for (int tau = 1; tau < yinBufferLength; tau++) {
        if (m_cancelFlag) return -1.0;
        for (int i = 0; i < yinBufferLength; i++) {
            double delta = buffer[i] - buffer[i + tau];
            yinBuffer[tau] += delta * delta;
        }
    }
    
    // Step 2: Cumulative Mean Normalized Difference
    yinBuffer[0] = 1.0;
    double runningSum = 0.0;
    for (int tau = 1; tau < yinBufferLength; tau++) {
        if (m_cancelFlag) return -1.0;
        runningSum += yinBuffer[tau];
        yinBuffer[tau] *= tau / runningSum;
    }
    
    // Step 3: Absolute Threshold
    int tauEstimate = -1;
    double threshold = 0.15; // standard YIN threshold
    for (int tau = 2; tau < yinBufferLength; tau++) {
        if (yinBuffer[tau] < threshold) {
            while (tau + 1 < yinBufferLength && yinBuffer[tau + 1] < yinBuffer[tau]) {
                tau++;
            }
            tauEstimate = tau;
            break;
        }
    }
    
    if (tauEstimate == -1) {
        // Fallback to absolute minimum if threshold never met
        tauEstimate = 2;
        double minVal = yinBuffer[2];
        for (int tau = 3; tau < yinBufferLength; tau++) {
            if (yinBuffer[tau] < minVal) {
                minVal = yinBuffer[tau];
                tauEstimate = tau;
            }
        }
        if (minVal > 0.5) return -1.0; // Too noisy
    }
    
    // Step 4: Parabolic Interpolation for better precision
    double betterTau = tauEstimate;
    if (tauEstimate > 0 && tauEstimate < yinBufferLength - 1) {
        double s0 = yinBuffer[tauEstimate - 1];
        double s1 = yinBuffer[tauEstimate];
        double s2 = yinBuffer[tauEstimate + 1];
        betterTau = tauEstimate + (s2 - s0) / (2.0 * (2.0 * s1 - s2 - s0));
    }
    
    return sampleRate / betterTau;
}

void AudioAnalyzer::findLoopWorker(QString filepath, double startPct, double endPct) {
    ma_decoder decoder;
    ma_decoder_config config = ma_decoder_config_init(ma_format_f32, 1, 44100); // mono
    
#ifdef _WIN32
    ma_result result = ma_decoder_init_file_w(filepath.toStdWString().c_str(), &config, &decoder);
#else
    ma_result result = ma_decoder_init_file(filepath.toUtf8().constData(), &config, &decoder);
#endif

    if (result != MA_SUCCESS) {
        QMetaObject::invokeMethod(this, [this, filepath](){ emit loopDetectionFailed(filepath); }, Qt::QueuedConnection);
        return;
    }
    
    ma_uint64 totalFrames = 0;
    ma_decoder_get_length_in_pcm_frames(&decoder, &totalFrames);
    std::vector<float> buffer(totalFrames);
    ma_uint64 framesRead = 0;
    ma_decoder_read_pcm_frames(&decoder, buffer.data(), totalFrames, &framesRead);
    ma_decoder_uninit(&decoder);
    
    if (totalFrames < 44100) { // Too short to auto-loop reasonably
        QMetaObject::invokeMethod(this, [this, filepath](){ emit loopDetectionFailed(filepath); }, Qt::QueuedConnection);
        return;
    }
    
    // Auto-Loop (SAD Phase Correlation)
    // Procurar candidatos a loop Start entre startPct e 50% da janela selecionada.
    // E Loop End entre 50% da janela selecionada e endPct.
    
    size_t searchStart = static_cast<size_t>(totalFrames * startPct);
    size_t searchEnd = static_cast<size_t>(totalFrames * endPct);
    size_t midPoint = searchStart + (searchEnd - searchStart) / 2;
    
    // Identificar Zero Crossings positivos (subindo)
    std::vector<size_t> zcStarts;
    std::vector<size_t> zcEnds;
    
    for (size_t i = searchStart; i < midPoint; ++i) {
        if (buffer[i-1] <= 0.0f && buffer[i] > 0.0f) zcStarts.push_back(i);
    }
    for (size_t i = midPoint; i < searchEnd; ++i) {
        if (buffer[i-1] <= 0.0f && buffer[i] > 0.0f) zcEnds.push_back(i);
    }
    
    if (zcStarts.empty() || zcEnds.empty()) {
        QMetaObject::invokeMethod(this, [this, filepath](){ emit loopDetectionFailed(filepath); }, Qt::QueuedConnection);
        return;
    }
    
    // Sub-amostrar os zero crossings para não travar a thread por muito tempo
    const int MAX_CANDIDATES = 100;
    if (zcStarts.size() > MAX_CANDIDATES) {
        std::vector<size_t> subsampled;
        size_t step = zcStarts.size() / MAX_CANDIDATES;
        for (size_t i = 0; i < zcStarts.size(); i += step) subsampled.push_back(zcStarts[i]);
        zcStarts = subsampled;
    }
    if (zcEnds.size() > MAX_CANDIDATES) {
        std::vector<size_t> subsampled;
        size_t step = zcEnds.size() / MAX_CANDIDATES;
        for (size_t i = 0; i < zcEnds.size(); i += step) subsampled.push_back(zcEnds[i]);
        zcEnds = subsampled;
    }
    
    size_t bestStart = 0;
    size_t bestEnd = 0;
    double minSad = 9999999.0;
    const size_t WINDOW = 512; // Analisar 512 samples pra frente (aprox 10ms)
    
    for (size_t startCand : zcStarts) {
        if (m_cancelFlag) return;
        if (startCand + WINDOW >= buffer.size()) continue;
        
        for (size_t endCand : zcEnds) {
            if (endCand + WINDOW >= buffer.size()) continue;
            
            double sad = 0.0;
            for (size_t w = 0; w < WINDOW; ++w) {
                double diff = buffer[startCand + w] - buffer[endCand + w];
                sad += std::abs(diff);
            }
            
            // Penalize short loops to favor longer loops
            double length = endCand - startCand;
            double score = sad / (length * 0.01); 
            
            if (score < minSad) {
                minSad = score;
                bestStart = startCand;
                bestEnd = endCand;
            }
        }
    }
    
    if (bestStart > 0 && bestEnd > 0 && bestEnd > bestStart) {
        double loopStartPct = static_cast<double>(bestStart) / totalFrames;
        double loopEndPct = static_cast<double>(bestEnd) / totalFrames;
        
        QMetaObject::invokeMethod(this, [this, filepath, loopStartPct, loopEndPct](){ 
            emit loopDetected(filepath, loopStartPct, loopEndPct); 
        }, Qt::QueuedConnection);
    } else {
        QMetaObject::invokeMethod(this, [this, filepath](){ emit loopDetectionFailed(filepath); }, Qt::QueuedConnection);
    }
}
