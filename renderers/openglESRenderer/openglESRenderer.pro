QT       -= core gui

TARGET = openglESRenderer
TEMPLATE = lib
CONFIG += staticlib
DESTDIR = $$PWD/../../Libs

INCLUDEPATH += ../../horde3d/source/shared \
    ../../horde3d/source/horde3dengine

SOURCES += renderdeviceES.cpp \
    rendererES.cpp \
    utOpenGLES.cpp

HEADERS += renderdeviceES.h \
    rendererES.h \
    utOpenGLES.h

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}
