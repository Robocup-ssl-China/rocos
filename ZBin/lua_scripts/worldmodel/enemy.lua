module(..., package.seeall)

function instance(role)
	if type(role) == "number" then
		return vision:theirPlayer(role)
	else
		print("Invalid role in enemy.instance!!!")
	end
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

function rotVel(role)
	return instance(role):RotVel()
end

function valid(role)
	return instance(role):Valid()
end

function attackNum()
	return defenceInfo:getAttackNum()
end

function situChanged()
 	return defenceInfo:getTriggerState()
end
--need to modify
function isGoalie(role)
	if pos(role):dist(CGeoPoint:new_local(param.pitchLength / 2.0, 0)) < 85 then
		return true
	end
	return false
end
--need to modify
function isDefender(role)
	if pos(role):dist(CGeoPoint:new_local(param.pitchLength / 2.0, 0)) < 120 and not isGoalie(role) then
		return true
	end
	return false
end

function isMarking(role)
	if pos(role):dist() and not isDefender(role) then
		return true
	end
	return false
end

function isAttacker(role)
	if posX(role) < 0 and not isMarking(role) then
		return true
	end
	return false
end

function isBallFacer(role)
	if pos(role):dist(ball.pos()) < 60 then
		return true
	end
	return false
end

function hasReceiver()
	return CEnemyHasReceiver()
end

gEnemyMsg = {
	-- 门将的位置（包括消失处理）
	goaliePos = CGeoPoint:new_local(param.pitchLength/2.0,0)
}

function updateCorrectGoaliePos()
	local theirGoalieNum = skillUtils:getTheirGoalie()
	if enemy.valid(skillUtils:getTheirGoalie()) then
		gEnemyMsg.goaliePos = enemy.pos(theirGoalieNum)
	end
	return gEnemyMsg.goaliePos
end

function getTheirGoaliePos()
	return gEnemyMsg.goaliePos
end

function best()
	return skillUtils:getTheirBestPlayer()
end

function bestVelMod()
	return velMod(best())
end

function bestPos()
	return pos(best())
end

function bestDir()
	return dir(best())
end

function bestToBallDist()
	return pos(best()):dist(ball.pos())
end

function bestToBallDir()
	return (ball.pos() - pos(best())):dir()
end

function nearest()
	local nearDist = 99999
	local nearNum = 0
	for i=1,6 do
		local theDist = enemy.pos(i):dist(ball.pos())
		if enemy.valid(i) and nearDist > theDist then
			nearDist = theDist
			nearNum = i
		end
	end
	return pos(nearNum), dir(nearNum)
end