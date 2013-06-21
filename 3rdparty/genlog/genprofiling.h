#ifndef QPROFILING_H_
#define QPROFILING_H_

#include "genlog.h"
#include <time.h>

#define QPROFILING_MAXSTACKSIZE 32

#if defined(Q_WS_MAEMO_5)

namespace QLogging
    {

    class ProfilerStack
        {
        friend class Profiler;
    public:
        static ProfilerStack& instance()
            {
            static ProfilerStack staticProfiler;
            return staticProfiler;
            }
    private:
        ProfilerStack()
            {
            memset(stackDelta, 0, QPROFILING_MAXSTACKSIZE * sizeof(timespec));
            }
        ProfilerStack(const ProfilerStack&);
        ProfilerStack& operator=(const ProfilerStack&);
        ~ProfilerStack()
            {
            }
    private:
        timespec stackDelta[QPROFILING_MAXSTACKSIZE];
        int stackLevel;
        };

    class Profiler
        {
    public:
        Profiler(ProfilerStack& profilerStack, QString location, timespec& microCount) :
            stack(profilerStack), profLocation(location),
                    profilerStopped(false)
            {
            ++stack.stackLevel;
            if (stack.stackLevel == QPROFILING_MAXSTACKSIZE)
                qCritical("Max stack profiling level reached");
            clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &microStart);
            microDelta.tv_sec = microStart.tv_sec - microCount.tv_sec;
            microDelta.tv_nsec = microStart.tv_nsec - microCount.tv_nsec;
            }
        ~Profiler()
            {
            end();
            }
        void end()
            {
            if (!profilerStopped)
                {
                timespec microEnd;
                clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &microEnd);
                profilerStopped = true;
//                long interval = 1000000 * (microEnd.tv_sec - microStart.tv_sec - stack.stackDelta[stack.stackLevel].tv_sec) +
//                                (microEnd.tv_nsec - microStart.tv_nsec - stack.stackDelta[stack.stackLevel].tv_nsec)/1000;
                QLOG_PROFILING(QString("<profile>\r\n\t<where>%1</where>\r\n\t<time>%2</time>\r\n</profile>").arg(profLocation).arg(interval).toAscii().constData());
                stack.stackDelta[stack.stackLevel].tv_sec = 0;
                stack.stackDelta[stack.stackLevel].tv_nsec = 0;
                timespec ts;
                clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts);
                stack.stackDelta[--stack.stackLevel].tv_sec += microDelta.tv_sec
                        + ts.tv_sec - microEnd.tv_sec;
                stack.stackDelta[--stack.stackLevel].tv_nsec += microDelta.tv_nsec
                        + ts.tv_nsec - microEnd.tv_nsec;
                if (stack.stackLevel <= 0)
                    {
                    stack.stackDelta[0].tv_sec = 0;
                    stack.stackDelta[0].tv_nsec = 0;
                    stack.stackLevel = 0;
                    }
                }
            }
    private:
        ProfilerStack& stack;
        QString profLocation;
        bool profilerStopped;
        timespec microDelta;
        timespec microStart;
        };

    } // namespace QLogging
#ifdef Q_LOGGING_PROFILE
#define QPROFILING_START(message) \
    timespec ts; \
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts); \
    QLogging::Profiler _profiler_(QLogging::ProfilerStack::instance(), message, ts)

#define QPROFILING_END() \
    _profiler_.end()
#else // Q_LOGGING_PROFILE
#define QPROFILING_START(message)
#define QPROFILING_END()
#endif // Q_LOGGING_PROFILE
#else // Q_WS_MAEMO_5

namespace QLogging
    {

    int getMicroSecondsCount();

    class ProfilerStack
        {
        friend class Profiler;
    public:
        static ProfilerStack& instance()
            {
            static ProfilerStack staticProfiler;
            return staticProfiler;
            }
    private:
        ProfilerStack()
            {
            memset(stackDelta, 0, QPROFILING_MAXSTACKSIZE * sizeof(int));
            }
        ProfilerStack(const ProfilerStack&);
        ProfilerStack& operator=(const ProfilerStack&);
        ~ProfilerStack()
            {
            }
    private:
        int stackDelta[QPROFILING_MAXSTACKSIZE];
        int stackLevel;
        };

    class Profiler
        {
    public:
        Profiler(ProfilerStack& profilerStack, QString location, int microCount) :
            stack(profilerStack), profLocation(location),
                    profilerStopped(false)
            {
            ++stack.stackLevel;
            if (stack.stackLevel == QPROFILING_MAXSTACKSIZE)
                qCritical("Max stacked profiling level reached");
            microStart = getMicroSecondsCount();
            microDelta = microStart - microCount;
            }
        ~Profiler()
            {
            end();
            }
        void end()
            {
            if (!profilerStopped)
                {
                int microEnd = getMicroSecondsCount();
                profilerStopped = true;
                QLOG_PROFILING(QString("<profile>\r\n\t<where>%1</where>\r\n\t<time>%2</time>\r\n</profile>").arg(profLocation).arg(microEnd - microStart - stack.stackDelta[stack.stackLevel]).toAscii().constData());
                stack.stackDelta[stack.stackLevel] = 0;
                stack.stackDelta[--stack.stackLevel] += microDelta
                        + getMicroSecondsCount() - microEnd;
                if (stack.stackLevel <= 0)
                    {
                    stack.stackDelta[0] = 0;
                    stack.stackLevel = 0;
                    }
                }
            }
    private:
        ProfilerStack& stack;
        QString profLocation;
        bool profilerStopped;
        int microDelta;
        int microStart;
        };

    } // namespace QLogging
#ifdef Q_LOGGING_PROFILE
#define QPROFILING_START(message) \
    QLogging::Profiler _profiler_(QLogging::ProfilerStack::instance(), message, QLogging::getMicroSecondsCount())

#define QPROFILING_END() \
    _profiler_.end()
#else // Q_LOGGING_PROFILE
#define QPROFILING_START(message)
#define QPROFILING_END()
#endif // Q_LOGGING_PROFILE
#endif // Q_WS_MAEMO_5

#endif /* QPROFILING_H_ */
