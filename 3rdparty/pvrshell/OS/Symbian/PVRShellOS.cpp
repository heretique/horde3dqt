/******************************************************************************

 @File         PVRShellOS.cpp

 @Title        SymbianS60/PVRShellOS

 @Version      

 @Copyright    Copyright (C)  Imagination Technologies Limited.

 @Platform     SymbianS60

 @Description  Makes programming for 3D APIs easier by wrapping window creation
               and other functions for use by a demo.

******************************************************************************/

/****************************************************************************
 ** INCLUDES                                                               **
 ****************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "PVRShell.h"
#include "PVRShellAPI.h"
#include "PVRShellOS.h"
#include "PVRShellImpl.h"

#include <AknGlobalNote.h>

#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }

/****************************************************************************
 ** GLOBAL CODE                                                            **
 ****************************************************************************/
void WriteToLog(const char *pszString, const char *pWriteMode)
{
#ifdef OUTPUT_DEBUG_LOG
	FILE *m_logf = fopen("e:\\PVRShellLOG.txt",pWriteMode);

	if(m_logf)
	{
		fprintf(m_logf,pszString);
		fflush(m_logf);
		fclose(m_logf);
	}
#endif
}

/**********************************************************
 **********************************************************
 **                                                      **
 **                     ENTRY POINT						 **
 **                                                      **
 **********************************************************
 **********************************************************/

const TUid KUidPVRShell = { PVR_UID };

//
// DLL interface
//

#if defined(__WINS__) && !defined(EKA2)
GLDEF_C TInt E32Dll(TDllReason)
	{
	return KErrNone;
	}
EXPORT_C TInt WinsMain(TDesC* aCmdLine)
	{
	return EikStart::RunApplication(KExampleUid, aCmdLine);
	}
#endif


CApaApplication* NewApplication()
{
	return new CPVRShellApplication;
}

TInt E32Main()
{
	WriteToLog("", "w"); //Clear the log file (assuming OUTPUT_DEBUG_LOG is defined)

	// this sets off a chain of events which generates an application object, a
	// document object, then a CPVRShellAppUI object, which is where the relevant
	// code starts.
	return EikStart::RunApplication(NewApplication);
}


/**************************
 **************************
 **                      **
 ** CLASS IMPLEMENTATION **
 **                      **
 **************************
 **************************/


/*******************************************************************************************************
 * Application class                                                                                   *
 * =================                                                                                   *
 * It serves to define the properties of the application, and also manufacture a new, blank, document. *
 *******************************************************************************************************/

TUid CPVRShellApplication::AppDllUid() const
{
	return KUidPVRShell;
}

CApaDocument* CPVRShellApplication::CreateDocumentL()
{
	return new (ELeave) CPVRShellDocument(*this);
}


/*******************************************************************************************************
 * Document class                                                                                      *
 * ==============                                                                                      *
 * A document represents the data model for the application. If the application is file-based, the     *
 * document is responsible for storing and restoring the application's data. Even if the application   *
 * is not file-based, it must have a document class, even though that class doesn't do much apart from *
 * creating the application user interface (app UI).                                                   *
 *******************************************************************************************************/

CPVRShellDocument::CPVRShellDocument(CAknApplication& aApp)
	: CAknDocument(aApp)
{
}

CPVRShellDocument::~CPVRShellDocument()
{
	// release all of the STDLIB resources associated with this thread
	// Satisfies the CONE policy that the heap should be balanced across
	// the lifetime of a CCoeEnv.
    //CloseSTDLIB();
}

CEikAppUi* CPVRShellDocument::CreateAppUiL()
{
	return new(ELeave) CPVRShellAppUi;
}

/*******************************************************************************************************
 * App UI class                                                                                        *
 * ============                                                                                        *
 * Main symbian class. Handles the keyboard and menu bar, owns the demo objects, and has a RunL()      *
 * member function, which is executed periodically and draws a frame for the demo.                     *
 *******************************************************************************************************/

