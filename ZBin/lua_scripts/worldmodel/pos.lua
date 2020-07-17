module(..., package.seeall)

function specified(p)
	return function ()
		return p
	end
end

function LEADER_STOP_POS()
	local dir1 = (theirGoal() - ball.pos()):dir()
	local dir2 = (ball.pos() - ourGoal()):dir()
	local x1 = param.pitchLength/2
	local x2 = param.pitchLength/3
	local x = ball.posX()
	if x > x1 then
		idir = dir1
	elseif x < x2 then
		idir = dir2
	else
		idir = dir1 + (dir2-dir1)*(x-x1)/(x2-x1)
	end
	return ball.pos() + Utils.Polar2Vector(-53, idir)
end

function ourGoal()
	return CGeoPoint:new_local(-param.pitchLength / 2.0, 0)
end

function theirGoal()
	return CGeoPoint:new_local(param.pitchLength / 2.0, 0)
end

function refStopAroundBall()
	local BLOCK_DIST = param.freeKickAvoidBallDist + param.playerRadius
	local AWAY_DIST = 2.5 + param.playerRadius
	local BLOCK_ANGLE = math.asin(AWAY_DIST / BLOCK_DIST)*2
	local factor = ball.antiY

	local SIDE_POS = function()
		return ball.pos() + Utils.Polar2Vector(BLOCK_DIST, dir.ballToOurGoal() + factor()*BLOCK_ANGLE)
	end

	local INTER_POS = function()
		return ball.pos() + Utils.Polar2Vector(BLOCK_DIST, dir.ballToOurGoal() - factor()*BLOCK_ANGLE)
	end

	local MIDDLE_POS = function()
		return ball.pos() + Utils.Polar2Vector(BLOCK_DIST, dir.ballToOurGoal())
	end

	local SIDE2_POS = function()
		return ball.pos() + Utils.Polar2Vector(BLOCK_DIST, dir.ballToOurGoal() + 2*factor()*BLOCK_ANGLE)
	end

	local INTER2_POS = function()
		return ball.pos() + Utils.Polar2Vector(BLOCK_DIST, dir.ballToOurGoal() - 2*factor()*BLOCK_ANGLE)
	end

	return SIDE_POS, MIDDLE_POS, INTER_POS, SIDE2_POS, INTER2_POS
end

function backBall(p)
	return function ()
		local targetP = CGeoPoint:new_local(p:x(), ball.antiY()*p:y())
		return  ball.pos() + Utils.Polar2Vector(18, Utils.Normalize((ball.pos() - targetP):dir()))
	end
end

-- function playerBest()
-- 	local robotNum = skillUtils:getOurBestPlayer()
-- 	if robotNum >0 and robotNum <= param.maxPlayer then
-- 		return player.pos(robotNum)
-- 	else
-- 		return ball.pos()
-- 	end
-- end

function fakeDown(p)
	local factor

	return function ()
		if ball.posY() > 10 then
			factor = -1
		else
			factor = 1
		end
		local targetP = CGeoPoint:new_local(p:x(), factor * p:y())
		local standVec = (targetP - ball.pos()):rotate(factor * math.pi * 100 / 180)
		return ball.pos() + Utils.Polar2Vector(30, standVec:dir())
	end
end

-- function multiBackPos(guardNum, index)
-- 	return function()
-- 		return guardpos:backPos(guardNum, index)
-- 	end
-- end

-- function leftBackPos()
-- 	return defPos2015:getDefPos2015(vision):getLeftPos()
-- end

-- function rightBackPos()
-- 	return defPos2015:getDefPos2015(vision):getRightPos()
-- end

-- function defendMiddlePos()
-- 	return defPos2015:getDefPos2015(vision):getMiddlePos()
-- end

-- function singleBackPos()
-- 	return defPos2015:getDefPos2015(vision):getSinglePos()
-- end

-- function sideBackPos()
-- 	return defPos2015:getDefPos2015(vision):getSidePos()
-- end

-- function goaliePos()
-- 	return defPos2015:getDefPos2015(vision):getGoaliePos()
-- end

-- function getSupportPos(role)
-- 	return function()
-- 		local p = supportPos:getSupPos(vision,gRoleNum[role])
-- 		return p
-- 	end
-- end

-- function getSupportPosNew(role1,role2)
-- 	return function()
-- 		local p = supportPos:getSupPosNew(vision,gRoleNum[role1],gRoleNum[role2])
-- 		return p
-- 	end
-- end

