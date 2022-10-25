#ifndef GOALIE_H
#define GOALIE_H
#include <skill/PlayerTask.h>

class CGoalie : public CPlayerTask{
public:
    CGoalie();
    virtual void plan(const CVisionModule* pVision);
    virtual bool isEmpty() const { return false; }
    virtual void toStream(std::ostream& os) const { os << "Goalie"; }
private:
    int _lastCycle;
    enum STATE{
        NOTHING = 0,
        STAND,
        CLEAR,
        SAVE
    };
    STATE _state;
    std::string debug_state;
};
#endif // GOALIE_H
