 metaadd = {}
function metaadd.__add(a, b)
	for _, value in ipairs(b) do
		table.insert(a, value)
	end
	return a
end
gPlay = setmetatable({}, metaadd)
gPlay = gPlay + gRefPlayTable + gTestPlayTable + gBayesPlayTable + gAutoRefTable

gPlayTable = {}
gTimeCounter = 0
gCurrentState = ""
gLastState  = ""
gLastPlay = ""
gCurrentPlay = ""
gNextPlay = ""
gRealState = ""
gLastCycle = 0
gLastRefMsg = ""
gActiveRole = {}
gIsRefPlayExit = false

gCurrentBallStatus="None"
gLastBallStatus=""

--gCurrentOurBallAction="None"
-----add by zhyaic 2014.5.22-----
gExternStopCycle = 0
gExternExitCycle = 0

function gPlay.Next()
	local index = 1
	return function()
		index = index + 1
		return gPlay[index % table.getn(gPlay) + 1]
	end
end

function gPlayTable.CreatePlay(spec)
	assert(type(spec.name) == "string")
	assert(spec.firstState ~= nil)
	-- assert(spec.applicable ~= nil)
	-- assert(spec.attribute ~= nil)
	if spec.timeout == nil then
		spec.timeout = 99999
	end
	assert(type(spec.timeout) == "number")
	-- print("Init Play: "..spec.name)

	for attr, attr_table in pairs(spec) do
		if type(attr_table) == "table" then
			if attr_table.match ~= nil then
				for rolename, task in pairs(attr_table) do
					if rolename == "match" then
						attr_table.match = DecodeMatchStr(attr_table.match)
					else
						--rolename =
					end
				end
			end
		end
	end

	gPlayTable[spec.name] = spec
	return spec
end

function IsRoleActive(rolename)
	for _, name in pairs(gActiveRole) do
		if rolename == name then
			return true
		end
	end
	return false
end

-- function SetRoleAndNumToCPlusPlus()
-- 	for rolename, task in pairs(curPlay[gRealState]) do
-- 		if(type(task) == "table" and IsRoleActive[rolename]) then
-- 			CSetRoleAndNum(rolename, gRoleNum[rolename])
-- 		end
-- 	end
-- end

-- 注意，此处只是针对间接和直接定位球的防守
-- 此时，Leader和Goalie不参与第二次防碰撞检测
function UsePenaltyCleaner(curPlay)
	for rolename, task in pairs(curPlay[gRealState]) do
		if(type(task) == "table" and rolename ~= "match" and rolename ~= "Goalie" and rolename ~= "Kicker") then
			local p
			if type(gRolePos[rolename]) == "function" then
				p = gRolePos[rolename]()
			else
				p = gRolePos[rolename]
			end
			CAddPenaltyCleaner(string.sub(rolename,1,1), gRoleNum[rolename], p:x(), p:y())
		end
	end
	CCleanPenalty()
	for rolename, task in pairs(curPlay[gRealState]) do
		if(type(task) == "table" and rolename ~= "match" and rolename ~= "Goalie" and rolename ~= "Kicker") then
			local x, y = CGetPenaltyCleaner(string.sub(rolename,1,1))
			gRolePos[rolename] = CGeoPoint:new_local(x,y)
		end
	end
	-- print(gCurrentState, CGetResetMatchStr())
	DoRoleMatchReset(CGetResetMatchStr())
end

function DoRolePosMatch(curPlay, isPlaySwitched, isStateSwitched)
	if gCurrentState == "exit" or gCurrentState == "finish" then
		gRealState = gLastState
	else
		gRealState = gCurrentState
	end
	gActiveRole = {}
	for rolename, itask in pairs(curPlay[gRealState]) do
		--YuN 2016.03.30 增加函数模式下的匹配
		if(type(itask) == "function" and rolename ~= "match" and rolename~="switch") then
			--print("closure of task!!!!!!!!!!!!!")
			--curPlay[gRealState][rolename]=task()
			itask = itask()
		end

		if(type(itask) == "table" and rolename ~= "match") then
			table.insert(gActiveRole, rolename)
			if itask.name == "continue" and gCurrentState ~= "exit" and gCurrentState ~= "finish" then
				curPlay[gRealState][rolename] = {}
				for i,v in ipairs(curPlay[gLastState][rolename]) do
					table.insert(curPlay[gRealState][rolename], v)
					table.insert(itask, v)
				end
				curPlay[gRealState][rolename].name = "continue"
			end
			gRolePos[rolename] = itask[2]--curPlay[gRealState][rolename][2]()
		end
	end

	UpdateRole(curPlay[gRealState].match, isPlaySwitched, isStateSwitched)
	-- SetRoleAndNumToCPlusPlus()
	-- add by zhyaic for test 2013.5.24
	-- if vision:getCurrentRefereeMsg() == "TheirIndirectKick" or
	--    vision:getCurrentRefereeMsg() == "TheirDirectKick" or
	--    vision:getCurrentRefereeMsg() == "GameStop" then
	-- 	UsePenaltyCleaner(curPlay)
	-- end
