local ORIGIN = CGeoPoint:new_local(0,0)
local CIRCLE_RADIUS = 800
local CIRCLE_DIST = 1500
local RUN_DIST = 1200
local WAIT_POS = {
    ORIGIN + Utils.Polar2Vector(CIRCLE_DIST, 0*math.pi*2/3+math.pi),
    ORIGIN + Utils.Polar2Vector(CIRCLE_DIST, 1*math.pi*2/3+math.pi),
    ORIGIN + Utils.Polar2Vector(CIRCLE_DIST, 2*math.pi*2/3+math.pi),
}

local area = 1
local minDist = 99999
local farArea = 1
local farDist = 0
local predictNextArea = 1
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

local playerNum = {
    -1,-1,-1
}

local update = function()
    local b = ball.rawPos()
    area, minDist = checkClosest(b)
    farArea, farDist = checkFarthest(b)

    for i = 1, 3 do
        for j=0,param.maxPlayer-1 do
            if enemy.valid(j) and (enemy.pos(j)-WAIT_POS[i]):mod() < CIRCLE_RADIUS then
                playerNum[i] = j
                break
            end
        end
    end
    local DIR = enemy.dir(playerNum[area])
    if playerNum[area] ~= -1 then
        local minAngle = 99999
        local minAngleNum = -1
        for i=1,3 do
            if i ~= area then
                local angle = (WAIT_POS[i]-WAIT_POS[area]):dir()
                local angDiff = math.abs(Utils.angDiff(DIR,angle))
                debugEngine:gui_debug_msg(CGeoPoint(0,150*i),"i-"..i.." "..angDiff)
                if angDiff < minAngle then
                    minAngle = angDiff
                    minAngleNum = i
                end
            end
        end
        debugEngine:gui_debug_msg(CGeoPoint(1000,1000),""..minAngleNum)
        if minAngle < math.pi/12 then
            predictNextArea = minAngleNum
        end
    end
    debugEngine:gui_debug_msg(CGeoPoint(0,0),"".. ball.velMod())
end

local dynPos = function()
    local aaa = -1
    for i=1,3 do
        if i ~= area and i ~= predictNextArea then
            aaa = i
            break
        end
    end
    if aaa == -1 then
        aaa = farArea
    end
    return ORIGIN+Utils.Polar2Vector(550,(ORIGIN-WAIT_POS[aaa]):dir())
end

local CHECK_POS = {
    ORIGIN + Utils.Polar2Vector(RUN_DIST, 0*math.pi*2/3),
    ORIGIN + Utils.Polar2Vector(RUN_DIST, 1*math.pi*2/3),
    ORIGIN + Utils.Polar2Vector(RUN_DIST, 2*math.pi*2/3),
}

local RUSH_TASK = function()
    local POS = player.rawPos("a")
    local DIR = player.dir("a")
    local ballVel = ball.vel()
    local ball = ball.pos()
    local targetPos = ball
    local minTimePos = ball
    local minTime = 99999
    for dist = 50, 500, 50 do
        local targetPos = ball + Utils.Polar2Vector(dist, ballVel:dir())
        local t1 = world:predictRunTime(player.instance("a"), targetPos, CVector(0,0))
        local t2 = ballModel:timeForDist(dist)
        if t1 < t2 then
            debugEngine:gui_debug_msg(CGeoPoint(100,100),"11111111")
            return task.goCmuRush(targetPos,DIR)
        end
        for i=1,3 do
            if (targetPos - WAIT_POS[i]):mod() < CIRCLE_RADIUS then
                break
            end
        end
        if t1 < minTime then
            minTime = t1
            minTimePos = targetPos
        end
    end
    local ref = -1
    local ref_dist = 99999
    for i=1,3 do
        if (CHECK_POS[i]-ball):mod() < ref_dist then
            ref_dist = (CHECK_POS[i]-ball):mod()
            ref = i
        end
    end
    local interLine = CGeoLine(CHECK_POS[ref],CGeoPoint(0,0))

    -- local runPos = minTimePos
    local ballLine = CGeoLine(ball, minTimePos)
    local runPos = CGeoLineLineIntersection(interLine,ballLine):IntersectPoint()
    -- local runPos = ballLine:projection(player.pos("a"))
    local runDir = (runPos - player.pos("a")):dir()
    debugEngine:gui_debug_msg(CGeoPoint(100,100),"2222222")
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
        if gSubPlay.getState("TC") ~= "run" then
            return "wait"
        end
        if ball.velMod() > 800 then
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
        debugEngine:gui_debug_msg(CGeoPoint(0,0),"!!!!!!!!" .. gSubPlay.getState("TC"))
        if gSubPlay.getState("TC") ~= "run" then
            return "wait"
        end
        if minDist < CIRCLE_RADIUS and ball.velMod() < 800 then
            return "run"
        end
    end,
    a = RUSH_TASK,
    match = "(a)"
},
name = "TC2024DefenseB",
}