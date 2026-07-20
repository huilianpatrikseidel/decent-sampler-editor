#pragma once
#include <QWidget>
#include <QVBoxLayout>
#include <QTableWidget>
#include "../../core/ProjectManager.h"

class ModulatorsView : public QWidget {
    Q_OBJECT
public:
    explicit ModulatorsView(ProjectManager* pm, QWidget* parent = nullptr);

private slots:
    void updateForm();

private:
    ProjectManager* m_pm;
    QVBoxLayout* m_layout;
    bool m_isUpdating = false;
    int m_savedScrollPos = 0;
};
