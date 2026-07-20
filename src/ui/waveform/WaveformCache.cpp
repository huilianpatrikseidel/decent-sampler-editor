#include "WaveformCache.h"
#include "thirdparty/miniaudio/miniaudio.h"
#include <cmath>
#include <QMetaObject>

WaveformCache::WaveformCache(QObject* parent) 
    : QObject(parent), m_cancelFlag(false), m_isReady(false) {}

WaveformCache::~WaveformCache() {
    cancel();
}

void WaveformCache::cancel() {
    m_cancelFlag = true;
    if (m_thread.joinable()) {
        m_thread.join();
    }
}

bool WaveformCache::isReady() const {
    return m_isReady;
}

const QVector<WaveformPeak>& WaveformCache::getPeaks() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_peaks;
}

void WaveformCache::loadAudio(const QString& filepath) {
    cancel();
    m_cancelFlag = false;
    m_isReady = false;
    
    std::lock_guard<std::mutex> lock(m_mutex);
    m_peaks.clear();
    m_totalFrames = 0;
    m_sampleRate = 0;
    
    emit loadStarted();
    m_thread = std::thread(&WaveformCache::workerThread, this, filepath);
}

void WaveformCache::workerThread(QString filepath) {
    ma_decoder decoder;
    ma_decoder_config config = ma_decoder_config_init(ma_format_f32, 0, 0);
    
#ifdef _WIN32
    ma_result result = ma_decoder_init_file_w(filepath.toStdWString().c_str(), &config, &decoder);
#else
    ma_result result = ma_decoder_init_file(filepath.toUtf8().constData(), &config, &decoder);
#endif

    if (result != MA_SUCCESS) {
        QMetaObject::invokeMethod(this, [this](){ emit loadFinished(false); }, Qt::QueuedConnection);
        return;
    }
    
    ma_uint64 totalFrames = 0;
    ma_decoder_get_length_in_pcm_frames(&decoder, &totalFrames);
    m_totalFrames = totalFrames;
    m_sampleRate = decoder.outputSampleRate;
    int channels = decoder.outputChannels;
    
    const int BLOCK_SIZE = 256; 
    const int CHUNK_FRAMES = 1024 * 16; 
    std::vector<float> buffer(CHUNK_FRAMES * channels);
    
    QVector<WaveformPeak> localPeaks;
    localPeaks.reserve((totalFrames / BLOCK_SIZE) + 1);
    
    ma_uint64 framesRead = 0;
    float currentMin = 1.0f;
    float currentMax = -1.0f;
    int samplesInBlock = 0;
    
    while (true) {
        if (m_cancelFlag) break;
        ma_uint64 read = 0;
        ma_result result = ma_decoder_read_pcm_frames(&decoder, buffer.data(), CHUNK_FRAMES, &read);
        framesRead = read;
        if (framesRead == 0) break;
        
        for (ma_uint64 i = 0; i < framesRead; ++i) {
            float monoSample = 0.0f;
            for (int c = 0; c < channels; ++c) {
                monoSample += buffer[i * channels + c];
            }
            monoSample /= channels;
            
            if (monoSample < currentMin) currentMin = monoSample;
            if (monoSample > currentMax) currentMax = monoSample;
            
            samplesInBlock++;
            if (samplesInBlock >= BLOCK_SIZE) {
                localPeaks.push_back({currentMin, currentMax});
                currentMin = 1.0f;
                currentMax = -1.0f;
                samplesInBlock = 0;
            }
        }
    }
    
    if (samplesInBlock > 0) {
        localPeaks.push_back({currentMin, currentMax});
    }
    
    ma_decoder_uninit(&decoder);
    
    if (!m_cancelFlag) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_peaks = std::move(localPeaks);
        m_isReady = true;
        QMetaObject::invokeMethod(this, [this](){ emit loadFinished(true); }, Qt::QueuedConnection);
    }
}

QPolygonF WaveformCache::getPolygon(int viewWidth, int viewHeight) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    QPolygonF poly;
    if (m_peaks.isEmpty() || viewWidth <= 0 || viewHeight <= 0) return poly;
    
    double xStep = static_cast<double>(viewWidth) / m_peaks.size();
    float halfHeight = viewHeight / 2.0f;
    
    // Top half (Max values)
    for (int i = 0; i < m_peaks.size(); ++i) {
        poly << QPointF(i * xStep, halfHeight - (m_peaks[i].maxVal * halfHeight));
    }
    // Bottom half (Min values - reversed for polygon closure)
    for (int i = m_peaks.size() - 1; i >= 0; --i) {
        poly << QPointF(i * xStep, halfHeight - (m_peaks[i].minVal * halfHeight));
    }
    
    return poly;
}
