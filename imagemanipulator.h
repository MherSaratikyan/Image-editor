#ifndef IMAGEMANIPULATOR_H
#define IMAGEMANIPULATOR_H

#include <QImage>

class ImageManipulator
{
public:
    ImageManipulator();
public:
    static QImage rotateLeft(const QImage& src);
    static QImage rotateRight(const QImage& src);
    static QImage rotate180(const QImage& src);
    static QImage flipHorizontally(const QImage& src);
    static QImage flipVertically(const QImage& src);
};

#endif // IMAGEMANIPULATOR_H
