// Copyright (c) 2010, Razvan Petru
// All rights reserved.

// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:

// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice, this
//   list of conditions and the following disclaimer in the documentation and/or other
//   materials provided with the distribution.
// * The name of the contributors may not be used to endorse or promote products
//   derived from this software without specific prior written permission.

// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
// IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
// INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
// BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
// OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
// OF THE POSSIBILITY OF SUCH DAMAGE.

#include "genlog.h"
#include "genlogdest.h"
#include <QMutex>
#include <QList>
#include <QDateTime>
#include <QtGlobal>
#include <cassert>
#include <cstdlib>

namespace QLogging
    {
    typedef QList<Destination*> DestinationList;

    static const char TraceString[] = "TRACE";
    static const char DebugString[] = "DEBUG";
    static const char InfoString[] = "INFO";
    static const char WarnString[] = "WARN";
    static const char ErrorString[] = "ERROR";
    static const char FatalString[] = "FATAL";
    static const char ProfilingString[] = "PROFILING";

    // not using Qt::ISODate because we need the milliseconds too
    static const QString fmtDateTime( "yyyy-MM-ddThh:mm:ss.zzz" );

    static const char* LevelToText( Level theLevel )
        {
        switch ( theLevel )
            {
            case TraceLevel:
                return TraceString;
            case DebugLevel:
                return DebugString;
            case InfoLevel:
                return InfoString;
            case WarnLevel:
                return WarnString;
            case ErrorLevel:
                return ErrorString;
            case FatalLevel:
                return FatalString;
            case ProfilingLevel:
                return ProfilingString;
            default:
                {
                assert(!"bad log level");
                return InfoString;
                }
            }
        }

    class LoggerImpl
        {
    public:
        LoggerImpl() :
            level( InfoLevel )
            {

            }
        QMutex logMutex;
        Level level;
        DestinationList destList;
        };

    Logger::Logger() :
        d( new LoggerImpl )
        {
        }

    Logger::~Logger()
        {
        delete d;
        }

    void Logger::addDestination( Destination* destination )
        {
        assert(destination);
        d->destList.push_back( destination );
        }

    void Logger::setLoggingLevel( Level newLevel )
        {
        d->level = newLevel;
        }

    Level Logger::loggingLevel() const
        {
        return d->level;
        }

    //! creates the complete log message and passes it to the logger
    void Logger::Helper::writeToLog()
        {
        const char* const levelName = LevelToText( level );
        QString completeMessage;
        if ( ProfilingLevel != level )
            {
            completeMessage
                = QString( "%1 %2 %3\r\n" ) .arg( levelName, 5 ) .arg(
                    QDateTime::currentDateTime().toString( fmtDateTime ) ) .arg(
                    buffer );
            }
        else
            {
            completeMessage = QString( "%1\r\n" ).arg( buffer );
            }

        Logger& logger = Logger::instance();
        QMutexLocker lock( &logger.d->logMutex );
        logger.write( completeMessage );
        }

    Logger::Helper::~Helper()
        {
        try
            {
            writeToLog();
            }
        catch ( ... )
            {
            // you've thrown an exception from a sink, haven't you!?
            assert(!"exception in logger helper destructor");
            abort();
            }
        }

    //! sends the message to all the destinations
    void Logger::write( const QString& message )
        {
        for ( DestinationList::iterator it = d->destList.begin(), endIt =
            d->destList.end(); it != endIt; ++it )
            {
            assert(*it);
            ( *it )->write( message );
            }
        }

    } // end namespace
