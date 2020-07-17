gPlayTable.CreatePlay{
firstState = "halt",
switch = function()
	if cond.isNormalStart() then
      return "exit"
    end
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

name = "Ref_KickOffDefV1",
applicable ={
	exp = "a",
	a = true
},
attribute = "attack",
timeout = 99999
}
