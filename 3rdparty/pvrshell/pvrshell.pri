
DEFINES += BUILD_OGLES2 #BUILD_OGLES3

SOURCES += $$PWD/PVRShell.cpp \
    $$PWD/API/KEGL/PVRShellAPI.cpp


HEADERS += $$PWD/PVRShell.h \
    $$PWD/PVRShellImpl.h \
    $$PWD/API/KEGL/PVRShellAPI.h

INCLUDEPATH += $$PWD \
    $$PWD/API/KEGL

win32 {
    SOURCES += $$PWD/OS/Windows/PVRShellOS.cpp
    HEADERS += $$PWD/OS/Windows/PVRShellOS.h
    INCLUDEPATH += $$PWD/OS/Windows
}

unix:!symbian {
    SOURCES += $$PWD/OS/LinuxRaw/PVRShellOS.cpp
    HEADERS += $$PWD/OS/LinuxRaw/PVRShellOS.h
    INCLUDEPATH += $$PWD/OS/LinuxRaw \
                    /home/user/opt/rasp-pi-rootfs/opt/vc/include \
                    /home/user/opt/rasp-pi-rootfs/opt/vc/include/interface/vcos/pthreads
    LIBS += -L/home/user/opt/rasp-pi-rootfs/opt/vc/lib -lbcm_host
}

symbian {
    SOURCES += $$PWD/OS/Symbian/PVRShellOS.cpp
    HEADERS += $$PWD/OS/Symbian/PVRShellOS.h
    INCLUDEPATH += $$PWD/OS/Symbian
}

contains(DEFINES, BUILD_OGLES2) {
    INCLUDEPATH += $$PWD/../../3rdparty/OGLES2
}

contains(DEFINES, BUILD_OGLES3) {
    INCLUDEPATH += $$PWD/../../3rdparty/OGLES3
}