void CPVRShellAppUi::ConstructL()
{
	/*
	Initialisation for Symbian AppUi object:
	*/

	// series60 blocks simultaneous key presses by default
    SetKeyBlockMode( ENoKeyBlock );

	BaseConstructL();
	Start("\0");
}

void CPVRShellAppUi::Start(char *CmdLine)
{
	iFrame = 0;

	/*
	Get a shell object, containing the user defined code, and pass it to a new
	application initializer object:
	*/
	init = new PVRShellInit();
	init->Init();

#ifdef USE_CONSOLE_TXT
	/*
		As the S60 3rd shell has no dialog box for entering commandline
		options I've made it look for a text file with some in.
	*/
	char cl[256];
	FILE *pFile=0;
	pFile = fopen("E:\\console.txt", "rt");

	if(pFile)
	{
		if(fgets(cl, 256, pFile))
		{
			init->CommandLine(&cl[0]);
			fclose(pFile);
			pFile = 0;
		}
	}
#endif
	//parse command line:
	init->CommandLine(CmdLine);

	/*
	Initialisation for fullscreen or windowed mode:
	"init" has a member called iView, which provides a blank canvas on which to draw. This object
	just draws a white rectangle, specified by the argument of the SetRect() method.
	*/

	init->iView = new CViewWindow();

	/* Force the app to Portrait mode. */
	CAknAppUi::SetOrientationL(CAknAppUi::EAppUiOrientationPortrait);

	init->iView->SetMopParent(this);
	init->iView->ConstructL();
	init->bIsWindowed = false;

	TSize UISize = init->iView->Size();

	m_bInForeground = true;
	m_bClose = false;

	init->m_pShell->PVRShellSet(prefWidth, UISize.iWidth);
	init->m_pShell->PVRShellSet(prefHeight, UISize.iHeight);
	init->m_pShell->PVRShellSet(prefIsRotated, (UISize.iWidth > UISize.iHeight) ? 0 : 1);

	AddToStackL(init->iView);

	CActiveScheduler::Add(this);			// these 4 lines of code tell the OS to start executing the appUI's
	TRequestStatus * status = &iStatus;		// RunL() method when it has the time
	User::RequestComplete(status, 0);
	SetActive();
	CActiveScheduler::Start();

	Exit();
}

CPVRShellAppUi::~CPVRShellAppUi()
{
	RemoveFromStack(init->iView);
	init->OsDoReleaseAPI();

	delete init;
}

void CPVRShellAppUi::HandleCommandL(TInt aCommand){ //handle menu bar commands

	switch (aCommand)
	{
		case EAknSoftkeyOk:
			init->KeyPressed(PVRShellKeyNameSELECT);
		break;
	}
}

void CPVRShellAppUi::HandleWsEventL(const TWsEvent& aEvent, CCoeControl* aDestination)
{
	switch (aEvent.Type())
	{
		case KAknUidValueEndKeyCloseEvent:
			init->KeyPressed(PVRShellKeyNameQUIT);
			m_bClose = true;
		return;
		default:
			CAknAppUi::HandleWsEventL(aEvent, aDestination);
		break;
	}
}

TKeyResponse CPVRShellAppUi::HandleKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType) //handle key events
{
	switch(aKeyEvent.iCode)
	{
		case EKeyOK:
			init->KeyPressed(PVRShellKeyNameSELECT);
			return EKeyWasConsumed;
		case EKeyCBA1:
			init->KeyPressed(PVRShellKeyNameACTION1);
			return EKeyWasConsumed;
		case EKeyYes:
			init->KeyPressed(PVRShellKeyNameACTION2);
			return EKeyWasConsumed;
		case EKeyUpArrow:
			init->KeyPressed(init->m_eKeyMapUP);
			return EKeyWasConsumed;
		case EKeyDownArrow:
			init->KeyPressed(init->m_eKeyMapDOWN);
			return EKeyWasConsumed;
		case EKeyLeftArrow:
			init->KeyPressed(init->m_eKeyMapLEFT);
			return EKeyWasConsumed;
		case EKeyRightArrow:
			init->KeyPressed(init->m_eKeyMapRIGHT);
			return EKeyWasConsumed;
		case EKeyCBA2:
			init->KeyPressed(PVRShellKeyNameQUIT);
			m_bClose = true;
			return EKeyWasConsumed;
	}

	return EKeyWasNotConsumed;
}

