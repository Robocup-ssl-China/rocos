local POS_X = -3500
local POS_X_STEP = 1000
local POS_Y = 800
local getPos = function(i,j)
	return CGeoPoint(POS_X + i * POS_X_STEP, POS_Y*j)
end
gPlayTable.CreatePlay{

firstState = "stop",

switch = function()
end,

-- headback sideback defend middle
["stop"] = {
	Leader = task.placeBall(ball.placementPos,_,true),
	a = task.goCmuRush(getPos(1, 1),0),
	b = task.goCmuRush(getPos(1,-1),0),
	c = task.goCmuRush(getPos(2, 1),0),
	d = task.goCmuRush(getPos(2,-1),0),
	e = task.goCmuRush(getPos(3, 1),0),
	f = task.goCmuRush(getPos(3,-1),0),
	Goalie   = task.goCmuRush(CGeoPoint(POS_X, 0)),
	match    = "(L)[abcdef]"
},

name = "Ref_BallPlace2Stop",
applicable ={
	exp = "a",
	a   = true
},
attribute = "defense",
timeout   = 99999
}