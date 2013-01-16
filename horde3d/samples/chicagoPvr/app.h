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

#include <PVRShell.h>
#include "crowd.h"
#include <string>


class Application: public PVRShell
{
public:
	Application( const std::string &appPath );
	
	void setKeyState( int key, bool state ) { _prevKeys[key] = _keys[key]; _keys[key] = state; }

	const char *getTitle() { return "Chicago - Horde3D Sample"; }
	
	void resize( int width, int height );

	void keyStateHandler();
	void mouseMoveEvent( float dX, float dY );

private:
	void keyHandler();

private:
	bool         _keys[320], _prevKeys[320];

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

	std::string  _contentDir;

public:
    virtual bool InitApplication();
    virtual bool InitView();
    virtual bool ReleaseView();
    virtual bool QuitApplication();
    virtual bool RenderScene();
};

#endif // _app_H_



