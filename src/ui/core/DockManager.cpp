#include "DockManager.h"
#include "MenuManager.h"
#include "../../core/ApplicationController.h"
#include "../MainWindow.h"
#include "../designer/UIDesignerView.h"
#include "../mapper/MappingEditorContainer.h"
#include "../canvas/InstrumentCanvasView.h"
#include "../inspector/PropertiesInspector.h"
#include "../assets/AssetManagerView.h"
#include "../editor/SampleEditorContainer.h"
#include "../editor/GroupEditorView.h"
#include "../mapper/WaveformView.h"
#include "../mixer/MixerView.h"
#include "../editor/ModulatorsView.h"
#include "../../commands/ModifyPropertyCommand.h"
#include "../../core/commands/ModifyZonePropertyCommand.h"
#include "../../commands/UiCommands.h"
#include "../sequencer/NoteSequenceEditorView.h"
#include <QDockWidget>
#include <QSplitter>
#include <QFileInfo>
#include <QGraphicsScene>
#include <QTabWidget>
#include <QTimer>
#include "PresetListWidget.h"
#include "ProjectTreeWidget.h"
#include "FocusModeOverlay.h"

DockManager::DockManager(MainWindow* mainWindow, ApplicationController* controller, 
                         UIDesignerView* uiDesigner, MappingEditorContainer* mappingEditor,
                         InstrumentCanvasView* canvas, QTabWidget* mainTabs)
    : QObject(mainWindow), m_mainWindow(mainWindow), m_controller(controller),
      m_uiDesigner(uiDesigner), m_mappingContainer(mappingEditor), m_canvas(canvas), m_mainTabs(mainTabs) {}

