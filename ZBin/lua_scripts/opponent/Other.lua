gOppoConfig = {
	CornerKick  = {2},
	FrontKick   = {1},
	MiddleKick  = {1},
	BackKick    = {1},
	CenterKick  = function()
	    if ball.refPosX() > 0 * param.pitchLength then
	        return {1}
	    else
	        return {2}
	    end
	end,
	KickOff     = "Ref_KickOffV1",

	CornerDef   = "Ref_CornerDefV1",
	BackDef     = "Ref_BackDefV1",
	MiddleDef   = "Ref_MiddleDefV1",
	FrontDef    = "Ref_FrontDefV1",
	KickOffDef  = "Ref_KickOffDefV1",

	PenaltyKick = "Ref_PenaltyKickV1",
	PenaltyDef  = "Ref_PenaltyDefV1",
	
	NorPlay     = "NormalPlayV1",
	IfHalfField = false,
	USE_ZPASS = false
}