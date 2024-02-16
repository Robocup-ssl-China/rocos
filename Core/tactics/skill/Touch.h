#ifndef CTOUCH_H
#define CTOUCH_H
#include "registry.h"

class CTouch : public CPlayerTask{
public:
    CTouch();
    virtual void plan(const CVisionModule* pVision);
    virtual bool isEmpty() const { return false; }
    virtual void toStream(std::ostream& os) const { os << "Touch"; }
private:
    int _lastCycle;
};
REGISTER_SKILL(Touch, CTouch);
#endif // CTOUCH_H
