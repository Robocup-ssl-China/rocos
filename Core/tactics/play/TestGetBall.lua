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
            local dist = (ball.pos() - player.pos("Assister")):mod()
            debugEngine:gui_debug_msg(ball.pos()+Utils.Polar2Vector(300,math.pi/2),"Dist : " .. dist)
        end,
        Assister = task.touchKick(CGeoPoint(6000,0),nil,1000,true),
        -- Assister = task.stop(),
        match = "[A]"
    },

    name = "TestGetBall",
    applicable = {
        exp = "a",
        a = true
    },
    attribute = "attack",
    timeout = 99999
}
