#include "MainWindow.h"
#include "core/ThemeManager.h"
#include "core/MenuManager.h"
#include "core/DockManager.h"
#include "designer/UIDesignerView.h"
#include "mapper/MappingEditorContainer.h"
#include "library/LibraryView.h"
#include "nodemap/NodeMapView.h"
#include "code/CodeEditorView.h"
#include "library/LibraryView.h"
#include <QTabWidget>
#include "canvas/InstrumentCanvasView.h"
#include "components/CustomTitleBar.h"
#include "components/NeumorphicMessageBox.h"
#include "core/ApplicationController.h"
#include "ui/AboutDialog.h"
#include "ui/SettingsDialog.h"
#include <QVBoxLayout>
#include <QTabBar>
#include <QSplitter>

MainWindow::MainWindow(ApplicationController* controller, QWidget *parent) 
    : QMainWindow(parent), m_controller(controller) 
{
    setWindowTitle("decent sampler editor");
    setMinimumSize(1024, 720);
    resize(1280, 720);
    
    ThemeManager::applyTheme(this);
    
    setupUi();
    
    m_menuManager = new MenuManager(this, m_controller->getProjectManager());
    m_menuManager->buildMenus();
    
    m_dockManager = new DockManager(this, m_controller, m_uiDesigner, m_mappingContainer, m_uiDesigner->getCanvas(), m_tabs);
    m_dockManager->setupDocks();
    m_dockManager->setDocksVisible(m_tabs->currentIndex() != 0);
    
    centralWidget()->layout()->addWidget(m_dockManager->getCentralContainer());
    
    CustomTitleBar* titleBar = new CustomTitleBar(this);
    menuBar()->setNativeMenuBar(false);
    titleBar->setMenuBar(menuBar());
    setMenuWidget(titleBar);
    
    connect(titleBar, &CustomTitleBar::minimizeRequested, this, &QMainWindow::showMinimized);
    connect(titleBar, &CustomTitleBar::closeRequested, this, [this]() {
        close();
    });
    connect(titleBar, &CustomTitleBar::settingsRequested, this, [this]() {
        SettingsDialog dlg(m_controller, this);
        dlg.exec();
    });
    connect(titleBar, &CustomTitleBar::aboutRequested, this, [this]() {
        new AboutDialog(m_controller->getDatabaseManager(), this);
    });
    
    if (m_tabs) {
        QTabBar* titleTabBar = new QTabBar();
        for (int i = 0; i < m_tabs->count(); ++i) {
            titleTabBar->addTab(m_tabs->tabText(i));
        }
        titleTabBar->setCurrentIndex(m_tabs->currentIndex());
        titleBar->setTabBar(titleTabBar);
        
        connect(titleTabBar, &QTabBar::currentChanged, m_tabs, &QTabWidget::setCurrentIndex);
        connect(m_tabs, &QTabWidget::currentChanged, titleTabBar, &QTabBar::setCurrentIndex);
        
        connect(m_controller->getProjectManager(), &ProjectManager::projectLoaded, this, [this, titleTabBar]() {
            for (int i = 1; i < m_tabs->count(); ++i) {
                titleTabBar->setTabEnabled(i, true);
                m_tabs->setTabEnabled(i, true);
            }
        });
        
        // Connect to DockManager to hide/show docks based on tab
        connect(m_tabs, &QTabWidget::currentChanged, m_dockManager, &DockManager::onTabChanged);
        m_dockManager->onTabChanged(m_tabs->currentIndex());
    }
    
    connect(m_controller->getProjectManager(), &ProjectManager::projectDirtyChanged, this, [this, titleBar](bool dirty) {
        if (m_controller->getProjectManager()->isDirty()) {
            setWindowTitle("decent sampler editor *");
            titleBar->setTitle("decent sampler editor *");
        } else {
            setWindowTitle("decent sampler editor");
            titleBar->setTitle("decent sampler editor");
        }
    });
}

MainWindow::~MainWindow() {}

void MainWindow::setupUi() {
    QWidget* centralWidget = new QWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    
    m_tabs = new QTabWidget(centralWidget);
    m_tabs->setDocumentMode(true);
    // Hide native tab bar, CustomTitleBar handles it
    m_tabs->tabBar()->hide();
    m_tabs->setMinimumSize(100, 100);
    
    m_libraryView = new LibraryView(m_controller, m_tabs);
    m_tabs->addTab(m_libraryView, "1. Libraries");
    connect(m_libraryView, &LibraryView::requestTabChange, m_tabs, &QTabWidget::setCurrentIndex);
    
    m_mappingContainer = new MappingEditorContainer(m_controller->getProjectManager(), m_tabs);
    m_tabs->addTab(m_mappingContainer, "2. Sampling");
    
    m_nodeMap = new NodeMapView(m_controller, m_tabs);
    m_tabs->addTab(m_nodeMap, "3. Node Map");
    
    m_uiDesigner = new UIDesignerView(m_controller->getProjectManager(), m_tabs);
    m_tabs->addTab(m_uiDesigner, "4. UI Designer");
    
    m_codeEditor = new CodeEditorView(m_controller->getProjectManager(), m_tabs);
    m_tabs->addTab(m_codeEditor, "5. Code Editor");
    
    // Connect Visual Keyboards to Audio Engine
    connect(m_mappingContainer, &MappingEditorContainer::notePressed, m_controller, &ApplicationController::playNote);
    connect(m_mappingContainer, &MappingEditorContainer::noteReleased, m_controller, &ApplicationController::stopNote);
    
    connect(m_uiDesigner, &UIDesignerView::notePressed, m_controller, &ApplicationController::playNote);
    // Note: UIDesignerView doesn't currently emit noteReleased but we can connect it if it gets one.
    // Assuming UIDesignerView emits noteReleased as well (wait, let's just connect what we have or add it later if needed).
    
    // Connect tab change to toggle docks
    connect(m_tabs, &QTabWidget::currentChanged, this, [this](int index) {
        if (m_tabs->widget(index) == m_codeEditor) {
            m_codeEditor->updateCode();
        }
    });
    // Start with Library active
    m_tabs->setCurrentIndex(0);
    
    setCentralWidget(centralWidget);
}

#ifdef Q_OS_WIN
bool MainWindow::nativeEvent(const QByteArray &eventType, void *message, qintptr *result) {
    if (ThemeManager::handleNativeEvent(this, eventType, message, result)) {
        return true;
    }
    return QMainWindow::nativeEvent(eventType, message, result);
}
#endif

void MainWindow::closeEvent(QCloseEvent *event) {
    if (m_controller) {
        ProjectManager* pm = m_controller->getProjectManager();
        if (pm && pm->isDirty()) {
            NeumorphicMessageBox msgBox(this);
            msgBox.setText("The project has been modified.");
            msgBox.setInformativeText("Do you want to save your changes?");
            msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
            msgBox.setDefaultButton(QMessageBox::Save);
            
            QMessageBox::StandardButton ret = msgBox.execDialog();
            if (ret == QMessageBox::Save) {
                if (!m_controller->getProjectManager()->saveProject()) {
                    NeumorphicMessageBox::warning(this, "Save Failed", "Could not save the project.");
                }
                if (m_controller->getProjectManager()->isDirty()) {
                    event->ignore();
                    return;
                }
            } else if (ret == QMessageBox::Cancel) {
                event->ignore();
                return;
            }
        }
    }
    QMainWindow::closeEvent(event);
}
