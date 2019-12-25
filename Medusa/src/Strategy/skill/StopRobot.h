#ifndef _STOP_ROBOT_V2_H
#define _STOP_ROBOT_V2_H
#include <skill/PlayerTask.h>
class CStopRobotV2 : public CPlayerTask{
public:
	CStopRobotV2();
	virtual CPlayerCommand* execute(const CVisionModule* pVision);
	virtual bool isEmpty() const { return false; }
protected:
	virtual void toStream(std::ostream& os) const { os << "Stopping"; }
};
#endif // _STOP_ROBOT_V2_H