#pragma once
#include <iostream>
#include <string>
#include <functional>
#include <unordered_map>
#include "PlayerTask.h"
#define REGISTER_SKILL(name,create_func) \
    bool name##_entry = SkillRegistry<CPlayerTask>::add(#name,(create_func))

template <typename T>
CPlayerTask* create(){
    return new T;
}

template <typename T>
class SkillRegistry{
public:
    using FactoryFunction = std::function<T*()>;
    using FactoryMap = std::unordered_map<std::string, FactoryFunction>;
    static bool add(const std::string& name, FactoryFunction fac){
        auto&& map = getFactoryMap();
        if(map.find(name) != map.end()){
            return false;
        }
        map[name] = fac;
        return true;
    }
    static T* create(const std::string& name){
        auto&& map = getFactoryMap();
        if(map.find(name) == map.end()){
            return nullptr;
        }
        return map[name]();
    }
    static std::vector<std::string> getSkillList(){
        auto&& map = getFactoryMap();
        std::vector<std::string> skill_list;
        for(auto& skill : map){
            skill_list.push_back(skill.first);
        }
        return skill_list;
    }
private:
    static FactoryMap& getFactoryMap() {
        static FactoryMap map;
        return map;
    }
};