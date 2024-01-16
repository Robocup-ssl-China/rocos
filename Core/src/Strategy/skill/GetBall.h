#ifndef __CPP_GET_BALL_H__
#define __CPP_GET_BALL_H__

#include <skill/PlayerTask.h>

class CGetBall : public CPlayerTask{
public:
    CGetBall() = default;
    virtual void plan(const CVisionModule* pVision);
    virtual bool isEmpty() const { return false; }
    virtual void toStream(std::ostream& os) const { os << "GetBall"; }
private:
    int _lastCycle;
};
#endif // __CPP_GET_BALL_H__
