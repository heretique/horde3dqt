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



#define LUA_CHECKS_ENABLED 1
#ifdef LUA_CHECKS_ENABLED
#define LUA_METHOD_CHECK     if (lua_isnoneornil(L, -1)) { \
                                qDebug() << "Lua method not found."; \
                                lua_pop(L, 1); \
                                return; \
                             }
#define LUA_CALL_CHECK     if (_luaError) { \
                            qDebug() << lua_tostring(L, -1); \
                            lua_pop(L, 1); /* pop error message from the stack */ \
                           }
#else
#define LUA_METHOD_CHECK
#define LUA_CALL_CHECK
#endif


MyGameWindow::MyGameWindow(const QString appPath, QWidget *parent /* = 0 */)
    : GE::GameWindow(parent), L(NULL), _luaError(0)
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
    LUA_METHOD_CHECK;

    _luaError = lua_pcall(L, 0, 0, 0);
    LUA_CALL_CHECK;
}


void MyGameWindow::onFreeEGL() {
    lua_getglobal(L, "onFreeEGL");
    LUA_METHOD_CHECK;

    _luaError = lua_pcall(L, 0, 0, 0);
    LUA_CALL_CHECK;

}

/*!
  From GameWindow
  \see gamewindow.cpp
*/
void MyGameWindow::onDestroy()
{
    lua_getglobal(L, "onDestroy");
    LUA_METHOD_CHECK;

    _luaError = lua_pcall(L, 0, 0, 0);
    LUA_CALL_CHECK;

    lua_close(L);
}

void MyGameWindow::onSizeChanged(int width, int height)
{
    lua_getglobal(L, "onSizeChanged");
    LUA_METHOD_CHECK;

    lua_pushnumber(L, width);
    lua_pushnumber(L, height);
    _luaError = lua_pcall(L, 2, 0, 0);
    LUA_CALL_CHECK;
}

/*!
  From GameWindow
  \see gamewindow.cpp
*/
void MyGameWindow::onUpdate(const float frameDelta)
{
    lua_getglobal(L, "onUpdate");
    LUA_METHOD_CHECK;

    lua_pushnumber(L, frameDelta);
    _luaError = lua_pcall(L, 1, 0, 0);
    LUA_CALL_CHECK;
}

void MyGameWindow::mousePressEvent(QMouseEvent *e) {
    lua_getglobal(L, "mousePressEvent");
    LUA_METHOD_CHECK;
    lua_pushinteger(L, e->buttons());
    lua_pushinteger(L, e->x());
    lua_pushinteger(L, e->y());
    _luaError = lua_pcall(L, 3, 0, 0);
    LUA_CALL_CHECK;
}

void MyGameWindow::mouseMoveEvent(QMouseEvent *e) {
    lua_getglobal(L, "mouseMoveEvent");
    LUA_METHOD_CHECK;
    lua_pushinteger(L, e->x());
    lua_pushinteger(L, e->y());
    _luaError = lua_pcall(L, 2, 0, 0);
    LUA_CALL_CHECK;
}

void MyGameWindow::mouseReleaseEvent(QMouseEvent *e) {
    lua_getglobal(L, "mouseReleaseEvent");
    LUA_METHOD_CHECK;
    lua_pushinteger(L, e->buttons());
    lua_pushinteger(L, e->x());
    lua_pushinteger(L, e->y());
    _luaError = lua_pcall(L, 3, 0, 0);
    LUA_CALL_CHECK;
}

void MyGameWindow::keyPressEvent(QKeyEvent *e) {
    lua_getglobal(L, "keyPressEvent");
    LUA_METHOD_CHECK;
    lua_pushinteger(L, e->key());
    _luaError = lua_pcall(L, 1, 0, 0);
    LUA_CALL_CHECK;
}

void MyGameWindow::keyReleaseEvent(QKeyEvent *e) {
    lua_getglobal(L, "keyReleaseEvent");
    LUA_METHOD_CHECK;
    lua_pushinteger(L, e->key());
    _luaError = lua_pcall(L, 1, 0, 0);
    LUA_CALL_CHECK;
}

/*!
  From GameWindow
  \see gamewindow.cpp
*/
void MyGameWindow::onRender()
{
    lua_getglobal(L, "onRender");
    LUA_METHOD_CHECK;

    _luaError = lua_pcall(L, 0, 0, 0);
    LUA_CALL_CHECK;
}
