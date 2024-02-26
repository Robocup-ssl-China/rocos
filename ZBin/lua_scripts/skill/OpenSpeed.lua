function OpenSpeed(task)
	local mflag = task.flag or 0
	local mspeedX = task.speedX or 0
	local mspeedY = task.speedY or 0
	local mspeedW = task.speedW or 0
	local needReport = task.needReport or false
	
	execute = function(runner)
		mspeedX = _c(task.speedX)
		mspeedY = _c(task.speedY)
		mspeedW = _c(task.speedW)

		task_param = TaskT:new_local()
		task_param.executor = runner
		task_param.player.speed_x = mspeedX
		task_param.player.speed_y = mspeedY
		task_param.player.rotate_speed = mspeedW
		task_param.player.flag = mflag
		task_param.player.needReport = needReport
		-- return COpenSpeed(runner, mspeedX, mspeedY, mspeedW, mflag)
		return skillapi:run("OpenSpeed", task_param)
	end

	matchPos = function()
		return ball.pos()
	end

	return execute, matchPos
end

gSkillTable.CreateSkill{
	name = "OpenSpeed",
	execute = function (self)
		print("This is in skill"..self.name)
	end
}
