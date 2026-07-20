#pragma once
#include <QMainWindow>
#include <QTabWidget>

class ApplicationController;
class MenuManager;
class DockManager;
class UIDesignerView;
class MappingEditorContainer;
class InstrumentCanvasView;
class LibraryView;
class NodeMapView;
class CodeEditorView;
class LibraryView;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(ApplicationController* controller, QWidget *parent = nullptr);
    ~MainWindow() override;

protected:
    void closeEvent(QCloseEvent *event) override;
#ifdef Q_OS_WIN
    bool nativeEvent(const QByteArray &eventType, void *message, qintptr *result) override;
#endif

private:
    ApplicationController* m_controller;
    
    QTabWidget* m_tabs = nullptr;
    LibraryView* m_library = nullptr;
    MappingEditorContainer* m_mappingContainer = nullptr;
    NodeMapView* m_nodeMap = nullptr;
    UIDesignerView* m_uiDesigner = nullptr;
    CodeEditorView* m_codeEditor = nullptr;
    LibraryView* m_libraryView = nullptr;

    MenuManager* m_menuManager = nullptr;
    DockManager* m_dockManager = nullptr;
    
    void setupUi();
};
