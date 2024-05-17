local ORIGIN = CGeoPoint:new_local(0,0)
local CIRCLE_RADIUS = 800
local CIRCLE_DIST = 1500
local RUN_DIST = 1200
local WAIT_POS = {
    ORIGIN + Utils.Polar2Vector(CIRCLE_DIST, 0*math.pi*2/3+math.pi),
    ORIGIN + Utils.Polar2Vector(CIRCLE_DIST, 1*math.pi*2/3+math.pi),
    ORIGIN + Utils.Polar2Vector(CIRCLE_DIST, 2*math.pi*2/3+math.pi),
}
local RUN_POS = {
    ORIGIN + Utils.Polar2Vector(RUN_DIST, 0*math.pi*2/3),
    ORIGIN + Utils.Polar2Vector(RUN_DIST, 1*math.pi*2/3),
    ORIGIN + Utils.Polar2Vector(RUN_DIST, 2*math.pi*2/3),
}

local area = 1
local minDist = 99999
local farArea = 1
local farDist = 0
local checkClosest = function(pos)
    local tempMinDist = 99999
    local tempArea = -1
    for i = 1, 3 do
        if (pos-WAIT_POS[i]):mod() < tempMinDist then
            tempMinDist = (pos-WAIT_POS[i]):mod()
            tempArea = i
        end
    end
    return tempArea, tempMinDist
end

local checkFarthest = function(pos)
    local tempMaxDist = 0
    local tempArea = -1
    for i = 1, 3 do
        if (pos-WAIT_POS[i]):mod() > tempMaxDist then
            tempMaxDist = (pos-WAIT_POS[i]):mod()
            tempArea = i
        end
    end
    return tempArea, tempMaxDist
end

local update = function()
    local ball = ball.rawPos()
    area, minDist = checkClosest(ball)
    farArea, farDist = checkFarthest(ball)
end

local dynPos = function()
    return ORIGIN+Utils.Polar2Vector(500,(ORIGIN-WAIT_POS[farArea]):dir())
end
local dynDir = function()
    return (ORIGIN - WAIT_POS[area]):dir() + math.pi/2
end

local RUSH_TASK = function()
    local DIR = player.dir("a")
    local ballVel = ball.vel()
    local ball = ball.pos()
    local targetPos = ball
    local minTimePos = ball
    local minTime = 99999
    for dist = 0, 1000, 50 do
        local targetPos = ball + Utils.Polar2Vector(dist, ballVel:dir())
        local t1 = world:predictRunTime(player.instance("a"), targetPos, CVector(0,0))
        local t2 = ballModel:timeForDist(dist)
        if t1 < t2 then
            return task.goCmuRush(targetPos,DIR)
        end
        if t1 < minTime then
            minTime = t1
            minTimePos = targetPos
        end
    end
    -- local runPos = minTimePos
    local runPos = CGeoLine(ball, minTimePos):projection(player.pos("a"))
    local runDir = (runPos - player.pos("a")):dir()
    return task.goCmuRush(runPos,DIR,_,_,Utils.Polar2Vector(2000,runDir))
end

return {
firstState = "reset",
["reset"] = {
    switch = function()
        gSubPlay.new("TC", "ZJRoboCon2024TC")
        return "run"
    end,
    a = task.stop(),
    match = "(a)"
},
["wait"] = {
    switch = function()
        update()
        return gSubPlay.getState("TC") == "run" and "run" or "wait"
    end,
    a = task.goCmuRush(ORIGIN, player.dir("a")),
    match = "(a)"
},
["run"] = {
    switch = function()
        update()
        if not gSubPlay.getState("TC") == "run" then
            return "wait"
        end
        if minDist > CIRCLE_RADIUS then
            return "rush"
        end
    end,
    -- a = task.runMultiPos(RUN_POS,true,30,0),
    a = task.goCmuRush(dynPos, player.dir("a")),
    match = "(a)"
},
['rush'] = {
    switch = function()
        update()
        if not gSubPlay.getState("TC") == "run" then
            return "wait"
        end
        if minDist < CIRCLE_RADIUS then
            return "run"
        end
    end,
    a = RUSH_TASK,
    match = "(a)"
},
name = "TC2024DefenseB",
}