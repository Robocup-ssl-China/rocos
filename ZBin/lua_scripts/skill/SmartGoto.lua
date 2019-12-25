function SmartGoto(task)
	local mpos
	local mdir
	local mflag = task.flag or 0
	local msender = task.sender or 0
	local mrole = task.srole or ""
	local macc = task.acc or 0

	matchPos = function()
		if type(task.pos) == "function" then
			mpos = task.pos()
		else
			mpos = task.pos
		end
		return mpos
	end
	
	execute = function(runner)
		if runner >=0 and runner < param.maxPlayer then
			if mrole ~= "" then
				CRegisterRole(runner, mrole)
			end
		else
			print("Error runner in SmartGoto", runner)
		end
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

		if type(task.sender) == "string" then
			msender = player.num(task.sender)
		end
		
		return SmartGotoPos(runner, mpos:x(), mpos:y(), mdir, mflag, msender, macc)
	end

	return execute, matchPos
end

gSkillTable.CreateSkill{
	name = "SmartGoto",
	execute = function (self)
		print("This is in skill"..self.name)
	end
}
