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
#include "crowd.h"
#include <string>


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
    float        _x, _y, _z, _rx, _ry;  // Viewer position and orientation
    float        _velocity;  // Velocity for movement
    float        _curFPS;

    int          _statMode;
    int          _freezeMode;
    bool         _debugViewMode, _wireframeMode;

    CrowdSim     *_crowdSim;

    // Engine objects
    H3DRes       _fontMatRes, _panelMatRes;
    H3DRes       _logoMatRes, _forwardPipeRes, _deferredPipeRes;
    H3DNode      _cam;

    QString  _contentDir;
};

#endif // _app_H_



