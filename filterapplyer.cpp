#include "filterapplyer.h"

#include <QPainter>
#include <QtMath>
#include <QVector>
#include <algorithm>

FilterApplyer::FilterApplyer()
{

}

QImage FilterApplyer::applyGrayscale(const QImage& src){
    QImage dst(src.width(), src.height(), src.format());
    for(int y = 0; y < src.height(); ++y){
        for(int x = 0; x < src.width(); ++x){
            QColor color(src.pixel(x, y));
            int gray = qGray(color.rgb());
            dst.setPixel(x, y, qRgba(gray, gray, gray, color.alpha()));
        }
    }

    return dst;
}

QImage FilterApplyer::applyInvert(const QImage& src){
    QImage dst(src.width(), src.height(), src.format());
    for(int y = 0; y < src.height(); ++y){
        for(int x = 0; x < src.width(); ++x){
            QColor color(src.pixel(x, y));
            color.setRgb(255 - color.red(), 255 - color.green(), 255 - color.blue(), color.alpha());
            dst.setPixel(x, y, color.rgba());
        }
    }

    return dst;
}

QImage FilterApplyer::applyBrightnessFilter(const QImage& src, int brightness){
    QImage dst(src.width(), src.height(), src.format());
    for(int y = 0; y < src.height(); ++y){
        for(int x = 0; x < src.width(); ++x){
            QColor color(src.pixel(x, y));
            int r = qBound(0, color.red() + brightness, 255);
            int g = qBound(0, color.green() + brightness, 255);
            int b = qBound(0, color.blue() + brightness, 255);
            color.setRgb(r, g, b, color.alpha());
            dst.setPixel(x, y, color.rgba());
        }
    }

    return dst;
}

QImage FilterApplyer::applyBlur(const QImage& src){
    QImage dst = src;
    QPainter painter(&dst);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter.drawImage(QRect(0, 1, dst.width() - 2, dst.height() - 2), dst);
    return dst;
}

QImage FilterApplyer::applySepia(const QImage& src){
    QImage dst(src.width(), src.height(), src.format());
    for(int y = 0; y < src.height(); ++y){
        for(int x = 0; x < src.width(); ++x){
            QColor color(src.pixel(x, y));
            int tr = 0.393 * color.red() + 0.769 * color.green() + 0.189 * color.blue();
            int tg = 0.349 * color.red() + 0.686 * color.green() + 0.168 * color.blue();
            int tb = 0.272 * color.red() + 0.534 * color.green() + 0.131 * color.blue();
            color.setRgb(qBound(0, tr, 255), qBound(0, tg, 255), qBound(0, tb, 255), color.alpha());
            dst.setPixel(x, y, color.rgba());
        }
    }

    return dst;
}

QImage FilterApplyer::applyContrast(const QImage& src, double factor){
    QImage dst(src.width(), src.height(), src.format());
    for(int y = 0; y < src.height(); ++y){
        for(int x = 0; x < src.width(); ++x){
            QColor color(src.pixel(x, y));
            int r = qBound(0, int((color.red() - 128) * factor + 128),255);
            int g = qBound(0, int((color.green() - 128) * factor + 128),255);
            int b = qBound(0, int((color.blue() - 128) * factor + 128),255);
            color.setRgb(r, g, b, color.alpha());
            dst.setPixel(x, y, color.rgba());
        }
    }

    return dst;
}

QImage FilterApplyer::applySaturation(const QImage& src, bool saturation){
    double saturationFactor = (saturation ? 1.3 : 0.7);
    QImage dst(src.width(), src.height(), src.format());

    for(int y = 0; y < src.height(); ++y){
        for(int x = 0; x < src.width(); ++x){
            QColor color = QColor(src.pixel(x, y));
            double p = sqrt(color.red() * color.red() * 0.299 +
                            color.green() * color.green() * 0.587 +
                            color.blue() * color.blue() * 0.114);
            int r = qBound(0, int(p + (color.red() - p) * saturationFactor), 255);
            int g = qBound(0, int(p + (color.green() - p) * saturationFactor), 255);
            int b = qBound(0, int(p + (color.blue() - p) * saturationFactor), 255);
            dst.setPixel(x, y, qRgb(r, g, b));
        }
    }

    return dst;
}

QImage FilterApplyer::applyHue(const QImage& src, int hueShift){
    QImage dst(src.width(), src.height(), src.format());
    for(int y = 0; y < src.height(); ++y){
        for(int x = 0; x < src.width(); ++x){
            QColor color(src.pixel(x, y));
            int h, s, v;
            color.getHsv(&h, &s, &v);
            h = (h + hueShift) % 360;
            color.setHsv(h, s, v);
            dst.setPixel(x, y, color.rgba());
        }
    }
    return dst;
}

