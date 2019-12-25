if vision:getCycle() - gOurIndirectTable.lastRefCycle > 6 then
    if type(gOppoConfig.BackKick) == "function" then
  	  gCurrentPlay = cond.getOpponentScript("Ref_BackKickV", gOppoConfig.BackKick(), 3)
    else
      gCurrentPlay = cond.getOpponentScript("Ref_BackKickV", gOppoConfig.BackKick, 3)
    end
  --end
end