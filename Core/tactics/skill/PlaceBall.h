#pragma once
#include "skill_registry.h"

class CPlaceBall : public Skill{
public:
    CPlaceBall();
    virtual void plan(const CVisionModule* pVision);
    virtual void toStream(std::ostream& os) const { os << "PlaceBall"; }
private:
    int _lastCycle;
};

REGISTER_SKILL(PlaceBall, CPlaceBall);
