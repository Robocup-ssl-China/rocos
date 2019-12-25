#ifndef TIMER_H
#define TIMER_H

#include <QtGlobal>

class Timer
{
public:
    Timer();

public:
    void setScaling(double scaling);
    void reset();
    qint64 currentTime() const;

public:
    static qint64 systemTime();

private:
    double m_scaling;
    qint64 m_start;
};

#endif // TIMER_H
