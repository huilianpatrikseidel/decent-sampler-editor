#include "InitWorker.h"
#include "ApplicationController.h"

InitWorker::InitWorker(ApplicationController* controller, QObject *parent)
    : QObject(parent), m_controller(controller) {}

void InitWorker::run() {
    emit progressChanged(0, "Starting initialization...");
    
    bool success = m_controller->initializeSubsystems(
        [this](int percent, const QString& message) {
            emit progressChanged(percent, message);
        },
        [this]() -> bool {
            return isCancelled();
        }
    );
    
    if (isCancelled()) {
        emit finished(false);
        return;
    }
    
    emit progressChanged(100, "Ready!");
    emit finished(success);
}
