module(..., package.seeall)

function instance(role)
	local realIns
	if type(role) == "string" then
		realIns = vision:ourPlayer(gRoleNum[role])
	elseif type(role) == "number" then
--	and	role >= 1 and role <= param.maxPlayer then
		realIns = vision:ourPlayer(role)
	else
		print("Invalid role in player.instance!!!2222222")
		return nil
	end
	return realIns
end

function num(role)
	local retNum
	if type(role) == "string" then
		retNum = gRoleNum[role]
	elseif type(role) == "number" then
		retNum = role
	else
		print("Invalid role in player.instance!!!11111")
	end
	return retNum
end

function pos(role)
	return instance(role):Pos()
end

function posX(role)
	return instance(role):X()
end

function posY(role)
	return instance(role):Y()
end

function dir(role)
	return instance(role):Dir()
end

function vel(role)
	return instance(role):Vel()
end

function velDir(role)
	return vel(role):dir()
end

function velMod(role)
	return vel(role):mod()
end

function rawVel(role)
	return instance(role):RawVel()
end

function rawVelMod(role)
	return rawVel(role):mod()
end

function rotVel(role)
	return instance(role):RotVel()
end

function valid(role)
	return instance(role):Valid()
end

function toBallDist(role)
	return pos(role):dist(ball.pos())
end

function toBallDir(role)
	return (ball.pos() - pos(role)):dir()
end

function backBallDir(role)
	return (pos(role) - ball.pos()):dir()
end

function toTheirGoalDist(role)
	return pos(role):dist(CGeoPoint:new_local(param.pitchLength / 2.0, 0))
end

function toOurGoalDist(role)
	return pos(role):dist(CGeoPoint:new_local(-param.pitchLength / 2.0, 0))
end

function toTheirGoalDir(role)
	return (CGeoPoint:new_local(param.pitchLength / 2.0, 0) - pos(role)):dir()
end

function toPlayerDir(role1, role2)
	if role2 == nil then
		return function(role2)
			return (player.pos(role1) - player.pos(role2)):dir()
		end
	else
		return (player.pos(role2) - player.pos(role1)):dir()
	end
end

function toPlayerHeadDir(role1, role2)
	if role2 == nil then
		return function(role2)
			local tmpPlayerHead = player.pos(role1) + Utils.Polar2Vector(8, player.dir(role1))
			return (tmpPlayerHead - player.pos(role2)):dir()
		end
	else
		local tmpPlayerHead = player.pos(role2) + Utils.Polar2Vector(8, player.dir(role2))
		return (tmpPlayerHead - player.pos(role1)):dir()
	end
end

function toPlayerDist(role1, role2)
	if role2 == nil then
		return function(role2)
			return (player.pos(role1) - player.pos(role2)):mod()
		end
	else
		return (player.pos(role2) - player.pos(role1)):mod()
	end
end

function toTargetTime(p,role)
	if role == nil then
		return function(role)
			local target
			if type(p) == "function" then
				target = p()
			else
				target = p
			end
			return world:timeToTarget(player.num(role), target) * param.frameRate
		end
	else
		local target
		if type(p) == "function" then
			target = p()
		else
			target = p
		end
		return world:timeToTarget(player.num(role), target) * param.frameRate
	end
end

function toTargetDist(role)
	local p
	if type(gRolePos[role]) == "function" then
		p = gRolePos[role]()
	else
		p = gRolePos[role]
	end
	return player.pos(role):dist(p)
end

function toTargetDir(p, role)
	if role == nil then
		return function(role)
			return (p - player.pos(role)):dir()
		end
	else
		return function()
			return (p - player.pos(role)):dir()
		end
	end
end
--need to modify
function toTheirPenaltyDist(role)
	local tmpToGoalDist = (CGeoPoint:new_local(param.pitchLength/2.0,0) - player.pos(role)):mod()
	return tmpToGoalDist - 80
end

