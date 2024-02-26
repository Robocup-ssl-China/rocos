function RunMultiPos(task)
 	local curIndex = 1
 	local mpos = {}
 	local mdir
 	local mflag = task.flag or 0
 	local msender = task.sender
 	local mclose  = task.close
 	local mdist = task.dist or 20
 	local macc = task.acc or 0
 	local lastCycle = 0

	execute = function(runner)
		for i = 1, table.getn(task.pos) do
			if type(task.pos[i]) == "function" then
				mpos[i] = task.pos[i]()
			else
				mpos[i] = task.pos[i]
			end
		end

		if player.pos(runner):dist(mpos[curIndex]) < mdist then
			local nextIndex = (curIndex) % table.getn(mpos) + 1
			if mclose == nil or mclose then
				curIndex = nextIndex
			else
				if nextIndex > curIndex then
				 	curIndex = nextIndex
				end 
			end
 		end
 		
 		if type(task.sender) == "string" then
 			msender = player.num(task.sender)
 		end

 		if type(task.dir) == "function" then
			mdir = task.dir(runner)
		else
			mdir = task.dir
		end

		lastCycle = vision:getCycle()
		
		task_param = TaskT:new_local()
		task_param.executor = runner
		task_param.player.pos = mpos[curIndex]
		task_param.player.angle = mdir
		task_param.player.flag = mflag
		task_param.ball.Sender = msender or 0
		task_param.player.max_acceleration = macc or 0
		-- return CGoCmuRush(runner, mpos[curIndex]:x(), mpos[curIndex]:y(), mdir, mflag, msender, macc)
		return skillapi:run("SmartGoto", task_param)
	end

	matchPos = function()
		if vision:getCycle() - lastCycle > 6 then
			curIndex = 1
		end
		for i=1, table.getn(task.pos) do
			if type(task.pos[i]) == "function" then
				mpos[i] = task.pos[i]()
			else	
				mpos[i] = task.pos[i]
			end
		end
		return mpos[curIndex]
	end

	return execute, matchPos
 end

 gSkillTable.CreateSkill{
 	name = "RunMultiPos",
 	execute = function (self)
 		print("This is in skill"..self.name)
 	end
 }
