local testPos = function()
    local x = -(param.pitchLength/2 - param.playerRadius)
    local y = ball.posY()
    if y > param.goalWidth/2 then
        y = param.goalWidth/2
    elseif y < -param.goalWidth/2 then
        y = -param.goalWidth/2
    end
    return CGeoPoint:new_local(x,y)
end
gPlayTable.CreatePlay {

firstState = "run",

["run"] = {
    switch = function()
    end,
    Goalie = task.goCmuRush(testPos,player.toBallDir),
    match = ""
},

name = "TestGoalie",
applicable = {
    exp = "a",
    a = true
},
attribute = "attack",
timeout = 99999
}
