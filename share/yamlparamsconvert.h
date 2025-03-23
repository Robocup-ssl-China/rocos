#ifndef YAMLPARAMSCONVERT_H
#define YAMLPARAMSCONVERT_H
#include <yaml-cpp/yaml.h>
#include <QColor>
namespace YAML{
template<>
struct convert<QColor>{
    static Node encode(const QColor& color){
        Node node;
        node.push_back(color.red());
        node.push_back(color.green());
        node.push_back(color.blue());
        node.push_back(color.alpha());
        return node;
    }
    static bool decode(const Node& node,QColor& color){
        if(!node.IsSequence() || node.size() < 3 || node.size() > 4) {
            return false;
        }
        color.setRgb(node[0].as<int>(),node[1].as<int>(),node[2].as<int>());
        if(node.size()>3){
            color.setAlpha(node[3].as<int>());
        }
        return true;
    }
};
}
#endif // YAMLPARAMSCONVERT_H
