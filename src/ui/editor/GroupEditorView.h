#pragma once
#include <QWidget>
#include <QHBoxLayout>
#include <QUuid>
#include "../../core/ProjectManager.h"
#include "../../core/ApplicationController.h"

class GroupEditorView : public QWidget {
    Q_OBJECT
public:
    explicit GroupEditorView(ApplicationController* controller, QWidget* parent = nullptr);

public slots:
    void setGroup(const QUuid& sgId);
    void setFocusMode(bool focus);

private slots:
    void onNodeModified(const QUuid& id, const QString& propertyName);

private:
    ApplicationController* m_controller;
    QUuid m_currentSgId;
    QHBoxLayout* m_layout;
    bool m_focusMode = false;
    
    bool m_isUpdatingUI = false;

    void rebuildForm();
};
