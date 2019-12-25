function Stop(task)	
	execute = function(runner)
		return StopRobot(runner)
	end

	matchPos = function(runner)
		if runner == nil then
			return CGeoPoint:new_local(0,0)
		else
			return player.pos(runner)
		end
	end

	return execute, matchPos
end

gSkillTable.CreateSkill{
	name = "Stop"
}
