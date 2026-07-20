#pragma once
#include <QWidget>
#include <QUuid>
#include "../../core/ProjectManager.h"
#include "../../core/ApplicationController.h"

class WaveformView;
class WaveformEditorView;
class QFormLayout;
class QTabWidget;

class SampleEditorContainer : public QWidget {
    Q_OBJECT
public:
    explicit SampleEditorContainer(ApplicationController* controller, QWidget* parent = nullptr);

public slots:
    void setZone(const QUuid& sgId, int zoneIndex, const Zone* zone);
    void setFocusMode(bool focus);

private:
    ApplicationController* m_controller;
    QUuid m_currentSgId;
    int m_currentZoneIndex = -1;
    
    class QBoxLayout* m_mainLayout;
    QTabWidget* m_tabWidget;
    WaveformView* m_waveformView;
    WaveformEditorView* m_waveformEditorView;
    QFormLayout* m_propsLayout;
    
    bool m_isUpdatingUI = false;

    void rebuildPropsForm(const Zone* zone);
};