QImage FilterApplyer::applySolarize(const QImage& src, int threshold){
    QImage dst(src.width(), src.height(), src.format());
    for(int y = 0; y < src.height(); ++y){
        for(int x = 0; x < src.width(); ++x){
            QColor color = QColor(src.pixel(x, y));
            int r = (color.red() > threshold) ? 255 - color.red() : color.red();
            int g = (color.green() > threshold) ? 255 - color.green() : color.green();
            int b = (color.blue() > threshold) ? 255 - color.blue() : color.blue();
            dst.setPixel(x, y, qRgba(r, g, b, color.alpha()));
        }
    }
    return dst;
}

QImage FilterApplyer::applyPosterize(const QImage& src, int levels){
    QImage dst(src.width(), src.height(), src.format());
    for(int y = 0; y < src.height(); ++y){
        for(int x = 0; x < src.width(); ++x){
            QColor color = QColor(src.pixel(x, y));
            int r = (color.red() / (256 / levels)) * (256 / levels);
            int g = (color.green() / (256 / levels)) * (256 / levels);
            int b = (color.blue() / (256 / levels)) * (256 / levels);
            dst.setPixel(x, y, qRgba(r, g, b, color.alpha()));
        }
    }
    return dst;
}

QImage FilterApplyer::applyPixelate(const QImage& src, int blockSize){
    QImage dst(src.width(), src.height(), src.format());
    for(int y = 0; y < src.height() - blockSize; ++y){
        for(int x = 0; x < src.width() - blockSize; ++x){
            QColor color = QColor(src.pixel(x, y));
            for(int dy = 0; dy < blockSize; ++dy){
                for(int dx = 0; dx < blockSize; ++dx){
                    if(x + dx < src.width() && y + dy < src.height()){
                        dst.setPixel(x + dx, y + dy, color.rgba());
                    }
                }
            }
        }
    }
    return dst;
}

QImage FilterApplyer::applyVignete(const QImage& src){
    int centerX = src.width() / 2;
    int centerY = src.height() / 2;
    int maxDistance = qMax(centerX, centerY);
    QImage dst(src.width(), src.height(), src.format());
    for(int y = 0; y < src.height(); ++y){
        for(int x = 0; x < src.width(); ++x){
            QColor color = QColor(src.pixel(x, y));
            int dist = qSqrt(qPow(x - centerX, 2) + qPow(y - centerY, 2));
            double factor = 1.0 - (double(dist) / maxDistance) * 0.6;
            color.setRed(qBound(0, int(color.red() * factor), 255));
            color.setGreen(qBound(0, int(color.green() * factor), 255));
            color.setBlue(qBound(0, int(color.blue() * factor), 255));
            dst.setPixel(x, y, color.rgba());
        }
    }

    return dst;
}

QImage FilterApplyer::applyNoiseReduction(const QImage &src){
    QImage dst(src.width(), src.height(), src.format());
    for(int y = 3; y < src.height() - 3; ++y){
        for(int x = 3; x < src.width() - 3; ++x){
            QVector<int> reds, greens, blues;
            for (int j = -3; j <= 3; ++j) {
                for (int i = -3; i <= 3; ++i) {
                    QColor color = QColor(src.pixel(x + i, y + j));
                    reds.append(color.red());
                    greens.append(color.green());
                    blues.append(color.blue());
                }
            }

            std::sort(reds.begin(), reds.end());
            std::sort(greens.begin(), greens.end());
            std::sort(blues.begin(), blues.end());
            dst.setPixel(x, y, qRgb(reds[24], greens[24], blues[24]));
        }
    }

    return dst;
}

QImage FilterApplyer::applyEdgeDetection(const QImage &src){
    QImage dst(src.width(), src.height(), src.format());
    int Gx[3][3] = { { -1, 0, 1 },
                     { -2, 0, 2 },
                     { -1, 0, 1 } };

    int Gy[3][3] = { { -1, -2, -1 },
                     { 0, 0, 0 },
                     { 1, 2, 1 } };

    for(int y = 1; y < src.height() - 1; ++y){
        for(int x = 1; x < src.width() - 1; ++x){
            int sumRx = 0, sumGx = 0, sumBx = 0;
            int sumRy = 0, sumGy = 0, sumBy = 0;

            for(int j = -1; j <= 1; ++j){
                for(int i = -1; i <= 1; ++i){
                    QColor color = QColor(src.pixel(x + i, y + j));
                    int weightX = Gx[j + 1][i + 1];
                    int weightY = Gy[j + 1][i + 1];

                    sumRx += color.red() * weightX;
                    sumGx += color.green() * weightX;
                    sumBx += color.blue() * weightX;

                    sumRy += color.red() * weightY;
                    sumGy += color.green() * weightY;
                    sumBy += color.blue() * weightY;
                }
            }

            int r = qBound(0, int(qSqrt(sumRx * sumRx + sumRy * sumRy)), 255);
            int g = qBound(0, int(qSqrt(sumGx * sumGx + sumGy * sumGy)), 255);
            int b = qBound(0, int(qSqrt(sumBx * sumBx + sumBy * sumBy)), 255);

            dst.setPixel(x, y, qRgb(r, g, b));
        }
    }

    return dst;
}

