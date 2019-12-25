#ifndef _SMART_GOTO_POSITION_V1_H_
#define _SMART_GOTO_POSITION_V1_H_
#include <skill/PlayerTask.h>
#include <WorldDefine.h>
#include <VisionModule.h>
#include "OtherLibs/cmu/obstacle.h"

struct PlayerCapabilityT;

/************************************************************************/
/*                      SmartGotoPosition                               */
/************************************************************************/
class CSmartGotoPositionV2 : public CPlayerTask{
public:
	CSmartGotoPositionV2();
	virtual void plan(const CVisionModule* pVision);
protected:
	virtual void toStream(std::ostream& os) const;

// ![22/5/2010 cliffyin: do trajectory check for obstacle_avoidance]
private:
	bool current_trajectory_safe;
	//void doTrajCheck(const CVisionModule* pVision,int playerNum,const PlayerPoseT& middlePose,PlayerPoseT& nextStep);
    void validateFinalTarget(CGeoPoint& finalTarget,CGeoPoint myPos, double avoidLength, bool isGoalie, bool isStop, obstacles& obs, bool planInCircle, CGeoPoint circleCenter, double circleRadius);
    void validateStartPoint(CGeoPoint& startPoint, double avoidLength, bool isGoalie, obstacles& obs);

    CGeoPoint finalPointBallPlacement(const CGeoPoint& startPoint, const CGeoPoint& target, const CGeoPoint& segPoint1, const CGeoPoint& segPoint2, const double avoidLength, bool& addObs);
    CVector validateFinalVel(const bool isGoalie, const CGeoPoint& startPos,const CGeoPoint &targetPos, const CVector &targetVel);
    PlayerCapabilityT setCapability(const CVisionModule* pVision);
    double getMiddleVel(const CVisionModule* pVision, CGeoPoint middleProj);
    CGeoPoint dealPlanFail(CGeoPoint startPoint, CGeoPoint nextPoint, double avoidLength);
	int playerFlag;
    PlayerCapabilityT _capability;
	PlayerPoseT nextStep;
    int _lastCycle;
};

#endif
