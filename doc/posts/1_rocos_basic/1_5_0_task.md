# Task - 丰富你的任务库

## 在战术包中创建任务库

你可以在你的战术包中创建任务库。假设你的战术包名称叫做`my_tactic_2024`
```{code-block} bash
# in Rocos root directory
mkdir -p Core/my_tactic_2024
cd Core/my_tactic_2024
cp ../tactic/task.lua.template task.lua
```
打开战术包目录下的`task.lua`文件，你可以看到如下内容：
```{code-block} lua
module(..., package.seeall)
-- write your own task functions, and use it with `task.xxx()` in our own play
```

## 编写任务函数 - 以后卫为例

在比赛中，经常会用到后卫角色，我们可以在刚刚创建的`task.lua`中添加如下代码：
::::{tab-set}
:::{tab-item} my_tactic_2024/task.lua
```{code-block} lua
:linenos:

module(..., package.seeall)

-- write your own task functions, and use it with `task.xxx()` in our own play

local inter2penalty = function(line)
    debugEngine:gui_debug_line(line:point1(), line:point2(),4)
    -- 定义禁区相关的坐标值
    local penaltyX = -(param.pitchLength / 2 - param.penaltyDepth - param.playerRadius)
    local penaltyBottom = -param.pitchLength / 2
    local penaltyY = param.penaltyWidth / 2 + param.playerRadius
    -- 定义禁区相关的点
    local penaltyCorner1 = CGeoPoint(penaltyX, penaltyY)
    local penaltyCorner2 = CGeoPoint(penaltyX, -penaltyY)
    local penaltyBottomCorner1 = CGeoPoint(penaltyBottom, penaltyY)
    local penaltyBottomCorner2 = CGeoPoint(penaltyBottom, -penaltyY)
    -- 定义禁区的边
    local penaltyLines = {
        CGeoSegment(penaltyBottomCorner1, penaltyCorner1),
        CGeoSegment(penaltyCorner1, penaltyCorner2),
        CGeoSegment(penaltyCorner2, penaltyBottomCorner2),
    }
    -- 绘制debug信息
    for _, penaltyLine in ipairs(penaltyLines) do
        debugEngine:gui_debug_line(penaltyLine:point1(), penaltyLine:point2(), 4)
    end
    -- 判断交点
    for _, penaltyLine in ipairs(penaltyLines) do
        local inter = CGeoLineLineIntersection(line,penaltyLine)
        local pos = inter:IntersectPoint()
        if penaltyLine:IsPointOnLineOnSegment(pos) then
            debugEngine:gui_debug_line(penaltyLine:point1(), penaltyLine:point2(), 4)
            return CGeoPoint(pos:x(),pos:y())
        end
    end
    return nil
end

function leftBack()
    local penaltyLeft = CGeoPoint(-param.pitchLength / 2, param.goalWidth/2)
    local ipos = function()
        return inter2penalty(CGeoLine(ball.pos(),penaltyLeft)) or penaltyLeft
    end
	local mexe, mpos = GoCmuRush{pos = ipos, dir = dir.playerToBall, vel = CVector(0,0)}
	return {mexe, mpos}
end

function rightBack()
    local penaltyRight = CGeoPoint(-param.pitchLength / 2, -param.goalWidth/2)
    local ipos = function()
        return inter2penalty(CGeoLine(ball.pos(),penaltyRight)) or penaltyRight
    end
    local mexe, mpos = GoCmuRush{pos = ipos, dir = dir.playerToBall, vel = CVector(0,0)}
	return {mexe, mpos}
end
```
:::
::::

在上述代码中，我们先定义了一个局部函数inter2penalty用于计算一条线与己方禁区的交点，然后定义了两个任务函数leftBack和rightBack，分别用于控制左后卫和右后卫。在任务函数中，我们使用了GoCmuRush函数并调用inter2penalty函数计算传入参数。虽然这样的实现方式有一些简略，且从性能的角度，inter2penalty函数这样的通用功能更适合放在c++端，但这不妨碍我们来展现**利用已有的较为通用的Skill进一步封装成任务函数**这一过程。书写一个状态调用两任务函数，可以呈现如下效果：
:::{thumbnail} ../../img/task_backs.png
:::