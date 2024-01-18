local waitPos = ball.antiYPos(CGeoPoint:new_local(2600,1500))
local waitPos2 = ball.antiYPos(CGeoPoint:new_local(1800,1500))
local mode = true
gPlayTable.CreatePlay{
firstState = "init",
["init"] = {
    switch = function()
        if player.toTargetDist("Assister") < 1000 then
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
	Leader = task.touchKick(waitPos,false,540,mode),
    Assister = task.goCmuRush(waitPos2),
	match = ""
},
["shoot"] = {
    switch = function()
    end,
    Leader = task.stop(),
    Assister = task.touchKick(pos.theirGoal(), false,300,mode),
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
