#pragma once
#include "skill_registry.h"

class CPlaceBall : public CPlayerTask{
public:
    CPlaceBall();
    virtual void plan(const CVisionModule* pVision);
    virtual void toStream(std::ostream& os) const { os << "PlaceBall"; }
private:
    int _lastCycle;
};

REGISTER_SKILL(PlaceBall, CPlaceBall);
