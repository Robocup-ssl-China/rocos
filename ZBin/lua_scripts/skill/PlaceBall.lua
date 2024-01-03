function PlaceBall(task)
    local mpos
    local mflag = task.flag or 0
    execute = function(runner)
        if type(task.pos) == "function" then
            mpos = task.pos()
        else
            mpos = task.pos
        end
        if type(task.flag) == "function" then
            mflag = task.flag()
        else
            mflag = task.flag
        end
        return CPlaceBall(runner, mpos:x(), mpos:y(), mflag)
    end
    matchPos = function()
        return ball.pos()
    end
    return execute, matchPos
end

gSkillTable.CreateSkill{
    name = "PlaceBall",
	execute = function (self)
		print("This is in skill"..self.name)
	end
}