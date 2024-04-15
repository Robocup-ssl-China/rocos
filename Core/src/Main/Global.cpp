#include "Global.h"

CVisionModule*   vision;
CKickStatus*    kickStatus;
CDribbleStatus* dribbleStatus;
CGDebugEngine*  debugEngine;
CWorldModel* world;
CSkillAPI* skillapi;
CBallSpeedModel *ballModel;
void initializeSingleton()
{
	vision        = VisionModule::Instance();
	kickStatus    = KickStatus::Instance();
	dribbleStatus = DribbleStatus::Instance();
    debugEngine   = GDebugEngine::Instance();
    world         = WorldModel::Instance();
	skillapi	  = SkillAPI::Instance();
	ballModel = BallSpeedModel::Instance();
}
