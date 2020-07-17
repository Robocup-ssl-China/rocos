module(..., package.seeall)

function specified(p)
	return function ()
		return p
	end
end

function full()
	return 8000
end

function touch()
	return 6500
end

function forReflect()
	return 8888
end

-- 当t有三种输入(userdate/point、role、function)
function toTarget(p)
	return function(role)
		local target
		local inSpeed = 4500 -- 需要的入嘴速度
		if type(p) == "function" then
			target = p() 
		elseif type(p) == "userdata" then
			target = p
		else
			target = player.pos(p)
		end
		local pw
		if IS_SIMULATION then
			pw =  player.toPointDist(role, target)*1 + 1000
		else
			pw = player.toPointDist(role, target)*1 + 1000
		end
		if pw < 2000 then    --50 --> 250 Modified by Soap, 2015/4/11
			pw = 2000 					--50 --> 250 Modified by Soap, 2015/4/11
		elseif pw > 7500 then
			pw = 7500
		end
		return pw
	end
end


function toTargetNormalPlay(p)
	return function(role)
		local target
		if type(p) == "function" then
			target = p()
		elseif type(p) == "userdata" then
			target = p
		else
			target = player.pos(p)
		end

		if IS_SIMULATION then
			pw =  player.toPointDist(role, target)*1.7 + 100
		else
			pw = player.toPointDist(role, target)*0.8+100
		end
		if pw < 200 then    --50 --> 250 Modified by Soap, 2015/4/11
			pw = 200 					--50 --> 250 Modified by Soap, 2015/4/11
		elseif pw > 750 then
			pw = 750
		end
			return pw
			-- local pw = -0.0068*dist*dist + 5.5774*dist - 287.8
	end
end

-- function toPlayer(role1)
-- 	return function(role2)
-- 		if IS_SIMULATION then
-- 			return player.toPlayerDist(role1, role2)*1.2 + 150
-- 		else
-- 			local dist = player.toPlayerDist(role1, role2) * 1.2
-- 			if dist < 70 then
-- 				dist = 70
-- 			end
-- 			return dist
-- 		end
-- 	end
-- end

-- function toPoint(p)
-- 	return function(role)
-- 		local target
-- 		if type(p) == "function" then
-- 			target = p()
-- 		elseif type(p) == "userdata" then
-- 			target = p
-- 		end
-- 		if IS_SIMULATION then
-- 			return player.toPointDist(role, target)*1.2 + 150
-- 		else
-- 			local dist = player.toPointDist(role, target) * 1.2
-- 			if dist < 70 then
-- 				dist = 70
-- 			end
-- 			return dist
-- 		end
-- 	end
-- end