local warningX = -param.pitchLength/2
local warningY = 0
function warning(msg)
    msg = "WARNING: " .. msg
    print( msg)
    debugEngine:gui_debug_msg_fix(CGeoPoint(warningX, warningY), msg, param.CYAN)
    warningY = warningY + 120
    if warningY > param.pitchWidth/2 then
        warningY = -param.pitchWidth/2
    end
end
function _c(f,...) -- auto convert for closure
	if type(f) == "function" then
		return f(...)
	end
	return f
end

function serializeTable(val, name, skipnewlines, depth)
    skipnewlines = skipnewlines or false
    depth = depth or 0

    local tmp = string.rep(" ", depth)

    if name then tmp = tmp .. name .. " = " end

    if type(val) == "table" then
        tmp = tmp .. "{" .. (not skipnewlines and "\n" or "")

        for k, v in pairs(val) do
            tmp =  tmp .. serializeTable(v, k, skipnewlines, depth + 1) .. "," .. (not skipnewlines and "\n" or "")
        end

        tmp = tmp .. string.rep(" ", depth) .. "}"
    elseif type(val) == "number" then
        tmp = tmp .. tostring(val)
    elseif type(val) == "string" then
        tmp = tmp .. string.format("%q", val)
    elseif type(val) == "boolean" then
        tmp = tmp .. (val and "true" or "false")
    else
        tmp = tmp .. "\"[inserializeable datatype:" .. type(val) .. "]\""
    end

    return tmp
end