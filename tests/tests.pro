TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp

INCLUDEPATH += ../3rdparty/shiny/include
DEPENDPATH += $$PWD/../Libs
LIBS += -L$$PWD/../Libs -lshiny
