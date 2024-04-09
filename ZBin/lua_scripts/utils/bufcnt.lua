local BufCnter = {}
function BufCnter.new(buf, cnt)
	return {
		cndCnter = 0,
		cycCnter = 0,
		BUF = buf,
		CNT = cnt or 9999,
		-- lastCycle = vision:getCycle(),
	}
end
function BufCnter.update(bc, cond, buf, cnt)
	local cnt = cnt or 9999
	-- bc.lastCycle = vision:getCycle()
	bc.cycCnter = bc.cycCnter + 1
	bc.cndCnter = cond and bc.cndCnter + 1 or 0
	if bc.cycCnter >= cnt then
		return true
	end
	if bc.cndCnter >= buf then
		return true
	end
	return false
end

local __BCs = {}
function bufcnt(cond, buf, cnt)
	local scope = gSubPlay.curScope
	if scope == nil or  scope == "" then
		scope = "__G__"
	end
	if __BCs[scope] == nil then
		__BCs[scope] = {
			index = 0,
			lastCycle = vision:getCycle(),
		}
	end
	if __BCs[scope].lastCycle ~= vision:getCycle() then
		__BCs[scope].index = 0
	end

	local index = __BCs[scope].index

	if __BCs[scope][index] == nil then
		__BCs[scope][index] = BufCnter.new(buf, cnt)
	end
	local res = BufCnter.update(__BCs[scope][index],cond, buf, cnt)
	__BCs[scope].index = index + 1
	return res
end

function bufcntClear()
	local scope = gSubPlay.curScope
	if scope == nil or  scope == "" then
		scope = "__G__"
	end
	__BCs[scope] = nil
end

function ___bufcnt( cond, buf, cnt )
	if buf == "normal" then
		buf = 6
	elseif buf == "slow" then
		buf = 10
	elseif buf == "fast" then
		buf = 1
	end
	if type(buf) == "function" then
		buf = buf()
	end
	if CTimeOut(cond, buf, cnt) == 1 then
		return true
	else
		return false
	end
end
