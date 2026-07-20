#pragma once
#include <QWidget>
#include <QVBoxLayout>
#include <QUuid>
#include "../../core/ProjectManager.h"
#include <QMap>

class SynthKnobWidget;

class MacrosView : public QWidget {
    Q_OBJECT
public:
    explicit MacrosView(ProjectManager* pm, QWidget* parent = nullptr);

private slots:
    void updateForm();
    void updateValues();

private:
    ProjectManager* m_pm;
    QVBoxLayout* m_layout;
    bool m_isUpdating = false;
    int m_savedScrollPos = 0;
    
    QUuid m_focusedMacroId;
    QMap<QUuid, SynthKnobWidget*> m_macroKnobs;
};
