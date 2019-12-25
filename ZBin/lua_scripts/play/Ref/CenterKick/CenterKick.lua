if vision:getCycle() - gOurIndirectTable.lastRefCycle > 6 then
    if type(gOppoConfig.CenterKick) == "function" then
    	gCurrentPlay = cond.getOpponentScript("Ref_CenterKickV",gOppoConfig.CenterKick(),20)
    else
    	gCurrentPlay = cond.getOpponentScript("Ref_CenterKickV",gOppoConfig.CenterKick,20)
    end
end
