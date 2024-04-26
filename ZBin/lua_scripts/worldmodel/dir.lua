module(..., package.seeall)

function specified(num)
	return function()
		return num * math.pi / 180
	end
end

---
function ballToPoint(p)
	local tp
	if type(p)=="function" then
		tp = p()
	else
		tp = p
	end
	return (tp - ball.pos()):dir()
end
---
function playerToPoint(p,role)
	if role == nil then
		print("Invalid Role in ourPlayerToBall: ", role)
	end
	local tp
	if type(p)=="function" then
		tp = p()
	else
		tp = p
	end
	return (tp - player.pos(role)):dir()
end

function ballToOurGoal()
	return (pos.ourGoal() - ball.pos()):dir()
end

function posToTheirGoal(p)
	return (pos.theirGoal() - p):dir()
end

function posToOurGoal(p)
	return (pos.ourGoal() - p):dir()
end

function  theirGoalToBall()
	return (ball.pos() - pos.theirGoal()):dir()
end

function ourGoalToBall()
	return (ball.pos() - pos.ourGoal()):dir()
end

function playerToBall(role)
	if role == nil then
		print("Invalid Role in ourPlayerToBall: ", role)
	end
	return (ball.pos() - player.pos(role)):dir()
end

function ballToPlayer(role)
	if role == nil then
		print("Invalid Role in ourPlayerToBall: ", role)
	end
	return (player.pos(role) - ball.pos()):dir()
end

function ourPlayerToPlayer(role1, role2)
	if role2 == nil then
		return function(role2)
			return (player.pos(role1) - player.pos(role2)):dir()
		end
	else
		return (player.pos(role2) - player.pos(role1)):dir()
	end
end

function shoot()
	return function (role)
		return posToTheirGoal(player.pos(role))
	end
end

function evaluateTouch(p)
	local lastCycle = {}
	local lastDir = {}

	for i = 0, param.maxPlayer do
		lastCycle[i] = 0
		lastDir[i] = 0
	end
	return function (role)
		if type(role) == "string" then
			role = gSubPlay.getRoleNum(role)
		elseif type(role) == "number" and
			role >= 1 and role <= param.maxPlayer then
			role = role
		else
			print("Error role in dir.shoot")
		end
		if vision:getCycle() - lastCycle[role] > 6 or player.toBallDist(role) > 50 then
			local tmpRawDir
			if p == nil then
				kickDirection:GenerateShootDir(role, player.pos(role))
				tmpRawDir = kickDirection:getRawKickDir()
			else
				if type(p) == "function" then
					p = p()
				end
				tmpRawDir = player.toPointDir(p,role)
			end

			local tmpTotalAngle = (tmpRawDir - player.toBallDir(role))*180/math.pi
			local tmpAbsValue = math.abs(tmpTotalAngle)
			local tmpEvaluateValue = 0.0008*tmpAbsValue*tmpAbsValue + 0.1145*tmpAbsValue
			if tmpTotalAngle <= 0 then
				lastDir[role] = tmpRawDir + tmpEvaluateValue*math.pi/180
			else
				lastDir[role] = tmpRawDir - tmpEvaluateValue*math.pi/180
			end

		end
		lastCycle[role] = vision:getCycle()
		return lastDir[role]
	end
end

-- p为传入的目标点
function compensate(p)
	local lastCycle = {}
	local lastDir = {}

	for i = 0, param.maxPlayer do
		lastCycle[i] = 0
		lastDir[i] = 0
	end

	return function (role)
		local ipos
		if type(p) == "function" then
			ipos = p()
		else
			ipos = p
		end

		if vision:getCycle() - lastCycle[role] > 6 or player.toBallDist(role) > 50 then
			lastDir[role] = CCalCompensateDir(player.num(role), ipos:x(), ipos:y())
		end
		lastCycle[role] = vision:getCycle()
		return lastDir[role]
	end
end

-- 无角度补偿的射门方向，p为传入的目标点
function nocompensation(p)
	local dir = {}

	for i = 0, param.maxPlayer do
		dir[i] = 0
	end

	return function (role)
		local ipos
		if type(p) == "function" then
			ipos = p()
		else
			ipos = p
		end
		dir[role] = (p - player.pos(role)):dir()
		return dir[role]
	end
end

function chase(role)
	kickDirection:GenerateShootDir(player.num(role), player.pos(role))
	return kickDirection:getRawKickDir()