-- function getAssistPoint(role1,role2)
-- 	return function()
-- 		local p = assistPoint:getBestAssistPoint(vision,gRoleNum[role1],gRoleNum[role2]);
-- 		return p
-- 	end 
-- end

function advance()
	if false then
		return ball.pos()
	else
		local robotNum = skillUtils:getOurBestPlayer()
		if Utils.PlayerNumValid(robotNum) then
			return player.pos(robotNum)
		else
			return ball.pos()
		end
	end
end

-- function tandemPos2013()
-- 	return  tandemPos:getTandemPos()
-- end


function oneKickDefPos(p1,p2,p3,p4,p5,p6)
	return function()
		if type(p1) == "function" then
			mp1 = p1()
		else
			mp1 = p1
		end
		if type(p2) == "function" then
			mp2 = p2()
		else
			mp2 = p2
		end
		if type(p3) == "function" then
			mp3 = p3()
		else
			mp3 = p3
		end
		if type(p4) == "function" then
			mp4 = p4()
		else
			mp4 = p4
		end
		if type(p5) == "function" then
			mp5 = p5()
		else
			mp5 = p5
		end
		if type(p6) == "function" then
			mp6 = p6()
		else
			mp6 = p6
		end
		return indirectDefender:getTwoDefPos(vision,mp1,mp2,mp3,mp4,mp5,mp6):getOnePos()
	end
end

function anotherKickDefPos(p1,p2,p3,p4,p5,p6)
	return function()
		if type(p1) == "function" then
			mp1 = p1()
		else
			mp1 = p1
		end
		if type(p2) == "function" then
			mp2 = p2()
		else
			mp2 = p2
		end
		if type(p3) == "function" then
			mp3 = p3()
		else
			mp3 = p3
		end
		if type(p4) == "function" then
			mp4 = p4()
		else
			mp4 = p4
		end
		if type(p5) == "function" then
			mp5 = p5()
		else
			mp5 = p5
		end
		if type(p6) == "function" then
			mp6 = p6()
		else
			mp6 = p6
		end
		--print(mp1,mp2:x(),mp3:x(),mp4,mp5:x(),mp6:x())
		return indirectDefender:getTwoDefPos(vision,mp1,mp2,mp3,mp4,mp5,mp6):getAnotherPos()
	end
end

function testTwoKickOffPos1()
	local BLOCK_DIST = 30
	return function ()
		return ball.pos() + Utils.Polar2Vector(BLOCK_DIST, dir.theirGoalToBall())
	end
end

function testTwoKickOffPos2()
	local BLOCK_DIST = 30
	local AWAY_DIST = 7.5 + param.playerRadius
	local BLOCK_ANGLE = math.asin(AWAY_DIST / BLOCK_DIST)*2
	local factor = ball.antiY
	return function()
		return ball.pos() + Utils.Polar2Vector(BLOCK_DIST, dir.theirGoalToBall()+ factor()*BLOCK_ANGLE)
	end
end

function theirBestPlayer()
	local oppNum = skillUtils:getTheirBestPlayer()
	return enemy.pos(oppNum)
end

function getTandemPos(role)
	return function ()
		return world:getTandemPos(gRoleNum[role])
	end
end

-- x, y分别为相对于球的偏移量, yys 2014-06-11
function reflectPos(x, y)
	return function ()
	    if ball.posY()<0 then 
			return (ball.refAntiYPos(CGeoPoint:new_local(ball.posX() + x, -(math.abs(ball.posY()) + y) )))()
		end
		if ball.posY()>=0 then 
			return (ball.refSyntYPos(CGeoPoint:new_local(ball.posX() + x, math.abs(ball.posY()) + y )))()	
		end
	end
end

function reflectPassPos(role)
	return function ()
		return world:getReflectPos(gRoleNum[role])
	end
end

-- y表示在距离中线多远的地方做touch, yys 2014-06-20
function reflectTouchPos(role, y)
	return function ()
		return world:getReflectTouchPos(gRoleNum[role], y)
	end
end

-- 定位球传球点, yys 2014-06-16
function generateFreeKickPassPos(role)
	return function ()
		return ( player.pos(role) + Utils.Polar2Vector(param.playerRadius + 2, dir.shoot()(role)) )
	end
end

