local testPos = {
    CGeoPoint:new_local(3000, 3100), 
    CGeoPoint:new_local(-3000, 3100),
    CGeoPoint:new_local(-3000,-3100),
    CGeoPoint:new_local(3000,-3100)
}
local vel = CVector:new_local(0, 0)
local maxvel = 0
local time = 1
local DSS_FLAG = bit:_or(flag.allow_dss, flag.dodge_ball)

local DIR = function()
    return (player.pos('Assister') - ball.pos()):dir()
end

return {
    firstState = "skill",
    ["skill"] = {
        switch = function()
        end,
        Leader = {CircleRun{pos=CGeoPoint(0,100), rotVel=4}},
        match = "[L]"
    },
    ["run1"] = {
        switch = function()
            if bufcnt(player.toTargetDist("a")<5,time) then
                return "run"..2
            end
        end,
        a = task.goCmuRush(testPos[1],0, _, DSS_FLAG),
        b = task.goCmuRush(testPos[1]+Utils.Polar2Vector(1000,-math.pi/2),0, _, DSS_FLAG),
        match = "{ab}"
    },
    ["run2"] = {
        switch = function()
            if bufcnt(player.toTargetDist("a")<5,time) then
                return "run"..3
            end
        end,
        a = task.goCmuRush(testPos[2],0, _, DSS_FLAG),
        b = task.goCmuRush(testPos[2]+Utils.Polar2Vector(1000,-math.pi/2),0, _, DSS_FLAG),
        match = "{ab}"
    },
    ["run3"] = {
        switch = function()
            if bufcnt(player.toTargetDist("a")<5,time) then
                return "run"..4
            end
        end,
        a = task.goCmuRush(testPos[3],0, _, DSS_FLAG),
        b = task.goCmuRush(testPos[3]+Utils.Polar2Vector(1000,-math.pi/2),0, _, DSS_FLAG),
        match = "{ab}"
    },
    ["run4"] = {
        switch = function()
            if bufcnt(player.toTargetDist("a")<5,time) then
                return "run"..1
            end
        end,
        a = task.goCmuRush(testPos[4],0, _, DSS_FLAG),
        b = task.goCmuRush(testPos[4]+Utils.Polar2Vector(1000,-math.pi/2),0, _, DSS_FLAG),
        match = "{ab}"
    },

    name = "TestMyRun",
    applicable = {
        exp = "a",
        a = true
    },
    attribute = "attack",
    timeout = 99999
}
