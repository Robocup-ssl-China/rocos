#ifndef __ZOS_SETTINGS_H__
#define __ZOS_SETTINGS_H__
#include <array>
#include <yaml-cpp/yaml.h>
#include <fstream>
#include "zos/posix.h"
#include "zos/log.h"
namespace zos{
class setting {
public:
    setting(const std::string& name):_name(fmt::format("{}.yaml",name)){
        exec(fmt::format("touch {}",_name));
        try {
            _config = YAML::LoadFile(_name);
        }catch(const YAML::ParserException& ex){
            zos::warning("load yaml file {} failed,{}",_name,ex.what());
        }
    }
    template<typename T>
    void loadParam(T& v,const std::string& key,const T& d={}){
        if(std::getenv("ZOS_DEBUG_SETTING")){
            zos::status("loaded {}\n",key);
        }
        auto node = findFinalNode(key);
        if(!node){
            node = d;
        }
        try{
            v = node.as<T>();
        }catch(const YAML::BadConversion& ex){
            zos::warning("load {} failed,{}\n",key,ex.what());
        }
    }
    template<typename T,size_t size>
    void loadParam(std::array<T,size>& v, const std::string& key,const std::array<T,size>& d){
        auto node = findFinalNode(key);
        if(std::getenv("ZOS_DEBUG_SETTING")){
            zos::log("load array '{}' size {},expect size {}\n",key,node.size(),size);
        }
        if(!node){
            node = YAML::Node(YAML::NodeType::Sequence);
        }else if(!node.IsSequence()){
            zos::warning("load array '{}' failed, not a sequence\n",key);
            return;
        }
        for(size_t i=node.size();i<size;i++){
            node[i] = d[i];
        }
        for(size_t i=0;i<size;i++){
            try{
                v[i] = node[i].as<T>();
            }catch(const YAML::BadConversion& ex){
                zos::warning("load array '{}' failed at pos{},{}\n",key,i,ex.what());
            }
        }
    }
    template<typename T,size_t size>
    void loadParam(std::array<T,size>& v, const std::string& key,const T& d={}){
        auto node = findFinalNode(key);
        if(std::getenv("ZOS_DEBUG_SETTING")){
            zos::log("load array '{}' size {},expect size {}\n",key,node.size(),size);
        }
        if(!node){
            node = YAML::Node(YAML::NodeType::Sequence);
        }else if(!node.IsSequence()){
            zos::warning("load array '{}' failed, not a sequence\n",key);
            return;
        }
        for(size_t i=node.size();i<size;i++){
            node[i] = d;
        }
        for(size_t i=0;i<size;i++){
            try{
                v[i] = node[i].as<T>();
            }catch(const YAML::BadConversion& ex){
                zos::warning("load array '{}' failed at pos{},{}\n",key,i,ex.what());
            }
        }
    }
    void save(){
        std::ofstream ff(_name);
        ff << _config;
    }
    ~setting(){
        save();
    }
private:
    YAML::Node _config;
    std::string _name;
    YAML::Node findFinalNode(const std::string& key){
        auto node = _config;
        size_t pos_start = 0,pos_end = 0;
        while((pos_end=key.find('/',pos_start)) != std::string::npos){
            auto k = key.substr(pos_start,pos_end-pos_start);
            pos_start = pos_end+1;
            if(!node[k] || node[k].Type() != YAML::NodeType::Map){
                node[k] = YAML::Node(YAML::NodeType::Map);
            }
            node.reset(node[k]);
        }
        auto k = key.substr(pos_start);
        return node[k];
    }
};
class setting_g : public Singleton<setting_g>,public setting{
public:
    setting_g(const char* _name=""):setting(_name){}
    ~setting_g() = default;
};
}// namespace zos
#endif // __ZOS_SETTINGS_H__