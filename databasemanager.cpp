#include "databasemanager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>
#include <QDir>

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
//    if (!q.exec("DROP TABLE IF EXISTS projects")) {
//        qDebug() << "Failed to delete projects table:" << q.lastError().text();
//        return false;
//    }
    if (!q.exec("CREATE TABLE IF NOT EXISTS projects ("
                "project_id INTEGER PRIMARY KEY AUTOINCREMENT, "
                "project_name TEXT, "
                "project_path TEXT, "
                "created_at TEXT, "
                "last_modified TEXT)")) {
        qDebug() << "Failed to create projects table:" << q.lastError().text();
        return false;
    }

//    if (!q.exec("DROP TABLE IF EXISTS images")) {
//        qDebug() << "Failed to delete projects table:" << q.lastError().text();
//        return false;
//    }

    if (!q.exec("CREATE TABLE IF NOT EXISTS images ("
                "image_id INTEGER PRIMARY KEY AUTOINCREMENT, "
                "image_name TEXT, "
                "project_path TEXT, "
                "created_at TEXT, "
                "last_modified_at TEXT, "
                "FOREIGN KEY(project_path) REFERENCES projects(project_path) ON DELETE CASCADE)")) {
        qDebug() << "Failed to create images table:" << q.lastError().text();
        return false;
    }

    if (!q.exec("DROP TABLE IF EXISTS procedures")) {
        qDebug() << "Failed to delete projects table:" << q.lastError().text();
        return false;
    }

//    if (!q.exec("CREATE TABLE IF NOT EXISTS procedures ("
//                "procedure_id INTEGER PRIMARY KEY AUTOINCREMENT, "
//                "procedure_name TEXT, "
//                "project_id INTEGER, "
//                "description TEXT, "
//                "procedure_sequence TEXT, "
//                "FOREIGN KEY(project_id) REFERENCES projects(project_id) ON DELETE CASCADE)")) {
//        qDebug() << "Failed to create procedures table:" << q.lastError().text();
//        return false;
//    }
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

bool DatabaseManager::createImage(const QString& projectPath, const QString &imageName)
{
    QSqlQuery q(m_db);
    q.prepare("INSERT INTO images (image_name, created_at, last_modified_at, project_path) "
              "VALUES (:name, DATETIME('now'), DATETIME('now'), :project_path)");
    q.bindValue(":name", imageName);
    q.bindValue(":project_path", projectPath);
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

QString DatabaseManager::getImage(const QString& projectPath){
    if (!m_db.isOpen()) {
        qDebug() << "Database is not open!";
        return QString("");
    }

    QSqlQuery q(m_db);
    q.prepare("SELECT images.image_name FROM images "
              "JOIN projects ON images.project_path = projects.project_path "
              "WHERE projects.project_path = :projectPath LIMIT 1");
    q.bindValue(":projectPath", projectPath);

    if (!q.exec()) {
        qDebug() << "SQL query failed:" << q.lastError().text();
        return QString("");
    }

    if (q.next()) {
        QString imageName = q.value(0).toString();
        QDir projectDir(projectPath);
        qDebug() << "Image found:" << imageName;
        return projectDir.filePath("assets/" + imageName);
    } else {
        qDebug() << "No image found for project:" << projectPath;
        return QString("");
    }
}

QSqlDatabase& DatabaseManager::db()
{
    return m_db;
}
