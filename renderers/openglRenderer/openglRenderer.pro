QT       -= core gui

TARGET = openglRenderer
TEMPLATE = lib
CONFIG += staticlib
DESTDIR = $$PWD/../../Libs

INCLUDEPATH += ../../horde3d/source/shared \
    ../../horde3d/source/horde3dengine

SOURCES += renderdevice.cpp \
    renderer.cpp \
    utOpenGL.cpp

HEADERS += renderdevice.h \
    renderer.h \
    utOpenGL.h

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}
