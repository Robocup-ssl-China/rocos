#include <fmt/format.h>
#include "skillapi.h"
#include "TaskMediator.h"
#include "GDebugEngine.h"

bool CSkillAPI::run(const std::string& name, const TaskT& task){
    int executor = task.executor;
    if(executor < 0 || executor >= PARAM::ROBOTNUM){
        std::cout << "Executor " << executor << " not exist, please check again" << std::endl;
        return false;
    }
    auto&& skill_pair = skills[executor];
    if(skill_pair.first != name || skill_pair.second == nullptr){
        // create and reset task pack
        auto&& pSkill = createTask(name, task);
        if(pSkill == nullptr){
            return false;
        }
        skills[executor] = std::make_pair(name, std::move(pSkill));
    }else{
        // simple reset task pack
        skill_pair.second->reset(task);
    }
    auto&& s = skills[executor].second.get();
    TaskMediator::Instance()->setPlayerTask(task.executor, s, 1);
    return true;
}

std::unique_ptr<Skill> CSkillAPI::createTask(const std::string& name, const TaskT& task){
    auto pSkill = Registry<Skill>::create(name);
    if(pSkill == nullptr){
        std::cout << "Skill " << name << " not exist, please check again" << std::endl;
        GDebugEngine::Instance()->gui_debug_msg(CGeoPoint(0,1000-task.executor*200),fmt::format("BOT {} - Skill {} not exist",task.executor,name).c_str(),COLOR_RED);
        return nullptr;
    }
    pSkill->reset(task);
    return pSkill;
}