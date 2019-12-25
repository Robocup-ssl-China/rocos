#ifndef _GOTO_POSITION_V2_H_
#define _GOTO_POSITION_V2_H_
#include <skill/PlayerTask.h>
#include <geometry.h>
/************************************************************************/
/*                     CGotoPositionV2 / 跑位                           */
/************************************************************************/

struct PlayerCapabilityT;

class CGotoPositionV2 : public CPlayerTask{
public:
	CGotoPositionV2();
	virtual void plan(const CVisionModule* pVision);
	virtual CPlayerCommand* execute(const CVisionModule* pVision);
	virtual bool isEmpty() const { return false; }
	const CGeoPoint& reTarget() const { return _target; }
protected:
	virtual void toStream(std::ostream& os) const;
    PlayerCapabilityT setCapability(const CVisionModule* pVision);
    CGeoPoint avoidPenaltyArea(const CVisionModule* pVision, const CGeoPoint& startPos, const CGeoPoint& targetPos, const double avoidLength, const int vecNumber);

private:
	CGeoPoint _target;
    int playerFlag;
};

#endif
