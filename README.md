# ChinaOpen SSL Code

> RoboCup中国公开赛 小型组开源代码

---
## 更新
* latest
  * 统一编译方式，去除脚本，更名Medusa->Core
* v0.0.1 (2022.10.25)
  * 稳定版本
    * Client使用cmake构建的客户端，内嵌sim模块（修改自开源grSim）
    * Medusa策略核心代码
    * ZBin使用lua完成的策略脚本



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
