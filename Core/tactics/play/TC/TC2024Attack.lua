local ORIGIN = CGeoPoint:new_local(0,0)
local CIRCLE_DIST = 1500
local WAIT_POS = {
    ORIGIN + Utils.Polar2Vector(CIRCLE_DIST, 0*math.pi*2/3),
    ORIGIN + Utils.Polar2Vector(CIRCLE_DIST, 1*math.pi*2/3),
    ORIGIN + Utils.Polar2Vector(CIRCLE_DIST, 2*math.pi*2/3),
}
return {
firstState = "init",
["init"] = {
    switch = function()
        if player.toTargetDist("a") < 10 then
            return "kickA"
        end
    end,
    a = task.goCmuRush(WAIT_POS[1]),
    b = task.goCmuRush(WAIT_POS[2]),
    c = task.goCmuRush(WAIT_POS[3]),
    match = "[abc]"
},
["kickA"] = {
    switch = function()
        if player.kickBall("a") then
            return "kickB"
        end
    end,
    a = task.touchKick(WAIT_POS[2],false,4000,true),
    b = task.goCmuRush(WAIT_POS[2]),
    c = task.goCmuRush(WAIT_POS[3]),
    match = "{abc}",
},
["kickB"] = {
    switch = function()
        if player.kickBall("b") then
            return "kickA"
        end
    end,
    b = task.touchKick(WAIT_POS[1],false,4000,true),
    a = task.goCmuRush(WAIT_POS[1]),
    c = task.goCmuRush(WAIT_POS[3]),
    match = "{abc}",
},
name = "TC2024Attack",
}