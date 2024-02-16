#include "PlayerTask.h"
#include "TaskMediator.h"
#include "skillapi.h"
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

void CPlayerTask::setSubTask(const std::string& name, const TaskT& task){
	_pSubTask = SkillAPI::Instance()->createTask(name, task);
}
