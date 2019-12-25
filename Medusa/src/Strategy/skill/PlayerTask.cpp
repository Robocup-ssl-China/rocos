#include "PlayerTask.h"
#include "TaskMediator.h"
/************************************************************************/
/*                 PlayerTask                                           */
/************************************************************************/
// 虚函数的缺省实现
void CPlayerTask::plan(const CVisionModule* pVision) ///<规划
{ 
	if( subTask() ){
		subTask()->plan(pVision);
	}
} 
CPlayerCommand* CPlayerTask::execute(const CVisionModule* pVision) ///<执行
{ 
	if( subTask() ){
		return subTask()->execute(pVision);
	}
	return 0;
} 

bool CPlayerTask::isEmpty() const
{
	// 该函数用于表达: 当没有设置subTask时, isEmpty函数表示上层skill是否直接给指令directCommand.
	// 若直接给指令,则重载isEmpty函数,并返回false; 否则不必重载;
	if( subTask() ){
		return subTask()->isEmpty();
	}
	return true;
}

/************************************************************************/
/*                       CStatedTask                                    */
/************************************************************************/
void CStatedTask::setState(int state)
{
	const int vecNumber = task().executor;
	_state = state;
}