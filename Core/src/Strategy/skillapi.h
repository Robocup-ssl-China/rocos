#pragma once
#include <singleton.hpp>
#include "registry.h"
#include "VisionModule.h"
class CSkillAPI{
public:
    CSkillAPI() = default;
    inline void registerVision(CVisionModule* vision){this->vision = vision;}
    inline bool run(const std::string& name, const TaskT& task){
        auto pSkill = Registry<Skill>::create(name);
        if(pSkill == nullptr){
            std::cout << "Skill " << name << " not exist, please check again" << std::endl;
            return false;
        }
        auto s = std::unique_ptr<Skill>(pSkill);
        s->reset(task);
        s->plan(vision);
        return true;
    }
    inline std::string get_name(int i){
        return Registry<Skill>::getList()[i];
    }
    inline int get_size(){
        return Registry<Skill>::getList().size();
    }
private:
    CVisionModule* vision = nullptr;
};
typedef Singleton<CSkillAPI> SkillAPI;