function toPointDist(role, p)
	local pos
	if type(p) == "function" then
		pos = p()
	else
		pos = p
	end
	return player.pos(role):dist(pos)
end

-- 用在冲球时求出传球点后面的一个点
function backShootPos(p)
	local tmpShootDir = (p - CGeoPoint:new_local(param.pitchLength/2.0,0)):dir()
	return p + Utils.Polar2Vector(9, tmpShootDir)
end

function toPointDir(p, role)
	if role == nil then
		if type(p) == "function" then
			return function ( role1 )
				return (p() - player.pos(role1)):dir()
			end
		else
			return function ( role1 )
				return (p - player.pos(role1)):dir()
			end
		end

	else
		return (p - player.pos(role)):dir()
	end
end

function kickBall(role)
	return world:IsBallKicked(num(role))
end

function infraredOn( role )
	return world:IsInfraredOn(num(role))
end

function infraredCount( role )
	return world:InfraredOnCount(num(role))
end
-- role1为传球车
function toShootOrRobot(role1)
	return function(role2)
		local shootDir = ( CGeoPoint:new_local(param.pitchLength / 2.0, 0) - pos(role2) ):dir()
		local faceDir
		if toBallDist(role1) > 50 then
			faceDir = (ball.pos() - pos(role2)):dir()
		else
			faceDir = (pos(role1) - pos(role2)):dir()
		end
		if math.abs(Utils.Normalize(shootDir - faceDir)) > math.pi * 65 / 180 then
			return faceDir
		else
			return shootDir
		end
	end
end

function canBreak(role)
	for i=1,param.maxPlayer do
		if enemy.valid(i) then
			local p
			if type(gRolePos[role]) == "function" then
				p = gRolePos[role]()
			else
				p = gRolePos[role]
			end
			local breakSeg = CGeoSegment:new_local(player.pos(role), p)
			local projP = breakSeg:projection(enemy.pos(i))
			if breakSeg:IsPointOnLineOnSegment(projP) then
				if enemy.pos(i):dist(projP) < 40 then
					return false
				end
			end
		end
	end

	return true
end

function isMarked(role)
	local closestDist = 9999
	for i=1,param.maxPlayer do
		if enemy.valid(i) then
			local dir1 = player.toPointDir(CGeoPoint:new_local(param.pitchLength / 2.0, 0),role)
			local dirDiff = Utils.Normalize( dir1- player.toPointDir(enemy.pos(i),role))
			if math.abs(dirDiff) < math.pi/2 then
				local tmpDist = player.toPointDist(role,enemy.pos(i))
				if tmpDist < closestDist then
					closestDist = tmpDist
				end
			end
		end
	end

	if closestDist < 30 then
		return true
	end
	return false
end

function testPassPos(role)
	local factor
	return function ()
		if posX(role) > 0 then
			factor = -1
		else
			factor = 1
		end

		return CGeoPoint:new_local(150*factor, 0)
	end
end

function canFlatPassTo(role1, role2)
	local p1 = player.pos(role1)
	local p2 = player.pos(role2)
	local seg = CGeoSegment:new_local(p1, p2)
	for i = 1, param.maxPlayer do
		if enemy.valid(i) then
			local dist = seg:projection(enemy.pos(i)):dist(enemy.pos(i))
			local isprjon = seg:IsPointOnLineOnSegment(seg:projection(enemy.pos(i)))
			if dist < 12 and isprjon then
				return false
			end
		end
	end

	return true
end
--for autoball placement
function ifBlockBallLine( role, originpos, targetpos)
	local p1
	local p2
	if type(originpos) == "function" then
		p1 = originpos()
	else
		p1 = originpos
	end
	if type(targetpos) == "function" then
		p2 = targetpos()
	else
		p2 = targetpos
	end
	local seg = CGeoSegment:new_local(p1,p2)
	local dist = seg:projection(player.pos(role)):dist(player.pos(role))
	local isprjon = seg:IsPointOnLineOnSegment(seg:projection(player.pos(role)))
	--print (role , dist,isprjon)
	if dist < 20 and isprjon then
		return true
	end
	return false
