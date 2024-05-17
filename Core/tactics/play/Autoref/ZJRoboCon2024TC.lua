local TEST_TIME = 60
local CYCLE_COUNTDOWN = TEST_TIME * param.frameRate

local ORIGIN = CGeoPoint:new_local(0,0)
local CIRCLE_DIST = 1500
local CIRCLE_RADIUS = 600
local PLAYER_RADIUS = 95
local BALL_RADIUS = 25

local CIRCLE_COUNT = 3
local CIRCLE_POS = {
    ORIGIN + Utils.Polar2Vector(CIRCLE_DIST, 0*math.pi*2/3 + math.pi),
    ORIGIN + Utils.Polar2Vector(CIRCLE_DIST, 1*math.pi*2/3 + math.pi),
    ORIGIN + Utils.Polar2Vector(CIRCLE_DIST, 2*math.pi*2/3 + math.pi),
}
local CIRCLE_STATE_EMPTY = param.RED
local CIRCLE_STATE_WAIT = param.BLUE
local STATE_STR = {
    [param.RED] = "Empty",
    [param.BLUE] = "Wait",
}
local CIRCLE_STATE = {
    CIRCLE_STATE_EMPTY,
    CIRCLE_STATE_EMPTY,
    CIRCLE_STATE_EMPTY,
}

local CIRCLE_BALL_EMPTY = param.GRAY
local CIRCLE_BALL_HOLDING = param.GREEN
local CIRCLE_BALL_RECEIVING = param.CYAN
local BALL_STR = {
    [param.GRAY] = "NULL",
    [param.GREEN] = "Holding",
    [param.CYAN] = "Receiving",
}
local CIRCLE_BALL_STATE = {
    CIRCLE_BALL_EMPTY,
    CIRCLE_BALL_EMPTY,
    CIRCLE_BALL_EMPTY,
}
local PLAYER_NUM_IN_CIRCLE = {
    -1,-1,-1
}
local DEFENDER_NUM = -1
local DEFENDER_CATCH_BALL = false

local msg = ""

local checkInCircle = function(pos, circleNum, radius)
    radius = radius or 0
    return (pos - CIRCLE_POS[circleNum]):mod() < CIRCLE_RADIUS - radius
end
local checkEnemyRobotInCircle = function(num, circleNum)
    if not enemy.valid(num) then
        return false
    end
    return checkInCircle(enemy.rawPos(num), circleNum, 0)
end

local WAIT_START = true
local reset = function()
    DEFENDER_NUM = -1
    DEFENDER_CATCH_BALL = false
    WAIT_START = true
    for i, pos in ipairs(CIRCLE_POS) do
        CIRCLE_STATE[i] = CIRCLE_STATE_EMPTY
        CIRCLE_BALL_STATE[i] = CIRCLE_BALL_EMPTY
        PLAYER_NUM_IN_CIRCLE[i] = -1
    end
end

local wait = function()
    -- reset()
    for i=0,param.maxPlayer-1 do
        for j=1, CIRCLE_COUNT do
            if checkEnemyRobotInCircle(i, j) then
                PLAYER_NUM_IN_CIRCLE[j] = i
                CIRCLE_STATE[j] = CIRCLE_STATE_WAIT
            end
        end
    end
    local ballPos = ball.rawPos()
    for i=1, CIRCLE_COUNT do
        CIRCLE_BALL_STATE[i] = checkInCircle(ballPos, i) and CIRCLE_BALL_HOLDING or CIRCLE_BALL_EMPTY
    end
    for i=0,param.maxPlayer-1 do
        if player.valid(i) then
            DEFENDER_NUM = i
            break
        end
    end
end
local waitCheck = function()
    if DEFENDER_NUM == -1 then
        msg = "No defender"
        return false
    end
    for i=1, CIRCLE_COUNT do
        if PLAYER_NUM_IN_CIRCLE[i] == -1 then
            msg = string.format("Circle %d is empty", i)
            return false
        end
    end
    for i, pos in ipairs(CIRCLE_POS) do
        if CIRCLE_BALL_STATE[i] == CIRCLE_BALL_HOLDING then
            msg = string.format("Circle %d is holding the ball", i)
            return true
        end
    end
end

local TOUCH_TEAM = false
local TOUCH_NUM = -1
local TOUCH_NEW_UPDATE = false
local updateLastTouch = function()
    -- local tempNum = vision:getBallLastTouch()
    -- local robotNum = math.mod(vision:getBallLastTouch(),param.maxPlayer)
    -- local team = (vision:getBallLastTouch() >= param.maxPlayer) == IS_YELLOW
    local team = false
    local robotNum = -1
    for i=0,param.maxPlayer-1 do
        if enemy.valid(i) then
            if (enemy.rawPos(i) - ball.rawPos()):mod() < PLAYER_RADIUS + BALL_RADIUS then
                team = false
                robotNum = i
                break
            end
        end
    end
    for i=0,param.maxPlayer-1 do
        if player.valid(i) then
            if (player.rawPos(i) - ball.rawPos()):mod() < PLAYER_RADIUS + BALL_RADIUS then
                team = true
                robotNum = i
                break
            end
        end
    end

    if robotNum == -1 or TOUCH_TEAM == team and TOUCH_NUM == robotNum then
        TOUCH_NEW_UPDATE = false
    else
        TOUCH_NEW_UPDATE = true
        TOUCH_TEAM = team
        TOUCH_NUM = robotNum
    end
    if team and robotNum == DEFENDER_NUM and TOUCH_NEW_UPDATE then
        DEFENDER_CATCH_BALL = true
    end
    return team, robotNum
end
local getLastTouch = function()
    return TOUCH_TEAM, TOUCH_NUM
end

