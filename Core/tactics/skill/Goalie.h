#pragma once
#include "skill_registry.h"

class CGoalie : public CPlayerTask{
public:
    CGoalie();
    virtual void plan(const CVisionModule* pVision);
    virtual void toStream(std::ostream& os) const { os << "Goalie"; }
private:
    int _lastCycle;
    enum STATE{
        NOTHING = 0,
        STAND,
        CLEAR,
        SAVE
    };
    STATE _state;
    std::string debug_state;
};

REGISTER_SKILL(Goalie, CGoalie);
