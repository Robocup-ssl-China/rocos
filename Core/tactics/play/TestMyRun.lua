local testPos = {CGeoPoint:new_local(1000, 1000), CGeoPoint:new_local(-1000, 1000), CGeoPoint:new_local(-1000, -1000),
                 CGeoPoint:new_local(1000, -1000)}
local vel = CVector:new_local(0, 0)
local maxvel = 0
local time = 120
local DSS_FLAG = bit:_or(flag.allow_dss, flag.dodge_ball)

local DIR = function()
    return (player.pos('Assister') - ball.pos()):dir()
end

return {
    firstState = "run1",

    ["run1"] = {
        switch = function()
        end,
        Assister = task.goCmuRush(testPos[1], DIR, nil, DSS_FLAG),
        match = "[A]"
    },

    name = "TestMyRun",
    applicable = {
        exp = "a",
        a = true
    },
    attribute = "attack",
    timeout = 99999
}
