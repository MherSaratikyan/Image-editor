#ifndef FILTERAPPLYER_H
#define FILTERAPPLYER_H

#include <QImage>

class FilterApplyer
{
public:
    FilterApplyer();
public:
    static QImage applyGrayscale(const QImage& src);
    static QImage applyInvert(const QImage& src);
    static QImage applyBrightnessFilter(const QImage& src, int brightness);
    static QImage applyBlur(const QImage& src);
    static QImage applySepia(const QImage& src);
    static QImage applyContrast(const QImage& src, double factor);
    static QImage applySaturation(const QImage& src, bool saturation);
    static QImage applyHue(const QImage& src, int hueShift);
    static QImage applySolarize(const QImage& src, int treshold);
    static QImage applyPosterize(const QImage& src, int levels);
    static QImage applyPixelate(const QImage& src, int blockSize);
    static QImage applyVignete(const QImage& src);
    static QImage applyNoiseReduction(const QImage& src);
    static QImage applyEdgeDetection(const QImage& src);
};

#endif // FILTERAPPLYER_H
