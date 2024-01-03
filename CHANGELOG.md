## 更新
* latest
  * 统一编译方式，去除脚本，更名Medusa->Core
* [**v0.0.1**](https://github.com/Robocup-ssl-China/rocos/releases/tag/v0.0.1) (2022.10.25)
  * 稳定版本
    * Client使用cmake构建的客户端，内嵌sim模块（修改自开源grSim）
    * Medusa策略核心代码
    * ZBin使用lua完成的策略脚本

## 2022ChinaOpen线上赛更新
- 连接远程仿真环境流程
  - 选择仿真环境所在服务器的IP
    - 127.0.0.1为本地仿真不支持联机
    ![image](img/20221122-144438.jpg)
  - 根据实际比赛情况在AlertPort/vision4Remote里更改接收仿真图像的端口
