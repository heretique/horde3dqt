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
// ************************************************************************************************

#include <QtGui/QApplication>
#include <QPaintEngine>
#include "genlog.h"
#include "genlogdest.h"
#include <QCoreApplication>
#include <QFileInfo>
#include "app.h"

#ifdef Q_OS_SYMBIAN
    #include <eikenv.h>
    #include <eikappui.h>
    #include <aknenv.h>
    #include <aknappui.h>
#endif


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);


#if defined(Q_LOGGING) || defined(Q_LOGGING_PROFILE)
    QLogging::Logger& logger = QLogging::Logger::instance();
    logger.setLoggingLevel(QLogging::LogALLLevel);
    const QString logPath("E:\\logs\\" + QFileInfo(
            QCoreApplication::applicationFilePath()).baseName() + ".log");
    QLogging::DestinationPtr fileDestination(
            QLogging::DestinationFactory::MakeFileDestination(logPath));
    QLogging::DestinationPtr debugDestination(
            QLogging::DestinationFactory::MakeDebugOutputDestination());
    logger.addDestination(debugDestination.get());
    logger.addDestination(fileDestination.get());
#endif

#ifdef Q_OS_SYMBIAN
    // Lock orientation to landscape
    CAknAppUi* appUi = dynamic_cast<CAknAppUi*>(CEikonEnv::Static()->AppUi());
    TRAPD(error,
        if (appUi) {
            appUi->SetOrientationL(CAknAppUi::EAppUiOrientationLandscape);
        }
    );
#endif

GE::GameWindow *theGameWindow = new MyGameWindow("");
    theGameWindow->create();
    theGameWindow->setWindowState(Qt::WindowNoState);

    qApp->installEventFilter(theGameWindow);

#ifdef Q_OS_WIN32
    theGameWindow->showMaximized();
#else
    theGameWindow->showFullScreen();
#endif

    int result = a.exec();
    theGameWindow->destroy();
    delete theGameWindow;
    return result;
}
