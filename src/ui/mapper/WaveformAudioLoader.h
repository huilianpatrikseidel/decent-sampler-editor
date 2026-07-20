#pragma once
#include <QObject>
#include <QString>
#include <vector>
#include <thread>
#include <atomic>


#include "ui/mapper/MapperLibExport.h"
class MAPPERLIB_EXPORT WaveformAudioLoader : public QObject {
    Q_OBJECT
public:
    explicit WaveformAudioLoader(QObject* parent = nullptr);
    ~WaveformAudioLoader();

    void load(const QString& filepath);
    void cancel();

signals:
    void loadCompleted(std::vector<float> peaks, long long totalFrames);
    void loadFailed(const QString& errorMsg);
    void loadStarted(const QString& filename);

private:
    std::atomic<bool> m_cancelFlag{false};
    std::thread m_workerThread;
    
    void loadAudioWorker(QString filepath);
};
