local testPos  = {
	IS_YELLOW and CGeoPoint:new_local(2500,1000) or CGeoPoint:new_local(-2500,-1300),
	IS_YELLOW and CGeoPoint:new_local(1500,1000) or CGeoPoint:new_local(-1500,-1300),
	IS_YELLOW and CGeoPoint:new_local(2500,2000) or CGeoPoint:new_local(-2500,-1700),
	IS_YELLOW and CGeoPoint:new_local(1500,2000) or CGeoPoint:new_local(-1500,-1700),
	IS_YELLOW and CGeoPoint:new_local(2500,500) or CGeoPoint:new_local(-2500,-700),
	IS_YELLOW and CGeoPoint:new_local(1500,500) or CGeoPoint:new_local(-1500,-700),
}
local vel = CVector:new_local(0, 0)
local maxvel=0
local time = 50
local openrac = 1
local n = function()
	return openrac + 1
end
local DSS_FLAG = bit:_or(flag.allow_dss, flag.dodge_ball) + flag.our_ball_placement --+ flag.dribbling
gPlayTable.CreatePlay{

firstState = "run1",

["move"] = {
	switch = function()
	end,
	Leader = task.openSpeed(4000,0,0,10),
	match = "[L]"
},

["run1"] = {
	switch = function()
		if bufcnt(player.toTargetDist("Assister")<10 or player.toTargetDist("Leader")<10,time) then
			return "run"..2--math.random(4)
		end
	end,
	Leader   = task.goCmuRush(testPos[1],0, _, DSS_FLAG),--task.openSpeed(0,0,3,10)
	Assister = task.goCmuRush(testPos[3],0, _, DSS_FLAG),--task.openSpeed(0,0,6,10)
	Special  = task.goCmuRush(testPos[5],0, _, DSS_FLAG),
	match = "[LAS]"
},
["run2"] = {
	switch = function()
		if bufcnt(player.toTargetDist("Assister")<10 or player.toTargetDist("Leader")<10,time) then
			return "run"..1
		end
	end,
	Leader   = task.goCmuRush(testPos[2],0, _, DSS_FLAG),
	Assister = task.goCmuRush(testPos[4],0, _, DSS_FLAG),
	Special = task.goCmuRush(testPos[6],0, _, DSS_FLAG),
	match = "[LAS]"
},
["run3"] = {
	switch = function()
		if bufcnt(player.toTargetDist("Kicker")<50,time) then
			return "run"..4
		end
	end,
	Kicker = task.goCmuRush(testPos[2],0, _, DSS_FLAG),
	-- Kicker = task.goBezierRush(testPos[1],0, _, DSS_FLAG, _, vel),
	match = ""
},
["run4"] = {
	switch = function()
		if bufcnt(player.toTargetDist("Kicker")<50,time) then
			return "run"..3--math.random(4)
		end
	end,
	Kicker = task.goCmuRush(testPos[4],math.pi, _, DSS_FLAG),
	-- Kicker = task.goBezierRush(testPos[2],math.pi, _, DSS_FLAG, _, vel),
	match = ""
},

name = "TestCarMotion",
applicable ={
	exp = "a",
	a = true
},
attribute = "attack",
timeout = 99999
}
