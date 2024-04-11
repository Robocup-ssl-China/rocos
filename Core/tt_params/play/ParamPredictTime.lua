local runX = 0
local runY = 0
local runPos = function()
    return CGeoPoint(runX, runY)
end
local getRand = function()
    local r = 2*(math.random()-0.5) -- [-1,1]
    return r * 4000
end

local getDataStr = function(v)
    local data = {
        os.clock(),
        v:X(),v:Y(),v:VelX(),v:VelY(),
        v:RawRotVel(),v:RotVel(),
        -- v:RawPos():x(),v:RawPos():y(),
        v:RawVel():x(),v:RawVel():y(),
        runX,runY,
    }
    local str = ""
    for i,value in pairs(data) do
        str = str .. string.format("%.3f;",value)
    end
    return str .. '\n'
end

local recFile = nil

return {
    firstState = "reset",
    ["reset"] = {
        switch = function()
            if recFile ~= nil then
                recFile:close()
                recFile = nil
            end
            runX = getRand()
            runY = getRand()
            return "randRun"
        end,
        Leader = task.stop(),
        match = "[L]"
    },
    ["randRun"] = {
        switch = function()
            if bufcnt(true, 100) then
                runX = getRand()
                runY = getRand()
                local fileName = "__data/robot_run/" .. os.date("%m%d%H%M%S") .. os.clock()
                recFile = io.open(fileName, 'w')
                recFile:write(getDataStr(player.instance("Leader")))
                return "testData"
            end
        end,
        Leader = task.goCmuRush(runPos,0),
        match = "{L}"
    },
    ["testData"] = {
        switch = function()
            if player.toTargetDist("Leader") > 99999 then
                return "reset"
            end
            local data = getDataStr(player.instance("Leader"))
            debugEngine:gui_debug_msg(CGeoPoint(0,0),data)
            recFile:write(getDataStr(player.instance("Leader")))
            if bufcnt(player.toTargetDist("Leader") < 10,10) then
                return "reset"
            end
        end,
        Leader = task.goCmuRush(runPos,0),
        match = "{L}"
    },
    name = "ParamPredictTime",
}