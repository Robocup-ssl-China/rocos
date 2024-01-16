# Rocos - **RO**bo**C**up **O**pen **S**ource small size league framework

![Rocos](img/rocos.png)

> RoboCup小型组开源框架

* [编译](INSTALL.md) （已更新Ubuntu编译指令）

* [更新记录](CHANGELOG.md)

* [Wiki](https://rocos.readthedocs.io/zh_CN/latest/index.html)

主要贡献名单：

| github                                 | 贡献模块     |
| -------------------------------------- | ------------ |
| [Wayne](https://github.com/zijinoier)  | VisionModule |
| [Luckky](https://github.com/guodashun) | LogModule    |
| [Mark](https://github.com/ZJUMark)     | GUI          |

-----
## 常见问题
* Linux使用发射机可能会遇到串口权限不足的问题

  ```bash
  sudo usermod -a -G dialout $USER
  ```

  执行上述语句后Log Out，再次进入可以获取权限

