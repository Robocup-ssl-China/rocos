local standPos = CGeoPoint:new_local(0,0)
local shootGen = function(dist)
	return function()
		local goalPos = CGeoPoint(param.pitchLength/2,0)
		local pos = ball.pos() + Utils.Polar2Vector(dist,(ball.pos() - goalPos):dir())
		return pos
	end
end
local DSS_FLAG = flag.allow_dss + flag.dodge_ball

local JUDGE = {
	BallInField = function()
		local x = ball.posX()
		local y = ball.posY()
		local mx = param.pitchLength
		local my = param.pitchWidth
		if not ball.valid() then
			return false
		end
		if x > mx or x < -mx or y > my or y < -my then
			return false
		end
		if math.abs(y) < param.penaltyWidth/2 and x > (param.pitchLength/2 - param.penaltyDepth) then
			return false
		end
		return true
	end,
}

gPlayTable.CreatePlay{

firstState = "run_to_zero",

["run_to_zero"] = {
	switch = function()
		if bufcnt(JUDGE.BallInField(),10) then
			return "run_to_ball"
		end
	end,
	a = task.goCmuRush(standPos, _, _, DSS_FLAG),
	match = "[a]"
},
["run_to_ball"] = {
	switch = function()
		if bufcnt(player.toTargetDist("a")<50,10) then
			return "try_dribble"
		end
		if bufcnt(not JUDGE.BallInField(),5) then
			return "run_to_zero"
		end
	end,
	a = task.goCmuRush(shootGen(150), _, _, DSS_FLAG),
	match = "[a]"
},
["try_dribble"] = {
	switch = function()
		if player.infraredCount("a")>10 then
			return "try_keep"
		end
		if bufcnt(not JUDGE.BallInField(),5) then
			return "run_to_zero"
		end
	end,
	a = task.goCmuRush(shootGen(100),_,_,flag.dribbling),
	match = "[a]"
},
["try_keep"] = {
	switch = function()
		if bufcnt(player.infraredCount("a")>1,100) then
			return "try_kick"
		end
		if bufcnt(not JUDGE.BallInField(),5) then
			return "run_to_zero"
		end
	end,
	a = task.goCmuRush(shootGen(250),_,_,flag.dribbling),
	match = "[a]"
},
["try_kick"] = {
	switch = function()
		if player.kickBall("a") then
			return "run_to_zero"
		end
		if bufcnt(not JUDGE.BallInField(),5) then
			return "run_to_zero"
		end
	end,
	a = task.shoot(shootGen(100)),
	match = "[a]"
},

name = "TestDribbleAndKick",
applicable ={
	exp = "a",
	a = true
},
attribute = "attack",
timeout = 99999
}
