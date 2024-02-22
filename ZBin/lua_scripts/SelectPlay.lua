--
-- if gSwitchNum["state"] == "normal" then
-- 	if not player.realNumExist(gSwitchNum["normal"]) and player.realNumExist(gSwitchNum["backup"]) then
-- 		print("change to backup")
-- 		gSwitchNum["state"] = "backup"
-- 	end
-- elseif gSwitchNum["state"] == "backup" then
-- 	if not player.realNumExist(gSwitchNum["backup"]) and player.realNumExist(gSwitchNum["normal"]) then
-- 		print("change to normal")
-- 		gSwitchNum["state"] = "normal"
-- 	end
-- end
gNormalPlay = gOppoConfig.NorPlay
-- 先注掉，在需要时可以开启使用
-- if (not IS_TEST_MODE) and USE_SWITCH and gSwitchNum["state"] == "backup" then
-- 	dofile("./lua_scripts/opponent/Backup.lua")
-- else
-- 	dofile("./lua_scripts/opponent/"..OPPONENT_NAME..".lua")	
-- end

function RunRefScript(name)
	local filename = "./lua_scripts/play/Ref/"..name..".lua"
	dofile(filename)
end

function SelectRefPlay()
	local curRefMsg = vision:getCurrentRefereeMsg()
	debugEngine:gui_debug_msg(CGeoPoint:new_local(0,param.pitchWidth/2+150),curRefMsg)
	if curRefMsg == "" then
		gLastRefMsg = curRefMsg
		return false
	end
	debugEngine:gui_debug_arc(ball.refPos(),6,0,360,3)
	debugEngine:gui_debug_arc(ball.refPos(),8,0,360,3)
	debugEngine:gui_debug_arc(ball.refPos(),10,0,360,3)
	if gLastRefMsg ~= curRefMsg or curRefMsg == "GameStop" then
		ball.updateRefMsg()
	end
	local curRealMsg = vision:getCurrentRefereeMsg()
	if curRealMsg == "OurBallPlacement" or curRealMsg == "TheirBallPlacement" then
		ball.updateRef2PlacePos()
	end
	if gLastRefMsg == curRefMsg and gNextPlay ~= "" then
		gIsRefPlayExit = true
	end
	if gLastRefMsg ~= curRefMsg then
		gIsRefPlayExit = false
	end
	if gIsRefPlayExit then
		gLastRefMsg = curRefMsg
		return false
	end
	RunRefScript(curRefMsg)
	gLastRefMsg = curRefMsg
	return true
end

function SelectBayesPlay()
	--gCurrentPlay = "Nor_PassAndShoot"
	-- gCurrentPlay = "Nor_MorePass"

	--add by twj 14/5/12
	--------------------
	if gNormalPlay == "NormalPlayMessi" then
		world:clearBallStateCouter()
	end
	---------------------
	gCurrentPlay = gNormalPlay
	ResetPlay(gCurrentPlay)
end

if SelectRefPlay() then
	-- or NeedExit(gCurrentPlay)不添加会出问题！！
	--print("gCurrentPlay",gCurrentPlay)
	--print("gLastPlay",gLastPlay)
	if gCurrentPlay ~= gLastPlay or NeedExit(gCurrentPlay) then
		ResetPlay(gCurrentPlay)
		if vision:getCurrentRefereeMsg() ~= "" then
			gIsRefPlayExit = true
		end
		print("New Play: ".. gCurrentPlay)
	end
else
	if gNextPlay ~= "" then
		-- jump into next play
		gCurrentPlay = gNextPlay
		print("JINP: "..gCurrentPlay)
		if gCurrentPlay ~= gLastPlay or
	    	NeedExit(gCurrentPlay) then
	    	ResetPlayWithLastMatch(gCurrentPlay)
	    end
	elseif IS_TEST_MODE then
		debugEngine:gui_debug_msg(CGeoPoint:new_local(param.pitchLength/2-1500,param.pitchWidth/2),"TEST_MODE",0)
	    if gLastPlay == "" or NeedExit(gCurrentPlay) then
	    	gCurrentPlay = gTestPlay
	    end
	    if gCurrentPlay ~= gLastPlay or
	    	NeedExit(gCurrentPlay) then
	    	ResetPlay(gCurrentPlay)
	    end
	else
		--add by twj 14/5/12
		------------------------------------------
		if gNormalPlay == "NormalPlayPass" or gNormalPlay == "NormalPlayPP" or gNormalPlay == "NormalPlayMessi"then
			world:setBallHandler(gRoleNum["Leader"])
			gLastBallStatus=gCurrentBallStatus
			gCurrentBallStatus=world:getBallStatus(vision:getCycle())
		end
		--------------------------------------------
		if gLastPlay == "" or NeedExit(gCurrentPlay) then
			if NeedExit(gCurrentPlay) then
				print("Play: "..gCurrentPlay.." Exit!!")
			end
			SelectBayesPlay()
		end
	end
end

if gCurrentPlay ~= gLastPlay then
	-- RESET TODO
end

gLastPlay = gCurrentPlay
gNextPlay = ""
debugEngine:gui_debug_msg(CGeoPoint:new_local(-param.pitchLength*2/5, param.pitchWidth/2+200),gCurrentPlay)
RunPlay(gCurrentPlay)
gLastTask = gRoleTask
gRoleTask = {}
-- local stateFile=io.open(".\\LOG\\"..gStateFileNameString..".txt","a")
-- if not stateFile then
-- 	print("-------- Can't create LOG FILE !!!!!!!!!!!!! --------")
-- end
-- stateFile:write(vision:getCycle().." "..gCurrentState.." "..gCurrentPlay.." me:"..skillUtils:getOurBestPlayer()
-- 	.." he:"..skillUtils:getTheirBestPlayer().."\n")
-- stateFile:close()

--print( world:getSuitSider())
--print("vel"..enemy.vel(skillUtils:getTheirGoalie()):y()*2)
--print(skillUtils:getTheirGoalie())
--print("hello",skillUtils:getOurBestPlayer())
--print(gCurrentState,vision:getCycle())
--print("ball",ball.posX(),ball.valid())
--print("raw",vision:rawBall():X(),vision:rawBall():Valid())
--print(vision:getBallVelStable(),vision:ballVelValid())
debugEngine:gui_debug_msg(CGeoPoint:new_local(-param.pitchLength*2/5, param.pitchWidth/2),gCurrentState)
--world:drawReflect(gRoleNum["Tier"])
--defenceInfo:setNoChangeFlag()
--print(vision:getCycle()..vision:getCurrentRefereeMsg(),vision:gameState():gameOn())
