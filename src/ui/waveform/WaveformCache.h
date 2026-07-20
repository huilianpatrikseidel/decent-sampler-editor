#pragma once
#include <QObject>
#include <QString>
#include <QVector>
#include <QPolygonF>
#include <atomic>
#include <thread>
#include <mutex>

struct WaveformPeak {
    float minVal = 0.0f;
    float maxVal = 0.0f;
};

class WaveformCache : public QObject {
    Q_OBJECT
public:
    explicit WaveformCache(QObject* parent = nullptr);
    ~WaveformCache();

    void loadAudio(const QString& filepath);
    void cancel();

    bool isReady() const;
    const QVector<WaveformPeak>& getPeaks() const;
    
    // Total frames in the audio file
    int64_t getTotalFrames() const { return m_totalFrames; }
    int getSampleRate() const { return m_sampleRate; }
    
    // Provides a visual polygon for a given view width
    QPolygonF getPolygon(int viewWidth, int viewHeight) const;

signals:
    void loadStarted();
    void loadProgress(int percent);
    void loadFinished(bool success);

private:
    void workerThread(QString filepath);

    QVector<WaveformPeak> m_peaks; // Cache of min/max per block
    int64_t m_totalFrames = 0;
    int m_sampleRate = 0;
    
    std::atomic<bool> m_cancelFlag;
    std::atomic<bool> m_isReady;
    std::thread m_thread;
    mutable std::mutex m_mutex;
};
