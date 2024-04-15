local waitPos = ball.antiYPos(CGeoPoint:new_local(2800,2800))
local waitPos2 = ball.antiYPos(CGeoPoint:new_local(2800,2800))
local mode = true

return {
firstState = "init",
["init"] = {
    switch = function()
        if player.toTargetDist("Assister") < 1000 and player.toTargetDist("Leader") < 100 then
            return "pass"
        end
    end,
    Leader = task.staticGetBall(waitPos),
    Assister = task.goCmuRush(waitPos2),
    match = "[L][A]"
},
["pass"] = {
	switch = function()
        if player.kickBall("Leader") then
            return "shoot"
        end
	end,
	Leader = task.touchKick(waitPos,false,2500,mode),
    Assister = task.goCmuRush(waitPos2),
	match = "{LA}"
},
["shoot"] = {
    switch = function()
        if player.kickBall("Assister") then
            return "init"
        end
    end,
    Leader = task.stop(),
    Assister = task.touchKick(pos.theirGoal(), false,6000,mode),
    match = "{LA}"
},

name = "TestPassAndKick",
applicable ={
	exp = "a",
	a = true
},
attribute = "attack",
timeout = 99999
}
