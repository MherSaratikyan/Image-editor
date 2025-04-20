#include "imageentry.h"
#include "databasemanager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>

ImageEntry::ImageEntry(int imageId)
    : m_imageId(imageId), m_projectId(-1)
{
}

bool ImageEntry::loadFromDatabase()
{
    if (m_imageId < 0)
        return false;

    QSqlQuery query(DatabaseManager::instance()->db());
    query.prepare("SELECT image_name, project_id, created_at, last_modified_at "
                  "FROM images WHERE image_id = :id");
    query.bindValue(":id", m_imageId);
    if (!query.exec()) {
        qDebug() << "ImageEntry load error:" << query.lastError().text();
        return false;
    }
    if (query.next()) {
        m_name = query.value(0).toString();
        m_projectId = query.value(1).toInt();
        m_createdAt = query.value(2).toString();
        m_lastModified = query.value(3).toString();
        return true;
    }
    return false;
}

bool ImageEntry::createImage(int projectId, const QString &imageName)
{
    QSqlQuery query(DatabaseManager::instance()->db());
    query.prepare("INSERT INTO images (image_name, project_id, created_at, last_modified_at) "
                  "VALUES (:name, :projectId, DATETIME('now'), DATETIME('now'))");
    query.bindValue(":name", imageName);
    query.bindValue(":projectId", projectId);
    if (!query.exec()) {
        qDebug() << "Failed to create image:" << query.lastError().text();
        return false;
    }

    QSqlQuery q(DatabaseManager::instance()->db());
    if (q.exec("SELECT last_insert_rowid()")) {
        if (q.next()) {
            m_imageId = q.value(0).toInt();
            m_name = imageName;
            m_projectId = projectId;
            return true;
        }
    }
    return false;
}

QImage ImageEntry::loadImageFile() const
{
    QImage img;
    if (!img.load(m_name)) {
        qDebug() << "Failed to load image file:" << m_name;
    }
    return img;
}

bool ImageEntry::saveImageFile(const QImage &img)
{
    return img.save(m_name);
}

int ImageEntry::imageId() const
{
    return m_imageId;
}

QString ImageEntry::name() const
{
    return m_name;
}

int ImageEntry::projectId() const
{
    return m_projectId;
}
