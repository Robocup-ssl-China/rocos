local waitPos = ball.antiYPos(CGeoPoint:new_local(2600,1500))
local waitPos2 = ball.antiYPos(CGeoPoint:new_local(1800,1500))
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
	Leader = task.touchKick(waitPos,false,3500,mode),
    Assister = task.goCmuRush(waitPos2),
	match = ""
},
["shoot"] = {
    switch = function()
        if player.kickBall("Assister") then
            return "init"
        end
    end,
    Leader = task.stop(),
    Assister = task.touchKick(pos.theirGoal(), false,6000,mode),
    match = ""
},

name = "TestPassAndKick",
applicable ={
	exp = "a",
	a = true
},
attribute = "attack",
timeout = 99999
}
