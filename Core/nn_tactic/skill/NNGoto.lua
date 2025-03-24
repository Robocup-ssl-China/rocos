function NNGoto(task)
    matchPos = function()
        if task.pos == nil then
            return CGeoPoint(0,0)
        end
        if type(task.pos) == "function" then
            return task.pos()
        end
        return task.pos
    end

    execute = function(runner)
        task_param = TaskT:new_local()
        task_param.executor = runner
        task_param.player.pos = CGeoPoint:new_local(0,0)
        return skillapi:run("NNGoto", task_param)
    end

    return execute, matchPos
end

gSkillTable.CreateSkill{
    name = "NNGoto",
    execute = function (self)
        print("This is in skill"..self.name)
    end
}