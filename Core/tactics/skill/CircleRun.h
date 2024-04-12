#pragma once
#include "skill_registry.h"

class CCircleRun : public CPlayerTask{
public:
    CCircleRun();
    virtual void plan(const CVisionModule* pVision);
	virtual CPlayerCommand* execute(const CVisionModule* pVision);
    virtual void toStream(std::ostream& os) const { os << "CircleRun"; }
private:
    int _lastCycle;
};

REGISTER_SKILL(CircleRun, CCircleRun);
