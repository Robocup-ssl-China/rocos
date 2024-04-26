#ifndef _WORLD_DEFINE_H_
#define _WORLD_DEFINE_H_
#include <geometry.h>
/************************************************************************/
/*                       ObjectPoseT                                    */
/************************************************************************/
class ObjectPoseT {
  public:
    ObjectPoseT() : _valid(false), _pos(CGeoPoint(-9999, -9999)), _rawPos(CGeoPoint(-9999, -9999)) { }
    const CGeoPoint& Pos() const {
        return _pos;
    }
    void SetPos(double x, double y) {
        _pos = CGeoPoint(x, y);
    }
    void SetPos(const CGeoPoint& pos) {
        _pos = pos;
    }
    double X() const {
        return _pos.x();
    }
    double Y() const {
        return _pos.y();
    }
    void SetVel(double x, double y) {
        _vel = CVector(x, y);
    }
    void SetVel(const CVector& vel) {
        _vel = vel;
    }
    void SetRawVel(double x, double y) {
        _rawVel = CVector(x, y);
    }
    void SetRawVel(const CVector& vel) {
        _rawVel = vel;
    }
    void SetAcc(double x, double y) {
        _acc = CVector(x, y);
    }
    void SetAcc(const CVector& acc) {
        _acc = acc;
    }
    const CVector& Vel() const {
        return _vel;
    }
    const CVector& RawVel() const {
        return _rawVel;
    }
    const CVector& Acc() const {
        return _acc;
    }
    double VelX() const {
        return _vel.x();
    }
    double VelY() const {
        return _vel.y();
    }
    double AccX() const {
        return _acc.x();
    }
    double AccY() const {
        return _acc.y();
    }
    void SetValid(bool v) {
        _valid = v;
    }
    bool Valid() const {
        return _valid;
    }

    const CGeoPoint& RawPos() const {
        return _rawPos;
    }
    const CGeoPoint& ChipPredictPos() const {
        return _chipPredict;
    }
    double RawDir() const {
        return _rawDir;
    }
    void SetChipPredict(const CGeoPoint& chipPos) {
        _chipPredict = chipPos;
    }
    void SetChipPredict(double x, double y) {
        _chipPredict =  CGeoPoint(x, y);
    }
    void SetRawPos(double x, double y) {
        _rawPos = CGeoPoint(x, y);
    }
    void SetRawPos(const CGeoPoint& pos) {
        _rawPos = pos;
    }
    void SetRawDir(double rawdir) {
        _rawDir = rawdir;
    }
  private:
    CGeoPoint _pos;
    CVector _vel;
    CVector _rawVel;
    CVector _acc;
    bool _valid;

    CGeoPoint _rawPos; // 视觉的原始信息，没有经过预测
    CGeoPoint _chipPredict; //挑球预测
    double _rawDir;
};

// using ObjectPoseT = ObjectPoseT;

/************************************************************************/
/*                        机器人姿态数据结构                               */
/************************************************************************/
struct PlayerPoseT : public ObjectPoseT { // 目标信息
  public:
    PlayerPoseT() : _dir(0), _rotVel(0) { }
    double Dir() const {
        return _dir;
    }
    void SetDir(double d) {
        _dir = d;
    }
    double RotVel() const {
        return _rotVel;
    }
    void SetRotVel(double d) {
        _rotVel = d;
    }
    double RawRotVel() const {
        return _rawRotVel;
    }
    void SetRawRotVel(double d) {
        _rawRotVel = d;
    }
  private:
    double _dir; // 朝向
    double _rotVel; // 旋转速度
    double _rawRotVel;
};
/************************************************************************/
/*                       PlayerVisionT                                  */
/************************************************************************/
class PlayerVisionT : public PlayerPoseT {
  public:
    PlayerVisionT() : _type(0) {}
    void SetType(int t) {
        _type = t;
    }
    int Type() const {
        return _type;
    }
  private:
    int _type;
};

/************************************************************************/
/*                        机器人能力数据结构                               */
/************************************************************************/
struct PlayerCapabilityT {
    PlayerCapabilityT(): maxAccel(0), maxSpeed(0), maxAngularAccel(0), maxAngularSpeed(0), maxDec(0), maxAngularDec(0) {}
    double maxAccel; // 最大加速度
    double maxSpeed; // 最大速度
    double maxAngularAccel; // 最大角加速度
    double maxAngularSpeed; // 最大角速度
    double maxDec;          // 最大减速度
    double maxAngularDec;   // 最大角减速度
};
#endif // _WORLD_DEFINE_H_
