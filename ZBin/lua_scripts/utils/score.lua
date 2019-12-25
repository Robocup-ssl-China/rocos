module(..., package.seeall)

-- 为现有的Play进行加分
function add(step)
	gPlayTable[gCurrentPlay].score = gPlayTable[gCurrentPlay].score + step
end

-- 为现有的Play进行减分
function minus(step)
	gPlayTable[gCurrentPlay].score = gPlayTable[gCurrentPlay].score - step
end