# ChinaOpen SSL Code

> RoboCup中国公开赛 小型组开源代码
> iLoboke机器人请使用[v0.0.1](https://github.com/Robocup-ssl-China/rocos/releases/tag/v0.0.1)，目前版本存在回包的bug
---
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

跳转

* [编译](INSTALL.md)

* [已知问题](ISSUE.md)

* [Wiki](https://rocos.readthedocs.io/zh_CN/latest/index.html)

主要贡献名单：

| github                                 | 贡献模块     |
| -------------------------------------- | ------------ |
| [Wayne](https://github.com/zijinoier)  | VisionModule |
| [Luckky](https://github.com/guodashun) | LogModule    |
| [Mark](https://github.com/ZJUMark)     | GUI          |

-----

Linux使用发射机可能会遇到串口权限不足的问题

```bash
sudo usermod -a -G dialout $USER
```

执行上述语句后Log Out，再次进入可以获取权限
