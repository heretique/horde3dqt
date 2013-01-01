TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt


DEFINES -= UNICODE

SOURCES += app.cpp \
    main.cpp


HEADERS += app.h


unix|win32: LIBS += -L$$PWD/../../../Libs -lglfw -lhorde3d -lhorde3dutils -lopengl32

INCLUDEPATH += $$PWD/../../../Libs \
    $$PWD/../../bindings/c++ \
    $$PWD/../glfw

DEPENDPATH += $$PWD/../../../Libs

win32 {
    DESTDIR = $$PWD/../../binaries/win32
    PRE_TARGETDEPS += $$PWD/../../../Libs/glfw.lib
    LIBS += -luser32
}
else:unix: PRE_TARGETDEPS += $$PWD/../../../Libs/libglfw.a
