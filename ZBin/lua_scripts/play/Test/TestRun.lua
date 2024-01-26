local testPos  = {
	CGeoPoint:new_local(1000,1000),
	CGeoPoint:new_local(-1000,1000),
	CGeoPoint:new_local(-1000,-1000),
	CGeoPoint:new_local(1000,-1000)
}
local vel = CVector:new_local(0, 0)
local maxvel=0
local time = 120
local DSS_FLAG = bit:_or(flag.allow_dss, flag.dodge_ball)
gPlayTable.CreatePlay{

firstState = "run1",

["run1"] = {
	switch = function()
		print(skillapi:get_size())
		for i=0,skillapi:get_size()-1 do
			print(i .. " " .. skillapi:get_name(i))
		end
	end,
	Assister = task.goCmuRush(testPos[1],0, nil, DSS_FLAG),
	Leader = task.goCmuRush(testPos[2],0, nil, DSS_FLAG),
	Defender = task.goCmuRush(testPos[3],0, nil, DSS_FLAG),
	Middle = task.goCmuRush(testPos[4],0, nil, DSS_FLAG),
	match = "[ALDM]"
},
["run2"] = {
	switch = function()
		if bufcnt(player.toTargetDist("Kicker")<5,time) then
			return "run"..1
		end
	end,
	Kicker = task.goCmuRush(testPos[3],math.pi, _, DSS_FLAG),
	-- Kicker = task.goBezierRush(testPos[4],math.pi, _, DSS_FLAG, _, vel),
	match = ""
},
["run3"] = {
	switch = function()
		if bufcnt(player.toTargetDist("Kicker")<5,time) then
			return "run"..4
		end
	end,
	Kicker = task.goCmuRush(testPos[2],0, _, DSS_FLAG),
	-- Kicker = task.goBezierRush(testPos[1],0, _, DSS_FLAG, _, vel),
	match = ""
},
["run4"] = {
	switch = function()
		if bufcnt(player.toTargetDist("Kicker")<5,time) then
			return "run"..1--math.random(4)
		end
	end,
	Kicker = task.goCmuRush(testPos[4],math.pi, _, DSS_FLAG),
	-- Kicker = task.goBezierRush(testPos[2],math.pi, _, DSS_FLAG, _, vel),
	match = ""
},

name = "TestRun",
applicable ={
	exp = "a",
	a = true
},
attribute = "attack",
timeout = 99999
}
