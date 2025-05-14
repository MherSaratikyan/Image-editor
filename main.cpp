#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication::setAttribute(Qt::AA_DontShowIconsInMenus, false);
    QApplication a(argc, argv);
    MainWindow w;
//    w.showFullScreen();
    w.showMaximized();
    return a.exec();
}
