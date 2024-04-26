function SimpleGoto(task)
	local mpos
	local mdir
	local mflag = task.flag or 0

	execute = function(runner)
		mpos = _c(task.pos,runner)
		mdir = _c(task.dir,runner)

		task_param = TaskT:new_local()
		task_param.executor = runner
		task_param.player.pos = CGeoPoint(mpos)
		task_param.player.angle = mdir
		task_param.player.flag = mflag
		-- return SimpleGotoPos(runner, mpos:x(), mpos:y(), mdir, mflag)
		return skillapi:run("Goto", task_param)
	end

	matchPos = function()
		return _c(task.pos)
	end

	return execute, matchPos
end

gSkillTable.CreateSkill{
	name = "SimpleGoto",
	execute = function (self)
		print("This is in skill"..self.name)
	end
}