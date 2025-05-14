QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    databasemanager.cpp \
    filterapplyer.cpp \
    filterdialog.cpp \
    graphicscanvas.cpp \
    imageentry.cpp \
    imagemanipulator.cpp \
    main.cpp \
    mainwindow.cpp \
    procedure.cpp \
    project.cpp \
    resizedialog.cpp

HEADERS += \
    databasemanager.h \
    filterapplyer.h \
    filterdialog.h \
    graphicscanvas.h \
    imageentry.h \
    imagemanipulator.h \
    mainwindow.h \
    procedure.h \
    project.h \
    resizedialog.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    icons.qrc

DISTFILES += \
    Icons/Color.png \
    Icons/Dropper.png \
    Icons/Pencil.png \
    Icons/clean.png \
    Icons/eraser.png \
    Icons/eye-dropper-half.png \
    Icons/fill (1).png \
    Icons/fill.png \
    Icons/flip-horizontal.png \
    Icons/makeup-brush.png \
    Icons/pencil (1).png \
    Icons/redo-alt.png \
    Icons/refresh.png \
    Icons/resize.png \
    Icons/rotate-left.png \
    Icons/rotate-right.png \
    Icons/search.png \
    Icons/square-dashed (1).png \
    Icons/square-dashed.png \
    Icons/tool-crop.png \
    Icons/undo-alt.png
