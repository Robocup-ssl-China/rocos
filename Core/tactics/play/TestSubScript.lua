local start_pos = CGeoPoint(0,0)
local dist = 1000
local rotateSpeed = 1 -- rad/s

local runPos = function()
    local angle = rotateSpeed * vision:getCycle() / param.frameRate
    return start_pos + Utils.Polar2Vector(dist, angle)
end
local subScript = false
local DSS_FLAG = bit:_or(flag.allow_dss, flag.dodge_ball)

local PLAY_NAME = ""
return {

    __init__ = function(name, args)
        print("in __init__ func : ",name, args)
        start_pos = args.pos or CGeoPoint(0,0)
        dist = args.dist or 1000
        subScript = true
        PLAY_NAME = name
    end,

    firstState = "init",
    ["init"] = {
        switch = function()
            if bufcnt(true,30) then 
                if not subScript then
                    gSubPlay.new(PLAY_NAME .. "task1", "TestSubScript", {pos=start_pos + Utils.Polar2Vector(2*dist, math.pi/4), dist=2000})
                    gSubPlay.new(PLAY_NAME .. "task2", "TestSubScript", {pos=start_pos + Utils.Polar2Vector(2*dist, -math.pi/4*3), dist=500})
                    rotateSpeed = -1
                end
                return "run"
            end
        end,
        Leader = task.stop(),
        match = "[L]"
    },
    ["run"] = {
        switch = function()
            -- print("markdebug : ",gSubPlayFiles)
            -- for key, value in pairs(gSubPlayFiles) do
            --     print("printFileTable: ", key, value)
            -- end
        end,
        Assister = gSubPlay.roleTask("task1","Leader"),
        Fronter = gSubPlay.roleTask("task2","Leader"),
        Leader = task.goCmuRush(runPos, 0, nil, DSS_FLAG),
        match = "(LAF)"
    },

    name = "TestSubScript",
    applicable = {
        exp = "a",
        a = true
    },
    attribute = "attack",
    timeout = 99999
}