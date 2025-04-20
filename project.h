#ifndef PROJECT_H
#define PROJECT_H

#include <QString>
#include <QList>
#include <QImage>
#include "imageentry.h"

class Project
{
public:
    Project(int projectId = -1);

    bool loadFromDatabase();
    bool saveToDatabase();
    bool create(const QString &name, const QString &path);

    int projectId() const;
    QString name() const;
    QString path() const;

    void setName(const QString &name);
    void setPath(const QString &path);

    QList<ImageEntry>& layers();
    bool addLayer(const QString &imagePath);
    bool removeLayer(int layerIndex);
    QImage compositeImage() const;

private:
    int m_projectId;
    QString m_name;
    QString m_path;
    QString m_createdAt;
    QString m_lastModified;

    QList<ImageEntry> m_layers;
};
#endif // PROJECT_H
