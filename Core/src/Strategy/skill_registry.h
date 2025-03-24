#pragma once
#include "registry.hpp"
#include "skillbase.h"

#define REGISTER_SKILL(name,SKILL_TYPE) \
    bool name##_entry = Registry<Skill>::add(#name,(std::make_unique<SKILL_TYPE>))
