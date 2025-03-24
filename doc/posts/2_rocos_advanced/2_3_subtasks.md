# Skill - 链式调用

> 对于lua的策略来说，一个task只对应一个skill，但一个skill通常是可以继续拆分。除了速度规划本身，几乎任何skill都可以最终拆分成一个机器人到达某个位置的问题。这种拆分的方式，可以让我们更好地复用代码。链式调用是一种常见的拆分方式，通过链式调用，我们可以将一个skill拆分成多个子skill，每个子skill负责一个小的任务，最终完成整个skill的任务。

```{code-block} c++
:linenos:

#include "MyTouch.h"
void MyTouch::plan(const CVisionModule* pVision){
    setSubTask("SmartGoto",task());
    Skill::plan(pVision);
}
```

在之前的小节中，我们在skill的书写时使用了一个`setSubTask("SmartGoto",task());`，这就是一个最简单的链式调用。在这个例子中，我们将`SmartGoto`作为子skill，`task()`作为参数传入。在`SmartGoto`中，在做完路径规划之后会继续调用速度规划的Skill-`Goto`，这样就形成了一个链式调用。

## `setSubTask`的使用

`setSubTask`是一个在`Skill`中的函数，它的定义如下：

```cpp
// skillbase.h
void setSubTask(const std::string& name, const TaskT& task); // 设置子任务
```

`setSubTask`的作用是设置一个子任务，这个子任务会在当前任务执行完毕后继续执行。在`setSubTask`中，我们需要传入两个参数，第一个参数是子任务的名字，第二个参数是子任务的参数。在`setSubTask`中，我们会将子任务的名字和参数保存在`PlayerTask`中，等到当前任务执行完毕后，会通过`Skill::plan(pVision)`调用基类`Skill`的函数继续执行这个子任务。

## `setSubTask`的`TaskT`参数

```{code-block} c++
:linenos:
// misc_types.h
struct TaskT{
	TaskT() : executor(0){ }
	TaskT(const int executor) : executor(executor){ }
	int executor;				// 执行任务的队员号码
	PlayerStatus player;		// 队员的状态
	stBallStatus ball;			// 球的状态
};
```
上面的代码是`TaskT`的定义，`TaskT`是一个结构体，它包含了一个`executor`和两个状态，`player`和`ball`。在`setSubTask`中，我们传入的参数就是`TaskT`类型的，这个参数会在子任务中使用。在子任务中，我们可以通过这个参数获取到当前的队员号码，队员的状态和球的状态，这样我们就可以根据这些信息来做一些决策。关于机器人和球的状态的定义，可以在`misc_types.h`中找到。

## `setSubTask`的使用场景举例

我们以实际策略中的单机器人的拿球后踢球`GetBallAndKick`为例，来说明`setSubTask`的使用场景。在这个例子中，我们需要机器人拿到球，并将球踢出。这个任务可以拆分成两个子任务，一个是拿球，一个是踢球。在拿球时，由于球的运动方向以及最终踢球任务的目标朝向不同，可能存在**截球/追球/静态拿球/逼抢**等不同情况。在踢球时，可能会调用**动态摆脱/推球转身/马赛回旋**等有特色的技能来完成球的转移。这些情况可以进一步交给不同子任务来完成并在`GetBall`中整合，最终完成整个任务。而整个过程中，都会用到避障、路径规划等技能，所以最终的链时调用可能会是这样的：

```{thumbnail} ../../img/2_3_subtask.png
```

## 总结

链式调用是一种常见的拆分方式，通过链式调用，我们可以将一个skill拆分成多个子skill，每个子skill负责一个小的任务，最终完成整个skill的任务。在实际的策略中，我们可以根据任务的复杂程度，将任务拆分成多个子任务，通过链式调用的方式来完成整个任务。这样可以更好地复用代码，提高代码的可读性和可维护性。
