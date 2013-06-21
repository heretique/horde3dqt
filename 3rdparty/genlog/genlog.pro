QT       -= gui

TARGET = genlog
TEMPLATE = lib
CONFIG += staticlib
DESTDIR = $$PWD/../../Libs

#DEFINES -= UNICODE
DEFINES += Q_LOGGING
INCLUDEPATH += include

SOURCES += genprofiling.cpp \
    genlogdest.cpp \
    genlog.cpp \
    gendebugoutput.cpp


HEADERS += genprofiling.h \
    genlogdest.h \
    genlog.h \
    gendebugoutput.h
