#include "skillapi.h"
#include "TaskMediator.h"

bool CSkillAPI::run(const std::string& name, const TaskT& task){
    int executor = task.executor;
    if(executor < 0 || executor >= PARAM::ROBOTNUM){
        std::cout << "Executor " << executor << " not exist, please check again" << std::endl;
        return false;
    }
    auto&& skill_pair = skills[executor];
    if(skill_pair.first != name || skill_pair.second == nullptr){
        auto&& pSkill = createTask(name, task);
        if(pSkill == nullptr){
            return false;
        }
        skills[executor] = std::make_pair(name, std::move(pSkill));
    }
    auto&& s = skills[executor].second.get();
    TaskMediator::Instance()->setPlayerTask(task.executor, s, 1);
    return true;
}

std::unique_ptr<Skill> CSkillAPI::createTask(const std::string& name, const TaskT& task){
    auto pSkill = Registry<Skill>::create(name);
    if(pSkill == nullptr){
        std::cout << "Skill " << name << " not exist, please check again" << std::endl;
        return nullptr;
    }
    pSkill->reset(task);
    return pSkill;
}