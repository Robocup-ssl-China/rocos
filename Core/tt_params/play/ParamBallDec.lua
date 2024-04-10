local count = 0
local predict = {}
local observation = {}
local debug = function()
    local __debug = function(info, color, symbol)
        for k, v in pairs(info) do
            local msg = symbol .. tostring(k) .. ":" .. v[2]
            local pos = v[1]:Pos()
            local vel = v[1]:Vel()
            debugEngine:gui_debug_x(pos, color)
            debugEngine:gui_debug_line(pos,pos+vel, color)
            debugEngine:gui_debug_msg(pos, msg, color)
        end
    end
    __debug(predict, param.GREEN, "P")
    __debug(observation, param.ORANGE, "O")
end
return {
    firstState = "reset",
    ["reset"] = {
        switch = function()
            debug()
            if bufcnt(ball.velMod() < 100, 30) then
                pos = {}
                msg = {}
                return "wait"
            end
        end,
        match = ""
    },
    ["wait"] = {
        switch = function()
            debug()
            if bufcnt(ball.valid() and ball.velMod() > 500, 20) then
                count = 0
                for i = 1, 4 do
                    local time = i/4.0
                    local pose = ballModel:poseForTime(time)
                    predict[i] = {pose, tostring(time)..'s'}
                end
                predict['FF'] = {ballModel:poseForTime(9999), 'FinalPos'}
                return "test"
            end
        end,
        match = ""
    },
    ["test"] = {
        switch = function()
            debug()
            if bufcnt(true,param.frameRate) then
                return "reset"
            end
        end,
        match = ""
    },
    name = "ParamBallDec",
}