void CPVRShellAppUi::RunL() //periodically executed function which runs the demo for a frame
{
	if (!IsActive())
	{
		++iFrame;

		//Demo initialisation or renderscene etc.
		if(m_bInForeground || m_bClose)
		{
			if(!init->Run())
			{
				CActiveScheduler::Stop(); //if the demo has signalled completion, stop the active schedular
				return;
			}

			// Signal request completion			  //signal to the OS that the RunL function has completed, and
			TRequestStatus * status = &iStatus;		  //tell it to run it again sometime:
			User::RequestComplete(status, 0);

			// Wait for request
			SetActive();
		}

		// To keep the background light on (only if we're not in power saving mode)
		if(!init->m_pShell->PVRShellGet(prefPowerSaving) && !(iFrame%100))
		{
		    User::ResetInactivityTime();
		}

		/*
			Suspend the current thread for a short while. Give some time
		       to other threads and AOs, avoids the ViewSrv error in ARMI and
		       THUMB release builds. One may try to decrease the callback
			function instead of this.
		*/
		if(!(iFrame%50))
		{
		      User::After(0);
    	}
	}

}

void CPVRShellAppUi::HandleResourceChangeL(TInt aType)
{
	CAknAppUi::HandleResourceChangeL(aType);

	if(aType == KEikDynamicLayoutVariantSwitch)
	{
		init->iView->SetExtentToWholeScreen();

		TSize UISize = init->iView->Size();
		init->m_pShell->PVRShellSet(prefWidth, UISize.iWidth);
		init->m_pShell->PVRShellSet(prefHeight,UISize.iHeight);

		init->m_pShell->PVRShellSet(prefIsRotated, (UISize.iWidth > UISize.iHeight) ? 0 : 1);
	}
}

void CPVRShellAppUi::HandleForegroundEventL(TBool aForeground)
{
	 // Call Base class method
	 CAknAppUi::HandleForegroundEventL(aForeground);

	if(aForeground)
	{
		m_bInForeground = true; 	  // We have gained the focus

		if(!IsActive())
		{
			// Signal request completion			  //signal to the OS that the RunL function has completed, and
			TRequestStatus * status = &iStatus;		  //tell it to run it again sometime:
			User::RequestComplete(status, 0);

			// Wait for request
			SetActive();
		}
	}
	else
		m_bInForeground = false; // We have lost the focus so don't render
}

/*******************************************************************************************************
 * View Window class                                                                                   *
 * =================                                                                                   *
 * Just provides a blank canvas to draw on top of. Movable in windowed mode.                           *
 *******************************************************************************************************/
void CViewWindow::ConstructL()
{
	CreateWindowL();
	SetExtentToWholeScreen();                // Take the whole screen into use

	iWin = Window();
	theWindow = &iWin;

	ActivateL();

	iWin.SetOrdinalPosition(0, ECoeWinPriorityAlwaysAtFront);

	SetPointerCapture(); // The current view fully controls the touchscreen device
	EnableDragEvents();  // Enable dragging (causes HandlePointerEventL to be called)

	m_bPointer = false;
}

void CViewWindow::HandleResourceChange(TInt aType)
{
	switch( aType )
    	{
	    case KEikDynamicLayoutVariantSwitch:
		    break;
	    }
}

TKeyResponse CViewWindow::OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType)
{
	return EKeyWasNotConsumed;
}

