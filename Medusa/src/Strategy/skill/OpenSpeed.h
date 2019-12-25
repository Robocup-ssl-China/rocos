#ifndef Open_Speed_h__
#define Open_Speed_h__

#include <skill/PlayerTask.h>
#include <vector>
/************************************************************************/
/*                     COpenSpeed  直接发开环速度                         */
/************************************************************************/
class COpenSpeed : public CPlayerTask {
public:
	COpenSpeed();
	virtual void plan(const CVisionModule* pVision) {}
	virtual CPlayerCommand* execute(const CVisionModule* pVision);
	virtual bool isEmpty() const { return false; }
};
#endif // Speed_h__