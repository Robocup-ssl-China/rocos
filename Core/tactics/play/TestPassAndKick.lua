local waitPos = ball.antiYPos(CGeoPoint:new_local(2600,1500))
gPlayTable.CreatePlay{
firstState = "init",
["init"] = {
    switch = function()
        if player.toTargetDist("Assister") < 200 then
            return "pass"
        end
    end,
    Leader = task.staticGetBall(waitPos),
    Assister = task.goCmuRush(waitPos),
    match = "[L][A]"
},
["pass"] = {
	switch = function()
        if player.kickBall("Leader") then
            return "shoot"
        end
	end,
	Leader = task.touchKick(waitPos,false),
    Assister = task.goCmuRush(waitPos),
	match = ""
},
["shoot"] = {
    switch = function()
    end,
    Leader = task.stop(),
    Assister = task.touchKick(pos.theirGoal(), false),
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