void CViewWindow::HandlePointerEventL (const TPointerEvent& aPointerEvent)
{
    switch(aPointerEvent.iType)
    {
		// Pen down
		case TPointerEvent::EButton1Down:
		case TPointerEvent::EDrag:
		{
			m_bPointer = true;

			TSize UISize = Size();
			m_vec2PointerLocation[0] = (float)aPointerEvent.iPosition.iX / (float) UISize.iWidth;
			m_vec2PointerLocation[1] = (float)aPointerEvent.iPosition.iY / (float) UISize.iHeight;
			break;
		}
		// Pen up
		case TPointerEvent::EButton1Up:
			m_bPointer = false;
			break;
	}

	CCoeControl::HandlePointerEventL(aPointerEvent);
}

void CViewWindow::SizeChanged()
{

}

void CViewWindow::InitWindow()  // informs EGL of the size and location of the window
{

}

/*!***************************************************************************
	Class: PVRShellInit
*****************************************************************************/

/*
	OS functionality
*/

void PVRShell::PVRShellOutputDebug(char const * const format, ...) const
{
	va_list arg;
	char	buf[1024];

	va_start(arg, format);
	vsprintf(buf, format, arg);
	va_end(arg);

	/* Passes the data to a platform dependant function */
	m_pShellInit->OsDisplayDebugString(buf);
}

/*!***********************************************************************
 @Function		OsInit
 @description	Initialisation for OS-specific code.
*************************************************************************/
void PVRShellInit::OsInit()
{
	/*
		Construct the binary path for GetReadPath() and GetWritePath()
	*/
	SetReadPath("..\\..\\System\\Data\\");
	SetWritePath("E:\\");
}

/*!***********************************************************************
 @Function		OsInitOS
 @description	Saves instance handle and creates main window
				In this function, we save the instance handle in a global variable and
				create and display the main program window.
*************************************************************************/
bool PVRShellInit::OsInitOS()
{
	// Initialise global strings
	m_pShell->m_pShellData->bFullScreen  = 1;

	return true;
}

/*!***********************************************************************
 @Function		OsReleaseOS
 @description	Destroys main window
*************************************************************************/
void PVRShellInit::OsReleaseOS()
{
}

/*!***********************************************************************
 @Function		OsExit
 @description	Destroys main window
*************************************************************************/
void PVRShellInit::OsExit()
{
	/*
		Show the exit message to the user
	*/

	const char* pExitMessage = (const char*) m_pShell->PVRShellGet(prefExitMessage);

	if(pExitMessage)
	{
		CAknGlobalNote* pDialog = CAknGlobalNote::NewLC();

		if(pDialog)
		{
			TPtrC8 pPtr((TUint8 *)(pExitMessage));

			HBufC* pEM = HBufC::NewLC(strlen(pExitMessage));

			if(pEM)
			{
				pEM->Des().Copy(pPtr);

				TInt i32NoteID = pDialog->ShowNoteL(EAknGlobalInformationNote, pEM->Des());

				User::After(5000000);

				pDialog->CancelNoteL(i32NoteID);

				CleanupStack::PopAndDestroy(pEM);
			}

			CleanupStack::PopAndDestroy(pDialog);
		}
	}
}

/*!***********************************************************************
 @Function		OsDoInitAPI
 @Return		true on success
 @description	Perform API initialisation and bring up window / fullscreen
*************************************************************************/
bool PVRShellInit::OsDoInitAPI()
{
	if(!ApiInitAPI())
	{
		OsDisplayDebugString("OsDoInitAPI ApiInitAPI() FAILED\n");
		return false;
	}

	if(bIsWindowed)
	{
		iView->InitWindow();
	}

	/* No problem occured */
	return true;
}

/*!***********************************************************************
 @Function		OsDoReleaseAPI
 @description	Clean up after we're done
*************************************************************************/
void PVRShellInit::OsDoReleaseAPI()
{
	ApiReleaseAPI();
}

