local _reset = function()
    local x = math.random(-3000, 3000)
    local y = math.random(-3000, 3000)
    trainEnv:placeRobot(player.num("Leader"), x, y, 0)
    x = math.random(-3000, 3000)
    y = math.random(-3000, 3000)
    trainEnv:placeBall(x, y, -x, -y)
end

return {
firstState = "test",
["reset"] = {
    switch = function()
        if bufcnt(true, 5) then
            _reset()
            return "test"
        end
    end,
    Leader = task.stop(),
    match = "{L}"
},
["test"] = {
    switch = function()
        if bufcnt(true, 60) then
            return "reset"
        end
    end,
    Leader = {NNGoto{}},
    match = "{L}"
},
name = 'EnvNNGoto',
}