end

function SetNextPlay(name)
	gNextPlay = name
end

function ResetPlay(name)
	local curPlay = gPlayTable[name]
	world:SPlayFSMSwitchClearAll(true)
	--------------------------------
	if curPlay.firstState ~= nil then
		gCurrentState = curPlay.firstState
		DoRolePosMatch(curPlay, true, false)
	else
		print("Error in ResetPlay!!")
	end
	gTimeCounter = 0
end

function ResetPlayWithLastMatch(name)
	local curPlay = gPlayTable[name]
	world:SPlayFSMSwitchClearAll(true)
	if curPlay.firstState ~= nil then
		gCurrentState = curPlay.firstState
	else
		print("Error in ResetPlay!!")
	end
	gTimeCounter = 0
end

function RunPlay(name)

	if(gPlayTable[name] == nil) then
		print("Error In RunPlay: "..name)
	else
		local curPlay = gPlayTable[name]
		local curState
--		gLastState = gCurrentState
		local isStateSwitched = false
		if curPlay.switch ~= nil then
			curState = curPlay:switch()
			--gCurrentState = curPlay:switch()
		else
			if gCurrentState ~= "exit" and gCurrentState ~= "finish" then
				curState = curPlay[gCurrentState]:switch()
			end
			--gCurrentState = curPlay[gCurrentState]:switch()
		end

		-- if gCurrentState == nil then
		-- 	gCurrentState = gLastState
		if curState ~= nil then
			gLastState = gCurrentState
			gCurrentState = curState
			isStateSwitched = true
			world:SPlayFSMSwitchClearAll(true)
		end

--		debugEngine:gui_debug_msg(vision:ourPlayer(gRoleNum[rolename]):Pos(), rolename)

		DoRolePosMatch(curPlay, false, isStateSwitched)
		gExceptionNum={}

--		for i,v in pairs(gRoleNum) do
--			print(i.."-"..v)
--		end

--~		Play中任务返回规则
--~		1 ---> task, 2 --> matchpos, 3--->kick, 4 --->dir, 5 --->pre, 6 --->kp, 7--->cp, 8--->flag
		kickStatus:clearAll()
		for rolename, task in pairs(curPlay[gRealState]) do
			if (type(task) == "function" and rolename ~= "match" and (gRoleNum[rolename] ~= nil or type(rolename)=="function")) then
				task = task(gRoleNum[rolename])
			end
			if (type(task) == "table" and rolename ~= "match" and (gRoleNum[rolename] ~= nil or type(rolename)=="function")) then
--~ 			统一进行射门和吸球的管理,并做任务处理
				if task[1] == nil then
					task = curPlay[gLastState][rolename]
				end

				local roleNum
				if type(rolename)=="string" then
					roleNum = gRoleNum[rolename]
					--print("Here in string : "..roleNum)
				elseif type(rolename)=="function" then
					roleNum = rolename()
					--print("Here in function : "..roleNum)
					
				end
				
				if roleNum ~= -1 then
					if task[3] ~= nil then
						local mkick = task[3](roleNum)
						local mdir = task[4](roleNum)
						local mpre = task[5](roleNum)
						local mkp  = task[6](roleNum)
						local mcp  = task[7](roleNum)
						local mflag = task[8]
						local isDirOk = world:KickDirArrived(vision:getCycle(), mdir, mpre, roleNum)

						local needDribble = bit:_and(mflag, flag.dribbling)

						if needDribble ~= 0 then
							dribbleStatus:setDribbleCommand(roleNum,3)
						end

						if isDirOk or bit:_and(mflag, flag.force_kick) ~= 0 then
							if mkick == kick.flat() then
								kickStatus:setKick(roleNum, mkp)
							elseif mkick == kick.chip() then
								kickStatus:setChipKick(roleNum, mcp)
							end
						end
					end
					
					if(type(rolename)=="string") then
						debugEngine:gui_debug_msg(vision:ourPlayer(roleNum):Pos(), string.sub(rolename, 1, 1))
					end
					task[1](roleNum) --Very Important !!!!
				end
			end
		end
		gTimeCounter = gTimeCounter + 1
	end
	gLastCycle = vision:getCycle()
end

function NeedExit(name)
	if name == "" then
		return false
	end
	if(gPlayTable[name] == nil) then
		print("Error Skill Name In NeedExit: "..name)
	else
		local curPlay = gPlayTable[name]
		if gCurrentState == "finish" or
		    gCurrentState == "exit" or
		    gTimeCounter > curPlay.timeout or
			vision:getCycle() - gLastCycle > param.frameRate*0.1 then
			gTimeCounter = 0
			-- print("wzdebug:", vision:getCycle(), gLastCycle, "fuck")
			return true
		end
	end
	return false
end
