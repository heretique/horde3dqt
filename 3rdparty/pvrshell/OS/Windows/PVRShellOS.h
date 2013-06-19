/******************************************************************************

 @File         PVRShellOS.h

 @Title        Windows/PVRShellOS

 @Version       @Version      

 @Copyright    Copyright (c) Imagination Technologies Limited.

 @Platform     WinCE/Windows

 @Description  Makes programming for 3D APIs easier by wrapping surface
               initialization, Texture allocation and other functions for use by a demo.

******************************************************************************/
#ifndef _PVRSHELLOS_
#define _PVRSHELLOS_

#include <windows.h>

// The following defines are for Windows PC platforms only
#if defined(_WIN32) && !defined(__MINGW32__)
// Enable the following 2 lines for memory leak checking - also see WinMain()
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#define PVRSHELL_DIR_SYM	'\\'
#define vsnprintf _vsnprintf

/*!***************************************************************************
 PVRShellInitOS
 @Brief Class. Interface with specific Operative System.
*****************************************************************************/
class PVRShellInitOS
{
public:
	HDC			m_hDC;
	HWND		m_hWnd;

	// Pixmap support: variables for the pixmap
	HBITMAP		m_hBmPixmap, m_hBmPixmapOld;
	HDC			m_hDcPixmap;

	HACCEL		m_hAccelTable;
	HINSTANCE	m_hInstance;
	int			m_nCmdShow;

	bool		m_bHaveFocus;

	unsigned int	m_u32ButtonState;

public:
	ATOM MyRegisterClass();
};

#endif /* _PVRSHELLOS_ */
/*****************************************************************************
 End of file (PVRShellOS.h)
*****************************************************************************/
