local COR_DEF_POS1 = CGeoPoint:new_local(-50*param.lengthRatio,-50*param.widthRatio)
local SIDE_POS, MIDDLE_POS, INTER_POS = pos.refStopAroundBall()

local LEADER_TASK = function()
	if cond.ourBallPlace() then
		return task.fetchBall(ball.placementPos,0,true)
	end
	return task.goCmuRush(pos.LEADER_STOP_POS, player.toBallDir, _, flag.dodge_ball+flag.allow_dss)
end

gPlayTable.CreatePlay{

firstState = "beginning",

switch = function()	
	if gCurrentState == "beginning" and 
		enemy.attackNum() <= 8 and enemy.attackNum() > 0 then
		return "attacker"..enemy.attackNum()
	else
		if cond.isGameOn() then
			return "finish"
		elseif enemy.situChanged() and
			enemy.attackNum() <= 8 and enemy.attackNum() > 0 then
			return "attacker"..enemy.attackNum()
		end
	end
end,

-- headback sideback defend middle
["beginning"] = {
	Leader   = LEADER_TASK,
	Special  = task.goPassPos("Leader"),
	Middle   = task.goCmuRush(COR_DEF_POS1, player.toBallDir, _, flag.dodge_ball+flag.allow_dss),
	Fronter  = task.goCmuRush(INTER_POS, player.toBallDir, _, flag.dodge_ball+flag.allow_dss),
	Center   = task.sideBack(),
	Defender = task.leftBack(),
	Assister = task.rightBack(),
	Goalie   = task.zgoalie(),
	match    = "(L)[SMDFCA]"
},

["attacker1"] = {
	Leader   = LEADER_TASK,
	Special  = task.goPassPos("Leader"),
	Middle   = task.goCmuRush(COR_DEF_POS1, player.toBallDir, _, flag.dodge_ball+flag.allow_dss),
	Fronter  = task.goCmuRush(INTER_POS, player.toBallDir, _, flag.dodge_ball+flag.allow_dss),
	Center   = task.sideBack(),
	Defender = task.leftBack(),
	Assister = task.rightBack(),
	Goalie   = task.zgoalie(),
	match    = "{L}[SMDFCA]"
},

["attacker2"] = {
	Leader   = LEADER_TASK,
	Special  = task.goPassPos("Leader"),
	Middle   = task.goCmuRush(COR_DEF_POS1, player.toBallDir, _, flag.dodge_ball+flag.allow_dss),
	Center   = task.zmarking("First"),
	Fronter  = task.leftBack(),
	Defender = task.defendHead(),
	Assister = task.rightBack(),
	Goalie   = task.zgoalie(),
	match    = "{L}[SMDFCA]"
},

["attacker3"] = {
	Leader   = LEADER_TASK,
	Special  = task.zmarking("First"),
	Middle   = task.zmarking("Second"),
	Defender = task.goCmuRush(COR_DEF_POS1, player.toBallDir, _, flag.dodge_ball+flag.allow_dss),
	Center   = task.leftBack(),
	Fronter  = task.rightBack(),
	Assister = task.defendHead(),
	Goalie   = task.zgoalie(),
	match    = "{L}[SMDFCA]"
},

["attacker4"] = {
	Leader   = LEADER_TASK,
	Special  = task.zmarking("First"),
	Middle   = task.zmarking("Second"),
	Defender = task.zmarking("Third"),
	Center   = task.leftBack(),
	Fronter  = task.rightBack(),
	Assister = task.defendHead(),
	Goalie   = task.zgoalie(),
	match    = "{L}[SMDFCA]"
},

["attacker5"] = {
	Leader   = LEADER_TASK,
	Special  = task.zmarking("First"),
	Middle   = task.zmarking("Second"),
	Defender = task.zmarking("Third"),
	Center   = task.zmarking("Fourth"),
	Fronter  = task.singleBack(),
	Assister = task.defendHead(),
	Goalie   = task.zgoalie(),
	match    = "{L}[SMDFCA]"
},

["attacker6"] = {
	Leader   = LEADER_TASK,
	Special  = task.zmarking("First"),
	Middle   = task.zmarking("Second"),
	Defender = task.zmarking("Third"),
	Center   = task.zmarking("Fourth"),
	Fronter  = task.zmarking("Fifth"),
	Assister = task.defendHead(),
	Goalie   = task.zgoalie(),
	match    = "{L}[SMDFCA]"
},

["attacker7"] = {
	Leader   = LEADER_TASK,
	Special  = task.zmarking("First"),
	Middle   = task.zmarking("Second"),
	Defender = task.zmarking("Third"),
	Center   = task.zmarking("Fourth"),
	Fronter  = task.zmarking("Fifth"),
	Assister = task.zmarking("Sixth"),
	Goalie   = task.zgoalie(),
	match    = "{L}[SMDFCA]"
},

["attacker8"] = {
	Leader   = LEADER_TASK,
	Special  = task.zmarking("First"),
	Middle   = task.zmarking("Second"),
	Defender = task.zmarking("Third"),
	Center   = task.zmarking("Fourth"),
	Fronter  = task.zmarking("Fifth"),
	Assister = task.defendHead(),
	Goalie   = task.zgoalie(),
	match    = "{L}[SMDFCA]"
},

name = "Ref_BallPlace2Stop",
applicable ={
	exp = "a",
	a   = true
},
attribute = "defense",
timeout   = 99999
}