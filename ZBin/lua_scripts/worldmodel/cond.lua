module(..., package.seeall)


--~ -----------------------------------------------
--~ referee box condition
--~ -----------------------------------------------

function ourBallPlace()
	return vision:getCurrentRefereeMsg() == "OurBallPlacement"
end

function theirBallPlace()
	return vision:getCurrentRefereeMsg() == "TheirBallPlacement"
end

-- condition for def script exit
function ballMoved()
	return (ball.pos()-ball.refPos()):mod() > 7 and not theirBallPlace() and not ourBallPlace()
end

function ballPlaceFinish()
	local res1 = world:getBPFinish()
	return res1
end

function ballPlaceUnfinish()
	return (ball.valid() and (ball.pos()-ball.placementPos()):mod() > 12)
end

function isGameOn()
	--print("check game on")
	return vision:gameState():gameOn()
end

function isNormalStart()
	return vision:gameState():canEitherKickBall()
end

function validNum()
	return vision:getValidNum()
end

function timeRemain()
	return vision:timeRemain()
end

function ourGoal()
	return vision:ourGoal()
end

function theirGoal()
	return vision:theirGoal()
end

function needExitAttackDef(p1,p2,str)
	--此区域是否还需要保持进攻性防守
	local mp1
	local mp2
	if type(p1) == "function" then
		mp1 = p1()
	else 
		mp1 = p1
	end
	if type(p2) == "fucntion" then
		mp2 = p2()
	else
		mp2 = p2
	end
	local mode
	if str=="horizal" then
		mode = 2
	end
	if str=="vertical" then
		mode = 1
	end
    return world:NeedExitAttackDef(mp1,mp2,mode)
end

function dist4ball2MarkTouch(p1,p2)
	local mp1
	local mp2
	if type(p1) == "function" then
		mp1 = p1()
	else 
		mp1 = p1
	end
	if type(p2) == "fucntion" then
		mp2 = p2()
	else
		mp2 = p2
	end
	return world:ball2MarkingTouch(mp1,mp2)
end

function canExitMiddleDef()
	local result = false
	if ball.velMod()<100 then
		result = true
	end
	return result
end

function  canExitMRLFrontDef()
	local result = false
	if ball.velMod()<150 and vision:ballVelValid() then
		result = true
	--else
		-- if ball.refAntiY() == -1 then
		-- 	--modified for Brazil by thj
		-- 	local leftUp = CGeoPoint:new_local(190,-param.pitchWidth/2)
		-- 	local k = CGeoPoint:new_local(0,-30)
		-- 	local bestEnemy = enemy.bestPos()
	
		-- 	if bestEnemy:x() < leftUp:x() and bestEnemy:x() >rightDown:x()
		-- 		and bestEnemy:y() > leftUp:y() and bestEnemy:y() < rightDown:y() then
		-- 		if ball.velMod()>50 and Utils.AngleBetween(ball.velDir(),Utils.Normalize((bestEnemy - ball.pos()):dir()-math.pi/3),Utils.Normalize((bestEnemy - ball.pos()):dir()+math.pi/3),0) then
		-- 			result = true
		-- 		end
		-- 	end
		-- elseif ball.refAntiY() == 1 then
		-- 	local leftUp = CGeoPoint:new_local(190,param.pitchWidth/2)
		-- 	local rightDown = CGeoPoint:new_local(0,30)
		-- 	local bestEnemy = enemy.bestPos()
		-- 	if bestEnemy:x() < leftUp:x() and bestEnemy:x() >rightDown:x()
		-- 		and bestEnemy:y() < leftUp:y() and bestEnemy:y() > rightDown:y() then
		-- 		if ball.velMod()> 50 and Utils.AngleBetween(ball.velDir(),Utils.Normalize((bestEnemy - ball.pos()):dir()-math.pi/3),Utils.Normalize((bestEnemy - ball.pos()):dir()+math.pi/3),0) then
		-- 			result = true
		-- 		end
		-- 	end
		-- end
	end
	return result
