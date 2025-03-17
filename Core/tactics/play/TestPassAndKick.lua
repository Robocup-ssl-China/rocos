local Y = 2000
local X_Threshold = 2800
local PASS_STEP = 400

local passPos = CGeoPoint:new_local(0,0)
local kickMode = 0
local kickPower = 0
local needShoot = false
local init = function()
    needShoot = false
    passPos = ball.pos()
end
local updatePassPos = function()
    local x = math.min(X_Threshold,passPos:x()+PASS_STEP)
    local y = passPos:y() > 0 and -Y or Y
    passPos = CGeoPoint(x, y)
    kickMode = kick.flat() -- math.random(kick.flat(),kick.chip())
    kickPower = kickMode == kick.flat() and 4000 or 2000
    needShoot = x > X_Threshold-100 and true or false
end
local c_passPos = function()
    return passPos
end
local c_kickmode = function()
    return kickMode
end
local c_kickpower = function()
    return kickPower
end

local debug = function()
    debugEngine:gui_debug_x(passPos)
    debugEngine:gui_debug_msg(passPos,"passPos")
end

return {
firstState = "init",
["init"] = {
    switch = function()
        if bufcnt(ball.valid(),3) then
            init()
            updatePassPos()
            return "start"
        end
    end,
    match = "",
},
["start"] = {
    switch = function()
        if player.toTargetDist("Assister") < 1000 and player.toTargetDist("Leader") < 100 then
            return "pass"
        end
    end,
    Leader = task.staticGetBall(c_passPos),
    Assister = task.goCmuRush(c_passPos),
    match = "[L][A]"
},
["pass"] = {
	switch = function()
        debug()
        if player.kickBall("Leader") then
            updatePassPos()
            if needShoot then
                return "shoot"
            else
                return "wait"
            end
        end
	end,
	Leader = task.touchKick(c_passPos, false, c_kickpower, c_kickmode),
    Assister = task.goCmuRush(c_passPos),
	match = "(LA)"
},
["wait"] = {
	switch = function()
        debug()
        if bufcnt(true, 10) then
            return "pass"
        end
	end,
	Leader = task.stop(),
    Assister = task.goCmuRush(c_passPos),
	match = "[LA]"
},
["shoot"] = {
    switch = function()
        if player.kickBall("Assister") then
            return "init"
        end
    end,
    Leader = task.stop(),
    Assister = task.touchKick(pos.theirGoal(), false, 6000, kick.flat),
    match = "{LA}"
},

name = "TestPassAndKick",
applicable ={
	exp = "a",
	a = true
},
attribute = "attack",
timeout = 99999
}
