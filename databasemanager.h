#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>
#include <QString>
#include <QList>

class DatabaseManager
{
public:
    static DatabaseManager* instance();

    bool openDatabase(const QString &dbPath);
    bool closeDatabase();
    bool createTables();

    bool createProject(const QString &name, const QString &path);
    QList<int> listAllProjects();

    bool createImage(const QString& projectName, const QString &imageName);
    bool createProcedure(int projectId, const QString &procName, const QString &desc, const QString &seq);
    QString getImage(const QString& projectPath);
    QSqlDatabase& db();

private:
    DatabaseManager();
    static DatabaseManager* m_instance;
    QSqlDatabase m_db;
};

#endif // DATABASEMANAGER_H