-- 根据touch所跑的跑位点, 计算需要传球的点 zhyaic 2014.6.28
-- p为传入的目Touch跑位点, p可以为返回值为坐标点的函数
function passForTouch(p)
	local length = 9.5
	--local length = 9.33 -- 2017年伊朗赛前测量
	-- local length = 9.33+2 --gty改
	if type(p) == "function" then
		return function ()
			return p() + Utils.Polar2Vector(length, dir.posToTheirGoal(p()))
		end
	else
		return p + Utils.Polar2Vector(length, dir.posToTheirGoal(p))
	end
end

function chipPassForTouch(p)
	local length = 10
	--local length = 9.33 -- 2017年伊朗赛前测量
	-- local length = 9.33+2 --gty改
	if type(p) == "function" then
		return function ()
			return p() + Utils.Polar2Vector(length, dir.posToTheirGoal(p()))
		end
	else
		return p + Utils.Polar2Vector(length, dir.posToTheirGoal(p))
	end
end

function passForHead(p)
	if type(p) == "function" then
		return function ()
			return p() + Utils.Polar2Vector(1, dir.posToTheirGoal(p()))
		end
	else
		return p + Utils.Polar2Vector(1, dir.posToTheirGoal(p))
	end
end

-- 利用图像使车消失的点. 传入第一辆车的位置和角度,返回第二辆(会消失的那辆)的坐标,角度在dir.lua里,
-- i表示用车的左边顶还是右边顶,-1表示左边,1表示右边(球在右边，若球在右边则-1表示右边,1表示左边)
-- 若pos为函数,dir最好也是函数
function disappearPos(pos, dir, i)
	if i == -1 then
		if type(pos) == "function" then
			if type(dir) == "function" then
				return function ()
					return pos() + Utils.Polar2Vector(15.0, Utils.Normalize(dir() + (ball.posY() > 0 and 1 or -1) * math.pi / 4))
				end
			else
				return function ()
					return pos() + Utils.Polar2Vector(15.0, Utils.Normalize(dir + (ball.posY() > 0 and 1 or -1) * math.pi / 4))
				end
			end
		else
			if type(dir) == "function" then
				return pos + Utils.Polar2Vector(15.0, Utils.Normalize(dir() + (ball.posY() > 0 and 1 or -1) * math.pi / 4))
			else
				return pos + Utils.Polar2Vector(15.0, Utils.Normalize(dir + (ball.posY() > 0 and 1 or -1) * math.pi / 4))
			end
		end
	elseif i == 1 then
		if type(pos) == "function" then
			if type(dir) == "function" then
				return function ()
					return pos() + Utils.Polar2Vector(15.0, Utils.Normalize(dir() + (ball.posY() > 0 and -1 or 1) * math.pi / 4))
				end
			else
				return function ()
					return pos() + Utils.Polar2Vector(15.0, Utils.Normalize(dir + (ball.posY() > 0 and -1 or 1) *  math.pi / 4))
				end
			end
		else
			if type(dir) =="function" then
				return pos + Utils.Polar2Vector(15.0, Utils.Normalize(dir() + (ball.posY() > 0 and -1 or 1) *  math.pi / 4))
			else
				return pos + Utils.Polar2Vector(15.0, Utils.Normalize(dir + (ball.posY() > 0 and -1 or 1) *  math.pi / 4))
			end
		end
	end
end


function getReceivePos()
	local rPos = function ()
		return messi:receiverPos()
	end
	return rPos
end

function getLeaderWaitPos()
	local lPos = function ()
		return messi:leaderWaitPos()
	end
	return lPos
end

function getOtherPos(i)
	local oPos = function ()
		return messi:otherPos(i)
	end
	return oPos
end

function getPassPos()
	local rPos = function ()
		return messi:passPos()
	end
	return rPos
end

function getBackPos()
	local bPos = function ()
		local bx
		local by
		if ball.posX()> 0 then
			bx = - param.pitchLength / 6 * (1 - (ball.posX() / (param.pitchLength / 2)))
			by = ball.posY() / (param.pitchWidth / 2) * (2 * (param.pitchWidth / 2) / 3)
		elseif ball.posY() > 0 then
			bx = - param.pitchLength / 12
			by = - param.pitchWidth / 4
		else
			bx = - param.pitchLength / 12
			by =   param.pitchWidth / 4
		end
		return CGeoPoint:new_local(bx, by)
	end
	return bPos
end
