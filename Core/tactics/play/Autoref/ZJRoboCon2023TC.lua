local BELONG_DIST = 125
local BELONG_ANGLE = 30
local SUM_TIME = 60
local FREQ = 73
local BELONGING_STATES = {
	NOT_BELONG = 1,
	YELLOW = 2,
	BLUE = 3,
	BOTH = 4,
}
local BELINGING_COLOR = {
	4,3,6,1 -- GREEN,YELLOW,BLUE,RED
}
local BELONGING_STATUS = BELONGING_STATES.NOT_BELONG
local playerDist2Ball = function(playerNum)
	return ball.valid() and player.pos(playerNum):dist(ball.rawPos()) or 9999
end
local playerDirWithBall = function(playerNum)
	local d = player.toBallDir(playerNum) - player.dir(playerNum)
	local dd = (d+math.pi)%(2*math.pi)-math.pi
	return math.abs(dd)*180/math.pi
end
local enemyDist2Ball = function(enemyNum)
	return ball.valid() and enemy.pos(enemyNum):dist(ball.rawPos()) or 9999
end
local enemyDirWithBall = function(enemyNum)
	local d = enemy.toBallDir(enemyNum) - enemy.dir(enemyNum)
	local dd = (d+math.pi)%(2*math.pi)-math.pi
	return math.abs(dd)*180/math.pi
end
local getColor = function()
	return BELINGING_COLOR[BELONGING_STATUS] or 1
end
local startTime = os.date('%Y-%m-%d %H:%M:%S')
local endTime = nil
local MY_COUNT = 0
local ENEMY_COUNT = 0
local ALL_COUNT = 0
local resultOutput = function(finished)
	if finished then
		debugEngine:gui_debug_msg(CGeoPoint:new_local(-4000,-2200),string.format("FINISHED"),0)
	end
	local BLUE_COUNT = IS_YELLOW and ENEMY_COUNT or MY_COUNT
	local YELLOW_COUNT = IS_YELLOW and MY_COUNT or ENEMY_COUNT
	debugEngine:gui_debug_msg(CGeoPoint:new_local(-4000,-2400),string.format("time : %4.2f",SUM_TIME - ALL_COUNT/FREQ))
	debugEngine:gui_debug_msg(CGeoPoint:new_local(-4000,-2600),string.format("BLUE : %4.2f",BLUE_COUNT/FREQ),6)
	debugEngine:gui_debug_msg(CGeoPoint:new_local(-4000,-2800),string.format("YELL : %4.2f",YELLOW_COUNT/FREQ),3)
end
local refRun = function(running)
	local playerCount = 0
	local enemyCount = 0
	local ballColor = getColor()
	debugEngine:gui_debug_arc(ball.rawPos(),400,0,360,ballColor)
	debugEngine:gui_debug_arc(ball.rawPos(),415,0,360,ballColor)
	debugEngine:gui_debug_arc(ball.rawPos(),430,0,360,ballColor)
	debugEngine:gui_debug_msg(ball.rawPos(),string.format("v: %.0f",ball.velMod()),ballColor)
	local blueClose = false
	local yellowClose = false
	for i = 0, param.maxPlayer - 1 do
		if player.valid(i) then
			playerCount = playerCount + 1
			local color = IS_YELLOW and 3 or 6
			local dist = playerDist2Ball(i)
			if dist < BELONG_DIST then
				if playerDirWithBall(i) < BELONG_ANGLE then
					blueClose = true
					BELONGING_STATUS = IS_YELLOW and BELONGING_STATES.YELLOW or BELONGING_STATES.BLUE
					color = 4
				end
			end
			debugEngine:gui_debug_msg(CGeoPoint:new_local(130,param.pitchWidth/2-30-playerCount*150),string.format("%02d : %4.0f",i,dist),color)
		end
		if enemy.valid(i) then
			enemyCount = enemyCount + 1
			local color = IS_YELLOW and 6 or 3
			local dist = enemyDist2Ball(i)
			if dist < BELONG_DIST then
				if enemyDirWithBall(i) < BELONG_ANGLE then
					yellowClose = true
					BELONGING_STATUS = IS_YELLOW and BELONGING_STATES.BLUE or BELONGING_STATES.YELLOW
					color = 4
				end
			end
			debugEngine:gui_debug_msg(CGeoPoint:new_local(-1000,param.pitchWidth/2-30-enemyCount*150),string.format("%02d : %4.0f",i,dist),color)
		end
	end
	if blueClose and yellowClose then
		BELONGING_STATUS = BELONGING_STATES.BOTH
	end
	if running then
		if IS_YELLOW then
			MY_COUNT = MY_COUNT + (BELONGING_STATUS == BELONGING_STATES.YELLOW and 1 or 0)
			ENEMY_COUNT = ENEMY_COUNT + (BELONGING_STATUS == BELONGING_STATES.BLUE and 1 or 0)
		else
			MY_COUNT = MY_COUNT + (BELONGING_STATUS == BELONGING_STATES.BLUE and 1 or 0)
			ENEMY_COUNT = ENEMY_COUNT + (BELONGING_STATUS == BELONGING_STATES.YELLOW and 1 or 0)
		end
		ALL_COUNT = ALL_COUNT + 1
	else
		debugEngine:gui_debug_msg(CGeoPoint:new_local(-4000,-2200),string.format("PAUSEING"),0)
	end
	resultOutput(false)
end
local ballOut = function()
	local x = math.abs(ball.pos():x())
	local y = math.abs(ball.pos():y())
	return not ball.valid() or x > param.pitchLength/2 or y > param.pitchWidth/2 or (x > param.pitchLength/2 - param.penaltyDepth and y < param.penaltyWidth/2 and ball.velMod() < 500)
end
gPlayTable.CreatePlay{

firstState = "init",
["init"] = {
	switch = function()
		refRun(false)
		if BELONGING_STATUS ~= BELONGING_STATES.NOT_BELONG then
			return "running"
		end
	end,
	match = ""
},
["running"] = {
	switch = function()
		refRun(true)
		if ALL_COUNT >= SUM_TIME*FREQ then
			return "finished"
		end
		if ballOut() then
			BELONGING_STATUS = BELONGING_STATES.NOT_BELONG
			return "init"
		end
	end,
	match = ""
},
["finished"] = {
	switch = function()
		resultOutput(true)
	end,
	match = ""
},

name = "ZJRoboCon2023TC",
applicable ={
	exp = "a",
	a = true
},
attribute = "autoref",
timeout = 99999
}
