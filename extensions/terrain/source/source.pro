QT       -= core gui

TARGET = terrainExtension
TEMPLATE = lib
CONFIG += staticlib

win32 {
    DESTDIR = $$PWD/../../../Libs
}

unix {
    DESTDIR = /home/user/opt/rasp-pi-rootfs/home/pi/horde3d
    target.path = /home/pi/horde3d
    INSTALLS += target
}


INCLUDEPATH += ../../../horde3d/source/shared \
    ../../../horde3d/source/horde3dengine

SOURCES += extension.cpp \
    terrain.cpp


HEADERS += extension.h \
    terrain.h
