#include "yamlparams.h"
#include <fstream>
namespace ZSS{
YAMLParams::YAMLParams(const std::string& name):_name(name){
    touch(_name);
    _config = YAML::LoadFile(_name);
}
YAMLParams::~YAMLParams(){
    std::ofstream fout(_name);
    fout << _config;
}
bool YAMLParams::touch(const std::string& name){
    std::ofstream output(name,std::ios::app);
    return true;
}
YAML::Node YAMLParams::findFinalNode(const std::string& key){
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
}
