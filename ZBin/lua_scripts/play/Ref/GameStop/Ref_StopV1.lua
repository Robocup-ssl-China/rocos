-- by Soap 2015-04-14
-- YYS 2015-06-10
local PENALTY_THRESHOLD_DIST = 91 --18+18+50+5
local MIDDLE_THRESHOLD_Y = 0.125 * param.pitchWidth --75

local topX = -(param.pitchLength/2 - param.penaltyDepth - 2 * param.playerRadius)
local sideY = param.penaltyWidth/2 + 2 * param.playerRadius
local rightTop = {
  CGeoPoint:new_local(topX, 0),
  CGeoPoint:new_local(topX, 20),
  CGeoPoint:new_local(topX, 40),
  CGeoPoint:new_local(topX, 60),
  CGeoPoint:new_local(topX, 80),
  CGeoPoint:new_local(topX, 100)
}
local leftTop = {
  CGeoPoint:new_local(topX, -0),
  CGeoPoint:new_local(topX, -20),
  CGeoPoint:new_local(topX, -40),
  CGeoPoint:new_local(topX, -60),
  CGeoPoint:new_local(topX, -80),
  CGeoPoint:new_local(topX, -100)
}
local rightSide = {
  CGeoPoint:new_local(topX - 0, sideY),
  CGeoPoint:new_local(topX - 20, sideY),
  CGeoPoint:new_local(topX - 40, sideY),
  CGeoPoint:new_local(topX - 60, sideY),
  CGeoPoint:new_local(topX - 80, sideY),
  CGeoPoint:new_local(topX - 100, sideY)
}
local leftSide = {
  CGeoPoint:new_local(topX - 0, -sideY),
  CGeoPoint:new_local(topX - 20, -sideY),
  CGeoPoint:new_local(topX - 40, -sideY),
  CGeoPoint:new_local(topX - 60, -sideY),
  CGeoPoint:new_local(topX - 80, -sideY),
  CGeoPoint:new_local(topX - 100, -sideY)
}
local standPos = leftSide
local standFunc = function(num)
  return function()
    return standPos[num]
  end
end

local function penaltyStop()
  local ballPosX = function()
    return ball.posX()
  end
  local ballPosY = function()
    return ball.posY()
  end
  if ballPosX() > topX and ballPosY() > 0 then --right top
    local ONE_POS = function()
      return CGeoPoint:new_local(topX, 0)
    end
    local TWO_POS = function()
      return CGeoPoint:new_local(topX, 20)
    end
    local THREE_POS = function()
      return CGeoPoint:new_local(topX, 40)
    end
    local FOUR_POS = function()
      return CGeoPoint:new_local(topX, 60)
    end
    local FIVE_POS = function()
      return CGeoPoint:new_local(topX, 80)
    end
    local SIX_POS = function()
      return CGeoPoint:new_local(topX, 100)
    end
  elseif ballPosX() > topX and ballPosY() < 0 then--left top
    local ONE_POS = function()
      return CGeoPoint:new_local(topX, 0)
    end
    local TWO_POS = function()
      return CGeoPoint:new_local(topX, -20)
    end
    local THREE_POS = function()
      return CGeoPoint:new_local(topX, -40)
    end
    local FOUR_POS = function()
      return CGeoPoint:new_local(topX, -60)
    end
    local FIVE_POS = function()
      return CGeoPoint:new_local(topX, -80)
    end
    local SIX_POS = function()
      return CGeoPoint:new_local(topX, -100)
    end
  elseif ballPosX() < topX and ballPosY() > 0 then--right side
    local ONE_POS = function()
      return CGeoPoint:new_local(topX - 0, sideY)
    end
    local TWO_POS = function()
      return CGeoPoint:new_local(topX - 20, sideY)
    end
    local THREE_POS = function()
      return CGeoPoint:new_local(topX - 40, sideY)
    end
    local FOUR_POS = function()
      return CGeoPoint:new_local(topX - 60, sideY)
    end
    local FIVE_POS = function()
      return CGeoPoint:new_local(topX - 80, sideY)
    end
    local SIX_POS = function()
      return CGeoPoint:new_local(topX - 100, sideY)
    end
  else --ballPosX() < topX and ballPosY() < 0 then--left side
    local ONE_POS = function()
      return CGeoPoint:new_local(topX - 0, -sideY)
    end
    local TWO_POS = function()
      return CGeoPoint:new_local(topX - 20, -sideY)
    end
    local THREE_POS = function()
      return CGeoPoint:new_local(topX - 40, -sideY)
    end
    local FOUR_POS = function()
      return CGeoPoint:new_local(topX - 60, -sideY)
    end
    local FIVE_POS = function()
      return CGeoPoint:new_local(topX - 80, -sideY)
    end
    local SIX_POS = function()
      return CGeoPoint:new_local(topX - 100, -sideY)
    end
  end

  return ONE_POS, TWO_POS, THREE_POS, FOUR_POS, FIVE_POS, SIX_POS
