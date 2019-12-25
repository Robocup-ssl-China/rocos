if vision:getCycle() - gOurIndirectTable.lastRefCycle > 6 then
	if type(gOppoConfig.FrontKick) == "function" then 
		gCurrentPlay = cond.getOpponentScript("Ref_FrontKickV", gOppoConfig.FrontKick(), 11)
	else
		gCurrentPlay = cond.getOpponentScript("Ref_FrontKickV", gOppoConfig.FrontKick, 11)
	end
end