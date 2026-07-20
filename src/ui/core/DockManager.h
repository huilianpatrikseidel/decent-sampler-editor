#pragma once
#include <QObject>
#include <QDockWidget>
class MainWindow;
class ApplicationController;
class PropertiesInspector;
class AssetManagerView;
class QComboBox;
class QToolButton;
class QPushButton;
class SampleEditorContainer;
class GroupEditorView;
class MixerView;
class FxRackView;
class ModulatorsView;
class MenuManager;
class UIDesignerView;
class PresetListWidget;
class ProjectTreeWidget;
class MappingEditorContainer;
class InstrumentCanvasView;
class QTabWidget;
class NoteSequenceEditorView;

class QSplitter;
class QWidget;

class DockManager : public QObject {
    Q_OBJECT
public:
    DockManager(MainWindow* mainWindow, ApplicationController* controller, 
                UIDesignerView* uiDesigner, MappingEditorContainer* mappingEditor,
                InstrumentCanvasView* canvas, QTabWidget* mainTabs);
    
    void setupDocks();
    void setDocksVisible(bool visible);
    
    PropertiesInspector* getInspector() const { return m_inspector; }
    QWidget* getCentralContainer() const { return m_centralContainer; }
    
public slots:
    void onTabChanged(int index);

private:
    MainWindow* m_mainWindow;
    ApplicationController* m_controller;
    UIDesignerView* m_uiDesigner;
    MappingEditorContainer* m_mappingContainer;
    InstrumentCanvasView* m_canvas;
    QTabWidget* m_mainTabs;
    
    PropertiesInspector* m_inspector;
    QDockWidget* m_propertyDock;
    QDockWidget* m_assetDock;
    QDockWidget* m_presetDock;
    QDockWidget* m_projectDock;
    
    PresetListWidget* m_presetWidget;
    ProjectTreeWidget* m_projectTreeWidget;
    
    QWidget* m_centralContainer;
    QWidget* m_editorPanel;
    QTabWidget* m_editorTabs;
    
    SampleEditorContainer* m_sampleEditorContainer = nullptr;
    GroupEditorView* m_groupEditorView = nullptr;
    MixerView* m_mixerView = nullptr;
    FxRackView* m_fxRackView = nullptr;
    ModulatorsView* m_modulatorsView = nullptr;
    NoteSequenceEditorView* m_sequencerView = nullptr;
    
    QPushButton* m_collapseBtn;
    QPushButton* m_focusBtn = nullptr;
    class FocusModeOverlay* m_focusOverlay = nullptr;
    bool m_isDockCollapsed = false;
};
