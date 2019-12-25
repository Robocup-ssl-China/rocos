#ifndef _PLAYER_TASK_H_
#define _PLAYER_TASK_H_
/**
* 队员的任务类.
* 这个类表示队员的各种任务，包括带球，传球，跑位，面向，拦截，卡位等
* 这个类的执行结果是产3系列基本动作
*/
#include <PlayerCommand.h>
#include <misc_types.h>
#include <string>
/************************************************************************/
/*                         CPlayerTask                                  */
/************************************************************************/
class CVisionModule; // 这里只用到CVisionModule的指针,不用include其头文件,防止依赖过多
class CPlayerTask{
public:
	CPlayerTask() : _pSubTask(0){ }
	virtual ~CPlayerTask() { }
	virtual void plan(const CVisionModule* pVision); ///<规划
	virtual CPlayerCommand* execute(const CVisionModule* pVision); ///<执行
	virtual bool isEmpty() const; // 是否空任务
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
    void setSubTask(CPlayerTask* pTask){ _pSubTask = pTask; } // 设置子任务
	CPlayerTask* subTask() const{ return _pSubTask; }
	const TaskT& task() const { return _task; }
	TaskT _task; //　任务的具体内容
private:	
	CPlayerTask* _pSubTask; // 子任务
};
/************************************************************************/
/*                         StatedTask                                   */
/************************************************************************/
class CStatedTask : public CPlayerTask{ // 有状态的task,维持一个状态机
public:	
	CStatedTask() : _state(BEGINNING){ }
	static const int BEGINNING = 0; // 开始状态
	static const int ABORT = 98; //skill执行失败跳出
	static const int ABOUT_TO_FINISH = 99;//进行着，还未完成added by shizhy 2009.1.9
	static const int FINISHED = 100; // 结束状态
	int state() const { return _state; }
	void setState(int state);
	bool isAborted() const { return _state == ABORT; }
	bool isFinished() const { return _state == FINISHED; }
	int getState() const { return _state; }
private:
	int _state; // 当前状态  
};
#endif // _PLAYER_TASK_H_
