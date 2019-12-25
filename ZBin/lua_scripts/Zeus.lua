package.path = package.path .. ";./lua_scripts/skill/?.lua"
package.path = package.path .. ";./lua_scripts/play/?.lua"
package.path = package.path .. ";./lua_scripts/worldmodel/?.lua"
package.path = package.path .. ";./lua_scripts/utils/?.lua"
package.path = package.path .. ";./lua_scripts/opponent/?.lua"

ranStr=tostring(os.time()):reverse():sub(1, 3)
math.randomseed(ranStr)
math.random(1,10)

--print("RanSeed:"..ranStr)
require(OPPONENT_NAME)
require("Skill")
require("Play")
require("cond")
require("pos")
require("dir")
require("flag")
require("kickflag")
require("param")
require("player")
require("ball")
require("enemy")
require("pre")
require("kp")
require("cp")
require("task")
require("kick")
require("bit")
require("bufcnt")
require("learn")
require("file")
require("score")
--require("combo")

for _, value in ipairs(gSkill) do
	local filename = "./lua_scripts/skill/"..value..".lua"
	dofile(filename)
end

for _, value in ipairs(gPlay) do
	local filename = "./lua_scripts/play/"..value..".lua"
	dofile(filename)
end
