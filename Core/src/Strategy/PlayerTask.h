#ifndef _PLAYER_TASK_H_
#define _PLAYER_TASK_H_
/**
* 队员的任务类.
* 这个类表示队员的各种任务，包括带球，传球，跑位，面向，拦截，卡位等
* 这个类的执行结果是产3系列基本动作
*/
#include <memory>
#include <PlayerCommand.h>
#include <misc_types.h>
#include <string>
/************************************************************************/
/*                         CPlayerTask                                  */
/************************************************************************/
class CVisionModule; // 这里只用到CVisionModule的指针,不用include其头文件,防止依赖过多
class CPlayerTask{
public:
	CPlayerTask() = default;
	virtual ~CPlayerTask() = default;
	virtual void plan(const CVisionModule* pVision); ///<规划
	virtual CPlayerCommand* execute(const CVisionModule* pVision); ///<执行
	friend std::ostream& operator << (std::ostream& os, const CPlayerTask& task)
	{
		task.toStream(os);
		if( task.subTask() ){
			os << "-->" << *(task.subTask());
		}
		return os;
	}
	virtual void reset(const TaskT& task) { _task = task; }
protected:
	virtual void toStream(std::ostream& os) const { os << "Invalid task"; } ///<输出字符串形式信息
	void setSubTask(const std::string& name, const TaskT& task); // 设置子任务
	CPlayerTask* subTask() const{ return _pSubTask.get(); }
	const TaskT& task() const { return _task; }
	TaskT _task; //　任务的具体内容
private:	
	std::unique_ptr<CPlayerTask> _pSubTask; // 子任务
};
using Skill = CPlayerTask;
#endif // _PLAYER_TASK_H_
