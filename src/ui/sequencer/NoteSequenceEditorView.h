#pragma once
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QPushButton>
#include <QComboBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QTimer>
#include "../../core/ApplicationController.h"
#include "../../core/ProjectManager.h"

class SequenceGraphWidget;

class NoteSequenceEditorView : public QWidget {
    Q_OBJECT
public:
    explicit NoteSequenceEditorView(ApplicationController* appCtrl, QWidget* parent = nullptr);
    ~NoteSequenceEditorView();

private slots:
    void updateUi();
    void addStep();
    void removeStep();
    void saveChanges();
    void sequenceSelected(int index);
    
    // Player slots
    void togglePlayback();
    
private:
    void buildUi();
    void populateSteps();
    void updatePlaybackUi(bool isPlaying);

    ApplicationController* m_appCtrl;
    ProjectManager* m_pm;
    QComboBox* m_seqCombo;
    QPushButton* m_btnAddSeq;
    
    // Transport & Settings
    QPushButton* m_btnPlay;
    QDoubleSpinBox* m_spinBpm;
    QComboBox* m_comboRate;
    QSpinBox* m_spinTsNum;
    QComboBox* m_comboTsDen;
    QComboBox* m_comboTargetGroup;
    
    SequenceGraphWidget* m_graphWidget;
    
    QPushButton* m_btnAddStep;
    QPushButton* m_btnRemoveStep;
    QPushButton* m_btnSave;
    
    int m_currentIndex = -1;
    bool m_isPopulating = false;
};
