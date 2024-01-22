#include "skill/Goalie.h"
#include "skill/GotoPosition.h"
#include "skill/OpenSpeed.h"
#include "skill/SmartGotoPosition.h"
#include "skill/Speed.h"
#include "skill/StopRobot.h"
#include "skill/Touch.h"

REGISTER_SKILL(Goalie, create<CGoalie>);
REGISTER_SKILL(Goto, create<CGotoPositionV2>);
REGISTER_SKILL(OpenSpeed, create<COpenSpeed>);
REGISTER_SKILL(SmartGoto, create<CSmartGotoPositionV2>);
REGISTER_SKILL(Speed, create<CSpeed>);
REGISTER_SKILL(Stop, create<CStopRobotV2>);
REGISTER_SKILL(Touch, create<CTouch>);

bool runSkill(const std::string& name, const TaskT& task){
    auto pSkill = SkillRegistry<Skill>::create(name);
}