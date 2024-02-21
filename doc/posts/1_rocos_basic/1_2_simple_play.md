# Play - 让机器人动起来

> 本节目标：
> - 理解什么是Play，Play有哪些组成部分？
> - 如何创建一个简单的Play？
> - 运行并测试Play。

## 什么是Play？

在[Rocos的架构](#0_1_rocos_structure)的讲解中，我们简要的了解了Core模块针对策略的五层。其中，**脚本Play**作为**多机器人配合策略**的具体实现，是Rocos上手的第一步。无论是响应比赛编写复杂策略，还是日常针对单Skill的测试，都需要通过Play来让机器人动起来。

我们先以一个最简单的Play为例，来看看Play的组成部分。

:::{card} 单机器人运动的Play（简化版）
```{code-block} lua
:linenos:
gPlayTable.CreatePlay{
    firstState = 'run1', -- 初始状态

    ['run1'] = {
        switch = function() -- 判断如果到达目标点则结束脚本
            if player.toTargetDist('Leader') < 100 then
                return 'finish'
            end
        end,
        Leader = task.goto(Pos(0,0)), -- 设置任务：移动到(0,0)
        match = '(L)' -- 用于Leader角色的单次匹配
    },

    name = "TestRun", -- Play的名称
}
```
:::

上面的代码是一个简化版的Play，它由`任务状态`/`初始状态`/`脚本名称`组成，状态下有一个Leader角色执行了`task.goto`任务并设置了匹配规则。Play的执行过程如下：
1. 初始状态为`run1`，Leader角色执行`task.goto(Pos(0,0))`任务，向目标点移动。
2. 在离目标点距离小于100mm后，切换到`finish`状态，Play结束。

:::{admonition} 提示
Play的状态切换是通过`switch`函数来实现的，`switch`函数返回值为下一个状态的名称，如果返回`nil`则表示不切换状态。
:::

## 创建一个简单的Play

从`Rocos v0.1`开始，我们引入了战术包的概念，战术包是一套Skill和Play的组合，可以在不修改其他框架代码的情况下在战术包内随意增加、删除和修改，提升了代码的可维护性和可扩展性。

首先，如果你在Core/目录下还没有创建过自己的战术包，可以创建一个文件夹，作为自己的战术包，例如我们创建一个叫做my_tactic_2024的战术包。在my_tactic_2024/目录下创建一个play文件夹，并在play下创建`TestMyRun.lua`文件

```bash
mkdir -p Core/my_tactic_2024/play # 创建战术包文件夹
cd Core/my_tactic_2024/play # 进入战术包文件夹
touch TestMyRun.lua # 创建空的skill文件
```
```{warning}
由于所有的战术包都会共用一个环境，所以切记不管是在c++还是在lua层，都不可以出现重名。
```
在创建之后，你可能会得到如下的目录结构：
```
Core
└── my_tactic_2024
    └── play
        └── TestMyRun.lua
```
我们在TestMyRun.lua中添加如下内容：
::::{tab-set}

:::{tab-item} TestMyRun.lua
```{code-block} lua
:linenos:
gPlayTable.CreatePlay{

firstState = "run1",

["run1"] = {
    switch = function()
        if player.toTargetDist('Leader') < 100 then
            return 'run2'
        end
    end,
	Leader = task.goCmuRush(CGeoPoint(1000,1000)),
	match = "(L)"
},
["run2"] = {
    switch = function()
        if player.toTargetDist('Leader') < 100 then
            return 'run1'
        end
    end,
	Leader = task.goCmuRush(CGeoPoint(-1000,-1000)),
	match = "{L}"
},
name = 'TestMyRun',
}
```
:::
::::

在这个Play中，我们定义了两个状态`run1`和`run2`，在`run1`状态下，Leader角色执行`task.goCmuRush(CGeoPoint(1000,1000))`任务，向目标点移动。在`run2`状态下，Leader角色执行`task.goCmuRush(CGeoPoint(-1000,-1000))`任务，向目标点移动。当Leader角色到达目标点时，切换到另一个状态。

(test_script)=
## 运行&测试Play

:::{admonition} 提示
如果你熟悉最初的Rocos，那么你可能会发现，在`Rocos v0.1`之后，我们引入了战术包的概念以此来分离框架代码和策略代码来进一步简化策略的构建和测试流程。针对运行和测试Play而言，你不再需要在`Config.lua`中进行手动设置，而可以直接在`Client`中图形化的进行操作。
:::

在完成Play的编写之后，我们可以通过`Client`来运行和测试Play。

::::{card} Client中针对Play测试的设置和按钮
:text-align: center

:::{thumbnail} ../../img/client_test_settings.png
:::

::::

`Client`的每次重启都会加载最新的战术包，如果不重启也可以手动刷新。你可以通过勾选设置是否进入测试模式，并在下拉框中选择你想要测试的Play，然后点击按钮来运行。

现在你可以通过界面配置并执行刚刚书写的`TestMyRun`了。