end

local SIDE_POS, MIDDLE_POS, INTER_POS, SIDE2_POS, INTER2_POS = pos.refStopAroundBall()
--local ONE_POS, TWO_POS, THREE_POS, FOUR_POS, FIVE_POS, SIX_POS = penaltyStop()--pos.stopPoses()

local STOP_FLAG = flag.dodge_ball
local STOP_DSS = bit:_or(STOP_FLAG, flag.allow_dss)

local gBallPosXInStop = 0
local gBallPosYInStop = 0

local OTHER_SIDE_POS1 = ball.antiYPos(CGeoPoint:new_local(-100,120))
local OTHER_SIDE_POS2 = ball.antiYPos(CGeoPoint:new_local(-100,-120))

local OTHER_SIDE_POS = function()
  local factor = 1
  if ball.posY() > 0 then
    factor = -1
  end
  return CGeoPoint:new_local(INTER_POS():x(), factor * param.pitchWidth * 0.35)
  --return CGeoPoint:new_local(ball.posX() - 50*param.lengthRatio, 160*factor*param.widthRatio)
end

local getBufTime = function()
  if IS_SIMULATION then
    return 9999
  else
    return 240
  end
end

local ACC = 300

gPlayTable.CreatePlay {

firstState = "start",

["start"] = {
  switch = function()
    gBallPosXInStop = ball.posX()
    gBallPosYInStop = ball.posY()
    if cond.isGameOn() then
      return "exit"
    elseif ball.toOurPenaltyDist() < PENALTY_THRESHOLD_DIST then
    --elseif ball.toOurGoalDist() < PENALTY_THRESHOLD_DIST then
      return "standByPenalty"
    elseif ball.posY() > MIDDLE_THRESHOLD_Y or ball.posY() < -MIDDLE_THRESHOLD_Y then
      return "standByLine"
    elseif ball.posY() < MIDDLE_THRESHOLD_Y and ball.posY() > -MIDDLE_THRESHOLD_Y then
      return "standInMiddle"
    else
      return "exit"
    end
  end,

  Assister = task.stop(),
  Special  = task.stop(),
  Leader   = task.stop(),
  Fronter  = task.stop(),
  Center   = task.stop(),
  Defender = task.stop(),
  Middle   = task.stop(),
  Goalie   = task.stop(),
  match    = "[AMDLSFC]"
},

["standInMiddle"] = {
  switch = function()
    if cond.isGameOn() then
      return "exit"
    --elseif ball.toOurGoalDist() < PENALTY_THRESHOLD_DIST then
    elseif ball.toOurPenaltyDist() < PENALTY_THRESHOLD_DIST then
      gBallPosXInStop = ball.posX()
      gBallPosYInStop = ball.posY()
      return "standByPenalty"
    elseif ball.posY() < MIDDLE_THRESHOLD_Y and ball.posY() > -MIDDLE_THRESHOLD_Y then
      if (math.abs(gBallPosXInStop - ball.posX()) >= 8 or math.abs(gBallPosYInStop - ball.posY()) >= 8) then
        gBallPosXInStop = ball.posX()
        gBallPosYInStop = ball.posY()
        return "standInMiddle"
      end
      if bufcnt( ((gRoleNum["Assister"] ~= 0) and (player.toTargetDist("Assister") > 8)) or
                 ((gRoleNum["Special"] ~= 0) and (player.toTargetDist("Special") > 8)) or
                 ((gRoleNum["Leader"] ~= 0) and (player.toTargetDist("Leader") > 8)) or
                 ((gRoleNum["Defender"] ~= 0) and (player.toTargetDist("Defender") > 8)) or
                 ((gRoleNum["Middle"] ~= 0) and (player.toTargetDist("Middle") > 8)), getBufTime()) then
        return "reDoStop"
      end
      if bufcnt( ((gRoleNum["Assister"] ~= 0) and (player.toTargetDist("Assister") < 10)) and
                 ((gRoleNum["Special"] ~= 0) and (player.toTargetDist("Special") < 10)) and
                 ((gRoleNum["Leader"] ~= 0) and (player.toTargetDist("Leader") < 10)) and
                 ((gRoleNum["Defender"] ~= 0) and (player.toTargetDist("Defender") < 10)) and
                 ((gRoleNum["Middle"] ~= 0) and (player.toTargetDist("Middle") < 10)), 15) then
        gBallPosXInStop = ball.posX()
        gBallPosYInStop = ball.posY()
        return nil
      end
    elseif ball.posY() > MIDDLE_THRESHOLD_Y + param.playerRadius or ball.posY() < -MIDDLE_THRESHOLD_Y - param.playerRadius then
      gBallPosXInStop = ball.posX()
      gBallPosYInStop = ball.posY()
      return "standByLine"
    else
      return "exit"
    end
  end,

  Assister = task.goCmuRush(SIDE_POS, dir.playerToBall, ACC, STOP_DSS),
  Special  = task.goCmuRush(INTER_POS, dir.playerToBall, ACC, STOP_DSS),
  Leader   = task.goCmuRush(MIDDLE_POS, dir.playerToBall, ACC, STOP_DSS),
  Fronter  = task.goCmuRush(OTHER_SIDE_POS, dir.playerToBall, ACC, STOP_DSS),--task.goCmuRush(SIDE2_POS, dir.playerToBall, ACC, STOP_DSS),--task.defendMiddle4Stop(),--TODO
  Center   = task.stop(),
  Defender = task.stop(),
  Middle   = task.stop(),
  Goalie   = task.stop(),
  match    = "[AFCMDLS]"
},

["standByPenalty"] = {
  switch = function()
    if ball.posX() > topX and ball.posY() > 0 then --right top
      standPos = rightTop
    elseif ball.posX() > topX and ball.posY() < 0 then--left top
      standPos = leftTop
    elseif ball.posX() < topX and ball.posY() > 0 then--right side
      standPos = rightSide
    else --ballPosX() < topX and ballPosY() < 0 then--left side
      standPos = leftSide
    end
    --local ONE_POS, TWO_POS, THREE_POS, FOUR_POS, FIVE_POS, SIX_POS = penaltyStop()
    if cond.isGameOn() then
      return "exit"
    --elseif ball.toOurGoalDist() < PENALTY_THRESHOLD_DIST then
    elseif ball.toOurPenaltyDist() < PENALTY_THRESHOLD_DIST then
      if (math.abs(gBallPosXInStop - ball.posX()) >= 8 or math.abs(gBallPosYInStop - ball.posY()) >= 8) then
        gBallPosXInStop = ball.posX()
        gBallPosYInStop = ball.posY()
        return nil
      end
      if bufcnt( ((gRoleNum["Assister"] ~= 0) and (player.toTargetDist("Assister") > 8)) or
                 ((gRoleNum["Special"] ~= 0) and (player.toTargetDist("Special") > 8)) or
                 ((gRoleNum["Leader"] ~= 0) and (player.toTargetDist("Leader") > 8)) or
                 ((gRoleNum["Defender"] ~= 0) and (player.toTargetDist("Defender") > 8)) or
                 ((gRoleNum["Middle"] ~= 0) and (player.toTargetDist("Middle") > 8)), getBufTime()) then
        return "reDoStop"
      end
      if bufcnt( ((gRoleNum["Assister"] ~= 0) and (player.toTargetDist("Assister") < 10)) and
                 ((gRoleNum["Special"] ~= 0) and (player.toTargetDist("Special") < 10)) and
                 ((gRoleNum["Leader"] ~= 0) and (player.toTargetDist("Leader") < 10)) and
                 ((gRoleNum["Defender"] ~= 0) and (player.toTargetDist("Defender") < 10)) and
                 ((gRoleNum["Middle"] ~= 0) and (player.toTargetDist("Middle") < 10)), 15) then
        gBallPosXInStop = ball.posX()
        gBallPosYInStop = ball.posY()
        return nil
      end
    elseif ball.posY() < MIDDLE_THRESHOLD_Y - param.playerRadius and ball.posY() > -MIDDLE_THRESHOLD_Y + param.playerRadius then
      gBallPosXInStop = ball.posX()
      gBallPosYInStop = ball.posY()
      return "standInMiddle"
    elseif ball.posY() > MIDDLE_THRESHOLD_Y or ball.posY() < -MIDDLE_THRESHOLD_Y then
      gBallPosXInStop = ball.posX()
      gBallPosYInStop = ball.posY()
      return "standByLine"
    else
      return "exit"
    end
  end,

  Assister = task.goCmuRush(standFunc(1), dir.playerToBall, ACC, STOP_DSS),
  Special  = task.goCmuRush(standFunc(2), dir.playerToBall, ACC, STOP_DSS),
  Leader   = task.goCmuRush(standFunc(3), dir.playerToBall, ACC, STOP_DSS),
  Defender = task.goCmuRush(standFunc(4), dir.playerToBall, ACC, STOP_DSS),
  Middle   = task.goCmuRush(standFunc(5), dir.playerToBall, ACC, STOP_DSS),
  Fronter  = task.goCmuRush(standFunc(6), dir.playerToBall, ACC, STOP_DSS),
  Center   = task.stop(),--task.goCmuRush(SEVEN_POS, dir.playerToBall, ACC, STOP_DSS),
  Goalie   = task.goCmuRush(CGeoPoint:new_local(-540, 0), dir.playerToBall),--task.zgoalie(),
  match    = "[AMDLSFC]"
},

["standByLine"] = {
  switch = function()
    if cond.isGameOn() then
      return "exit"
    --elseif ball.toOurGoalDist() < PENALTY_THRESHOLD_DIST then
    elseif ball.toOurPenaltyDist() < PENALTY_THRESHOLD_DIST then
      gBallPosXInStop = ball.posX()
      gBallPosYInStop = ball.posY()
      return "standByPenalty"
    elseif ball.posY() < MIDDLE_THRESHOLD_Y and ball.posY() > -MIDDLE_THRESHOLD_Y then
      gBallPosXInStop = ball.posX()
      gBallPosYInStop = ball.posY()
      return "standInMiddle"
    elseif ball.posY() > MIDDLE_THRESHOLD_Y or ball.posY() < -MIDDLE_THRESHOLD_Y then
      if (math.abs(gBallPosXInStop - ball.posX()) >= 8 or math.abs(gBallPosYInStop - ball.posY()) >= 8) then
        gBallPosXInStop = ball.posX()
        gBallPosYInStop = ball.posY()
        return "standByLine"
      end
      if bufcnt( ((gRoleNum["Assister"] ~= 0) and  (player.toTargetDist("Assister") > 8)) or
                 ((gRoleNum["Special"] ~= 0) and (player.toTargetDist("Special") > 8)) or
                 ((gRoleNum["Leader"] ~= 0) and (player.toTargetDist("Leader") > 8)) or
                 ((gRoleNum["Defender"] ~= 0) and (player.toTargetDist("Defender") > 8)) or
                 ((gRoleNum["Middle"] ~= 0) and (player.toTargetDist("Middle") > 8)), getBufTime()) then
        return "reDoStop"
      end
      if bufcnt( ((gRoleNum["Assister"] ~= 0) and  (player.toTargetDist("Assister") < 10)) and
                 ((gRoleNum["Special"] ~= 0) and (player.toTargetDist("Special") < 10)) and
                 ((gRoleNum["Leader"] ~= 0) and (player.toTargetDist("Leader") < 10)) and
                 ((gRoleNum["Defender"] ~= 0) and (player.toTargetDist("Defender") > 10)) and
                 ((gRoleNum["Middle"] ~= 0) and (player.toTargetDist("Middle") > 10)), 15) then
        gBallPosXInStop = ball.posX()
        gBallPosYInStop = ball.posY()
        return nil
      end
    else
      return "exit"
    end
  end,

  Assister = task.goCmuRush(MIDDLE_POS, dir.playerToBall, ACC, STOP_DSS),
  Special  = task.goCmuRush(INTER_POS, dir.playerToBall, ACC, STOP_DSS),
  Leader   = task.goCmuRush(SIDE_POS, dir.playerToBall, ACC, STOP_DSS),
  Fronter  = task.goCmuRush(OTHER_SIDE_POS, dir.playerToBall, ACC, STOP_DSS),--task.goCmuRush(SIDE2_POS, dir.playerToBall, ACC, STOP_DSS),--task.defendMiddle4Stop(),--TODO
  Center   = task.stop(),
  Defender = task.stop(),
  Middle   = task.stop(),
  Goalie   = task.stop(),
  match    = "[AFCMDLS]"
},

["reDoStop"] = {
  switch = function()
    if bufcnt(true, 20) then
      return "start"
    end
  end,

  Assister = task.goCmuRush(ball.refAntiYPos(CGeoPoint:new_local(-140,-120)), dir.playerToBall, ACC, STOP_DSS),
  Special  = task.goCmuRush(ball.refAntiYPos(CGeoPoint:new_local(-140, 120)), dir.playerToBall, ACC, STOP_DSS),
  Leader   = task.goCmuRush(CGeoPoint:new_local(-120, 0), dir.playerToBall, ACC, STOP_DSS),
  Defender = task.goCmuRush(CGeoPoint:new_local(-290, -130), dir.playerToBall, ACC, STOP_DSS),
  Middle   = task.goCmuRush(CGeoPoint:new_local(-290, 130), dir.playerToBall, ACC, STOP_DSS),
  Fronter  = task.goCmuRush(CGeoPoint:new_local(-290,260),dir.playerToBall,ACC,STOP_DSS),
  Center   = task.goCmuRush(CGeoPoint:new_local(-290,-260),dir.playerToBall,ACC,STOP_DSS),
  Goalie   = task.stop(),
  match    = "[AMDLSFC]"
},

name = "Ref_StopV1",
applicable = {
  exp = "a",
  a = true
},
attribute = "attack",
timeout = 99999
}