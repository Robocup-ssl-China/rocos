# Rocos 更新记录

## [**dev**](https://github.com/Robocup-ssl-China/rocos/tree/dev)
## [**v0.0.3**](https://github.com/Robocup-ssl-China/rocos/releases/tag/v0.0.3)（2024.2.26）
  * 可以直接在Client中配置TestScripts以及RefConfig
  * 战术包：用户可以创建独立文件夹作为战术包，战术包可以在自动被识别
  * 添加战术包中对裁判盒脚本跳转的支持
  * 添加持续维护且可编译的文档
  * 简化了Skill创建流程
  * 删除无用的接口和类，简化调用流程
  * 添加Wiki支持，随工程一同更新
  * 在CMakeLists中添加对于tolua:pkg和proto文件的改动监测
## [**v0.0.2**](https://github.com/Robocup-ssl-China/rocos/releases/tag/v0.0.2)（2024.1.15）
  * 统一编译方式为cmake，使用root下CMakeLists.txt合并为一个工程，代替原本的三个工程
  * 更新主题色！（Teal - #279BE6）
  * 更名CrazyNaN - Controller，Medusa - Core
  * 修正Controller的布局，适配不同宽度
  * 更新部分接口
    * OpenSpeed
    * GoCmuRush的加速度/速度限定
  * 添加Benchmark脚本用于测试
    * TestBenchmark_Acc
    * TestBenchmark_Dribble
  * 添加用于Debug的接口（gui_debug_msg支持不同字体样式-字号，加粗）
  * 修复部分bug
    * 修复旋转速度的在left或right时的错误问题
    * 修复吸球力度不正确的bug
    * 修复TZ-S01与旧机器人回报机器人号码不一致的问题
    * 修复RunMultiPos与“保持”匹配规则不适配的问题
  * 添加更直观的RobotSensor可视化
  * wiki中添加Ubuntu的安装依赖
  * 支持TZ-S01机型，适配旧版本发射机
  * Controller支持机器人回包输出
  * 添加机器人速度的补偿接口（velRegulation）
## [**v0.0.1**](https://github.com/Robocup-ssl-China/rocos/releases/tag/v0.0.1) (2022.10.25)
  * 稳定版本
    * Client使用cmake构建的客户端，内嵌sim模块（修改自开源grSim）
    * Medusa策略核心代码
    * ZBin使用lua完成的策略脚本

## 2022ChinaOpen线上赛更新
- 连接远程仿真环境流程
  - 选择仿真环境所在服务器的IP
    - 127.0.0.1为本地仿真不支持联机 \
    ![image](../../img/chinaopen2022.jpg){w=600px}
  - 根据实际比赛情况在AlertPort/vision4Remote里更改接收仿真图像的端口