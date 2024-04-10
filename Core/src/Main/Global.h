#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include "VisionModule.h"
#include "DribbleStatus.h"
#include "KickStatus.h"
#include "GDebugEngine.h"
#include "singleton.hpp"
#include "WorldModel.h"
#include "skillapi.h"
#include "BallSpeedModel.h"

extern CVisionModule*  vision;
extern CKickStatus*    kickStatus;
extern CDribbleStatus* dribbleStatus;
extern CGDebugEngine*  debugEngine;
extern CWorldModel* world;
extern CSkillAPI* skillapi;
extern CBallSpeedModel* ballModel;
void initializeSingleton();
#endif