/*!***********************************************************************
 @Function		OsRenderComplete
 @Returns		false when the app should quit
 @description	Main message loop / render loop
*************************************************************************/
void PVRShellInit::OsRenderComplete()
{
}

/*!***********************************************************************
 @Function		OsPixmapCopy
 @Return		true if the copy succeeded
 @description	When using pixmaps, copy the render to the display
*************************************************************************/
bool PVRShellInit::OsPixmapCopy()
{
	return false;
}

/*!***********************************************************************
 @Function		OsGetNativeDisplayType
 @Return		The 'NativeDisplayType' for EGL
 @description	Called from InitAPI() to get the NativeDisplayType
*************************************************************************/
void *PVRShellInit::OsGetNativeDisplayType()
{
	return 0;
}

/*!***********************************************************************
 @Function		OsGetNativePixmapType
 @Return		The 'NativePixmapType' for EGL
 @description	Called from InitAPI() to get the NativePixmapType
*************************************************************************/
void *PVRShellInit::OsGetNativePixmapType()
{
	return 0;
}

/*!***********************************************************************
 @Function		OsGetNativeWindowType
 @Return		The 'NativeWindowType' for EGL
 @description	Called from InitAPI() to get the NativeWindowType
*************************************************************************/
void *PVRShellInit::OsGetNativeWindowType()
{
	return iView->theWindow;
}

/*!***********************************************************************
 @Function		OsGet
 @Input			prefName	Name of value to get
 @Modified		pn A pointer set to the value asked for
 @Returns		true on success
 @Description	Retrieves OS-specific data
*************************************************************************/
bool PVRShellInit::OsGet(const prefNameIntEnum prefName, int *pn)
{
	return false;
}

/*!***********************************************************************
 @Function		OsGet
 @Input			prefName	Name of value to get
 @Modified		pp A pointer set to the value asked for
 @Returns		true on success
 @Description	Retrieves OS-specific data
*************************************************************************/
bool PVRShellInit::OsGet(const prefNamePtrEnum prefName, void **pp)
{
	switch(prefName)
	{
		case prefPointerLocation:
		{
			if (iView->m_bPointer)
			{
				*pp = iView->m_vec2PointerLocation;
				return true;
			}
			*pp = NULL;
			return false;
		}
		default:
			return false;
	}
}

/*!***********************************************************************
 @Function		OsSet
 @Input			prefName				Name of preference to set to value
 @Input			value					Value
 @Return		true for success
 @Description	Sets OS-specific data
*************************************************************************/
bool PVRShellInit::OsSet(const prefNameBoolEnum prefName, const bool value)
{
	return false;
}

/*!***********************************************************************
 @Function		OsSet
 @Input			prefName	Name of value to set
 @Input			i32Value 	The value to set our named value to
 @Returns		true on success
 @Description	Sets OS-specific data
*************************************************************************/
bool PVRShellInit::OsSet(const prefNameIntEnum prefName, const int i32Value)
{
	return false;
}

/*!***********************************************************************
 @Function		OsDisplayDebugString
 @Input			str		string to output
 @Description	Prints a debug string
*************************************************************************/
void PVRShellInit::OsDisplayDebugString(char const * const str)
{
	RDebug::Printf(str);

	WriteToLog("OsDisplayDebugString: ", "a");
	WriteToLog(str, "a");
	WriteToLog("\r\n", "a");
}

/*!***********************************************************************
 @Function		OsGetTime
 @Return		Time in milliseconds since the beginning of the application
 @Description	Gets the time in milliseconds since the beginning of the application
*************************************************************************/
unsigned long PVRShellInit::OsGetTime()
{
	TTime timeNow;

	timeNow.UniversalTime();

	return (static_cast<unsigned long>(timeNow.Int64())/1000);
}

/*****************************************************************************
 End of file (PVRShellOS.cpp)
*****************************************************************************/

