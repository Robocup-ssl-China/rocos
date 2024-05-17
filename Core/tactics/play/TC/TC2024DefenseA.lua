local ORIGIN = CGeoPoint:new_local(0,0)
local RUN_DIST = 1150
local RUN_POS = {
    ORIGIN + Utils.Polar2Vector(RUN_DIST, 0*math.pi*2/3),
    ORIGIN + Utils.Polar2Vector(RUN_DIST, 2*math.pi*2/3),
    ORIGIN + Utils.Polar2Vector(RUN_DIST, 1*math.pi*2/3),
}
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
        return gSubPlay.getState("TC") == "run" and "run" or "wait"
    end,
    a = task.goCmuRush(ORIGIN,0),
    match = "(a)"
},
["run"] = {
    switch = function()
        return gSubPlay.getState("TC") == "run" and "run" or "wait"
    end,
    a = task.runMultiPos(RUN_POS,true,60,0),
    match = "(a)"
},
name = "TC2024DefenseA",
}