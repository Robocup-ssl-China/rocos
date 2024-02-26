#pragma once
#include <string>
#include <memory>
#include <functional>
#include <unordered_map>

template <typename T>
class Registry{
    using FactoryFunction = std::function<std::unique_ptr<T>()>;
    using FactoryMap = std::unordered_map<std::string, FactoryFunction>;
    static FactoryMap& getFactoryMap() {
        static FactoryMap map;
        return map;
    }
public:

    static bool add(const std::string& name, FactoryFunction fac){
        auto&& map = getFactoryMap();
        if(map.find(name) != map.end()){
            return false;
        }
        map[name] = fac;
        return true;
    }
    static std::unique_ptr<T> create(const std::string& name){
        auto&& map = getFactoryMap();
        if(map.find(name) == map.end()){
            return nullptr;
        }
        return map[name]();
    }
    static std::vector<std::string> getList(){
        auto&& map = getFactoryMap();
        std::vector<std::string> all_list;
        for(auto& reg : map){
            all_list.push_back(reg.first);
        }
        return all_list;
    }
};