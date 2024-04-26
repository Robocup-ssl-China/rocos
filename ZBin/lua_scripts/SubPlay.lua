SubPlay = {
}
function SubPlay.pack(name, play, param)
    local curState = play.firstState
    if play.__init__ ~= nil then
        play.__init__(name, param)
    end
    return {
        name = name,
        roleMapping = {},
        play = play,
        curState = curState,
        lastState = nil,
    }
end

gSubPlay = {
    playTable = {},
    curScope = "",
}
gSubPlayFiles = {}

function gSubPlay.new(name, playName, initParam)
    print("in initPlay : ", name, initParam)
    if gSubPlay.playTable[name] ~= nil then
        warning("subPlay exist, reinit a new one - ", name)
    end
    if gSubPlayFiles[playName] == nil then
        warning(string.format("subPlay file not exist - %s", playName))
        return
    end
    spec = dofile(gSubPlayFiles[playName])
    gSubPlay.playTable[name] = SubPlay.pack(name, spec, initParam)
end

function gSubPlay.del(name)
    gSubPlay.playTable[name] = nil
end

function gSubPlay.step()
    local debugX = -2000
    local debugY = param.pitchWidth/2+200
    local step = -130
    for key, play in pairs(gSubPlay.playTable) do
        gSubPlay.curScope = key
        local _subPlay = play.play
        local curState = play.curState
        curState = _RunPlaySwitch(_subPlay, curState)
        local isStateSwitched = false
        if curState ~= nil then
            play.lastState = play.curState
            play.curState = curState
            isStateSwitched = true
            PlayFSMClearAll()
        end
        -- print("ID : , play : , curState :", key,  _subPlay.name, play.curState)
        gSubPlay.curScope = ""
        debugEngine:gui_debug_msg_fix(CGeoPoint(debugX, debugY), key .. " - " .. play.curState, param.GREEN, 0,100)
        debugY = debugY + step
    end
end

function gSubPlay.getState(name)
    if gSubPlay.playTable[name] == nil then
        warning("(call gSubPlay.getState) subPlay not exist - ", name)
        return nil
    end
    return gSubPlay.playTable[name].curState
end

function gSubPlay.roleTask(name, role)
    return {
        name = "subPlayTask",
        task = function()
            if gSubPlay.playTable[name] == nil then
                warning("roleTask not exist - " .. name .. " " .. role)
                return
            end
            local _subPlayState = gSubPlay.playTable[name].play[gSubPlay.playTable[name].curState]
            subTask = _subPlayState[role]
            if subTask ~= nil and type(subTask) == "table" and subTask.name == "subPlayTask" then -- subtask call subtask
                gSubPlay.register(name, role, subTask.args)
                return subTask.task()
            end
            local _subRoleTask = _subPlayState[role]
            if type(_subRoleTask) == "function" then
                return _subRoleTask()
            end
            return _subPlayState[role]
        end,
        args = {
            name = name,
            role = role,
        }
    }
end

function gSubPlay.register(playName, rolename, args)
    local msg = string.format("%s:%s - %s:%s ",args.name, args.role, playName, rolename)
    -- print("register : ",msg)
    if gSubPlay.playTable[args.name] == nil then
        warning("register failed, play not exist - " .. msg)
        return
    end
    gSubPlay.playTable[args.name].roleMapping[args.role] = {playName, rolename}
end

function gSubPlay.getRole(roleName)
    local role, num = gSubPlay.getRoleAndNum(roleName)
    return role
end

function gSubPlay.getRoleNum(roleName)
    local role, num = gSubPlay.getRoleAndNum(roleName)
    return num
end

function gSubPlay.getRoleAndNum(roleName)
    local scope = gSubPlay.curScope
    local role = roleName
    local iterCount = 0
    local findPath = {}
    local pathOutput = function(path)
        local str = ""
        for i, v in ipairs(path) do
            str = str .. " -> " .. v[1] .. ":" .. v[2]
        end
        return str
    end
    -- print("getRoleNum - ", scope, role)
    while true do
        if scope == "" then
            return role, gRoleNum[role]
        else
            if gSubPlay.playTable[scope] == nil then
                warning("subPlay not exist - " .. scope .. " " .. role .. pathOutput(findPath))
                return "",-1
            end
            if gSubPlay.playTable[scope].roleMapping[role] == nil then
                warning("role not exist - " .. scope .. " " .. role .. pathOutput(findPath))
                return "",-1
            end
            local tRoleMap = gSubPlay.playTable[scope].roleMapping[role]
            scope, role = tRoleMap[1], tRoleMap[2]
        end
        table.insert(findPath, {scope,role})
        -- prevent infinite loop
        iterCount = iterCount + 1
        if iterCount > 10 then
            warning("getRoleNum failed - " .. pathOutput(findPath))
            return "",-1
        end
    end
end