#include "project.h"
#include "databasemanager.h"

#include <QSqlQuery>
#include <QVariant>
#include <QSqlError>
#include <QDebug>
#include <QPainter>

Project::Project(int projectId)
    : m_projectId(projectId)
{}

bool Project::create(const QString &name, const QString &path)
{
    if (!DatabaseManager::instance()->createProject(name, path))
        return false;

    QSqlQuery q(DatabaseManager::instance()->db());
    q.exec("SELECT last_insert_rowid()");
    if (q.next()) {
        m_projectId = q.value(0).toInt();
        m_name = name;
        m_path = path;
    }
    return (m_projectId >= 0);
}

bool Project::loadFromDatabase()
{
    if (m_projectId < 0)
        return false;
    QSqlQuery q(DatabaseManager::instance()->db());
    q.prepare("SELECT project_name, project_path, created_at, last_modified "
              "FROM projects WHERE project_id = :pid");
    q.bindValue(":pid", m_projectId);
    if (!q.exec() || !q.next()) {
        qDebug() << "Project load error:" << q.lastError().text();
        return false;
    }
    m_name = q.value(0).toString();
    m_path = q.value(1).toString();
    m_createdAt = q.value(2).toString();
    m_lastModified = q.value(3).toString();

    return true;
}

bool Project::saveToDatabase()
{
    if (m_projectId < 0)
        return false;
    QSqlQuery q(DatabaseManager::instance()->db());
    q.prepare("UPDATE projects SET project_name = :name, project_path = :path, "
              "last_modified = DATETIME('now') WHERE project_id = :pid");
    q.bindValue(":name", m_name);
    q.bindValue(":path", m_path);
    q.bindValue(":pid", m_projectId);
    if (!q.exec()) {
        qDebug() << "Project save error:" << q.lastError().text();
        return false;
    }
    return true;
}

int Project::projectId() const
{
    return m_projectId;
}

QString Project::name() const
{
    return m_name;
}

QString Project::path() const
{
    return m_path;
}

void Project::setName(const QString &name)
{
    m_name = name;
}

void Project::setPath(const QString &path)
{
    m_path = path;
}

QList<ImageEntry>& Project::layers()
{
    return m_layers;
}

bool Project::addLayer(const QString &imagePath)
{
    ImageEntry newLayer;
    if (!newLayer.createImage(m_projectId, imagePath))
        return false;
    m_layers.append(newLayer);
    return true;
}

bool Project::removeLayer(int layerIndex)
{
    if (layerIndex < 0 || layerIndex >= m_layers.size())
        return false;
    m_layers.removeAt(layerIndex);
    return true;
}

QImage Project::compositeImage() const
{
    if (m_layers.isEmpty())
        return QImage();

    QImage finalImg = m_layers.first().loadImageFile();
    finalImg = finalImg.convertToFormat(QImage::Format_ARGB32);

    QPainter painter(&finalImg);
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);

    for (int i = 1; i < m_layers.size(); ++i) {
        QImage layerImg = m_layers.at(i).loadImageFile().convertToFormat(QImage::Format_ARGB32);
        painter.drawImage(0, 0, layerImg);
    }
    painter.end();

    return finalImg;
}
