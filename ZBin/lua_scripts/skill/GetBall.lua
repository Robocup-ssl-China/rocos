function GetBall(task)
    local mflag = task.flag or 0
    local kick_flag = task.kick_flag or 0
    local mpos = task.pos
    local mpower = task.power
    local mprecision = task.precision

    execute = function(runner)
        if type(task.pos) == "function" then
            mpos = task.pos()
        end

        if type(task.power) == "function" then
            mpower = task.power()
        end

        if type(task.precision) == "function" then
            mprecision = task.precision()
        end

		if type(task.flag) == "function" then
			mflag = task.flag()
		else
			mflag = task.flag or 0
		end
        
        return CGetBall(runner, mpos:x(), mpos:y(), mpower, mprecision, mflag, kick_flag)
    end
    matchPos = function(runner)
        -- TODO
        return ball.pos()
    end
end