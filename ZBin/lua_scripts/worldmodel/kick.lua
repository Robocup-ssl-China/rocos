module(..., package.seeall)

function none()
	return 0
end

function flat()
	return 1
end

function chip()
	return 2
end

function auto(role1, role2)
	local flatFlag
	local chipFlag
	
	local autoCal = function (passer, recver)	
		flatFlag = player.canFlatPassTo(passer, recver)
		chipFlag = player.canChipPassTo(passer, recver)
		if not chipFlag and not flatFlag then
			return 0
		elseif flatFlag then
			return 1
		else
			return 2
		end
	end
	
	if role2 == nil then	
		return function(passer)		
			return autoCal(passer, role1)
		end
	else
		return autoCal(role1, role2)
	end			
end