# Play - 响应裁判指令

> 在之前的内容中，我们已经介绍了如何书写一个简单的`Play`。在本节中，我们将介绍如何配置`Play`来响应裁判指令。

## 查看当前裁判信息

你可以在开启了`Core`程序后，在裁判盒界面模拟任意裁判指令，在调试面板上你将看到由`Core`程序输出的裁判指令。

```{thumbnail} ../../img/client_ref.png
```
面板的**左上角**显示了当前运行的**脚本与状态**，在**中央的上部分**显示了**当前的裁判指令**，其中第一行是lua层的**裁判指令字符串**，第二行为c++层收到的裁判指令。接收的流程为，c++层接收到的裁判信息存在内存中，lua层在每次被调用时通过判断是否有新的裁判信息来决定脚本的跳转。如果需要依据裁判信息来进行判断，会根据上述的**裁判指令字符串**选取脚本并执行。

目前的所有指令为

| 指令 | 说明 | 编号 | Protobuf |
| --- | --- | --- | --- |
| `GameHalt` | 比赛停止 | 0 | `HALT` |
| `GameStop` | 比赛暂停 | 1 | `STOP` |
| `OurTimeout` | 我方暂停 | 12或13 | `TIMEOUT_YELLOW` 或 `TIMEOUT_BLUE` |
| `TheirIndirectKick` | 对方间接任意球 | 10或11 |`INDIRECT_FREE_YELLOW` 或 `INDIRECT_FREE_BLUE` |
| `OurIndirectKick` | 我方间接任意球 | 10或11 | `INDIRECT_FREE_YELLOW` 或 `INDIRECT_FREE_BLUE` |
| `TheirKickOff` | 对方开球 | 4或5 | `PREPARE_KICKOFF_YELLOW` 或 `PREPARE_KICKOFF_BLUE` |
| `OurKickOff` | 我方开球 | 4或5 | `PREPARE_KICKOFF_YELLOW` 或 `PREPARE_KICKOFF_BLUE` |
| `TheirBallPlacement` | 对方自动放球 | 16或17 | `BALL_PLACEMENT_YELLOW` 或 `BALL_PLACEMENT_BLUE` |
| `OurBallPlacement` | 我方自动放球 | 16或17 | `BALL_PLACEMENT_YELLOW` 或 `BALL_PLACEMENT_BLUE` |
| `TheirPenaltyKick` | 对方点球 | 6或7 | `PREPARE_PENALTY_YELLOW` 或 `PREPARE_PENALTY_BLUE` |
| `OurPenaltyKick` | 我方点球 | 6或7 | `PREPARE_PENALTY_YELLOW` 或 `PREPARE_PENALTY_BLUE` |

:::{admonition} 提示
裁判指令到字符串的转换在`Core`的VisionModule中完成，可以在`VisionModule.cpp`的`updateRefereeMsg`函数中查看。
裁判指令的编号以及对应的protobuf消息可以在`ssl_referee.proto`中查看。
:::


## 响应裁判指令

在之前`Play`的创建时，我们提到了战术包的概念，为了响应裁判指令，我们依然可以在战术包中进行配置。我们在之前创建的`my_tactic_2024`文件夹中使用`Rocos`的模板文件创建一个`PlayConfig.lua`的文件

```{code-block} bash
# in Rocos root directory
mkdir -p Core/my_tactic_2024
cd Core/my_tactic_2024
cp ../tactic/PlayConfig.lua.template ./PlayConfig.lua
```

打开`PlayConfig.lua`文件，将第二段注释中，针对`gRefConfig`的部分取消注释。

```{code-block} lua
:linenos:

--[[
- `GameHalt`            : 比赛停止
- `GameStop`            : 比赛暂停
- `OurTimeout`          : 我方暂停
- `TheirIndirectKick`   : 对方间接任意球
- `OurIndirectKick`     : 我方间接任意球
- `TheirKickOff`        : 对方开球
- `OurKickOff`          : 我方开球
- `TheirBallPlacement`  : 对方自动放球
- `OurBallPlacement`    : 我方自动放球
- `TheirPenaltyKick`    : 对方点球
- `OurPenaltyKick`      : 我方点球
- `NormalPlay`          : 正常比赛
]]

---[[
gRefConfig = {
    GameHalt = "",
    GameStop = "",
    OurTimeout = "",
    TheirIndirectKick = "",
    OurIndirectKick = "",
    TheirKickOff = "",
    OurKickOff = "",
    TheirBallPlacement = "",
    OurBallPlacement = "",
    TheirPenaltyKick = "",
    OurPenaltyKick = "",
    NormalPlay = "",
}
--]]
```
你可以对上面的table进行配置，当裁判指令到来时，`Core`会根据`gRefConfig`的配置来选择对应的`Play`进行执行。
对于每一项指令的运行脚本，你可以选择以下任意一种类型：
```{code-block} lua
-- 第一种：String类型，直接使用脚本名称
gRefConfig = {
    GameHalt = "Halt_v1",
    ...
}
-- 第二种：Table类型，框架会在每次选择时随机选择一个脚本
gRefConfig = {
    ...
    OurKickOff = {"KickOff_v1", "KickOff_v2"},
    ...
}
-- 第三种：Function类型，返回类型为String或Table，框架会在每次选择时调用该函数，例如模拟旧版Rocos的定位球分区
gRefConfig = {
    ...
    OurIndirectKick = function()
        if ball.posX() > 0 then
            return {"FrontKick_v1","FrontKick_v2"}
        else
            return "BackKick_v1"
        end
    end,
    ...
}
```
配置的脚本名称可以是Rocos自带或任意战术包的任意Play，至此组成了一个Tactic的完整配置。如果有多个战术包中存在`PlayConfig.lua`文件，则可以在`Client`中启动`Core`之前进行选择，同时记得勾选`USE`启用战术包。
:::{thumbnail} ../../img/client_ui_ref.png
:width: 50%
:align: center
:::