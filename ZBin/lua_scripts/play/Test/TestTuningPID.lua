local msg = ""
gPlayTable.CreatePlay{

firstState = "init",

["init"] = {
	switch = function()
		msg = player.rotVel("Leader")
		debugEngine:gui_debug_msg(CGeoPoint:new_local(0,1000),msg)
	end,
	-- Leader = task.openSpeed(0,0,12.79),
	Leader = task.openSpeed(0,0,1,true),
	match = "(L)"
},

name = "TestTuningPID",
applicable ={
	exp = "a",
	a = true
},
attribute = "attack",
timeout = 99999
}
