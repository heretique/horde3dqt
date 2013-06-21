/******************************************************************************

 @File         PVRShellOS.h

 @Title        SymbianS60/PVRShellOS

 @Version      

 @Copyright    Copyright (C)  Imagination Technologies Limited.

 @Platform     Support for windowed Symbian TechView

 @Description  Makes programming for 3D APIs easier by wrapping surface
               initialization, Texture allocation and other functions for use by a demo.

******************************************************************************/
#ifndef _PVRSHELLOS_
#define _PVRSHELLOS_

/****************************************************************************
 ** INCLUDES                                                               **
 ****************************************************************************/
#include <fbs.h>   // FbsStartup

#include <akndoc.h>
#include <aknappui.h>
#include <aknapp.h>
#include <eikenv.h>
#include <eikmenub.h>
#include <eikspane.h>
#include <eikstart.h>
#include <string.h>

#include <eikapp.h>
#include <eikdoc.h>

#include <e32debug.h>	// RDebug
#include <e32std.h>
#include <e32math.h> 	// Random()
#include <e32cmn.h>		// HBufC
#include <w32std.h>

#include <coecntrl.h>
#include <coeccntx.h>


/****************************************************************************
 ** DEFINES                                                               **
 ****************************************************************************/
#define PVRSHELL_DIR_SYM	'\\'
#define _stricmp strcasecmp
#define _strnicmp strncasecmp

#include <avkon.hrh>

/*!***************************************************************************
 Declarations
*****************************************************************************/
class CPVRShellAppUi;
class CViewMover;
class CViewWindow;
class  CEikAppUi;

/*!*****************************************************************************************************
 * @Class CPVRShellApplication
 * @Brief  Symbian PVRShell Application interface
 * @Description It serves to define the properties of the application, and also manufacture a new, blank, document.
 * In the simplest case, as here, the only property that you have to define is the application's
 * unique identifier, or UID.
 *******************************************************************************************************/
class CPVRShellApplication : public CAknApplication
{
private: // from CAknApplication
	CApaDocument* CreateDocumentL();
	TUid AppDllUid() const;
};

/*!*****************************************************************************************************
 * @Class CPVRShellDocument
 * @Brief Symbian PVRShell Document interface
 * @Description  A document represents the data model for the application. If the application is file-based, the
 * document is responsible for storing and restoring the application's data. Even if the application
 * is not file-based, it must have a document class, even though that class doesn't do much apart from
 * creating the application user interface (app UI).
 *******************************************************************************************************/
class CPVRShellDocument : public CAknDocument
{
public:
	// Construction & destruction
	CPVRShellDocument(CAknApplication& aApp);
	~CPVRShellDocument();

private:
	// from CAknDocument
	CEikAppUi* CreateAppUiL();
};


/*!*****************************************************************************************************
 * @Class CViewWindow
 * @Brief Symbian Fullscreen Control interface
 * @Description Just provides a blank canvas to draw on top of in fullscreen mode. Means the clock doesn't
 * continually get drawn underneath the demo.
 *******************************************************************************************************/
class CViewWindow : public CCoeControl
{
public:
	void ConstructL();
	void InitWindow();
	void SizeChanged();
	void HandleResourceChange(TInt aType);
	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);
	void HandlePointerEventL(const TPointerEvent& aPointerEvent);


	RWindow			iWin;
	RWindow*		theWindow;

	// Touchscreen variables
	bool  m_bPointer;
	float m_vec2PointerLocation[2];
};

/*!*****************************************************************************************************
 * @Class CPVRShellAppUi
 * @Brief Symbian App UI class
 * @Description Daddy symbian class. Handles the keyboard and menu bar, owns the demo objects, and has a RunL()
 * member function, which is executed periodically and draws a frame in the demo.
 *******************************************************************************************************/
class CPVRShellAppUi : public CAknAppUi,CActive {


public:
	// Construction & destruction
	void ConstructL();
	CPVRShellAppUi() : CActive(EPriorityStandard) {};
	~CPVRShellAppUi();

private:
	void RunL();
	void DoCancel() {};
	void Start(char * CmdLine);
	void HandleCommandL(TInt aCommand);
	void HandleWsEventL(const TWsEvent& aEvent, CCoeControl* aDestination) ;
	TKeyResponse HandleKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);
	void HandleResourceChangeL(TInt aType);
	void HandleForegroundEventL(TBool aForeground);

private:
	int iFrame;

	PVRShellInit*	init;

	bool m_bInForeground;
	bool m_bClose;
};



/*!***************************************************************************
 PVRShellInitOS
 @Brief Class. Interface with specific Operative System.
*****************************************************************************/
class PVRShellInitOS
{
public:
	~PVRShellInitOS() {
		if(iView)
			delete iView;
	};

	bool		bIsWindowed;

public:
	CViewWindow*					iView;					/*!< The AppView object to get some graphical data (client size) */

private:

};

#endif /* _PVRSHELLOS_ */

/*****************************************************************************
 End of file (PVRShellOS.h)
*****************************************************************************/

