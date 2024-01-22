#ifndef CTOUCH_H
#define CTOUCH_H

#include <PlayerTask.h>

class CTouch : public CPlayerTask{
public:
    CTouch();
    virtual void plan(const CVisionModule* pVision);
    virtual bool isEmpty() const { return false; }
    virtual void toStream(std::ostream& os) const { os << "Touch"; }
private:
    int _lastCycle;
};
#endif // CTOUCH_H
