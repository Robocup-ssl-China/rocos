local goaliePos = CGeoPoint:new_local(-param.pitchLength/2+param.playerRadius,0)
local middlePos = function()
  local ballPos = ball.pos()
  local idir = (pos.ourGoal() - ballPos):dir()
  local pos = ballPos + Utils.Polar2Vector(530+param.playerFrontToCenter,idir)
  return pos
end
local leftPos = function()
  local ballPos = ball.pos()
  local idir = ((pos.ourGoal() - ballPos):dir()) + 20/59
  local pos = ballPos + Utils.Polar2Vector(530+param.playerFrontToCenter,idir)
  return pos
end
local rightPos = function()
  local ballPos = ball.pos()
  local idir = ((pos.ourGoal() - ballPos):dir()) - 20/59
  local pos = ballPos + Utils.Polar2Vector(530+param.playerFrontToCenter,idir)
  return pos
end
gPlayTable.CreatePlay {

firstState = "start",

["start"] = {
  switch = function()
    debugEngine:gui_debug_arc(ball.pos(),500,0,360,1)
    if cond.isGameOn() then
      return "exit"
    end
  end,
  Leader   = task.goCmuRush(middlePos,dir.playerToBall),
  Assister = task.goCmuRush(leftPos,dir.playerToBall),
  Special  = task.goCmuRush(rightPos,dir.playerToBall),
  Goalie   = task.goCmuRush(goaliePos),
  match    = "[ASL]"
},

name = "Ref_StopV2",
applicable = {
  exp = "a",
  a = true
},
attribute = "attack",
timeout = 99999
}