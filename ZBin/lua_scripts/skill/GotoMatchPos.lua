function GotoMatchPos(task)
	local mpos
	local mdir
	local mflag = task.flag or 0
	local msender = task.sender or 0
	local mrole = task.srole or ""
	local macc = task.acc or 0
	local mmethod = task.method or 4

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
			print("Error runner in GotoMatchPos", runner)
		end
		
		for rolename, num in pairs(gRoleNum) do
			if num == runner and IsRoleActive(rolename) then
				local p
				if type(gRolePos[rolename]) == "function" then
					p = gRolePos[rolename]()
				else
					p = gRolePos[rolename]
				end
				mpos = p
				break
			end
		end
		if type(task.dir) == "function" then
			mdir = task.dir(runner)
		else
			mdir = task.dir
		end

		if type(task.sender) == "string" then
			msender = player.num(task.sender)
		end
		
		if mrole == "leftBack" or mrole == "rightBack" or mrole == "singleBack" then
			if player.posX(runner) < -param.pitchLength/4 then
				macc = 475*10
			else
				macc = 475*10
			end

			if player.toOurGoalDist(runner) > param.penaltyDepth * 1.5 then
				mflag = bit:_and(mflag, bit:_not(flag.not_avoid_our_vehicle))
			end
		end

		if mmethod == 4 then
			return SmartGotoPos(runner, mpos:x(), mpos:y(), mdir, mflag, msender, macc)
		elseif mmethod == 5 then
			local ballVel = ball.vel()
			local vel = Utils.Polar2Vector(300*10, ball.velDir())
			if ball.velMod() < 50*10 then
				vel = CVector:new_local(0, 0)
			end
			local velX = vel:x()
			local velY = vel:y()
			return CNoneZeroGoCmuRush(runner, mpos:x(), mpos:y(), mdir, mflag, msender, macc, 0, velX, velY)
		else
			return CGoCmuRush(runner, mpos:x(), mpos:y(), mdir, mflag, msender, macc)
		end
	end

	return execute, matchPos
end

gSkillTable.CreateSkill{
	name = "GotoMatchPos",
	execute = function (self)
		print("This is in skill"..self.name)
	end
}
