#ifndef Open_Speed_h__
#define Open_Speed_h__

#include "skill_registry.h"
#include <vector>
/************************************************************************/
/*                     COpenSpeed  直接发开环速度                         */
/************************************************************************/
class COpenSpeed : public CPlayerTask {
public:
	COpenSpeed();
	virtual void plan(const CVisionModule* pVision) {}
	virtual CPlayerCommand* execute(const CVisionModule* pVision);
};

REGISTER_SKILL(OpenSpeed, COpenSpeed);
#endif // Open_Speed_h__
