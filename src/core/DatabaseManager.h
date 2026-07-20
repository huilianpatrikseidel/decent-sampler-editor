#pragma once

#include <QString>
#include <QList>
#include <QDateTime>
#include <QSqlDatabase>

struct ProjectRecord {
    int id = -1;
    QString name;
    QDateTime createdAt;
    QDateTime modifiedAt;
    QString thumbnailPath;
    QString projectData;
};

#include "CoreLibExport.h"

class CORELIB_EXPORT DatabaseManager {
public:
    DatabaseManager();
    ~DatabaseManager();

    bool initialize();
    
    QList<ProjectRecord> getProjects() const;
    ProjectRecord getProject(int id) const;
    
    int addProject(const QString& name, const QString& thumbnailPath, const QString& projectData);
    bool updateProject(const ProjectRecord& record);
    bool updateProjectData(int id, const QString& projectData);
    QString getProjectData(int id) const;
    void updateProjectModifiedAt(int id);
    bool deleteProject(int id);

    QString getDatabasePath() const;
    QString getProjectsDirectory() const;

private:
    QSqlDatabase m_db;
    QString m_dbPath;
    QString m_projectsDir;
    
    bool createTables();
};
