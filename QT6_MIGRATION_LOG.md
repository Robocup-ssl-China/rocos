* 安装Qt6及相关依赖
    * `apt install qt6-base-dev qt6-declarative-dev libqt6serialport6-dev qml6-module-qtquick qml6-module-qtquick-controls qml6-module-qtquick-layouts qml6-module-qtquick-window qml6-module-qtqml-workerscript qml6-module-qtquick-templates qml6-module-qtquick-dialogs `
* 安装KDDockWidgets解决界面历史遗留问题
    * 安装`qt6-base-private-dev qt6-declarative-private-dev`
    * `cmake .. -DKDDockWidgets_QT6=ON`