local run = function()
    for i=1, CIRCLE_COUNT do
        if not checkEnemyRobotInCircle(PLAYER_NUM_IN_CIRCLE[i], i) then
            PLAYER_NUM_IN_CIRCLE[i] = -1
            CIRCLE_STATE[i] = CIRCLE_STATE_EMPTY
        end
    end
    local ballPos = ball.rawPos()
    local tempEndPos = ballModel:poseForTime(9999)
    local ballEnd = CGeoPoint(tempEndPos:X(), tempEndPos:Y())
    local ballSeg = CGeoSegment(ballPos,ballEnd)
    for i=1, CIRCLE_COUNT do
        local proj = ballSeg:projection(CIRCLE_POS[i])
        if checkInCircle(ballPos, i) then
            CIRCLE_BALL_STATE[i] = CIRCLE_BALL_HOLDING
        elseif (checkInCircle(proj, i) and ballSeg:IsPointOnLineOnSegment(proj)) or checkInCircle(ballEnd, i) then
            CIRCLE_BALL_STATE[i] = CIRCLE_BALL_RECEIVING
        else
            CIRCLE_BALL_STATE[i] = CIRCLE_BALL_EMPTY
        end
    end
end
local runCheck = function()
    -- out of circle
    for i=1, CIRCLE_COUNT do
        if PLAYER_NUM_IN_CIRCLE[i] == -1 then
            msg = string.format("Circle %d is empty", i)
            return true
        end
    end
    -- catched by defender(me)
    if DEFENDER_CATCH_BALL then
        return true
    end
    for i=1, CIRCLE_COUNT do
        if CIRCLE_BALL_STATE[i] == CIRCLE_BALL_HOLDING or CIRCLE_BALL_STATE[i] == CIRCLE_BALL_RECEIVING then
            msg = string.format("Circle %d is holding/receiving the ball", i)
            return false
        end
    end
    -- can not get ball
    return true
end

local kickCheck = function()
    for i=1, CIRCLE_COUNT do
        if CIRCLE_BALL_STATE[i] == CIRCLE_BALL_HOLDING then
            msg = string.format("Circle %d is holding the ball", i)
            return false
        end
    end
    return true
end

local RESET_COUNT = 0
local INTER_COUNT = 0
local PASS_COUNT = 0
local STATE = ""
local OFFSET = Utils.Polar2Vector(CIRCLE_RADIUS+50, -math.pi/3*2)
local STATE_STR = function()
    if STATE=="run" then
        if WAIT_START then
            return "等待传球", param.BLUE
        else
            return "正常运行", param.GREEN
        end
    elseif STATE=="stop" then
        return "停止", param.RED
    else
        return "等待归位", param.YELLOW
    end
end
local debug = function()
    updateLastTouch() -- trick
    for i, pos in ipairs(CIRCLE_POS) do
        debugEngine:gui_debug_arc(pos, CIRCLE_RADIUS, 0, 360, CIRCLE_STATE[i])
        debugEngine:gui_debug_arc(pos, CIRCLE_RADIUS+50, 0, 360, CIRCLE_BALL_STATE[i])
        debugEngine:gui_debug_msg(pos+OFFSET, string.format("C%d: %s", i, BALL_STR[CIRCLE_BALL_STATE[i]]), CIRCLE_BALL_STATE[i])
    end
    local team, robotNum = getLastTouch()
    if robotNum ~= -1 then
        debugEngine:gui_debug_arc(team and player.pos(robotNum) or enemy.pos(robotNum), 1.5*PLAYER_RADIUS, 0, 360, param.ORANGE)
    end
    local OFFSET_Y = -180
    local STATE,COLOR = STATE_STR()
    debugEngine:gui_debug_msg_fix(CGeoPoint(1000,-1000 + 0*OFFSET_Y),string.format("剩余时间 : %.1f", CYCLE_COUNTDOWN/param.frameRate),param.WHITE)
    debugEngine:gui_debug_msg_fix(CGeoPoint(1000,-1000 + 1*OFFSET_Y),string.format("重置次数 : %d", RESET_COUNT))
    debugEngine:gui_debug_msg_fix(CGeoPoint(1000,-1000 + 2*OFFSET_Y),string.format("拦截次数 : %d", INTER_COUNT))
    debugEngine:gui_debug_msg_fix(CGeoPoint(1000,-1000 + 3*OFFSET_Y),string.format("传球次数 : %d", PASS_COUNT),param.GREEN)
    debugEngine:gui_debug_msg_fix(CGeoPoint(1000,-1000 + 4*OFFSET_Y),string.format("当前状态 : %s", STATE),COLOR)
end

return {
firstState = "reset",
["reset"] = {
    switch = function()
        debug()
        reset()
        STATE = "wait"
        return "wait"
    end,
    match = ""
},
["wait"] = {
    switch = function()
        debug()
        wait()
        if bufcnt(waitCheck(), 20) then
            STATE = "run"
            return "run"
        end
    end,
    match = ""
},
["run"] = {
    switch = function()
        debug()
        if not WAIT_START then
            CYCLE_COUNTDOWN = CYCLE_COUNTDOWN - 1
        end
        if CYCLE_COUNTDOWN <= 0 then
            return "stop"
        end
        run()
        if runCheck() then
            if DEFENDER_CATCH_BALL then
                INTER_COUNT = INTER_COUNT + 1
            end
            RESET_COUNT = RESET_COUNT + 1
            return "reset"
        end
        if not WAIT_START and TOUCH_NEW_UPDATE then
            PASS_COUNT = PASS_COUNT + 1
        end
        if kickCheck() then
            WAIT_START = false
        end
    end,
    match = ""
},
["stop"] = {
    switch = function()
        STATE = "stop"
        debug()
    end,
    match = ""
},
name = "ZJRoboCon2024TC",
}