#ifndef __CPLACE_BALL__H__
#define __CPLACE_BALL__H__

#include <skill/PlayerTask.h>

class CPlaceBall : public CPlayerTask{
public:
    CPlaceBall() = default;
    virtual void plan(const CVisionModule* pVision);
    virtual bool isEmpty() const { return false; }
    virtual void toStream(std::ostream& os) const { os << "PlaceBall"; }
private:
    int _lastCycle;
};
#endif // __CPLACE_BALL__H__
