--
gRoleNum = {
	["Goalie"] = -1,
	["Kicker"] = -1,
	["Assister"] = -1,
	["Special"] = -1,
	["Defender"] = -1,
	["Middle"] = -1,
	["Leader"] = -1,
	["Tier"] = -1,
	["Breaker"] = -1,
	["Fronter"] = -1,
	["Receiver"] = -1,
	["Center"]  = -1,
	["a"] = -1,
	["b"] = -1,
	["c"] = -1,
	["d"] = -1,
	["e"] = -1,
	["f"] = -1,
	["g"] = -1,
	["h"] = -1,
	["i"] = -1,
	["j"] = -1,
	["k"] = -1,
	["l"] = -1,
	["m"] = -1,
	["n"] = -1,
	["o"] = -1,
	["p"] = -1,
	["q"] = -1,
	["r"] = -1,
	["s"] = -1,
	["t"] = -1,
	["u"] = -1,
	["v"] = -1,
	["w"] = -1,
	["x"] = -1,
	["y"] = -1,
	["z"] = -1
}


gLastRoleNum = {

}

gRolePos = {

}

gRolePriority = { "Goalie","Kicker","Tier"}


gOurExistNum = {} --为了使车号统一从0开始，强行给gOurExistNum[0]赋值，但table自身接口全都无法使用了

gRoleLookUpTable = {
-- 角色分为两类
-- 第一类为在gRolePriority和gRoleFixNum中进行真实车号配置的
["G"] = "Goalie",
["K"] = "Kicker",
["T"] = "Tier",
["R"] = "Receiver",

-- 第二类为在脚本中可能会发生匹配变化的角色
["A"] = "Assister",
["S"] = "Special",
["D"] = "Defender",
["M"] = "Middle",
["L"] = "Leader",
["B"] = "Breaker",
["F"] = "Fronter",
["C"] = "Center",
["a"] = "a",
["b"] = "b",
["c"] = "c",
["d"] = "d",
["e"] = "e",
["f"] = "f",
["g"] = "g",
["h"] = "h",
["i"] = "i",
["j"] = "j",
["k"] = "k",
["l"] = "l",
["m"] = "m",
["n"] = "n",
["o"] = "o",
["p"] = "p",
["q"] = "q",
["r"] = "r",
["s"] = "s",
["t"] = "t",
["u"] = "u",
["v"] = "v",
["w"] = "w",
["x"] = "x",
["y"] = "y",
["z"] = "z"
}

function DecodeMatchStr(str)
	local strTable = {}

	local teamCnt = 1
	while true do
		local character = string.sub(str, 1, 1)
		local endChar
		local mode
		if character == '[' then
			endChar = ']'
			mode = "RealTime"
		elseif character == '(' then
			endChar = ')'
			mode = "Once"
		elseif character == '{' then
			endChar = '}'
			mode = "Never"
		else
			break
		end

		local endPos, _ = string.find(str, endChar)
		local teamTable = {}
		for subIndex = 2, endPos-1 do
			table.insert(teamTable, gRoleLookUpTable[string.sub(str, subIndex, subIndex)])
		end
		teamTable["mode"] = mode
		strTable[teamCnt] = teamTable
		teamCnt = teamCnt + 1
		str = string.sub(str,endPos+1)
		if str == "" then break end
	end
	return strTable
end

function GetMatchPotential(num, role)
	local targetPos
	if type(gRolePos[role]) == "function" then
		targetPos = gRolePos[role](num)
	else
		targetPos = gRolePos[role]
	end
	local playerPos = player.pos(num)
	local vec = playerPos - targetPos
	local t = vec:mod2() / 4.0 / 1000000.0
	if DEBUG_MATCH then
		debugEngine:gui_debug_line(targetPos,playerPos,4)
		debugEngine:gui_debug_msg(playerPos+(targetPos-playerPos)/3,string.format("%.2f",t),3)
	end

	return t
end

function RemoveExistNum(num)
	gOurExistNum[num] = -1
end

function DoRoleMatchReset(str)
	-- 循环获得需要重新匹配的角色
	while true do
		local character = string.sub(str, 1, 1)
		
		if character ~= '[' then
			break
		end

		local endPos, _ = string.find(str, ']')
		local roleTable = {}
		local numTable = {}
		for subIndex = 2, endPos-1 do
			local roleNameStr = gRoleLookUpTable[string.sub(str, subIndex, subIndex)]
			table.insert(roleTable, roleNameStr)
			table.insert(numTable, gRoleNum[roleNameStr])
		end

		-- 开始对每个分组进行重新的匹配
		local nrows = table.getn(roleTable)
		local ncols = table.getn(numTable)
		local matrix = Matrix_double_:new_local(nrows, ncols)
		for row = 1, nrows do
			for col = 1, ncols do
				matrix:setValue(row-1, col-1, GetMatchPotential(numTable[col], roleTable[row]))
			end
		end

		local m = Munkres:new_local()
		m:solve(matrix)

		for row = 1, nrows do
			for col = 1, ncols do
				if matrix:getValue(row-1, col-1) == 0 then
					gRoleNum[roleTable[row]] = numTable[col]				
					break
				end
			end
		end

		str = string.sub(str,endPos+1)
		if str == "" then break end
	end
