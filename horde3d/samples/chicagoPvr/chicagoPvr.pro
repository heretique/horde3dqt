TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

DEFINES -= UNICODE

include(../../../3rdparty/pvrshell/pvrshell.pri)

SOURCES += app.cpp \
    crowd.cpp \
    main.cpp


HEADERS += app.h \
    crowd.h


win32: LIBS += -L$$PWD/../../../Libs -L$$PWD/../../../Libs/GLES2 -lhorde3d -lhorde3dutils -llibegl -llibGLESv2

INCLUDEPATH += $$PWD/../../../Libs \
    $$PWD/../../bindings/c++

DEPENDPATH += $$PWD/../../../Libs

win32 {
    DESTDIR = $$PWD/../../binaries/win32
    LIBS += -luser32 -lgdi32
}



