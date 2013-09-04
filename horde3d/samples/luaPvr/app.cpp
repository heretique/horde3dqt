// *************************************************************************************************
//
// Horde3D
//   Next-Generation Graphics Engine
//
// Sample Application
// --------------------------------------
// Copyright (C) 2006-2011 Nicolas Schulz
//
//
// This sample source file is not covered by the EPL as the rest of the SDK
// and may be used without any restrictions. However, the EPL's disclaimer of
// warranty and liability shall be in effect for this file.
//
// *************************************************************************************************

#include "app.h"
#include "Horde3D.h"
#include "Horde3DUtils.h"
#include "luaHorde3d.h"
#include <QMouseEvent>
#include <QCoreApplication>


MyGameWindow::MyGameWindow(const QString appPath, QWidget *parent /* = 0 */)
    : GE::GameWindow(parent), _L(NULL)
{
#ifdef __SYMBIAN32__
    _luaFile = "e:/horde3dContent/Content/lua/knightDemo.lua";
#else
    _luaFile = "../Content/lua/knightDemo.lua";
#endif

}


/*!
  Destructor.
*/
MyGameWindow::~MyGameWindow()
{

}


/*!
  From GameWindow
  \see gamewindow.cpp
*/
void MyGameWindow::onCreate()
{
    // Initialize engine
    _L = luaL_newstate();
    luaL_openlibs(_L);
    luaopen_Horde3d(_L);
    luaL_dofile(_L, _luaFile.toAscii());
    lua_getglobal(_L, "onCreate");
    lua_call(_L, 0, 0);
}


void MyGameWindow::onFreeEGL() {
    lua_getglobal(_L, "onFreeEGL");
    lua_call(_L, 0, 0);
}

/*!
  From GameWindow
  \see gamewindow.cpp
*/
void MyGameWindow::onDestroy()
{
    lua_getglobal(_L, "onDestroy");
    lua_call(_L, 0, 0);
    lua_close(_L);
}

void MyGameWindow::onSizeChanged(int width, int height)
{
    lua_getglobal(_L, "onSizeChanged");
    lua_pushnumber(_L, width);
    lua_pushnumber(_L, height);
    lua_call(_L, 2, 0);
}

void MyGameWindow::keyStateHandler()
{

}

/*!
  From GameWindow
  \see gamewindow.cpp
*/
void MyGameWindow::onUpdate(const float frameDelta)
{
    lua_getglobal(_L, "onUpdate");
    lua_pushnumber(_L, frameDelta);
    lua_call(_L, 1, 0);
}

void MyGameWindow::mouseMoveEvent(QMouseEvent *e)
{
}

void MyGameWindow::mouseReleaseEvent(QMouseEvent *e)
{
    QCoreApplication::quit();
}

void MyGameWindow::keyPressEvent(QKeyEvent *e) {

}

void MyGameWindow::mousePressEvent(QMouseEvent *e) {

}

/*!
  From GameWindow
  \see gamewindow.cpp
*/
void MyGameWindow::onRender()
{
    lua_getglobal(_L, "onRender");
    lua_call(_L, 0, 0);
}
