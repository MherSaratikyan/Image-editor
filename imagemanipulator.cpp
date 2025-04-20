#include "imagemanipulator.h"

ImageManipulator::ImageManipulator()
{

}

QImage ImageManipulator::rotateLeft(const QImage& src){
    int image_height = src.height();
    int image_width = src.width();

    QImage dst(image_height, image_width, src.format());
    for(int y = 0; y < image_height; ++y){
        for(int x = 0; x < image_width; ++x){
            dst.setPixel(y, image_width - x - 1, src.pixel(x, y));
        }
    }

    return dst;
}

QImage ImageManipulator::rotateRight(const QImage& src){
    int image_height = src.height();
    int image_width = src.width();

    QImage dst(image_height, image_width, src.format());
    for(int y = 0; y < image_height; ++y){
        for(int x = 0; x < image_width; ++x){
            dst.setPixel(image_height - y - 1, x, src.pixel(x, y));
        }
    }

    return dst;
}

QImage ImageManipulator::rotate180(const QImage& src){
    int image_height = src.height();
    int image_width = src.width();

    QImage dst(image_width, image_height, src.format());
    for(int y = 0; y < image_height; ++y){
        for(int x = 0; x < image_width; ++x){
            dst.setPixel(image_width - x - 1, image_height - y - 1, src.pixel(x, y));
        }
    }

    return dst;
}

QImage ImageManipulator::flipHorizontally(const QImage& src){
    int image_height = src.height();
    int image_width = src.width();

    QImage dst(image_width, image_height, src.format());
    for(int y = 0; y < image_height; ++y){
        for(int x = 0; x < image_width; ++x){
            dst.setPixel(image_width - x - 1, y, src.pixel(x, y));
        }
    }

    return dst;
}

QImage ImageManipulator::flipVertically(const QImage& src){
    int image_height = src.height();
    int image_width = src.width();

    QImage dst(image_width, image_height, src.format());
    for(int y = 0; y < image_height; ++y){
        for(int x = 0; x < image_width; ++x){
            dst.setPixel(x,image_height - y - 1, src.pixel(x, y));
        }
    }

    return dst;
}
