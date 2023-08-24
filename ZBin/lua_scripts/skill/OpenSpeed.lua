function OpenSpeed(task)
	local mflag = task.flag or 0
	local mspeedX = task.speedX or 0
	local mspeedY = task.speedY or 0
	local mspeedW = task.speedW or 0
	local needReport = task.needReport or false
	
	execute = function(runner)
		if type(task.speedX) == "function" then
			mspeedX = task.speedX()
		end

		if type(task.speedY) == "function" then
			mspeedY = task.speedY()
		end

		if type(task.speedW) == "function" then
			mspeedW = task.speedW()
		end

		if task.dir ~= nil then
			local mdir
			if type(task.dir) == "function" then
				mdir = task.dir(runner)
			else
				mdir = task.dir
			end
			mspeedX = task.mod * math.cos(mdir)
			mspeedY = task.mod * math.sin(mdir)
		end
		
		return COpenSpeed(runner, mspeedX, mspeedY, mspeedW, needReport)
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
