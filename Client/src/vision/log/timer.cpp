#include "timer.h"

#if _POSIX_TIMERS > 0
    #define WITH_POSIX_TIMERS
#endif

#ifdef WITH_POSIX_TIMERS
    #include <time.h>
#else
    #include <time.h>
    #ifdef Q_OS_WIN
        #include <windows.h>
    #endif // Q_OS_WIN
#endif

/*!
 * \class Timer
 * \brief High precision timer
 */

/*!
 * \brief Creates a new timer object
 */
Timer::Timer() :
    m_scaling(1.0)
{
    m_start = systemTime();
}

/*!
 * \brief Sets time scaling. Time is guaranteed to be continous
 * \param scaling New scaling factor
 */
void Timer::setScaling(double scaling)
{
    Q_ASSERT(scaling >= 0);
    // scaling is relative to start time, update start to prevent a jump
    m_start = currentTime();
    m_scaling = scaling;
}

/*!
 * \brief Reset timer to current time and reset Scaling
 */
void Timer::reset()
{
    m_scaling = 1.0;
    m_start = systemTime();
}

/*!
 * \brief Query internal time
 * \return The internal time in nanoseconds
 */
qint64 Timer::currentTime() const
{
    const qint64 sys = systemTime();
    return m_start + (sys - m_start) * m_scaling;
}

/*!
 * \brief Query system time
 * \return The current system time in nanoseconds
 */
qint64 Timer::systemTime()
{
#ifdef WITH_POSIX_TIMERS
    timespec ts;
    ts.tv_sec = 0;
    ts.tv_nsec = 0;
    if (clock_gettime(CLOCK_REALTIME, &ts) == -1)
        return 0;

    return qint64(ts.tv_sec) * 1000000000LL + qint64(ts.tv_nsec);
#else // WITH_POSIX_TIMERS
    #ifdef Q_OS_WIN
    static bool isInitialized = false;

    static quint64 timerFrequency;
    static quint64 startTick;
    static quint64 startTime;

    if (!isInitialized) {
        isInitialized = true;
        timerFrequency = 0; // !!! disable QueryPerformanceCounter for initialization
        startTime = Timer::systemTime(); // get time via gettimeofday

        // the timing provided by this code is far from optimal, but it should do
        // as the time deltas are what we're interested in.
        LARGE_INTEGER freq;
        // set timerFrequency to zero if QueryPerformanceCounter can't be used
        if (!QueryPerformanceFrequency(&freq)) {
            timerFrequency = 0;
        } else {
            timerFrequency = freq.QuadPart;
        }
        if (timerFrequency > 0) {
            LARGE_INTEGER ticks;
            if (QueryPerformanceCounter(&ticks)) {
                startTick = ticks.QuadPart;
            } else {
                timerFrequency = 0;
            }
        }
    }

    if (timerFrequency > 0) {
        LARGE_INTEGER ticks;
        if (QueryPerformanceCounter(&ticks)) {
            return (ticks.QuadPart - startTick) * 1000000000LL / timerFrequency + startTime;
        }
    }
    #endif // Q_OS_WIN

//    timeval tv;
//    if (gettimeofday(&tv, NULL) != 0)
//        return 0;

//    return qint64(tv.tv_sec) * 1000000000LL + qint64(tv.tv_usec) * 1000LL;
#endif // WITH_POSIX_TIMERS
}