void DockManager::setupDocks() {
    m_mainWindow->setTabPosition(Qt::AllDockWidgetAreas, QTabWidget::North);

    m_propertyDock = new QDockWidget("Properties Panel", m_mainWindow);
    m_propertyDock->setObjectName("PropertyDock");
    m_propertyDock->setAllowedAreas(Qt::RightDockWidgetArea | Qt::LeftDockWidgetArea);
    m_propertyDock->setTitleBarWidget(new QWidget());
    
    m_inspector = new PropertiesInspector(m_controller->getProjectManager(), m_propertyDock);
    m_inspector->setMinimumHeight(200);
    m_propertyDock->setWidget(m_inspector);
    m_mainWindow->addDockWidget(Qt::RightDockWidgetArea, m_propertyDock);
    
    m_assetDock = new QDockWidget("UI Assets", m_mainWindow);
    m_assetDock->setObjectName("AssetDock");
    m_assetDock->setAllowedAreas(Qt::RightDockWidgetArea | Qt::LeftDockWidgetArea);
    m_assetDock->setTitleBarWidget(new QWidget());
    
    QSplitter* libSplitter = new QSplitter(Qt::Vertical, m_assetDock);
    if (m_uiDesigner && m_uiDesigner->getLibraryWidget()) {
        libSplitter->addWidget(m_uiDesigner->getLibraryWidget());
    }
    
    AssetManagerView* assetManager = new AssetManagerView(m_controller->getProjectManager(), libSplitter);
    assetManager->setMinimumWidth(280);
    libSplitter->addWidget(assetManager);
    
    m_assetDock->setWidget(libSplitter);
    m_mainWindow->addDockWidget(Qt::RightDockWidgetArea, m_assetDock);
    m_mainWindow->tabifyDockWidget(m_propertyDock, m_assetDock);
    m_propertyDock->raise();
    
    connect(m_controller->getProjectManager(), &ProjectManager::nodeModified, m_inspector, &PropertiesInspector::onNodeModified);
    connect(m_inspector, &PropertiesInspector::propertyChanged, [this](const QUuid& id, const QString& prop, const QVariant& oldV, const QVariant& newV){
        m_controller->getProjectManager()->getUndoStack()->push(new ModifyPropertyCommand(m_controller->getProjectManager(), id, prop, oldV, newV));
    });

    connect(m_mappingContainer, &MappingEditorContainer::zoneSelected, m_inspector, &PropertiesInspector::setZone);
    connect(m_inspector, &PropertiesInspector::zonePropertyChanged, [this](const QUuid& groupId, int zoneIndex, const QString& prop, const QVariant& oldV, const QVariant& newV){
        m_controller->getProjectManager()->getUndoStack()->push(new ModifyZonePropertyCommand(m_controller->getProjectManager(), groupId, zoneIndex, prop, oldV, newV));
    });

    connect(m_canvas, &InstrumentCanvasView::componentSelected, m_inspector, &PropertiesInspector::setUiComponent);
    connect(m_controller->getProjectManager(), &ProjectManager::uiComponentModified, m_inspector, &PropertiesInspector::onUiComponentModified);
    connect(m_inspector, &PropertiesInspector::uiPropertyChanged, [this](const QUuid& id, const QString& prop, const QVariant& oldV, const QVariant& newV){
        m_controller->getProjectManager()->getUndoStack()->push(new ModifyUiPropertyCommand(m_controller->getProjectManager(), id, prop, oldV, newV));
    });
    connect(m_inspector, &PropertiesInspector::canvasSizePropertyChanged, [this](int w, int h){
        ProjectManager* pm = m_controller->getProjectManager();
        pm->getUndoStack()->push(new ModifyCanvasSizeCommand(pm, pm->getUiState()->getWidth(), pm->getUiState()->getHeight(), w, h));
    });
    connect(m_inspector, &PropertiesInspector::canvasBgPropertyChanged, [this](const QString& bgImg, int bgMode){
        ProjectManager* pm = m_controller->getProjectManager();
        pm->getUndoStack()->push(new ModifyCanvasBgCommand(pm, pm->getUiState()->getBgImage(), bgImg, pm->getUiState()->getBgMode(), static_cast<BgMode>(bgMode)));
    });
    connect(m_controller->getProjectManager()->getUiState(), &UiStateModel::canvasSizeChanged, m_inspector, &PropertiesInspector::onCanvasSizeChanged);
    connect(m_controller->getProjectManager()->getUiState(), &UiStateModel::canvasSizeChanged, m_canvas, &InstrumentCanvasView::onCanvasSizeChanged);
    
    m_presetDock = new QDockWidget("Presets", m_mainWindow);
    m_presetDock->setObjectName("PresetDock");
    m_presetDock->setAllowedAreas(Qt::LeftDockWidgetArea);
    m_presetDock->setFeatures(QDockWidget::NoDockWidgetFeatures);
    
    m_presetWidget = new PresetListWidget(m_controller, m_canvas);
    m_presetDock->setWidget(m_presetWidget);
    m_mainWindow->addDockWidget(Qt::LeftDockWidgetArea, m_presetDock);

    m_projectDock = new QDockWidget("Project Explorer", m_mainWindow);
    m_projectDock->setObjectName("ProjectDock");
    m_projectDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    m_projectDock->setTitleBarWidget(new QWidget());
    
    m_projectTreeWidget = new ProjectTreeWidget(m_controller, m_inspector, m_mappingContainer);
    m_projectDock->setWidget(m_projectTreeWidget);    
    m_mainWindow->addDockWidget(Qt::RightDockWidgetArea, m_projectDock);
    m_mainWindow->tabifyDockWidget(m_propertyDock, m_projectDock);
    m_projectDock->raise();
    
    m_mainWindow->setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
    m_mainWindow->setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);
    
    m_centralContainer = new QWidget(m_mainWindow);
    m_centralContainer->setObjectName("CentralContainer");
    QVBoxLayout* centralLayout = new QVBoxLayout(m_centralContainer);
    centralLayout->setContentsMargins(0, 0, 0, 0);
    centralLayout->setSpacing(0);
    
    centralLayout->addWidget(m_mainTabs);
    
    m_editorPanel = new QWidget();
    m_editorPanel->setObjectName("EditorPanel");
    QVBoxLayout* editorLayout = new QVBoxLayout(m_editorPanel);
    editorLayout->setContentsMargins(0, 0, 0, 0);
    editorLayout->setSpacing(0);
    
    QPushButton* collapseHandle = new QPushButton();
    collapseHandle->setObjectName("EditorCollapseHandle");
    collapseHandle->setFixedHeight(16);
    collapseHandle->setCursor(Qt::PointingHandCursor);
    collapseHandle->setStyleSheet("QPushButton { border: none; background: transparent; } QPushButton:hover { background: rgba(255, 255, 255, 10); }");
    QHBoxLayout* handleLayout = new QHBoxLayout(collapseHandle);
    handleLayout->setContentsMargins(0, 0, 0, 0);
    
    m_collapseBtn = new QPushButton("▼");
    m_collapseBtn->setFlat(true);
    m_collapseBtn->setCursor(Qt::PointingHandCursor);
    m_collapseBtn->setStyleSheet("QPushButton { border: none; background: transparent; color: #888; font-size: 10px; } QPushButton:hover { color: #fff; }");
    
    handleLayout->addStretch();
    handleLayout->addWidget(m_collapseBtn);
    handleLayout->addStretch();
    
    editorLayout->addWidget(collapseHandle);
    
    m_editorTabs = new QTabWidget();
    m_editorTabs->setTabPosition(QTabWidget::South);
    m_editorTabs->setStyleSheet("QTabWidget::tab-bar { alignment: center; }");
    
    m_focusBtn = new QPushButton("⤢ Focus Mode");
    m_focusBtn->setFlat(true);
    m_focusBtn->setCursor(Qt::PointingHandCursor);
    m_focusBtn->setStyleSheet("QPushButton { border: none; background: transparent; color: #ff6600; font-weight: bold; } QPushButton:hover { color: #fff; }");
    m_editorTabs->setCornerWidget(m_focusBtn, Qt::BottomRightCorner);
    
    m_sampleEditorContainer = new SampleEditorContainer(m_controller, m_editorPanel);
    m_sampleEditorContainer->setAttribute(Qt::WA_StyledBackground, true);
    m_groupEditorView = new GroupEditorView(m_controller, m_editorPanel);
    m_groupEditorView->setAttribute(Qt::WA_StyledBackground, true);
    m_mixerView = new MixerView(m_controller, m_editorPanel);
    m_mixerView->setAttribute(Qt::WA_StyledBackground, true);
    m_modulatorsView = new ModulatorsView(m_controller->getProjectManager(), m_editorPanel);
    m_modulatorsView->setAttribute(Qt::WA_StyledBackground, true);
    m_sequencerView = new NoteSequenceEditorView(m_controller, m_editorPanel);
    m_sequencerView->setAttribute(Qt::WA_StyledBackground, true);
    
    auto toggleCollapse = [this, collapseHandle]() {
        m_isDockCollapsed = !m_isDockCollapsed;
        
        if (m_isDockCollapsed) {
            m_sampleEditorContainer->hide();
            m_groupEditorView->hide();
            m_mixerView->hide();
            m_modulatorsView->hide();
            m_sequencerView->hide();
            collapseHandle->setFixedHeight(0);
            m_collapseBtn->hide();
            
            int collapsedHeight = 35;
            if (QTabBar* tabBar = m_editorTabs->findChild<QTabBar*>()) {
                collapsedHeight = tabBar->sizeHint().height();
            }
            m_editorPanel->setFixedHeight(collapsedHeight);
        } else {
            m_sampleEditorContainer->show();
            m_groupEditorView->show();
            m_mixerView->show();
            m_modulatorsView->show();
            m_sequencerView->show();
            
            collapseHandle->setFixedHeight(16);
            m_collapseBtn->show();
            
            m_editorPanel->setFixedHeight(320);
        }
    };
    
    connect(m_collapseBtn, &QPushButton::clicked, this, toggleCollapse);
    connect(collapseHandle, &QPushButton::clicked, this, toggleCollapse);
    
    connect(m_focusBtn, &QPushButton::clicked, this, [this, editorLayout]() {
        if (!m_focusOverlay) {
            m_focusBtn->setText("⤢ Exit Focus Mode");
            if (m_groupEditorView) {
                m_groupEditorView->setFocusMode(true);
            }
            m_focusOverlay = new FocusModeOverlay(m_mainWindow, m_editorTabs, m_sampleEditorContainer);
            connect(m_focusOverlay, &FocusModeOverlay::closed, this, [this, editorLayout]() {
                if (m_groupEditorView) {
                    m_groupEditorView->setFocusMode(false);
                }
                editorLayout->addWidget(m_editorTabs);
                m_focusBtn->setText("⤢ Focus Mode");
                m_focusOverlay = nullptr;
            });
        } else {
            m_focusOverlay->closeOverlay();
        }
    });
    
    QTabBar* tabBar = m_editorTabs->findChild<QTabBar*>();
    if (tabBar) {
        connect(tabBar, &QTabBar::tabBarClicked, this, [this, toggleCollapse](int index) {
            if (index == m_editorTabs->currentIndex()) {
                toggleCollapse();
            } else if (m_isDockCollapsed) {
                toggleCollapse();
            }
        });
    }
    
    m_editorTabs->addTab(m_groupEditorView, "Group Settings");
    m_editorTabs->addTab(m_sampleEditorContainer, "Sample Editor");
    m_editorTabs->addTab(m_mixerView, "Mixer & FX");
    m_editorTabs->addTab(m_modulatorsView, "Modulators");
    m_editorTabs->addTab(m_sequencerView, "Sequencer");
    
    editorLayout->addWidget(m_editorTabs);
    
    centralLayout->addWidget(m_editorPanel);
    m_editorPanel->setFixedHeight(320);
    
    connect(m_mappingContainer, &MappingEditorContainer::groupSelected, this, [this](const QUuid& sgId){
        m_inspector->setNode(sgId);
        
        ProjectManager* pm = m_controller->getProjectManager();
        Node* n = pm->getNode(sgId);
        if (n && n->type == "SampleGroup") {
            SampleGroup* sg = static_cast<SampleGroup*>(n);
            m_groupEditorView->setGroup(sgId);
            
            bool isSynth = sg->isSynthContainer || sg->isOscillator;
            m_editorTabs->setTabVisible(1, !isSynth); // Index 1 is Sample Editor
        } else {
            m_editorTabs->setTabVisible(1, true);
        }
    });

    connect(m_mappingContainer, &MappingEditorContainer::zoneSelected, this, [this](const QUuid& sgId, int index){
        ProjectManager* pm = m_controller->getProjectManager();
        Node* n = pm->getNode(sgId);
        if (n && n->type == "SampleGroup") {
            SampleGroup* sg = static_cast<SampleGroup*>(n);
            if (index >= 0 && index < sg->zones.size()) {
                m_sampleEditorContainer->setZone(sgId, index, &sg->zones[index]);
                return;
            }
        }
        m_sampleEditorContainer->setZone(QUuid(), -1, nullptr);
    });
    
    }

void DockManager::setDocksVisible(bool visible) {
    if (m_presetDock) m_presetDock->setVisible(visible);
    if (m_propertyDock) m_propertyDock->setVisible(visible);
    if (m_projectDock) m_projectDock->setVisible(visible);
    if (m_editorPanel) m_editorPanel->setVisible(visible);
}

void DockManager::onTabChanged(int index) {
    // 0 = Library, 1 = Sampling, 2 = Node Map, 3 = UI Designer, 4 = Code Editor
    bool isUiDesigner = (index == 3);
    bool isMappingEditor = (index == 1);
    
    m_propertyDock->setVisible(isUiDesigner || isMappingEditor);
    m_assetDock->setVisible(isUiDesigner);
    m_projectDock->setVisible(isMappingEditor);
    m_editorPanel->setVisible(isMappingEditor);
    m_presetDock->setVisible(isMappingEditor);
    
    if (isUiDesigner) {
        if (m_canvas->scene()->selectedItems().isEmpty()) {
            m_inspector->setUiComponent(QUuid());
        } else {
            auto item = dynamic_cast<UiComponentItem*>(m_canvas->scene()->selectedItems().first());
            if (item) m_inspector->setUiComponent(item->getComponentId());
        }
    }
}

