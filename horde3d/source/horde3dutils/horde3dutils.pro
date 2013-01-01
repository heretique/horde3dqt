QT       -= core gui

TARGET = horde3dutils
TEMPLATE = lib
DESTDIR = $$PWD/../../../Libs


INCLUDEPATH += ../shared \
    ../../bindings/c++

SOURCES += main.cpp

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}

INCLUDEPATH += $$PWD/../../.. $$PWD/../../../Libs
DEPENDPATH += $$PWD/../../../Libs

unix|win32: LIBS += -L$$PWD/../../../Libs -lhorde3d
LIBS += -lopengl32

win32 {
LIBS += -lgdi32
}

