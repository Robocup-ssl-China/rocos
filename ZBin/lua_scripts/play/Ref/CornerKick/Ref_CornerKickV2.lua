local waitPos = CGeoPoint:new_local(3300,-1400)

local leaderPos = function()
	local ballPos = ball.pos()
	local targetDir = (ballPos - waitPos):dir()
	local target = ballPos + Utils.Polar2Vector(200,targetDir)
	return target
end
local leaderDir = function()
	local ballPos = ball.pos()
	local targetDir = (waitPos - ballPos):dir()
	return targetDir
end
local leaderFlag = flag.dodge_ball
gPlayTable.CreatePlay{
firstState = "get",
["get"] = {
  switch = function()
  	if bufcnt(player.toTargetDist("Leader")<10 and player.toTargetDist("Special")<10,10)then
  		return "pass"
  	end
  end,
  ["Leader"]   = task.goCmuRush(leaderPos,leaderDir,_,leaderFlag),
  ["Special"]  = task.goCmuRush(waitPos),
  match = "(LS)"
},
["pass"] = {
  switch = function()
  	if player.kickBall("Leader") then
  		return "shoot"
  	end
  end,
  ["Leader"]   = task.touchKick(waitPos),
  ["Special"]  = task.goCmuRush(waitPos),
  match = "{LS}"
},
["shoot"] = {
  switch = function()
  	if player.kickBall("Special") then
  		return "exit"
  	end
  end,
  ["Leader"]   = task.stop(),
  ["Special"]  = task.touchKick(_,false),
  match = "{LS}"
},
name = "Ref_CornerKickV2",
applicable = {
  exp = "a",
  a   = true
},
attribute = "attack",
timeout = 99999
}