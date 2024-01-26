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
        auto pSkill = Registry<Skill>::create(name);
        if(pSkill == nullptr){
            std::cout << "Skill " << name << " not exist, please check again" << std::endl;
            return false;
        }
        skills[executor] = std::make_pair(name, std::unique_ptr<Skill>(pSkill));
    }
    auto s = skills[executor].second.get();
    s->reset(task);
    TaskMediator::Instance()->setPlayerTask(task.executor, s, 1);
    // s->plan(vision);
    return true;
}