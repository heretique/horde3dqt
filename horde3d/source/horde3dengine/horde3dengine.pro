QT       -= core gui

TARGET = horde3d
TEMPLATE = lib
win32 {
    DESTDIR = $$PWD/../../../Libs
}

unix {
    DESTDIR = /home/user/opt/rasp-pi-rootfs/home/pi/horde3d
    target.path = /home/pi/horde3d
    INSTALLS += target
}


DEFINES += \
    USE_OPENGLES_RENDERER \
#    USE_OPENGL_RENDERER \
    USE_TERRAIN_EXT


INCLUDEPATH += ../shared \
    ../../bindings/c++ \
    ../../../3rdparty/shiny/include

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


INCLUDEPATH += $$PWD/../../.. $$PWD/../../../Libs
DEPENDPATH += $$PWD/../../../Libs

LIBS += -L$$PWD/../../../Libs -lshiny

contains(DEFINES, USE_OPENGL_RENDERER) {
    unix|win32: LIBS += -L$$PWD/../../../Libs -lopenglRenderer

    INCLUDEPATH += $$PWD/../../../renderers/openglRenderer
    DEPENDPATH += $$PWD/../../../renderers/openglRenderer

    win32: PRE_TARGETDEPS += $$PWD/../../../Libs/openglRenderer.lib
    else:unix: PRE_TARGETDEPS += /home/pi/horde3d/libopenglRenderer.a

    LIBS += -lopengl32
}

contains(DEFINES, USE_OPENGLES_RENDERER) {
    LIBS += -L$$PWD/../../../Libs -lopenglESRenderer

    INCLUDEPATH += $$PWD/../../../Libs $$PWD/../../../renderers/openglESRenderer
    DEPENDPATH += $$PWD/../../../renderers/openglESRenderer

    win32 {
        PRE_TARGETDEPS += $$PWD/../../../Libs/openglESRenderer.lib
        LIBS += -L$$PWD/../../../Libs/GLES2 -llibEGL -llibGLESv2
        INCLUDEPATH += $$PWD/../../../Libs/GLES2
        DEPENDPATH += $$PWD/../../../Libs/GLES2
    }

    unix {
        PRE_TARGETDEPS += /home/user/opt/rasp-pi-rootfs/home/pi/horde3d/libopenglESRenderer.a
        LIBS += -L/home/user/opt/rasp-pi-rootfs/home/pi/horde3d -L/home/user/opt/rasp-pi-rootfs/opt/vc/lib -lEGL -lGLESv2
    }
}

contains(DEFINES, USE_TERRAIN_EXT) {
    unix|win32: LIBS += -L$$PWD/../../../Libs -lterrainExtension

    INCLUDEPATH += $$PWD/../../../Libs $$PWD/../../../extensions/terrain/source
    DEPENDPATH += $$PWD/../../../extensions/terrain/source

    win32: PRE_TARGETDEPS += $$PWD/../../../Libs/terrainExtension.lib
    else:unix: PRE_TARGETDEPS += /home/pi/horde3d/libterrainExtension.a
}