end

--for dynamicKick
function canFlatPassToPos(role, targetpos)
	local p1 = player.pos(role)
	local p2
	if type(targetpos) == "function" then
		p2 = targetpos()
	else
		p2 = targetpos
	end
	local seg = CGeoSegment:new_local(p1, p2)
	for i = 1, param.maxPlayer do
		if enemy.valid(i) then
			local dist = seg:projection(enemy.pos(i)):dist(enemy.pos(i))
			local isprjon = seg:IsPointOnLineOnSegment(seg:projection(enemy.pos(i)))
			if dist < 15 and isprjon then
				return false
			end
		end
	end
	for j = 1, param.maxPlayer do
		if player.valid(j) and j ~= gRoleNum["Leader"] and player.pos(j):dist(p2) > 20 then
			local dist = seg:projection(player.pos(j)):dist(player.pos(j))
			local isprjon = seg:IsPointOnLineOnSegment(seg:projection(player.pos(j)))
			if dist < 12 and isprjon then
				return false
			end
		end
	end
	return true
end

-- d为前方的距离 proj_d是对方车投影到射门线的距离阈值
function canDirectShoot(role1, d, proj_d)
	if d == nil then
		d = 70
	end
	if proj_d == nil then
		proj_d = 12
	end
	local p1 = player.pos(role1)
	local p2 = player.pos(role1) + Utils.Polar2Vector(d,player.dir(role1))
	local seg = CGeoSegment:new_local(p1, p2)
	for i = 1, param.maxPlayer do
		if enemy.valid(i) then
			local dist = seg:projection(enemy.pos(i)):dist(enemy.pos(i))
			local isprjon = seg:IsPointOnLineOnSegment(seg:projection(enemy.pos(i)))
			if dist < proj_d and isprjon then
				return false
			end
		end
	end

	return true
end

function canChipPassTo(role1, role2)
	local p1 = player.pos(role1)
	local p2 = player.pos(role2)
	for i = 1, param.maxPlayer do
		if enemy.valid(i) then
			local dist1 = enemy.pos(i):dist(p1)
			local dist2 = enemy.pos(i):dist(p2)
			if dist1 < 30 or dist2 < 30 then
				return false
			end
		end
	end

	return true
end

------------------------------------------------------
function isBallPassed(role1,role2)
	local p1 = player.pos(role1)
	local p2 = player.pos(role2)
	local ptrDir = ( p2 - p1 ):dir()
	if (math.abs(Utils.Normalize(ball.velDir() - ptrDir)) < math.pi / 18) and
	   (ball.velMod() > 80) then
		return true
	else
		return false
	end
end

function isBallPassedNormalPlay(role1,role2)
	local passerDir = player.dir(role1)
	local p1 = player.pos(role1)
	local p2 = player.pos(role2)
	local ptrDir = ( p2 - p1 ):dir()
	if (math.abs(Utils.Normalize(ball.velDir() - ptrDir)) < math.pi / 18) and
	   (math.abs(Utils.Normalize(ball.velDir() - passerDir)) < math.pi / 10) and
	   (ball.velMod() > 160)  then
		return true
	else
		return false
	end
end

function passIntercept(role)
	local receiver = player.pos(role)
	local ptrDir = ( receiver - ball.pos()):dir()
	if ball.toPointDist(receiver) >50 then
		if math.abs(Utils.Normalize(ball.velDir() - ptrDir)) > math.pi / 10 or
			ball.velMod() < 120 then
			return true
		else
			return false
		end
	else
		return false
	end
end


function InfoControlled(role)
	local roleNum = player.num(role)
	--local isVisionBallControlled = skillUtils:withBall() and (skillUtils:getOurBestPlayer() == roleNum)
	--print("withBall: ", skillUtils:withBall())
	--print("BestPlayer: ", skillUtils:getOurBestPlayer() == roleNum)
	--if skillUtils:getOurBestPlayer() == roleNum then
	--	return true
	--else
	--	return false
	--end
	return skillUtils:getOurBestPlayer() == roleNum
