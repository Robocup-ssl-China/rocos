#pragma once
#include "skill_registry.h"

class CTouch : public CPlayerTask{
public:
    CTouch();
    virtual void plan(const CVisionModule* pVision);
    virtual void toStream(std::ostream& os) const { os << "Touch"; }
private:
    int _lastCycle;
};

REGISTER_SKILL(Touch, CTouch);
