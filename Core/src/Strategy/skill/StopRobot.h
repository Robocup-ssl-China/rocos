#ifndef _STOP_ROBOT_V2_H
#define _STOP_ROBOT_V2_H
#include "skill_registry.h"
class CStopRobotV2 : public CPlayerTask{
public:
	CStopRobotV2();
	virtual CPlayerCommand* execute(const CVisionModule* pVision);
protected:
	virtual void toStream(std::ostream& os) const { os << "Stopping"; }
};
REGISTER_SKILL(Stop, CStopRobotV2);
#endif // _STOP_ROBOT_V2_H
