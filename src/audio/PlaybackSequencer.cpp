#include "PlaybackSequencer.h"

PlaybackSequencer::PlaybackSequencer(AudioEngine* audioEngine, QObject* parent) 
    : QObject(parent), m_audioEngine(audioEngine) {
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &PlaybackSequencer::onTimerTick);
}

PlaybackSequencer::~PlaybackSequencer() {
    stop();
}

void PlaybackSequencer::setTargetGroup(const QUuid& groupId) {
    m_targetGroupId = groupId;
}

void PlaybackSequencer::setSequenceAndRate(const QVector<SequenceStep>& steps, double bpm, double rateMultiplier) {
    m_steps = steps;
    m_bpm = bpm;
    m_rateMultiplier = rateMultiplier;
    
    if (isPlaying()) {
        int intervalMs = (int)((60000.0 / m_bpm) * m_rateMultiplier);
        m_timer->setInterval(intervalMs);
    }
}

void PlaybackSequencer::play() {
    if (m_steps.isEmpty() || m_targetGroupId.isNull()) return;
    
    stop();
    
    m_currentStepIndex = -1;
    int intervalMs = (int)((60000.0 / m_bpm) * m_rateMultiplier);
    m_timer->start(intervalMs);
    onTimerTick(); 
}

void PlaybackSequencer::stop() {
    m_timer->stop();
    m_currentStepIndex = -1;
    emit stepChanged(-1);
}

bool PlaybackSequencer::isPlaying() const {
    return m_timer->isActive();
}

void PlaybackSequencer::onTimerTick() {
    if (m_steps.isEmpty()) {
        stop();
        return;
    }
    
    m_currentStepIndex++;
    if (m_currentStepIndex >= m_steps.size()) {
        m_currentStepIndex = 0; // Loop
    }
    
    emit stepChanged(m_currentStepIndex);
    
    const SequenceStep& step = m_steps[m_currentStepIndex];
    
    AudioMessage playMsg;
    playMsg.type = AudioCommandType::PlayNote;
    playMsg.note = step.note;
    playMsg.velocity = step.velocity;
    playMsg.groupId = m_targetGroupId;
    playMsg.isOscillator = true; 
    
    m_audioEngine->pushCommand(playMsg);
    
    // Schedule stop note based on length
    int intervalMs = (int)((60000.0 / m_bpm) * m_rateMultiplier);
    int durationMs = (int)(intervalMs * step.length);
    
    int noteToStop = step.note;
    QUuid targetGroup = m_targetGroupId;
    AudioEngine* engine = m_audioEngine;
    
    QTimer::singleShot(durationMs, this, [engine, targetGroup, noteToStop]() {
        AudioMessage stopMsg;
        stopMsg.type = AudioCommandType::StopNote;
        stopMsg.note = noteToStop;
        stopMsg.groupId = targetGroup;
        engine->pushCommand(stopMsg);
    });
}
