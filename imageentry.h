#ifndef IMAGEENTRY_H
#define IMAGEENTRY_H

#include <QImage>
#include <QString>

class ImageEntry
{
public:
    explicit ImageEntry(int imageId = -1);

    bool loadFromDatabase();

    bool createImage(int projectId, const QString &imageName);

    QImage loadImageFile() const;

    bool saveImageFile(const QImage &img);

    int imageId() const;
    QString name() const;
    int projectId() const;

private:
    int m_imageId;
    QString m_name;
    int m_projectId;
    QString m_createdAt;
    QString m_lastModified;
};

#endif // IMAGEENTRY_H
