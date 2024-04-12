module(..., package.seeall)

-- maxPlayer   = 12
-- pitchLength = 600
-- pitchWidth  = 400
-- goalWidth = 70
-- goalDepth = 20
-- freeKickAvoidBallDist = 50
-- playerRadius	= 9
-- penaltyWidth    = 195
-- penaltyDepth	= 80
-- penaltyRadius	= 80
-- penaltySegment	= 35
-- playerFrontToCenter = 7.6
-- lengthRatio	= 1.5
-- widthRatio	= 1.5
-- stopRatio = 1.1


maxPlayer   = 16
pitchLength = CGetSettings("field/width","Int")
pitchWidth  = CGetSettings("field/height","Int")
goalWidth = CGetSettings("field/goalWidth","Int")
goalDepth = CGetSettings("field/goalDepth","Int")
freeKickAvoidBallDist = 500
playerRadius	= 90
penaltyWidth    = CGetSettings("field/penaltyLength","Int")
penaltyDepth	= CGetSettings("field/penaltyWidth","Int")
penaltyRadius	= 1000
penaltySegment	= 500
playerFrontToCenter = 76
lengthRatio	= 1.5
widthRatio	= 1.5
stopRatio = 1.1
frameRate = 73
--~ -------------------------------------------
--~ used for debug
--~ -------------------------------------------
WHITE=0
RED=1
ORANGE=2
YELLOW=3
GREEN=4
CYAN=5
BLUE=6
PURPLE=7
GRAY=9
BLACK=0
