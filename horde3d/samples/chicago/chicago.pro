TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt


DEFINES -= UNICODE

SOURCES += app.cpp \
    crowd.cpp \
    main.cpp


HEADERS += app.h \
    crowd.h


unix|win32: LIBS += -L$$PWD/../../../Libs -lglfw -lhorde3d -lhorde3dutils -lopengl32

INCLUDEPATH += $$PWD/../../../Libs \
    $$PWD/../../bindings/c++ \
    $$PWD/../../../3rdparty/glfw

DEPENDPATH += $$PWD/../../../Libs

win32 {
    DESTDIR = $$PWD/../../binaries/win32
    PRE_TARGETDEPS += $$PWD/../../../Libs/glfw.lib
    LIBS += -luser32
}
else:unix: PRE_TARGETDEPS += $$PWD/../../../Libs/libglfw.a



