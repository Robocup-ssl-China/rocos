local START_POS = CGeoPoint:new_local(1000,1000)

local VW_MAX = 12.7
local AW = 16

local vw = 0.0
local w = 0.0
local F_vw = function()
    return vw
end
local F_w = function()
    return w
end

local result_list = {} -- {acc,vw_max}

local poses = {
    CGeoPoint:new_local(2500,2500),
    CGeoPoint:new_local(2500,500),
    CGeoPoint:new_local(500,500),
    CGeoPoint:new_local(500,2500),
}

local DEBUG_MSG = function()
    local sx,sy = 200,-1000
    local span = 140
    local sp = CGeoPoint:new_local(sx,sy)
    local v = CVector:new_local(0,-span)
    debugEngine:gui_debug_msg(sp + v*0,string.format("Acc : %4.1f",AW),param.BLUE)
    debugEngine:gui_debug_msg(sp + v*1,string.format("Vw  : %4.1f",vw),param.BLUE)
    debugEngine:gui_debug_msg(sp + v*2,string.format("INF : %4d",player.infraredCount(9)),param.BLUE)
    debugEngine:gui_debug_msg(sp + v*3,string.format("INF : %4d",player.infraredOn(9) and 1 or 0),param.BLUE)
end

local update = function()
    vw = vw + AW / param.frameRate
    if vw > VW_MAX then
        vw = VW_MAX
        AW = -AW
    elseif vw < -VW_MAX then
        vw = -VW_MAX
        AW = -AW
    end
    w = (w + vw*param.frameRate)
    w = math.modf(w,math.pi*2)
    DEBUG_MSG()
end

local reset = function()
    vw = 0.0
end


gPlayTable.CreatePlay{
firstState = "start",
["start"] = {
    switch = function()
        DEBUG_MSG()
        if bufcnt(player.toTargetDist("Leader") < 100 and player.infraredCount("Leader")>20 ,param.frameRate*1) then
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
    -- Leader = task.openSpeed(0,0,F_vw,flag.dribbling),
    Leader = task.runMultiPos(poses, true, 100, 0.0, 2000, flag.dribbling),
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
