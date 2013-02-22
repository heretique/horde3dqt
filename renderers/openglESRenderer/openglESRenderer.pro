QT       -= core gui

TARGET = openglESRenderer
TEMPLATE = lib
CONFIG += staticlib
win32 {
    DESTDIR = $$PWD/../../Libs
}

unix {
    DESTDIR = /home/user/opt/rasp-pi-rootfs/home/pi/horde3d
    target.path = /home/pi/horde3d
    INSTALLS += target
}

DEFINES += BUILD_OGLES2 \
           #BUILD_OGLES3

INCLUDEPATH += ../../horde3d/source/shared \
    ../../horde3d/source/horde3dengine \
    ../../3rdparty/shiny/include

SOURCES += renderdeviceES.cpp \
    rendererES.cpp \
    utOpenGLES.cpp

HEADERS += renderdeviceES.h \
    rendererES.h \
    utOpenGLES.h

contains(DEFINES, BUILD_OGLES2) {
win32 {
    INCLUDEPATH += $$PWD/../../3rdparty/OGLES2
}
unix {
    INCLUDEPATH += /home/user/opt/rasp-pi-rootfs/opt/vc/include \
                   /home/user/opt/rasp-pi-rootfs/opt/vc/include/interface/vcos/pthreads
}
}

contains(DEFINES, BUILD_OGLES3) {
    INCLUDEPATH += $$PWD/../../3rdparty/OGLES3
}
