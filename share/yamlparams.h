#ifndef __YAMLPARAMS_H__
#define __YAMLPARAMS_H__
#include <string>
#include "zos/utils/singleton.h"
#include "yamlparamsconvert.h"
namespace ZSS{
class YAMLParams {
public:
    YAMLParams(const std::string&);
    template<typename T>
    void loadParam(T&, const std::string&, T d = {});
    template<typename T,size_t size>
    void loadParam(T[size], const std::string&, T d[size]);
    template<typename T,size_t size>
    void loadParam(T[size], const std::string&, T d = {});
    ~YAMLParams();
private:
    YAML::Node _config;
    std::string _name;
    YAML::Node findFinalNode(const std::string&);
    bool touch(const std::string& name);
};
template<typename T>
void YAMLParams::loadParam(T& v,const std::string& key,T d){
    auto node = findFinalNode(key);
    if(!node){
        node = d;
    }
    v = node.as<T>();
}
template<typename T,size_t size>
void YAMLParams::loadParam(T v[], const std::string& key, T d){
    auto node = findFinalNode(key);
    if(!node || !node.IsSequence()){
        node = YAML::Node(YAML::NodeType::Sequence);
    }
    for(size_t i=node.size();i<size;i++){
        node[i] = d;
    }
    for(size_t i=0;i<size;i++){
        v[i] = node[i].as<T>();
    }
}
template<typename T,size_t size>
void YAMLParams::loadParam(T v[size], const std::string& key, T d[size]){
    auto node = findFinalNode(key);
    if(!node || !node.IsSequence()){
        node = YAML::Node(YAML::NodeType::Sequence);
    }
    for(size_t i=node.size();i<size;i++){
        node[i] = d[i];
    }
    for(size_t i=0;i<size;i++){
        v[i] = node[i].as<T>();
    }
}

class ParamTheme:public YAMLParams{
public:
    ParamTheme():YAMLParams("theme.yaml"){}
    ~ParamTheme() = default;
};
typedef Singleton<ParamTheme> Theme;
}

#endif // __YAMLPARAMS_H__
