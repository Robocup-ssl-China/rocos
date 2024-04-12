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
            debugEngine:gui_debug_x(player.pos("Assister"), param.GREEN)
            debugEngine:gui_debug_msg(player.pos("Assister"), "Pos", param.GREEN)
            debugEngine:gui_debug_x(player.rawPos("Assister"), param.PURPLE)
            debugEngine:gui_debug_msg(player.rawPos("Assister"), "RawPos", param.PURPLE)
        end,
        Assister = task.touchKick(CGeoPoint(6000,0),nil,6000,true),
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
