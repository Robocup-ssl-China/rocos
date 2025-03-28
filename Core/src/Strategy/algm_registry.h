#pragma once
#include <set>
#include "fmt/core.h"
#include "registry.hpp"
#include "algmbase.h"

#define REGISTER_ALGM(name, ALGM_NAME) \
    static bool name##reg = Registry<Algm>::add(#name, (std::make_unique<ALGM_NAME>));

class AlgmDeps{
    using DepsMap = std::unordered_map<std::string, std::set<std::string>>;
    static DepsMap& getDepsMap(){
        static DepsMap map;
        return map;
    }
public:
    // static bool addDeps(const std::string& name, const std::set<std::string>& deps){
    //     fmt::print("addDeps: {}\n", name);
    //     auto&& map = getDepsMap();
    //     if(map.find(name) != map.end()){
    //         auto old_deps = map[name];
    //         for(auto& dep : deps){
    //             old_deps.insert(dep);
    //         }
    //         return true;
    //     }
    //     map[name] = deps;
    //     fmt::print("addDeps: {} success\n", name);
    //     return true;
    // }
    static bool addDep(const std::string& name, const std::string& dep){
        auto&& map = getDepsMap();
        if(map.find(name) != map.end()){
            map[name].insert(dep);
            return true;
        }
        map[name] = {dep};
        return true;
    }
    static DepsMap& getDeps(){
        return getDepsMap();
    }
};

#define DECLARE_ALGM_DEP(ALGM_NAME, DEP_NAME) \
    static bool ALGM_NAME##_dep_##DEP_NAME = AlgmDeps::addDep(#ALGM_NAME, #DEP_NAME);
