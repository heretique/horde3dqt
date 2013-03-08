QT       -= core gui

TARGET = horde3dutils
TEMPLATE = lib
win32 {
    DESTDIR = $$PWD/../../../Libs
}

unix {
    DESTDIR = /home/user/opt/rasp-pi-rootfs/home/pi/horde3d
    target.path = /home/pi/horde3d
    INSTALLS += target
}

INCLUDEPATH += ../shared \
    ../../bindings/c++

SOURCES += main.cpp

INCLUDEPATH += $$PWD/../../.. $$PWD/../../../Libs
DEPENDPATH += $$PWD/../../../Libs

unix|win32: LIBS += -L$$PWD/../../../Libs -lhorde3d 

win32 {
LIBS += -lgdi32 -lopengl32
}

unix {
LIBS += -L/home/user/opt/rasp-pi-rootfs/home/pi/horde3d
}

