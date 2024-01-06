local START_POS = CGeoPoint:new_local(1000,1000)

local VW_MAX = 12.8
local AW = 4

local vw = 0.0
local F_vw = function()
    return vw
end
-- local DEBUG_MSG = function()
--     debugEngine:
-- end

local update = function()
    vw = vw + AW/param.frameRate
    if vw > VW_MAX then
        vw = VW_MAX
        AW = -AW
    elseif vw < -VW_MAX then
        vw = -VW_MAX
        AW = -AW
    end
    -- DEBUG_MSG()
end

local reset = function()
    vw = 0.0
end


gPlayTable.CreatePlay{
firstState = "start",
["start"] = {
    switch = function()
        if bufcnt(player.toTargetDist("Leader") < 20 and player.infraredCount("Leader")>20 ,param.frameRate*1) then
            return "run"
        end
    end,
    -- Leader = task.placeBall(START_POS),
    Leader = task.goCmuRush(START_POS,0,nil,flag.dribbling),
    match = "(L)"
},

["run"] = {
    switch = function()
        update()
        if bufcnt(not player.infraredOn("Leader"), 5) then
            reset()
            return "start"
        end
    end,
    Leader = task.openSpeed(0,0,F_vw,flag.dribbling),
    match = "{L}"
},

name = "TestBenchmark_Dribble",
applicable ={
	exp = "a",
	a = true
},
attribute = "attack",
timeout = 99999
}
