#pragma once
#include <QObject>

class ApplicationController;

class InitWorker : public QObject {
    Q_OBJECT
public:
    explicit InitWorker(ApplicationController* controller, QObject *parent = nullptr);

public slots:
    void run();
    void cancel() { m_cancelRequested.store(true, std::memory_order_relaxed); }

    bool isCancelled() const { return m_cancelRequested.load(std::memory_order_relaxed); }

signals:
    void progressChanged(int percent, const QString& message);
    void finished(bool success);

private:
    ApplicationController* m_controller;
    std::atomic<bool> m_cancelRequested{false};
};
