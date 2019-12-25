function bufcnt( cond, buf, cnt )
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

local COND_FIX_STATUS = false

function minbufcnt(c, min_cycle, buf, max_cycle)
	if bufcnt(true, min_cycle) then
    	COND_FIX_STATUS = true
    end
    if bufcnt(COND_FIX_STATUS and c, buf, max_cycle) then
        COND_FIX_STATUS = false
        return true
    end

    return false
end