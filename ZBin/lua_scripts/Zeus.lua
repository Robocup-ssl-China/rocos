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
pfile = io.popen('./tools/scan_tool tactic_dir')
for line in pfile:lines() do
	table.insert(tactic_packages, line)
end
-- print("Tactic Packages : ",table.concat(tactic_packages, ","))

local scan_scripts = function(tactic_dir)
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
	local tactic_dir = value .. "/skill/"
	if path_exists(tactic_dir) then
		local skill_files = scan_scripts(tactic_dir)
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

-- load task.lua for each tactic package
for _, value in ipairs(tactic_packages) do
	local task_file = value .. "/task.lua"
	if path_exists(task_file) then
		package.path = value .. "/?.lua;" .. package.path
		package.loaded['task'] = nil
		print("Load Task File : ",task_file)
		require('task')
	end
end

for _, value in ipairs(gPlay) do
	local filename = "./lua_scripts/play/"..value..".lua"
	print("Init : ",filename)
	dofile(filename)
end

-- init play from tactic packages
for _, value in ipairs(tactic_packages) do
	local tactic_dir = value .. "/play/"
	if path_exists(tactic_dir) then
		local play_files = scan_scripts(tactic_dir)
		-- print("Tactic Dir : ",tactic_dir)
		-- print("Skill Files : ",table.concat(play_files, ","))
		for _, filename in ipairs(play_files) do
			print("Init TPs Play : ",filename)
			dofile(filename)
		end
	else
		print("Tactic Dir Not Exists : ",tactic_dir)
	end
end

gRefConfigFiles = {}
if USE_CUSTOM_REF_CONFIG then
	local ref_config_file = "../Core/" .. REF_CONFIG_TACTIC_NAME .. "/PlayConfig.lua"
	if path_exists(ref_config_file) then
		print("Load Ref Config File : ",ref_config_file)
		dofile(ref_config_file)
	else
		debugEngine:gui_debug_msg(CGeoPoint(0,0),"Ref Config File Not Exists : " .. ref_config_file)
	end
end

print("Registered Skill Size : ",skillapi:get_size())
for i=0,skillapi:get_size()-1 do
	print("" .. i .. " SkillName :  " .. skillapi:get_name(i))
end