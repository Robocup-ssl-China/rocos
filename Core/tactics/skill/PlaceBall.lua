function PlaceBall(task)
	matchPos = function()
		return ball.pos()
	end

	execute = function(runner)
		task_param = TaskT:new_local()
		task_param.executor = runner
		task_param.player.pos = _c(task.pos)
		return skillapi:run("PlaceBall", task_param)
	end

	return execute, matchPos
end

gSkillTable.CreateSkill{
	name = "PlaceBall",
	execute = function (self)
		print("This is in skill"..self.name)
	end
}