end
------------------------------------------------------

-- p为传入的点
function antiYDir(p)
	return function (role)
		if type(p) == "userdata" then
			return (ball.antiYPos(p)() - player.pos(role)):dir()
		end
	end
end

-- function realNumExist(n)
-- 	if player.valid(CGetStrategyNum(n)) then
-- 		return true
-- 	end
-- 	return false
-- end

function faceball2target(role,t,diff)
  local target
  local d

  if diff == nil then
  	d = 0.2
  elseif type(diff) == "function" then
    d = diff()
  else
    d = diff
  end

  if type(t) == "function" then
    target = t()
  else
    target = t
  end

  local temp = ball.toPointDir(target)
   -- print(temp())
   --  print(player.dir(role))
   if math.abs(temp()-player.dir(role))<=d or math.abs(temp()-player.dir(role))>=6.28-d then
    return true
  else
    return false
  end
end

function faceball2target(role,t,diff)
  local target
  local d

  if diff == nil then
  	d = 0.2
  elseif type(diff) == "function" then
    d = diff()
  else
    d = diff
  end

  if type(t) == "function" then
    target = t()
  else
    target = t
  end

  local temp = ball.toPointDir(target)
   -- print(temp())
   --  print(player.dir(role))
   if math.abs(temp()-player.dir(role))<=d or math.abs(temp()-player.dir(role))>=6.28-d then
    return true
  else
    return false
  end
end

function isInForbiddenZone4ballplace(role)
  local thereShouldDist=60--规则要求50cm，这里设定大于规则
  local p1= CGeoPoint:new_local(ball.placementPos():x(), ball.placementPos():y())
  local p2 =ball.pos()
  local seg = CGeoSegment:new_local(p1,p2)
  local dist = seg:projection(player.pos(role)):dist(player.pos(role))
  local isprjon = seg:IsPointOnLineOnSegment(seg:projection(player.pos(role)))
   if player.toBallDist(role)<=thereShouldDist or player.toPointDist(role,p1)<=thereShouldDist or (dist<=thereShouldDist and isprjon) then
     return true
  else
    return false
  end
end

function stayPos4ballplace(role)
		local thereShouldDist=70
        local myposX
        local myposY
        local TargetPos = function ()
        	return CGeoPoint:new_local(ball.placementPos():x(), ball.placementPos():y())
		end
        local getPos = function()
                if player.isInForbiddenZone4ballplace(role) then
                   local seg = CGeoSegment:new_local(ball.pos(),TargetPos())
                   local projectPoint=CGeoPoint:new_local(seg:projection(player.pos(role)):x(),seg:projection(player.pos(role)):y())
                    myposX = (projectPoint + Utils.Polar2Vector(-thereShouldDist,player.toPointDir(projectPoint,role))):x()
                    myposY =(projectPoint + Utils.Polar2Vector(-thereShouldDist,player.toPointDir(projectPoint,role))):y()
                    if ((math.abs(myposX)>(param.pitchLength/2-param.penaltyDepth)) and 
                      math.abs(myposY)<(param.penaltyWidth/2))
                      or (math.abs(myposX) > param.pitchLength/2) 
                      or (math.abs(myposY) > param.pitchWidth/2) 
                      then 
                      myposX = (projectPoint + Utils.Polar2Vector(thereShouldDist,player.toPointDir(projectPoint,role))):x()
                      myposY =(projectPoint + Utils.Polar2Vector(thereShouldDist,player.toPointDir(projectPoint,role))):y()
                    end
                else
                        myposX = player.posX(role)
                        myposY = player.posY(role)

                end
        end
        local compute = function()
                getPos()
                return CGeoPoint:new_local(myposX,myposY)
        end
        return compute
end