QT       -= core gui

TARGET = horde3dutils
TEMPLATE = lib
symbian {
    CONFIG += staticlib
    DEFINES += BUILD_STATIC
}
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
LIBS += -L$$PWD/../../../Libs -lshiny

unix|win32:!symbian: LIBS += -L$$PWD/../../../Libs -lhorde3d
#LIBS += -lopengl32

win32 {
LIBS += -lgdi32
}

unix:!symbian {
LIBS += -L/home/user/opt/rasp-pi-rootfs/home/pi/horde3d
}

!contains(DEFINES, BUILD_STATIC) {
    symbian {
        TARGET.UID3 = 0xE8AB120b
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

        TARGET.EPOCALLOWDLLDATA = 1
        MMP_RULES += EXPORTUNFROZEN

        addFiles.sources = $${TARGET}.dll
        addFiles.path = !:/sys/bin
        DEPLOYMENT += addFiles

        LIBS += -lhorde3d
    }
}
