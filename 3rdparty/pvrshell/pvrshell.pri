
DEFINES += BUILD_OGLES2 #BUILD_OGLES3

SOURCES += $$PWD/PVRShell.cpp \
    $$PWD/OS/Windows/PVRShellOS.cpp \
    $$PWD/API/KEGL/PVRShellAPI.cpp


HEADERS += $$PWD/PVRShell.h \
    $$PWD/PVRShellImpl.h \
    $$PWD/OS/Windows/PVRShellOS.h \
    $$PWD/API/KEGL/PVRShellAPI.h

INCLUDEPATH += $$PWD \
    $$PWD/OS/Windows \
    $$PWD/API/KEGL


contains(DEFINES, BUILD_OGLES2) {
    INCLUDEPATH += $$PWD/../../3rdparty/OGLES2
}

contains(DEFINES, BUILD_OGLES3) {
    INCLUDEPATH += $$PWD/../../3rdparty/OGLES3
}