end

function DoMunkresMatch(rolePos)
	local nrows = table.getn(rolePos)
	local ncolsIndex = {}
	local ncols = 0 --table.getn(gOurExistNum)
	for i = 0, param.maxPlayer - 1 do
		if gOurExistNum[i] ~= -1 then
			ncols = ncols + 1
			ncolsIndex[ncols] = i
		end
	end
	local matrix = Matrix_double_:new_local(nrows, ncols)
	for row = 1, nrows do
		for col = 1, ncols do
			matrix:setValue(row-1, col-1, GetMatchPotential(gOurExistNum[ncolsIndex[col]], rolePos[row]))
		end
	end

	local m = Munkres:new_local()
	m:solve(matrix)

	local eraseList = {}
	for row = 1, nrows do	
		for col = 1, ncols do
			if matrix:getValue(row-1, col-1) == 0 then
--~ 				print(rolePos[row], gOurExistNum[col])
				gRoleNum[rolePos[row]] = gOurExistNum[ncolsIndex[col]]
				table.insert(eraseList, gOurExistNum[ncolsIndex[col]])
				break
			end
		end
	end

	for _, value in ipairs(eraseList) do
		RemoveExistNum(value)
	end
end


function DoFixNumMatch(fixNums)
	for _, fixNum in ipairs(fixNums) do
		for i = 0, param.maxPlayer - 1 do
			if gOurExistNum[i] == fixNum then
				gOurExistNum[i] = -1
				return i
			end
		end
		-- for index, carNum in ipairs(gOurExistNum) do
		-- 	if carNum == fixNum then  --CGetRealNum(carNum) //test by wz
		-- 		-- print("Wzdebug: rolematch", "carNum", carNum, "C++ num" , CGetRealNum(carNum))
		-- 		table.remove(gOurExistNum, index)
		-- 		return carNum
		-- 	end
		-- end
	end
	return -1
end

function SetNoMatchRoleZero()
	for _, rolename in pairs(gRoleLookUpTable) do
		if gRoleNum[rolename] == nil then
			gRoleNum[rolename] = -1
		end
	end	
end

function UpdateRole(matchTactic, isPlaySwitched, isStateSwitched)
	if isPlaySwitched then
		gRoleNum = {}
	end
	--print("---------------------------------")

	-- 为了使车号统一从0开始
	-- 但这样lua的table接口全都无法使用，所以只能强行trick放弃代码简洁性保证功能
	for i = 0, param.maxPlayer - 1 do
		if player.valid(i) then
			gOurExistNum[i] = i -- table.insert(ourExistNum, i)
		else
			gOurExistNum[i] = -1
		end
	end
		
	for _, rolename in pairs(gRolePriority) do
		for existname, _ in pairs(gRolePos) do
			if rolename == existname and
				type(gRoleFixNum[rolename]) == "table" then
				gRoleNum[rolename] = DoFixNumMatch(gRoleFixNum[rolename])
				if rolename == "Goalie" then
					CRegisterRole(gRoleNum[rolename], "goalie")
				end
			end
		end
	end
	
	local matchList = {}
	for tactic, value in pairs(matchTactic) do
		if value.mode == "RealTime" or isPlaySwitched or
			(isStateSwitched and value.mode == "Once") then
			table.insert(matchList, value)
		else
			for _, rolename in ipairs(value) do
				gRoleNum[rolename] = gLastRoleNum[rolename]
				RemoveExistNum(gRoleNum[rolename])
			end
		end
	end
	
	for _, value in ipairs(matchList) do -- value -> 需要匹配的角色名表
		local role = {}
		for index = 1, table.getn(value) do
			local haveMatchRobot = false
			local canMatchNum = 0 --table.getn(gOurExistNum)
			for i = 0, param.maxPlayer - 1 do
				if gOurExistNum[i] ~= -1 then
					canMatchNum = canMatchNum + 1
				end
			end
			for rolename, pos in pairs(gRolePos) do
				if value[index] == rolename and canMatchNum >= index then
					table.insert(role, rolename)
					haveMatchRobot = true
					break
				end
			end
			if not haveMatchRobot then
				gRoleNum[value[index]] = -1
			end
		end
		DoMunkresMatch(role)
	end
	
	SetNoMatchRoleZero()
	gLastRoleNum = gRoleNum	
end
