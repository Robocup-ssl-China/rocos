#pragma once
#include <singleton.hpp>
#include "skill_registry.h"
#include "staticparams.h"
#include "VisionModule.h"
class CSkillAPI{
public:
    CSkillAPI() = default;
    inline void registerVision(CVisionModule* vision){this->vision = vision;}
    bool run(const std::string& name, const TaskT& task);
    std::unique_ptr<Skill> createTask(const std::string& name, const TaskT& task);
    inline std::string get_name(int i){
        return Registry<Skill>::getList()[i];
    }
    inline int get_size(){
        return Registry<Skill>::getList().size();
    }
private:
    CVisionModule* vision = nullptr;
    std::array<std::pair<std::string,std::unique_ptr<Skill>> , PARAM::ROBOTNUM> skills;
    std::map<std::string, std::array<std::unique_ptr<Skill>, PARAM::ROBOTNUM>> skillPool;
};
typedef Singleton<CSkillAPI> SkillAPI;
