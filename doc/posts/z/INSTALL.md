# 旧版本编译操作

## Windows VS2015-64bit Client与Sim编译

### 安装需求

* [Qt≥5.10](https://www.qt.io/)
* [CMaket≥3.10](<https://cmake.org/download/>)
* [Visual Studio 2015](<https://visualstudio.microsoft.com/zh-hans/vs/support/vs2015/?c=Download+and+Installation>)
* [git bash](<https://gitforwindows.org/>)

### 安装方式

* 下载[第三方库](https://github.com/Robocup-ssl-China/rocos/releases/download/3rdparty-1.0/3rdParty.zip)，放于该工程目录下，解压3rdParty文件夹
* 进入Client文件夹运行build_win64.sh

### 注意事项

* 若系统有多个版本Qt，则在CMake中添加需要版本的路径，如

```cmake
set(CMAKE_PREFIX_PATH C:/Qt/5.14.1/msvc2015_64/lib/cmake)
```
* 若装有Anaconda3，则CMake可能会链接到其中的Qt库，简单的办法是在编译时更改anaconda3文件夹名字，编译后再改回来
* Windows版在链接库中需要添加**Qt5::WinMain**
* 若运行Client.exe后无法打开或后台运行，请在：我的电脑-高级系统设置-环境变量-系统变量-Path中添加Qt动态库的路径，如

```bash
C:/Qt/5.14.1/msvc2015_64/bin
```
若其他路径下也有Qt动态库，请将上述路径置于Path中最前面

* Bug反馈：wangyunkai@zju.edu.cn