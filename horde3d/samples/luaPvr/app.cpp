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
    : GE::GameWindow(parent), L(NULL)
{
#ifdef __SYMBIAN32__
    _luaFile = "e:/horde3dContent/Content/lua/chicago.lua";
#else
    _luaFile = "../Content/lua/chicago.lua";
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
    L = luaL_newstate();
    luaL_openlibs(L);
    luaopen_Horde3d(L);
    luaL_dofile(L, _luaFile.toAscii());
    lua_getglobal(L, "onCreate");
    lua_call(L, 0, 0);
}


void MyGameWindow::onFreeEGL() {
    lua_getglobal(L, "onFreeEGL");
    lua_call(L, 0, 0);
}

/*!
  From GameWindow
  \see gamewindow.cpp
*/
void MyGameWindow::onDestroy()
{
    lua_getglobal(L, "onDestroy");
    lua_call(L, 0, 0);
    lua_close(L);
}

void MyGameWindow::onSizeChanged(int width, int height)
{
    lua_getglobal(L, "onSizeChanged");
    lua_pushnumber(L, width);
    lua_pushnumber(L, height);
    lua_call(L, 2, 0);
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
    lua_getglobal(L, "onUpdate");
    lua_pushnumber(L, frameDelta);
    lua_call(L, 1, 0);
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
    lua_getglobal(L, "onRender");
    lua_call(L, 0, 0);
}
