#pragma once
#include <QWidget>
#include <QComboBox>
#include <QSpinBox>
#include <QStackedLayout>
#include <QStackedWidget>
#include <QPushButton>
#include "MappingEditorView.h"
#include "SynthEditorView.h"
#include "ZoneImportOverlay.h"


#include "ui/mapper/MapperLibExport.h"
class MAPPERLIB_EXPORT MappingEditorContainer : public QWidget {
    Q_OBJECT
public:
    explicit MappingEditorContainer(ProjectManager* pm, QWidget* parent = nullptr);
    void setSampleGroup(const QUuid& sgId);
    MappingEditorView* getView() const { return m_view; }
    void showImportOverlay(const QStringList& files);
    
public slots:
    void updateGroupList();
    
signals:
    void groupSelected(const QUuid& groupId);
    void zoneSelected(const QUuid& sgId, int index);
    void zoneObjectSelected(const Zone* zone);
    void notePressed(int note, int vel);
    void noteReleased(int note);

private:
    ProjectManager* m_pm;
    QStackedWidget* m_modeStack;
    MappingEditorView* m_view;
    SynthEditorView* m_synthView;
    QComboBox* m_groupCombo;
    QPushButton* m_btnAddGroup;
    QPushButton* m_btnEditGroup;
    QPushButton* m_btnImportSamples;
    
    QStackedLayout* m_stackedLayout;
    QWidget* m_mainViewContainer;
    ZoneImportOverlay* m_importOverlay;
};
