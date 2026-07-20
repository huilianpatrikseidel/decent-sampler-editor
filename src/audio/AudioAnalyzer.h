#pragma once
#include <QString>
#include <QObject>
#include <vector>
#include <thread>
#include <atomic>

#include "AudioEngineExport.h"

class AUDIOENGINE_EXPORT AudioAnalyzer : public QObject {
    Q_OBJECT
public:
    explicit AudioAnalyzer(QObject* parent = nullptr);
    ~AudioAnalyzer();

    // Async task for YIN Pitch Detection
    void detectPitchAsync(const QString& filepath);
    
    // Async task for Auto-Loop (SAD)
    struct LoopPoints {
        double loopStart = 0.0; // In normalized percentage (0.0 to 1.0)
        double loopEnd = 1.0;
    };
    void findBestLoopAsync(const QString& filepath, double searchStartPercent = 0.3, double searchEndPercent = 0.9);

signals:
    void pitchDetected(const QString& filepath, int midiNote, double frequency);
    void pitchDetectionFailed(const QString& filepath);
    
    void loopDetected(const QString& filepath, double loopStartPercent, double loopEndPercent);
    void loopDetectionFailed(const QString& filepath);

private:
    std::thread m_workerThread;
    std::atomic<bool> m_cancelFlag{false};

    void detectPitchWorker(QString filepath);
    void findLoopWorker(QString filepath, double startPct, double endPct);
    
    // YIN Algorithm Implementation
    double computeYin(const std::vector<float>& buffer, int sampleRate);
};
