QT       -= core gui

TARGET = shiny
TEMPLATE = lib
CONFIG += staticlib
DESTDIR = $$PWD/../../Libs

#DEFINES -= UNICODE

INCLUDEPATH += include

SOURCES += src\ShinyManager.cpp \
    src/ShinyNode.cpp \
    src/ShinyNodePool.cpp \
    src/ShinyOutput.cpp \
    src/ShinyTools.cpp

HEADERS += include/Shiny.h \
    include/ShinyConfig.h \
    include/ShinyData.h \
    include/ShinyMacros.h \
    include/ShinyManager.h \
    include/ShinyNode.h \
    include/ShinyNodePool.h \
    include/ShinyOutput.h \
    include/ShinyPrereqs.h \
    include/ShinyTools.h \
    include/ShinyZone.h

