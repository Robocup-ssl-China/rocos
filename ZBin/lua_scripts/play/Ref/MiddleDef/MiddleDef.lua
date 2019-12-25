if type(gOppoConfig.MiddleDef) == "function" then
	gCurrentPlay = cond.getOpponentScript("Ref_MiddleDefV",gOppoConfig.MiddleDef(),6)
else
	gCurrentPlay = cond.getOpponentScript("Ref_MiddleDefV",gOppoConfig.MiddleDef,6)
end