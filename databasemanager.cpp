#include "databasemanager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>

DatabaseManager* DatabaseManager::m_instance = nullptr;

DatabaseManager* DatabaseManager::instance()
{
    if (m_instance == nullptr)
    {
        m_instance = new DatabaseManager();
    }
    return m_instance;
}

DatabaseManager::DatabaseManager()
{
}

bool DatabaseManager::openDatabase(const QString &dbPath)
{
    if (m_db.isOpen()) {
        m_db.close();
    }
    m_db = QSqlDatabase::addDatabase("QSQLITE", "ProjectConnection");
    m_db.setDatabaseName(dbPath);
    if (!m_db.open()) {
        qDebug() << "Failed to open database:" << m_db.lastError().text();
        return false;
    }
    return true;
}


bool DatabaseManager::closeDatabase(){
    if(m_db.isOpen()){
        m_db.close();
    }
}
bool DatabaseManager::createTables()
{
    QSqlQuery q(m_db);
    if(!q.exec("PRAGMA foreign_keys = ON")){
        return false;
    }
    if (!q.exec("CREATE TABLE IF NOT EXISTS projects ("
                "project_id INTEGER PRIMARY KEY AUTOINCREMENT, "
                "project_name TEXT, "
                "project_path TEXT, "
                "created_at TEXT, "
                "last_modified TEXT)")) {
        qDebug() << "Failed to create projects table:" << q.lastError().text();
        return false;
    }

    if (!q.exec("CREATE TABLE IF NOT EXISTS images ("
                "image_id INTEGER PRIMARY KEY AUTOINCREMENT, "
                "image_name TEXT, "
                "project_id INTEGER, "
                "created_at TEXT, "
                "last_modified_at TEXT, "
                "FOREIGN KEY(project_id) REFERENCES projects(project_id) ON DELETE CASCADE)")) {
        qDebug() << "Failed to create images table:" << q.lastError().text();
        return false;
    }

    if (!q.exec("CREATE TABLE IF NOT EXISTS procedures ("
                "procedure_id INTEGER PRIMARY KEY AUTOINCREMENT, "
                "procedure_name TEXT, "
                "project_id INTEGER, "
                "description TEXT, "
                "procedure_sequence TEXT, "
                "FOREIGN KEY(project_id) REFERENCES projects(project_id) ON DELETE CASCADE)")) {
        qDebug() << "Failed to create procedures table:" << q.lastError().text();
        return false;
    }
    return true;
}

bool DatabaseManager::createProject(const QString &name, const QString &path)
{
    QSqlQuery q(m_db);
    q.prepare("INSERT INTO projects (project_name, project_path, created_at, last_modified) "
              "VALUES (:name, :path, DATETIME('now'), DATETIME('now'))");
    q.bindValue(":name", name);
    q.bindValue(":path", path);
    if (!q.exec()) {
        qDebug() << "Failed to create project:" << q.lastError().text();
        return false;
    }
    return true;
}

QList<int> DatabaseManager::listAllProjects()
{
    QList<int> projectIds;
    QSqlQuery q(m_db);
    if (q.exec("SELECT project_id FROM projects")) {
        while (q.next()) {
            projectIds.append(q.value(0).toInt());
        }
    } else {
        qDebug() << "Failed to list projects:" << q.lastError().text();
    }
    return projectIds;
}

bool DatabaseManager::createImage(int projectId, const QString &imageName)
{
    QSqlQuery q(m_db);
    q.prepare("INSERT INTO images (image_name, created_at, last_modified_at, project_id,) "
              "VALUES (:name, DATETIME('now'), DATETIME('now')), :project_id");
    q.bindValue(":name", imageName);
    q.bindValue(":project_id", projectId);
    if (!q.exec()) {
        qDebug() << "Failed to create image:" << q.lastError().text();
        return false;
    }
    return true;
}

bool DatabaseManager::createProcedure(int projectId, const QString &procName, const QString &desc, const QString &seq)
{
    QSqlQuery q(m_db);
    q.prepare("INSERT INTO procedures (procedure_name, description, procedure_sequence, project_id) "
              "VALUES (:name, :desc, :seq, :project_id)");
    q.bindValue(":project_id", projectId);
    q.bindValue(":name", procName);
    q.bindValue(":desc", desc);
    q.bindValue(":seq", seq);
    if (!q.exec()) {
        qDebug() << "Failed to create procedure:" << q.lastError().text();
        return false;
    }
    return true;
}

QSqlDatabase& DatabaseManager::db()
{
    return m_db;
}
