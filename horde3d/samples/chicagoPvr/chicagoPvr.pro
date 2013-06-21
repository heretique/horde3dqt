TEMPLATE = app
#CONFIG += console
#CONFIG -= app_bundle
QT += core gui

DEFINES -= UNICODE

include(../../../3rdparty/qtgameenabler/qtgameenabler.pri)

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

symbian {
    TARGET.UID3 = 0xE077F808
    TARGET.EPOCSTACKSIZE = 0x14000
    TARGET.EPOCHEAPSIZE = 0x020000 \
        0x1000000

    TARGET.CAPABILITY += NetworkServices \
                        LocalServices \
                        ReadUserData \
                        WriteUserData \
                        UserEnvironment \
                        Location \
                        ReadDeviceData \
                        WriteDeviceData \
                        SwEvent \
                        ProtServ \
                        PowerMgmt \
                        TrustedUI \
                        SurroundingsDD

    ICON = ../../../3rdparty/qtgameenabler/icons/qtgameenabler.svg

    LIBS += -lhorde3d -lhorde3dutils -lshiny -lopenglESRenderer -lterrainExtension
}

win32 {
    DESTDIR = $$PWD/../../binaries/win32
    LIBS += -L$$PWD/../../../Libs/GLES2 -lhorde3d -lhorde3dutils
}

    unix:!symbian {
        DESTDIR = /home/user/opt/rasp-pi-rootfs/home/pi/horde3d
        LIBS += -L/home/user/opt/rasp-pi-rootfs/home/pi/horde3d \
                -L/home/user/opt/rasp-pi-rootfs/opt/vc/lib \
                -L$$OUT_PWD/../../source/horde3dengine/ \
                -L$$OUT_PWD/../../source/horde3dutils/ \
                -lhorde3d -lhorde3dutils -lEGL -lGLESv2

        target.path = /home/pi/horde3d
        INSTALLS += target
    }


