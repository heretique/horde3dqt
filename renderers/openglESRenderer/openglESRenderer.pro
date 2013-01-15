QT       -= core gui

TARGET = openglESRenderer
TEMPLATE = lib
CONFIG += staticlib
DESTDIR = $$PWD/../../Libs

DEFINES += BUILD_OGLES2 #BUILD_OGLES3

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

contains(DEFINES, BUILD_OGLES2) {
    INCLUDEPATH += $$PWD/../../3rdparty/OGLES2
}

contains(DEFINES, BUILD_OGLES3) {
    INCLUDEPATH += $$PWD/../../3rdparty/OGLES3
}
