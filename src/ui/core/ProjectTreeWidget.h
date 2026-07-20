#pragma once
#include <QWidget>

class QTreeWidget;
class ApplicationController;
class PropertiesInspector;
class MappingEditorContainer;

class ProjectTreeWidget : public QWidget {
    Q_OBJECT
public:
    ProjectTreeWidget(ApplicationController* controller, PropertiesInspector* inspector, MappingEditorContainer* mappingContainer, QWidget* parent = nullptr);
    void updateTree();

private:
    ApplicationController* m_controller;
    PropertiesInspector* m_inspector;
    MappingEditorContainer* m_mappingContainer;
    QTreeWidget* m_projectTree;
};
