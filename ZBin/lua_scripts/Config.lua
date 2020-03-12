IS_TEST_MODE = true
IS_SIMULATION = CGetIsSimulation()
USE_SWITCH = false
USE_AUTO_REFEREE = false
OPPONENT_NAME = "Other"
SAO_ACTION = CGetSettings("Alert/SaoAction","Int")
IS_YELLOW = CGetSettings("ZAlert/IsYellow","Bool")
IS_RIGHT = CGetSettings("ZAlert/IsRight", "Bool")
DEBUG_MATCH = CGetSettings("Debug/RoleMatch","Bool")

gStateFileNameString = string.format(os.date("%Y%m%d%H%M"))

gTestPlay = "TestDribbleAndKick"

gRoleFixNum = {
        ["Kicker"]   = {1},
        ["Goalie"]   = {0},
        ["Tier"]     = {},
}

-- 用来进行定位球的保持
-- 在考虑智能性时用table来进行配置，用于OurIndirectKick
gOurIndirectTable = {
        -- 在OurIndirectKick控制脚本中可以进行改变的值
        -- 上一次定位球的Cycle
        lastRefCycle = 0
}

gSkill = {
        "SmartGoto",
        "SimpleGoto",
        "RunMultiPos",
        "Stop",
        "OpenSpeed",
        "Speed",
        "GotoMatchPos",
        "GoCmuRush",
        "NoneZeroRush",
        "SpeedInRobot"
}

gRefPlayTable = {
        "Ref/Ref_HaltV1",
        "Ref/Ref_OurTimeoutV1",
        "Ref/GameStop/Ref_StopV1",
-- BallPlacement
        -- "Ref/BallPlacement/Ref_BallPlace2Stop",
-- Penalty
        "Ref/PenaltyDef/Ref_PenaltyDefV1",
        "Ref/PenaltyKick/Ref_PenaltyKickV1",
-- KickOff
        "Ref/KickOffDef/Ref_KickOffDefV1",
        "Ref/KickOff/Ref_KickOffV1",
-- FreeKickDef
        "Ref/CornerDef/Ref_CornerDefV1",
        "Ref/FrontDef/Ref_FrontDefV1",
        "Ref/MiddleDef/Ref_MiddleDefV1",
        "Ref/BackDef/Ref_BackDefV1",
-- FreeKick
        "Ref/CornerKick/Ref_CornerKickV1",
        "Ref/CenterKick/Ref_CenterKickV1",
        "Ref/FrontKick/Ref_FrontKickV1",
        "Ref/MiddleKick/Ref_MiddleKickV1",
        "Ref/BackKick/Ref_BackKickV1",
}

gBayesPlayTable = {
        "Nor/NormalPlayV1",
}

gTestPlayTable = {
        "Test/TestRun",
        "Test/TestDribbleAndKick",
        "Test/RunMilitaryBoxing",
}
