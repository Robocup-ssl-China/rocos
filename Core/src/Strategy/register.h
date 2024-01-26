#include "skill/Goalie.h"
#include "skill/GotoPosition.h"
#include "skill/OpenSpeed.h"
#include "skill/SmartGotoPosition.h"
#include "skill/Speed.h"
#include "skill/StopRobot.h"
#include "skill/Touch.h"

REGISTER_SKILL(Goalie, CGoalie);
REGISTER_SKILL(Goto, CGotoPositionV2);
REGISTER_SKILL(OpenSpeed, COpenSpeed);
REGISTER_SKILL(SmartGoto, CSmartGotoPositionV2);
REGISTER_SKILL(Speed, CSpeed);
REGISTER_SKILL(Stop, CStopRobotV2);
REGISTER_SKILL(Touch, CTouch);
