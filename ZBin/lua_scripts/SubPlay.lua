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
            print("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!")
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
    return function()
        if gSubPlay.playTable[name] == nil then
            warning("roleTask not exist - ", name, role)
            return
        end
        local _subPlayState = gSubPlay.playTable[name].play[gSubPlay.playTable[name].curState]
        return _subPlayState[role]
    end
end

-- function gSubPlay.getRoleNum(roleName)
--     print("in getRoleNum : ", roleName)
--     if gSubPlay.curScope == nil then
--         return gRoleNum[roleName]
--     end
--     return -1
-- end