TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

DEFINES -= UNICODE

SOURCES += converter.cpp \
    daeMain.cpp \
    main.cpp \
    optimizer.cpp \
    utils.cpp

HEADERS += converter.h \
    daeCommon.h \
    daeLibAnimations.h \
    daeLibControllers.h \
    daeLibEffects.h \
    daeLibGeometries.h \
    daeLibImages.h \
    daeLibMaterials.h \
    daeLibNodes.h \
    daeLibVisualScenes.h \
    daeMain.h \
    optimizer.h \
    utils.h

INCLUDEPATH += ../shared