end

function backBall(p)
	return function ()
		local targetP = CGeoPoint:new_local(p:x(), ball.antiY()*p:y())
		return  Utils.Normalize((targetP - ball.pos()):dir())
	end
end

function fakeDown(p)
	local factor

	return function ()
		if ball.posY() > 10 then
			factor = -1
		else
			factor = 1
		end

		local targetP = CGeoPoint:new_local(p:x(), factor * p:y())
		local faceVec = (targetP - ball.pos()):rotate(factor * math.pi * 120 / 180)
		return faceVec:dir()
	end
end

function defendBackClear()
	local targetdir = 0
	local angle_left = 0
	local angle_right = 0
	local NearGoalDist = -param.pitchLength/2 + 10
	return function()
		if ball.posY() >= -param.goalWidth/2 and ball.posY() <= param.goalWidth/2 then
			if ball.posX() > NearGoalDist then
				angle_left = -math.pi/2
				angle_right = math.pi/2
			else
				angle_left = -math.pi/3
				angle_right = math.pi/3
			end
		elseif ball.posY()<-param.goalWidth/2 then
			angle_left = - math.pi*2/3
			angle_right = math.pi/2
		elseif ball.posY()>param.goalWidth/2 then
			angle_left = -math.pi/2
			angle_right = math.pi*2/3
		end
		--debugEngine:gui_debug_line(ball.pos(),ball.pos()+Utils.Polar2Vector(150,(angle_left+angle_right)/2))
		return (angle_left+angle_right)/2
	end
end

function defendMiddleClear(role)
	local angle_left = 0
	local angle_right = 0
	local NearGoalDist = -param.pitchLength/2+10
	local temp = 0

	return function()
		if ball.posX()<-140 then
			if ball.posY() > -param.goalWidth/2 and ball.posY() < param.goalWidth/2 then
				if ball.posX()>NearGoalDist then
					angle_left = -math.pi/3
					angle_right = math.pi/3
				else
					angle_left = -math.pi/4
					angle_right = math.pi/4
				end
			elseif ball.posY() < -param.goalWidth then
				angle_left = -math.pi*2/3
				angle_right = math.pi/3
			elseif ball.posY() > param.goalWidth then
				angle_left = -math.pi/3
				angle_right = math.pi*2/3
			end
			temp = (angle_left +angle_right)/2*180/math.pi
			return (angle_left +angle_right)/2
		else
			temp = player.toBallDir(role)*180/math.pi
			return player.toBallDir(role)
		end
	end
end

function backSmartGotoDir()
	local oppnum = skillUtils:getTheirBestPlayer()
	if Utils.InOurPenaltyArea(ball.pos(),5) and enemy.posX(oppnum)<0 then
		return (enemy.pos(oppnum) - pos.ourGoal()):dir()
	else
		return (ball.pos()- pos.ourGoal()):dir()
	end
end

function sideBackDir()
	return (pos.sideBackPos()- pos.ourGoal()):dir()
end

function reflectDir(d)
	return ball.refSyntYDir(d)
end

-- 利用图像使车消失的点. 传入第一辆车的位置和角度,返回第二辆(会消失的那辆)的角度,位置在pos.lua里,
-- i表示用车的左边顶还是右边顶,-1表示左边,1表示右边
function disappearDir(pos, dir, i)
	if i == -1 then
		if type(dir) == "function" then
			return function ()
				return Utils.Normalize(dir() - (ball.posY() > 0 and 1 or -1) * math.pi * 3 / 4)
			end
		else
		  return Utils.Normalize(dir - (ball.posY() > 0 and 1 or -1) * math.pi * 3 / 4)
		end
	elseif i == 1 then
		if type(dir) == "function" then
			return function ()
				return Utils.Normalize(dir() - (ball.posY() > 0 and -1 or 1) * math.pi * 3 / 4)
			end
		else
			return Utils.Normalize(dir - (ball.posY() > 0 and -1 or 1) * math.pi * 3 / 4)
		end
	end
end

function dirForDribbleTurnKick()
	local  rotAngle = 0.6
	local  angle
	if ball.posY()>0 then
		angle=ball.toTheirGoalDir()-rotAngle
	else
		angle=ball.toTheirGoalDir()+rotAngle
	end
	return Utils.Normalize(angle)
end