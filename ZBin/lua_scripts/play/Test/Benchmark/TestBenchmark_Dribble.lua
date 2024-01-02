local START_POS = CGeoPoint:new_local(1000,1000)
gPlayTable.CreatePlay{
firstState = "start",
-- ["start"] = {
--     switch = function()
--     end,
--     Leader = task.placeBall(START_POS),
--     match = "(L)"
-- },

["run"] = {
    switch = function()
    end,
    match = ""
},

name = "TestBenchmark_Dribble",
applicable ={
	exp = "a",
	a = true
},
attribute = "attack",
timeout = 99999
}
