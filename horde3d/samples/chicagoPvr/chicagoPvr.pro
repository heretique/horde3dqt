TEMPLATE = app
#CONFIG += console
#CONFIG -= app_bundle
CONFIG -= qt

DEFINES -= UNICODE

include(../../../3rdparty/pvrshell/pvrshell.pri)

SOURCES += app.cpp \
    crowd.cpp \
    main.cpp


HEADERS += app.h \
    crowd.h



INCLUDEPATH += $$PWD/../../../Libs \
    $$PWD/../../bindings/c++ \
    $$PWD/../../source/horde3dengine \
    $$PWD/../../source/horde3dutils

DEPENDPATH += $$PWD/../../../Libs \
    $$PWD/../../source/horde3dengine \
    $$PWD/../../source/horde3dutils

LIBS += -L$$PWD/../../../Libs

win32 {
    DESTDIR = $$PWD/../../binaries/win32
    LIBS += -L$$PWD/../../../Libs/GLES2 -lhorde3d -lhorde3dutils -llibegl -llibGLESv2 -luser32 -lgdi32
}

    unix {
        DESTDIR = /home/user/opt/rasp-pi-rootfs/home/pi/horde3d
        LIBS += -L/home/user/opt/rasp-pi-rootfs/home/pi/horde3d \
                -L/home/user/opt/rasp-pi-rootfs/opt/vc/lib \
                -L$$OUT_PWD/../../source/horde3dengine/ \
                -L$$OUT_PWD/../../source/horde3dutils/ \
                -lhorde3d -lhorde3dutils -lEGL -lGLESv2

        target.path = /home/pi/horde3d
        INSTALLS += target
    }


