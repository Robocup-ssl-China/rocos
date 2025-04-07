#include <fmt/ranges.h>
#include "algmmanager.h"

/*
    * 1. init one algorithm instance for each algorithm using Register<Algm>::create()
    * 2. register algorithm dependencies using AlgmDeps::getDeps()
*/
void AlgmManager::init(){
    fmt::print("Init algorithm mods\n");
    auto&& algm_names = Registry<Algm>::getList();
    for(auto& name : algm_names){
        fmt::print("algm: {} ...", name);
        auto&& algm = Registry<Algm>::create(name);
        algms_[name] = std::move(algm);
        algm_tasks_[name] = taskflow_.emplace([this, name](){
            algms_[name]->process(data_map_);
        }).name(name);
        fmt::print("inited\n", name);
    }
    auto&& deps = AlgmDeps::getDeps();
    for(auto& [name, dep_names] : deps){
        // fmt::print("Algm {} depends on algms: {}\n", name, fmt::join(dep_names, "||"));
        auto&& algm_task = algm_tasks_[name];
        for(auto& dep_name : dep_names){
            fmt::print("Algm {} depends on algm {}\n", name, dep_name);
            auto&& dep_task = algm_tasks_[dep_name];
            dep_task.precede(algm_task);
        }
    }
    fmt::print("------------------------------------\n");
    _step_thread_ = std::thread([this](){
        while(true){
            signal_to_step_.Wait();
            _step();
        }
    });
}

void AlgmManager::signal(const VisualInfoT& _vision, const RefRecvMsg& _refmsg){
    {
        std::lock_guard<std::mutex> lock(input_mutex_);
        input_swap_["vision"] = _vision;
        input_swap_["refmsg"] = _refmsg;
    }
    signal_to_step_.Signal();
}

void AlgmManager::_step(){
    fmt::print("step: ----------------------------\n");
    {
        std::lock_guard<std::mutex> lock(input_mutex_);
        data_map_ = input_swap_;
    }
    executor_.run(taskflow_).wait();
    {
        std::lock_guard<std::mutex> lock(output_mutex_);
        output_swap_ = data_map_;
    }
    fmt::print("step:  done ----------------------\n");
}