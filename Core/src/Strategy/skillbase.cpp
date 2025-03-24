#include "skillbase.h"
#include "skillapi.h"
/************************************************************************/
/*                 Skill                                           */
/************************************************************************/
// 虚函数的缺省实现
void Skill::plan(const CVisionModule* pVision) ///<规划
{ 
	if( subTask() ){
		subTask()->plan(pVision);
	}
}
CPlayerCommand* Skill::execute(const CVisionModule* pVision) ///<执行
{ 
	if( subTask() ){
		return subTask()->execute(pVision);
	}
	return 0;
} 

void Skill::setSubTask(const std::string& name, const TaskT& task){
	_pSubTask = SkillAPI::Instance()->createTask(name, task);
}
