#include "DatabaseManager.h"
#include <QDir>
#include <QStandardPaths>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QVariant>
#include <QUuid>

DatabaseManager::DatabaseManager() {
    QString appData = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir(appData);
    
    m_projectsDir = dir.filePath("Databases/Libraries/Projects");
    m_dbPath = dir.filePath("Databases/Libraries/libraries.db");
    
    // Ensure directories exist
    dir.mkpath(m_projectsDir);
}

DatabaseManager::~DatabaseManager() {
    if (m_db.isOpen()) {
        m_db.close();
    }
}

bool DatabaseManager::initialize() {
    if (QSqlDatabase::contains("qt_sql_default_connection")) {
        m_db = QSqlDatabase::database("qt_sql_default_connection");
    } else {
        m_db = QSqlDatabase::addDatabase("QSQLITE");
    }
    
    m_db.setDatabaseName(m_dbPath);
    
    if (!m_db.open()) {
        qCritical() << "Failed to open database:" << m_db.lastError().text();
        return false;
    }
    
    return createTables();
}

bool DatabaseManager::createTables() {
    QSqlQuery query(m_db);
    
    QString createSql = 
        "CREATE TABLE IF NOT EXISTS projects ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "name TEXT NOT NULL, "
        "created_at DATETIME DEFAULT CURRENT_TIMESTAMP, "
        "modified_at DATETIME DEFAULT CURRENT_TIMESTAMP, "
        "thumbnail_path TEXT, "
        "project_data TEXT"
        ")";
        
    if (!query.exec(createSql)) {
        qCritical() << "Failed to create projects table:" << query.lastError().text();
        return false;
    }
    return true;
}

QList<ProjectRecord> DatabaseManager::getProjects() const {
    QList<ProjectRecord> list;
    QSqlQuery query("SELECT id, name, created_at, modified_at, thumbnail_path, project_data FROM projects ORDER BY modified_at DESC", m_db);
    
    while (query.next()) {
        ProjectRecord rec;
        rec.id = query.value(0).toInt();
        rec.name = query.value(1).toString();
        rec.createdAt = query.value(2).toDateTime();
        rec.modifiedAt = query.value(3).toDateTime();
        rec.thumbnailPath = query.value(4).toString();
        rec.projectData = query.value(5).toString();
        list.append(rec);
    }
    return list;
}

ProjectRecord DatabaseManager::getProject(int id) const {
    ProjectRecord rec;
    QSqlQuery query(m_db);
    query.prepare("SELECT id, name, created_at, modified_at, thumbnail_path, project_data FROM projects WHERE id = ?");
    query.addBindValue(id);
    
    if (query.exec() && query.next()) {
        rec.id = query.value(0).toInt();
        rec.name = query.value(1).toString();
        rec.createdAt = query.value(2).toDateTime();
        rec.modifiedAt = query.value(3).toDateTime();
        rec.thumbnailPath = query.value(4).toString();
        rec.projectData = query.value(5).toString();
    }
    return rec;
}

int DatabaseManager::addProject(const QString& name, const QString& thumbnailPath, const QString& projectData) {
    QSqlQuery query(m_db);
    query.prepare("INSERT INTO projects (name, created_at, modified_at, thumbnail_path, project_data) "
                  "VALUES (?, CURRENT_TIMESTAMP, CURRENT_TIMESTAMP, ?, ?)");
    query.addBindValue(name);
    query.addBindValue(thumbnailPath);
    query.addBindValue(projectData);
    
    if (query.exec()) {
        return query.lastInsertId().toInt();
    } else {
        qCritical() << "Failed to add project:" << query.lastError().text();
        return -1;
    }
}

bool DatabaseManager::updateProject(const ProjectRecord& record) {
    QSqlQuery query(m_db);
    query.prepare("UPDATE projects SET name = ?, modified_at = CURRENT_TIMESTAMP, "
                  "thumbnail_path = ?, project_data = ? WHERE id = ?");
    query.addBindValue(record.name);
    query.addBindValue(record.thumbnailPath);
    query.addBindValue(record.projectData);
    query.addBindValue(record.id);
    
    return query.exec();
}

bool DatabaseManager::updateProjectData(int id, const QString& projectData) {
    QSqlQuery query(m_db);
    query.prepare("UPDATE projects SET project_data = ?, modified_at = CURRENT_TIMESTAMP WHERE id = ?");
    query.addBindValue(projectData);
    query.addBindValue(id);
    return query.exec();
}

QString DatabaseManager::getProjectData(int id) const {
    QSqlQuery query(m_db);
    query.prepare("SELECT project_data FROM projects WHERE id = ?");
    query.addBindValue(id);
    if (query.exec() && query.next()) {
        return query.value(0).toString();
    }
    return "";
}

void DatabaseManager::updateProjectModifiedAt(int id) {
    QSqlQuery query(m_db);
    query.prepare("UPDATE projects SET modified_at = CURRENT_TIMESTAMP WHERE id = ?");
    query.addBindValue(id);
    query.exec();
}

bool DatabaseManager::deleteProject(int id) {
    QSqlQuery query(m_db);
    query.prepare("DELETE FROM projects WHERE id = ?");
    query.addBindValue(id);
    
    return query.exec();
}

QString DatabaseManager::getDatabasePath() const {
    return m_dbPath;
}

QString DatabaseManager::getProjectsDirectory() const {
    return m_projectsDir;
}
