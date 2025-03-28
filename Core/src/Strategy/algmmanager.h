#pragma once
#include <string>
#include <unordered_map>
#include <memory>
#include "taskflow/taskflow.hpp"
#include "algm_registry.h"
class AlgmManager{
public:
    AlgmManager() = default;
    void init();
    void step(int);
private:
    std::unordered_map<std::string, std::unique_ptr<Algm>> algms_;
    std::unordered_map<std::string, tf::Task> algm_tasks_;
    tf::Taskflow taskflow_;
    tf::Executor executor_;
    DataMap data_map_;
};