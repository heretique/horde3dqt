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

#ifndef _app_H_
#define _app_H_

#include <gamewindow.h>
#include <string>
#include "lua.hpp"


class MyGameWindow : public GE::GameWindow
{
    Q_OBJECT

public:
    explicit MyGameWindow(const QString appPath, QWidget *parent = 0);
    virtual ~MyGameWindow();

public:
    void keyPressEvent(QKeyEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent( QMouseEvent *e );
    void keyStateHandler();

protected: // From GameWindow
    void onRender();
    void onUpdate(const float frameDelta);
    void onCreate();
    void onFreeEGL();
    void onDestroy();
    void onSizeChanged(int width, int height);

private:
    lua_State   *_L;
    QString _luaFile;
};

#endif // _app_H_



