QT       -= core gui

TARGET = terrainExtension
TEMPLATE = lib
CONFIG += staticlib
DESTDIR = $$PWD/../../../Libs

INCLUDEPATH += ../../../horde3d/source/shared \
    ../../../horde3d/source/horde3dengine

SOURCES += extension.cpp \
    terrain.cpp


HEADERS += extension.h \
    terrain.h

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}
