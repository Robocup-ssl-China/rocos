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
require("utils")
require("learn")
require("file")
require("score")
--require("combo")

for _, value in ipairs(gSkill) do
	local filename = "./lua_scripts/skill/"..value..".lua"
	print("Init Skill : ",filename)
	dofile(filename)
end

-- init skill from tactic packages
local tactic_packages = {}
for line in io.lines("tactic_packages.txt") do
	table.insert(tactic_packages, line)
end
print("Tactic Packages : ",table.concat(tactic_packages, ","))

local scan_skill = function(tactic_dir)
    local t = {}
    local pfile = io.popen('find "'..tactic_dir..'" -name "*.lua" -type f -print')
    for filename in pfile:lines() do
		table.insert(t, filename)
    end
    pfile:close()
    return t
end

local path_exists = function(file)
	local ok, err, code = os.rename(file, file)
	return ok, err
end

for _, value in ipairs(tactic_packages) do
	local tactic_dir = "../Core/"..value.."/skill/"
	if path_exists(tactic_dir) then
		local skill_files = scan_skill(tactic_dir)
		-- print("Tactic Dir : ",tactic_dir)
		-- print("Skill Files : ",table.concat(skill_files, ","))
		for _, filename in ipairs(skill_files) do
			print("Init TPs Skill : ",filename)
			dofile(filename)
		end
	else
		print("Tactic Dir Not Exists : ",tactic_dir)
	end
end

for _, value in ipairs(gPlay) do
	local filename = "./lua_scripts/play/"..value..".lua"
	print("Init : ",filename)
	dofile(filename)
end

-- init play from tactic packages
for _, value in ipairs(tactic_packages) do
	local tactic_dir = "../Core/"..value.."/play/"
	if path_exists(tactic_dir) then
		local skill_files = scan_skill(tactic_dir)
		-- print("Tactic Dir : ",tactic_dir)
		-- print("Skill Files : ",table.concat(skill_files, ","))
		for _, filename in ipairs(skill_files) do
			print("Init TPs Play : ",filename)
			dofile(filename)
		end
	else
		print("Tactic Dir Not Exists : ",tactic_dir)
	end
end

print("Registered Skill Size : ",skillapi:get_size())
for i=0,skillapi:get_size()-1 do
	print("" .. i .. " SkillName :  " .. skillapi:get_name(i))
end