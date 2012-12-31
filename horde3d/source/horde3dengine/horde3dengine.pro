QT       -= core gui

TARGET = horde3d
TEMPLATE = lib
DESTDIR = $$PWD/../../../Libs

DEFINES += USE_OPENGL_RENDERER \
    USE_OPENGLES_RENDERER \
    USE_TERRAIN_EXT


INCLUDEPATH += ../shared \
    ../../bindings/c++

SOURCES += egAnimatables.cpp \
    egAnimation.cpp \
    egCamera.cpp \
    egCom.cpp \
    egExtensions.cpp \
    egGeometry.cpp \
    egLight.cpp \
    egMain.cpp \
    egMaterial.cpp \
    egModel.cpp \
    egModules.cpp \
    egParticle.cpp \
    egPipeline.cpp \
    egPrimitives.cpp \
    egRenderDeviceBase.cpp \
    egRendererBase.cpp \
    egRenderManager.cpp \
    egResource.cpp \
    egScene.cpp \
    egSceneGraphRes.cpp \
    egShader.cpp \
    egTexture.cpp \
    utImage.cpp

HEADERS += config.h \
    egAnimatables.h \
    egAnimation.h \
    egCamera.h \
    egCom.h \
    egExtensions.h \
    egGeometry.h \
    egLight.h \
    egMaterial.h \
    egModel.h \
    egModules.h \
    egParticle.h \
    egPipeline.h \
    egPrerequisites.h \
    egPrimitives.h \
    egRenderDeviceBase.h \
    egRendererBase.h \
    egRenderManager.h \
    egResource.h \
    egScene.h \
    egSceneGraphRes.h \
    egShader.h \
    egTexture.h \
    utImage.h \
    utTimer.h \

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}

INCLUDEPATH += $$PWD/../../.. $$PWD/../../../Libs
DEPENDPATH += $$PWD/../../../Libs


contains(DEFINES, USE_OPENGL_RENDERER) {
    unix|win32: LIBS += -L$$PWD/../../../Libs -lopenglRenderer

    INCLUDEPATH += $$PWD/../../../renderers/openglRenderer
    DEPENDPATH += $$PWD/../../../renderers/openglRenderer

    win32: PRE_TARGETDEPS += $$PWD/../../../Libs/openglRenderer.lib
    else:unix: PRE_TARGETDEPS += $$PWD/../../../Libs/libopenglRenderer.a

    LIBS += -lopengl32
}

contains(DEFINES, USE_OPENGLES_RENDERER) {
    unix|win32: LIBS += -L$$PWD/../../../Libs -lopenglESRenderer

    INCLUDEPATH += $$PWD/../../../Libs $$PWD/../../../renderers/openglESRenderer
    DEPENDPATH += $$PWD/../../../renderers/openglESRenderer

    win32: PRE_TARGETDEPS += $$PWD/../../../Libs/openglESRenderer.lib
    else:unix: PRE_TARGETDEPS += $$PWD/../../../Libs/libopenglESRenderer.a

    LIBS += -L$$PWD/../../../Libs/GLES2 -llibEGL -llibGLESv2
    INCLUDEPATH += $$PWD/../../../Libs/GLES2
    DEPENDPATH += $$PWD/../../../Libs/GLES2
}

contains(DEFINES, USE_TERRAIN_EXT) {
    unix|win32: LIBS += -L$$PWD/../../../Libs -lterrainExtension

    INCLUDEPATH += $$PWD/../../../Libs $$PWD/../../../extensions/terrain/source
    DEPENDPATH += $$PWD/../../../extensions/terrain/source

    win32: PRE_TARGETDEPS += $$PWD/../../../Libs/terrainExtension.lib
    else:unix: PRE_TARGETDEPS += $$PWD/../../../Libs/libterrainExtension.a
}
