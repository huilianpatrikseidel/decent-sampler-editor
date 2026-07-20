#pragma once

#include <QWidget>
#include <QFileSystemModel>
#include <QListView>
#include <QString>
#include "../../core/ProjectManager.h"

class AssetManagerView : public QWidget {
    Q_OBJECT
public:
    explicit AssetManagerView(ProjectManager* pm, QWidget* parent = nullptr);

private slots:
    void onOpenFolderClicked();

private:
    ProjectManager* m_pm;
    QFileSystemModel* m_fileModel;
    QListView* m_listView;
    QString m_currentPath;
};