end

function canExitRoboDragonMarkingFront()
	local result = false
	if ball.velMod()<50 then
		result = true
	-- elseif ball.velMod()>50 and Utils.AngleBetween(ball.velDir(),-1.57,1.57,0) then
	-- 	result = true
	end
	return result
end

function canBeImmortalShoot()
	local result = false
	if ball.toPointDist(enemy.bestPos()) < 50 and vision:ballVelValid() then
		result = true
	end
	return result
end

function  checkBallPassed(p1,p2)
	local result = false
	local lastCycle = 0
	local mp1
	local mp2
	local flag = false
	local check = 0
	local checkPosX = 0
	local checkPosY = 0
	if type(p1) == "function" then
		mp1 = p1()
	else 
		mp1 = p1
	end
	if type(p2) == "fucntion" then
		mp2 = p2()
	else
		mp2 = p2
	end
	--reset
	if vision:getCycle() - lastCycle > 6 then
		flag = false
	end
	lastCycle =vision:getCycle()

	if ball.velMod()> 50 and ball.toBestEnemyDist()> 15 and flag == false then
		check = enemy.bestDir()
		local checkPos = enemy.bestPos()
		checkPosX = checkPos:x()
		checkPosY = checkPos:y()
		flag = true
		--print("ball kicked")
	end

	if flag == true then
		local checkField = CGeoRectangle:new_local(mp1,mp2)
		local checkLine = CGeoLine:new_local(CGeoPoint:new_local(checkPosX,checkPosY),CGeoPoint:new_local(checkPosX,checkPosY)+Utils.Polar2Vector(100,check))
		local intersect = CGeoLineRectangleIntersection:new_local(checkLine,checkField)
		if intersect:intersectant() then
			result = true
		else
			result = false
		end
	end
	return result
end

function getValidMarkingTouchArea(p1,p2,p3,p4)
	return world:getMarkingTouchArea(p1,p2,p3,p4)
end

function markingFrontValid(p1,p2)
	return world:isMarkingFrontValid(p1,p2)
end

function kickOffEnemyNum()
	return world:getEnemyKickOffNum()
end

function  kickOffEnemyNumChanged()
	return world:checkEnemyKickOffNumChanged()
end

--------------------------------------For Freekick--------------------------------------------------

-- str 为所在的区域
-- script 为所使用的脚本
-- MaxRandom 为最大的随机数
local randNumLast = 0 --防止单一脚连续使用
function getOpponentScript(str, script, MaxRandom)
	if type(script) == "table" then
		local totalNum = table.getn(script)
		local randNum = 1
		
		for i=1,2 do
			randNum=math.random(1,totalNum)
			--print("\n## RandNum:"..randNum.." ##")
			if (randNum~=randNumLast) then
				break
			end
		end
		randNumLast=randNum

		if (type(script[randNum]) == "string") then
			return script[randNum]
		else
			return str..script[randNum]
		end
	elseif type(script) == "string" then
		if script == "random" then
			local randNum = math.random(1,MaxRandom)
			print("randNum",str..randNum)
			return str..randNum
		else
			-- 使用固定的模式打定位球
			return script
		end
	else
		print("Error in getOpponentScript "..str)
	end	
end

function canNorPass2Def(kickDir)
	if kickDir == nil then
		--refBall = CGeoPoint:new_local(ball.refPosX(),ball.refPosY())
		return ball.velMod() < gNorPass2NorDefBallVel
			and vision:ballVelValid() 
			--and not (ball.velMod()>10 and (ball.velDir() + math.pi) < math.pi*3/2 and (ball.velDir() + math.pi) > math.pi/2 and ball.toPointDist(refBall) < 100)
	else
		--print("dir: ",math.abs(Utils.Normalize(ball.velDir()-kickDir))/math.pi*180)
		return (ball.velMod() < gNorPass2NorDefBallVel or math.abs(Utils.Normalize(ball.velDir()-kickDir)) > math.pi/2)  
			and vision:ballVelValid()
	end
end