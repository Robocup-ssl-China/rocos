# Rocos 更新记录

## [**dev**](https://github.com/Robocup-ssl-China/rocos/tree/dev)
## [**v0.0.4**](https://github.com/Robocup-ssl-China/rocos/releases/tag/v0.0.4)（alpha:2024.4.26）
  * 将Client:gui/Client:sim/Core:cpp/Core:lua四部分的关键param合成一份，使用settings面板控制(PITCH,PENALTY_AREA,GOAL)
  * 增加从Core到Client的机器人指令的可视化（使用settings - debug/DeviceCmd打开）
  * 增加踢球力度的线性映射接口（encodeNJLegacy函数中），可以使用该接口对不同机器人踢球进行统一（平射速度，挑射距离）
  * 将相机观测范围测量(CameraEdgeTest)步骤自动化，利用球的视觉信息做到实时测量
  * 修正球的滚动模型（使用settings - AlertParam/BallDec_xxx进行修改）
  * 整理接口
    * 整合WorldDefine中的接口，删除多继承方案
    * 删除PlayerKick的Command接口，直接使用CommandInterface进行指令下发
    * 删除无用的traj规划接口
    * 删除无用的椭圆形禁区代码
    * 删除cond.lua中的无用函数
  * 增加机器人时间预测的接口，允许临场手动调节（使用settings - CGotoPositionV2/PredictAccRatio进行调节）
  * 回退compute_motion_1d和compute_motion_2d，修正一些非零速时的bug
  * 为gui_debug_x增加size参数
  * 使用新的bufcnt，避免之前c++版本的bufcnt导致的崩溃问题
  * 增加SubPlay提高lua层代码重用性
    * 在parentPlay中
      * 使用gSubPlay.new(PLAY_ID,PlayName)进行subPlay创建，例如gSubPlay.new("MySubPlay","TestRun")
      * 在对应的state中，使用gSubPlay.roleTask(PLAY_ID, RoleName)进行从subPlay到parentPlay的映射，例如`Leader = gSubPlay.roleTask("MySubPlay", "Assister")`将刚以TestRun为模板创建的SubPlay当前状态中的Assister映射到parentPlay的Leader
      * 在不需要使用时，可以使用gSubPlay.del(PLAY_ID)进行删除
      * subPlay中的switch在每一帧都会在后台自行调用，并像普通Play一样根据switch返回值进行状态维护（即使没有使用subPlay中的task）
      * 由于subPlay和parentPlay的match会出现冲突，所以subPlay中的match会被作废，只使用parentPlay中的match匹配，例如上述角色映射后，直接在parentPlay中使用'L'对subPlay中的Assister角色进行匹配即可
    * 在任意想要重用的Play中
      * 将原结构gPlayTable.CreatePlay{}改变成return {}，使其成为可重用的SubPlay（SubPlay依然可以像普通Play一样使用）
      * subPlay中可以有一个新的__init__函数作为Play的构造函数，parentPlay可以在创建时传入参数
      * switch函数中遇到RoleName可以放心使用，在lua框架中会对RoleName进行重新映射，例如在parentPlay如果将subPlay的Assister映射到Leader，那么在subPlay中的switch中继续使用Assister即可
  * 增加一个Skill - CircleRun，用于绕圆运动
  * 修补touch的部分问题，使其功能更强
  * 增加ParamBallDec和ParamPredicTime，用于相关的参数测量，两个脚本放在tt_param战术包中
    * 增加py脚本用于处理ParamPredicTime生成的数据集，方便后续接入机器学习模型
  * 将部分debug_msg信息替换成gui_debug_msg_fix，防止left/right切换时出现显示问题
  * 在utils.lua中增加warning接口，用于显示警告信息
  * 增加一个geomcalc.cpp文件，增加anglediff接口，用于计算从一个角度到另一个角度的最小旋转角度（-pi ~ pi）
  * 测试功能 ： 增加机器人和球的预测移动作为障碍物（圆形->胶囊体），（使用settings - CSmartGotoV2/USE_CAPSULE_OBS打开）
## [**v0.0.3**](https://github.com/Robocup-ssl-China/rocos/releases/tag/v0.0.3)（alpha:2024.2.26, stable:2024.4.26）
  * 可以直接在Client中配置TestScripts以及RefConfig
  * 战术包：用户可以创建独立文件夹作为战术包，战术包可以在自动被识别
  * 添加战术包中对裁判盒脚本跳转的支持
  * 添加持续维护且可编译的文档
  * 简化了Skill创建流程
  * 删除无用的接口和类，简化调用流程
  * 添加Wiki支持，随工程一同更新
  * 在CMakeLists中添加对于tolua:pkg和proto文件的改动监测
  * fix ISSUE
    * [#7](https://github.com/Robocup-ssl-China/rocos/issues/7) 增加skillapi时引进的bug，在lua层对于const CGeoPoint的报错，暂修复
    * [#8](https://github.com/Robocup-ssl-China/rocos/issues/8) 修复*战术包中的NormalPlay不起作用*的问题
  * 增加一个非官方Wiki，包含一些技巧和API解释[Wiki from ZJHU](https://rocos.salta.top/)
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