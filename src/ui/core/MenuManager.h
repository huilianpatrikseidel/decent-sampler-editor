#pragma once
#include <QObject>
#include <QMainWindow>
class ProjectManager;

class MenuManager : public QObject {
    Q_OBJECT
public:
    explicit MenuManager(QMainWindow* mainWindow, ProjectManager* pm);
    void buildMenus();
private:
    QMainWindow* m_mainWindow;
    ProjectManager* m_pm;
};
