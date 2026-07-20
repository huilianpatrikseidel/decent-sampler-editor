#pragma once
#include <QObject>
#include <QTimer>
#include <QVector>
#include <QUuid>
#include "../core/models/ProjectStates.h"
#include "AudioEngine.h"

#include "AudioEngineExport.h"

class AUDIOENGINE_EXPORT PlaybackSequencer : public QObject {
    Q_OBJECT
public:
    explicit PlaybackSequencer(AudioEngine* audioEngine, QObject* parent = nullptr);
    ~PlaybackSequencer() override;

    void setTargetGroup(const QUuid& groupId);
    void setSequenceAndRate(const QVector<SequenceStep>& steps, double bpm, double rateMultiplier);
    
    void play();
    void stop();
    bool isPlaying() const;
    
signals:
    void stepChanged(int index);

private slots:
    void onTimerTick();

private:
    AudioEngine* m_audioEngine;
    QTimer* m_timer;
    QUuid m_targetGroupId;
    
    QVector<SequenceStep> m_steps;
    double m_bpm = 120.0;
    double m_rateMultiplier = 1.0;
    
    int m_currentStepIndex = -1;
};
