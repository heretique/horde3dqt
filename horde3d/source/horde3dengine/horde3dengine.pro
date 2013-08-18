QT       -= gui

TARGET = horde3d
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


DEFINES += \
    USE_OPENGLES_RENDERER \
#    USE_OPENGL_RENDERER \
    USE_TERRAIN_EXT \
    Q_LOGGING


INCLUDEPATH += ../shared \
    ../../bindings/c++ \
    ../../../3rdparty/shiny/include \
    ../../../3rdparty/genlog

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

    win32-msvc*: PRE_TARGETDEPS += $$PWD/../../../Libs/openglRenderer.lib
    else:win32-g++: PRE_TARGETDEPS += $$PWD/../../../Libs/libopenglRenderer.a
    else:unix: PRE_TARGETDEPS += /home/pi/horde3d/libopenglRenderer.a

    LIBS += -lopengl32
}

contains(DEFINES, USE_OPENGLES_RENDERER) {
    symbian: QT += opengl
    symbian: LIBS += -lopenglESRenderer
    else:unix|win32: LIBS += -L$$PWD/../../../Libs -lopenglESRenderer

    INCLUDEPATH += $$PWD/../../../Libs $$PWD/../../../renderers/openglESRenderer
    DEPENDPATH += $$PWD/../../../renderers/openglESRenderer

    win32 {
        win32-msvc*: PRE_TARGETDEPS += $$PWD/../../../Libs/openglESRenderer.lib
        else:win32-g++: PRE_TARGETDEPS += $$PWD/../../../Libs/libopenglESRenderer.a
        LIBS += -L$$PWD/../../../Libs/GLES2 -llibEGL -llibGLESv2
        INCLUDEPATH += $$PWD/../../../Libs/GLES2
        DEPENDPATH += $$PWD/../../../Libs/GLES2
    }

    unix:!symbian {
        PRE_TARGETDEPS += /home/user/opt/rasp-pi-rootfs/home/pi/horde3d/libopenglESRenderer.a
        LIBS += -L/home/user/opt/rasp-pi-rootfs/home/pi/horde3d -L/home/user/opt/rasp-pi-rootfs/opt/vc/lib -lEGL -lGLESv2
    }
}

contains(DEFINES, USE_TERRAIN_EXT) {
    symbian: LIBS += -lterrainExtension
    else:unix|win32: LIBS += -L$$PWD/../../../Libs -lterrainExtension

    INCLUDEPATH += $$PWD/../../../Libs $$PWD/../../../extensions/terrain/source
    DEPENDPATH += $$PWD/../../../extensions/terrain/source

    win32-msvc*: PRE_TARGETDEPS += $$PWD/../../../Libs/terrainExtension.lib
    else:win32-g++: PRE_TARGETDEPS += $$PWD/../../../Libs/libterrainExtension.a
    else:unix:!symbian PRE_TARGETDEPS += /home/pi/horde3d/libterrainExtension.a
}

!contains(DEFINES, BUILD_STATIC) {
    symbian {
        TARGET.UID3 = 0xE8AB120a
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
    }
}
