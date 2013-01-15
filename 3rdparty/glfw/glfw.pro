QT       -= core gui

TARGET = glfw
TEMPLATE = lib
CONFIG += staticlib
DESTDIR = $$PWD/../../Libs

DEFINES -= UNICODE

INCLUDEPATH += lib

SOURCES += lib/enable.c \
    lib/fullscreen.c \
    lib/glext.c \
    lib/image.c \
    lib/init.c \
    lib/input.c \
    lib/joystick.c \
    lib/stream.c \
    lib/tga.c \
    lib/thread.c \
    lib/time.c \
    lib/window.c

HEADERS += glfw.h \
    lib/internal.h


win32 {
    INCLUDEPATH += lib/win32

    SOURCES += lib/win32/win32_dllmain.c \
    lib/win32/win32_enable.c \
    lib/win32/win32_fullscreen.c \
    lib/win32/win32_glext.c \
    lib/win32/win32_init.c \
    lib/win32/win32_joystick.c \
    lib/win32/win32_thread.c \
    lib/win32/win32_time.c \
    lib/win32/win32_window.c

    HEADERS += lib/win32/platform.h
}

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}
