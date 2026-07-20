#include "WaveformAudioLoader.h"
#include <QFileInfo>
#include "../../thirdparty/miniaudio/miniaudio.h"

WaveformAudioLoader::WaveformAudioLoader(QObject* parent) : QObject(parent) {}

WaveformAudioLoader::~WaveformAudioLoader() {
    cancel();
}

void WaveformAudioLoader::cancel() {
    m_cancelFlag = true;
    if (m_workerThread.joinable()) {
        m_workerThread.join();
    }
}

void WaveformAudioLoader::load(const QString& filepath) {
    cancel(); // cancel any previous load
    m_cancelFlag = false;
    
    if (filepath.isEmpty()) {
        emit loadFailed("No sample selected");
        return;
    }
    
    emit loadStarted(QString("Loading %1...").arg(QFileInfo(filepath).fileName()));
    
    m_workerThread = std::thread(&WaveformAudioLoader::loadAudioWorker, this, filepath);
}

void WaveformAudioLoader::loadAudioWorker(QString filepath) {
    ma_decoder decoder;
    ma_decoder_config config = ma_decoder_config_init(ma_format_f32, 1, 0); 
    
#ifdef _WIN32
    ma_result result = ma_decoder_init_file_w(filepath.toStdWString().c_str(), &config, &decoder);
#else
    ma_result result = ma_decoder_init_file(filepath.toUtf8().constData(), &config, &decoder);
#endif

    if (result != MA_SUCCESS) {
        emit loadFailed("Failed to load audio");
        return;
    }
    
    ma_uint64 totalFrames = 0;
    ma_decoder_get_length_in_pcm_frames(&decoder, &totalFrames);
    
    if (totalFrames == 0) {
        ma_decoder_uninit(&decoder);
        emit loadFailed("Invalid audio length");
        return;
    }
    
    int numPeaks = 4000;
    std::vector<float> peaks;
    peaks.reserve(numPeaks);
    
    ma_uint64 framesPerPeak = totalFrames / numPeaks;
    if (framesPerPeak == 0) framesPerPeak = 1;
    
    std::vector<float> chunk(framesPerPeak);
    
    for (int i = 0; i < numPeaks; ++i) {
        if (m_cancelFlag) break;
        ma_uint64 framesRead = 0;
        ma_decoder_read_pcm_frames(&decoder, chunk.data(), framesPerPeak, &framesRead);
        if (framesRead == 0) break;
        
        float maxAmp = 0.0f;
        for (ma_uint64 j = 0; j < framesRead; ++j) {
            float amp = std::abs(chunk[j]);
            if (amp > maxAmp) maxAmp = amp;
        }
        peaks.push_back(maxAmp);
    }
    
    ma_decoder_uninit(&decoder);
    
    if (!m_cancelFlag) {
        emit loadCompleted(std::move(peaks), totalFrames);
    }
}
