gPlayTable.CreatePlay{
firstState = "halt",
switch = function()
	return "halt"
end,
["halt"] = {
	["Leader"]   = task.stop(),
	["Special"]  = task.stop(),
	["Assister"] = task.stop(),
	["Defender"] = task.stop(),
	["Middle"]   = task.stop(),
	["Center"]   = task.stop(),
	["Breaker"]  = task.stop(),
	["Goalie"]   = task.stop(),
	match = "[LSADMCB]"
},

name = "Ref_HaltV1",
applicable ={
	exp = "a",
	a = true
},
attribute = "attack",
timeout = 99999
}
