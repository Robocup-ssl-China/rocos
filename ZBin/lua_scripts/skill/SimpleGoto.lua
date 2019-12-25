function SimpleGoto(task)
	local mpos
	local mdir
	local mflag = task.flag or 0

	execute = function(runner)
		if type(task.pos) == "function" then
			mpos = task.pos()
		else
			mpos = task.pos
		end

		if type(task.dir) == "function" then
			mdir = task.dir(runner)
		else
			mdir = task.dir
		end

		return SimpleGotoPos(runner, mpos:x(), mpos:y(), mdir, mflag)
	end

	matchPos = function()
		if type(task.pos) == "function" then
			mpos = task.pos()
		else
			mpos = task.pos
		end
		return mpos
	end

	return execute, matchPos
end

gSkillTable.CreateSkill{
	name = "SimpleGoto",
	execute = function (self)
		print("This is in skill"..self.name)
	end
}