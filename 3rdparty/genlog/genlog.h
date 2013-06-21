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

#ifndef QLOG_H
#define QLOG_H

#include <QDebug>
#include <QString>

namespace QLogging
    {
    class Destination;
    enum Level
        {
        TraceLevel = 0x01,
        DebugLevel = 0x02,
        InfoLevel = 0x04,
        WarnLevel = 0x08,
        ErrorLevel = 0x10,
        FatalLevel = 0x20,
        ProfilingLevel = 0x40,
        LogALLLevel = 0xff
        };

    class LoggerImpl; // d pointer
    class Logger
        {
    public:
        static Logger& instance()
            {
            static Logger staticLog;
            return staticLog;
            }

        //! adds a log message destination
        void addDestination( Destination* destination );
        //! logging at a level < 'newLevel' will be ignored
        void setLoggingLevel( Level newLevel );
        //! the default level is INFO
        Level loggingLevel() const;

        //! the helper forwards the streaming to QDebug and builds the final
        //! log message
        class Helper
            {
        public:
            explicit Helper( Level logLevel ) :
                level( logLevel ), qtDebug( &buffer )
                {
                }
            ~Helper();
            QDebug& stream()
                {
                return qtDebug;
                }

        private:
            void writeToLog();

            Level level;
            QString buffer;
            QDebug qtDebug;
            };

    private:
        Logger();
        Logger( const Logger& );
        Logger& operator=( const Logger& );
        ~Logger();

        void write( const QString& message );

        LoggerImpl* d;
        };

    } // end namespace

//! Logging macros: define QS_LOG_LINE_NUMBERS to get the file and line number
//! in the log output.
#ifdef Q_LOGGING
#ifndef Q_LOG_LINE_NUMBERS
#define QLOG_TRACE(message) \
      if( QLogging::Logger::instance().loggingLevel() & QLogging::TraceLevel ) \
      QLogging::Logger::Helper(QLogging::TraceLevel).stream() << message
#define QLOG_DEBUG(message) \
      if( QLogging::Logger::instance().loggingLevel() & QLogging::DebugLevel ) \
      QLogging::Logger::Helper(QLogging::DebugLevel).stream() << message
#define QLOG_INFO(message)  \
      if( QLogging::Logger::instance().loggingLevel() & QLogging::InfoLevel ) \
      QLogging::Logger::Helper(QLogging::InfoLevel).stream() << message
#define QLOG_WARN(message)  \
      if( QLogging::Logger::instance().loggingLevel() & QLogging::WarnLevel ) \
      QLogging::Logger::Helper(QLogging::WarnLevel).stream() << message
#define QLOG_ERROR(message) \
      if( QLogging::Logger::instance().loggingLevel() & QLogging::ErrorLevel ) \
      QLogging::Logger::Helper(QLogging::ErrorLevel).stream() << message
#define QLOG_FATAL(message) QLogging::Logger::Helper(QLogging::FatalLevel).stream() << message
#else // Q_LOG_LINE_NUMBERS
#define QLOG_TRACE(message) \
   if( QLogging::Logger::instance().loggingLevel() & QLogging::TraceLevel ) \
      QLogging::Logger::Helper(QLogging::TraceLevel).stream() << __FILE__ << '@' << __LINE__ << message
#define QLOG_DEBUG(message) \
      if( QLogging::Logger::instance().loggingLevel() & QLogging::DebugLevel ) \
      QLogging::Logger::Helper(QLogging::DebugLevel).stream() << __FILE__ << '@' << __LINE__ << message
#define QLOG_INFO(message)  \
      if( QLogging::Logger::instance().loggingLevel() & QLogging::InfoLevel ) \
      QLogging::Logger::Helper(QLogging::InfoLevel).stream() << __FILE__ << '@' << __LINE__ << message
#define QLOG_WARN(message)  \
      if( QLogging::Logger::instance().loggingLevel() & QLogging::WarnLevel ) \
      QLogging::Logger::Helper(QLogging::WarnLevel).stream() << __FILE__ << '@' << __LINE__ << message
#define QLOG_ERROR(message) \
      if( QLogging::Logger::instance().loggingLevel() & QLogging::ErrorLevel ) \
      QLogging::Logger::Helper(QLogging::ErrorLevel).stream() << __FILE__ << '@' << __LINE__ << message
#define QLOG_FATAL(message) QLogging::Logger::Helper(QLogging::FatalLevel).stream() << __FILE__ << '@' << __LINE__ << message
#endif
#else // Q_LOGGING
#define QLOG_TRACE(message)
#define QLOG_DEBUG(message)
#define QLOG_INFO(message)
#define QLOG_WARN(message)
#define QLOG_ERROR(message)
#define QLOG_FATAL(message)
#endif // QT_DEBUG
#ifdef Q_LOGGING_PROFILE
#define QLOG_PROFILING(message) \
    if( QLogging::Logger::instance().loggingLevel() & QLogging::ProfilingLevel ) \
    QLogging::Logger::Helper(QLogging::ProfilingLevel).stream() << message
#else // Q_LOGGING_PROFILE
#define QLOG_PROFILING(message)
#endif // Q_LOGGING_PROFILE
#endif // QLOG_H
