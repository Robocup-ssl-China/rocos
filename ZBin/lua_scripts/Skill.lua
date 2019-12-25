gSkillTable = {}

function gSkillTable.CreateSkill(spec)
	assert(type(spec.name) == "string")
	--print("Init Skill: "..spec.name)	
	gSkillTable[spec.name] = spec
	return spec
end
