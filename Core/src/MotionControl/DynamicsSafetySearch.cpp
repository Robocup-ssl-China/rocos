#include "DynamicsSafetySearch.h"
#include "time.h"
#include <vector>
#include <cstdlib>
#include <utility>
#include <algorithm>
#include "parammanager.h"
#define min(a,b)    (((a) < (b)) ? (a) : (b))
#define max(a,b)    (((a) > (b)) ? (a) : (b))

#ifndef PI
    #define PI	3.1415926f
#endif

namespace {
#define DIST_IGNORE 3000
#define SAFE 1
#define UNSAFE 0
#define OURPLAYER 0
#define THEIRPLAYER 1
#define THISPLAYER 2
#define BALL 3
#define VELMIN 10 // 1cm/s
#define ACCMIN 10 // 1cm/s^2
#define DEBUGFLAG 0  // 2014/03/12 关闭，为了调试时画面干净
#define ACCURATE_AOID 2500

    bool DRAW_DEBUG_MSG = false;
    double MIN_TEAMMATE_AVOID_DIST = 3*10;
    double MAX_TEAMMATE_AVOID_DIST = 9*10;
    double MIN_OPP_AVOID_DIST = 5*10;
    double MAX_OPP_AVOID_DIST = 18*10;

    double TEAMMATE_AVOID_DIST = 9*10; //PARAM::AvoidDist::TEAMMATE_AVOID_DIST / 2 - 2; // 厘米 13.5
    double OPP_AVOID_DIST = 18*10; //PARAM::AvoidDist::OPP_AVOID_DIST - 2; // 厘米 18
    double MIN_BALL_AVOID_DIST = 3*10;
    double MAX_BALL_AVOID_DIST = 5*10;
    int MIN_PREDICT_FRAME_NUM = 15;
    double BALL_AVOID_DIST = PARAM::AvoidDist::BALL_AVOID_DIST / 2; // 厘米 1.5
    double MAX_DEC_TO_STOP = 800*10;

    double MAX_TRANSLATION_SPEED; // 350
    double MAX_TRANSLATION_ACC;   // 650
    double MAX_TRANSLATION_DEC;   // 650
    double MIN_SPEED_OPEN_DSS;
    int SAO_ACTION = 0;
}
/************************************************************************/
/* modified 2011-12-26 by HQS
 初始化
**********************************************************************/
CDynamicSafetySearch::CDynamicSafetySearch() {
    ZSS::ZParamManager::instance()->loadParam(MAX_TRANSLATION_SPEED,"CGotoPositionV2/MNormalSpeed",300*10);
    ZSS::ZParamManager::instance()->loadParam(MAX_TRANSLATION_ACC,"CGotoPositionV2/MNormalAcc",450*10);
    ZSS::ZParamManager::instance()->loadParam(MAX_TRANSLATION_DEC,"CGotoPositionV2/MNormalDec",450*10);
    ZSS::ZParamManager::instance()->loadParam(MIN_SPEED_OPEN_DSS,"CGotoPositionV2/DSS_MinSpeedOpen",100*10);
    ZSS::ZParamManager::instance()->loadParam(MIN_TEAMMATE_AVOID_DIST,"DSS/DSS_TeammateMinAvoidDist",1*10);
    ZSS::ZParamManager::instance()->loadParam(MAX_TEAMMATE_AVOID_DIST,"DSS/DSS_TeammateMaxAvoidDist",9*10);
    ZSS::ZParamManager::instance()->loadParam(MIN_OPP_AVOID_DIST,"DSS/DSS_OppMinAvoidDist",1*10);
    ZSS::ZParamManager::instance()->loadParam(MAX_OPP_AVOID_DIST,"DSS/DSS_OppMaxAvoidDist",9*10);

    ZSS::ZParamManager::instance()->loadParam(MIN_BALL_AVOID_DIST,"DSS/DSS_BallMinAvoidDist",3*10);
    ZSS::ZParamManager::instance()->loadParam(MAX_BALL_AVOID_DIST,"DSS/DSS_BallMaxAvoidDist",5*10);
    ZSS::ZParamManager::instance()->loadParam(DRAW_DEBUG_MSG,"DSS/DSS_DRAW_DEBUG_MSG",false);
    ZSS::ZParamManager::instance()->loadParam(MIN_PREDICT_FRAME_NUM,"DSS/DSS_PredictFrameNum",8);

    ZSS::ZParamManager::instance()->loadParam(SAO_ACTION,"Alert/SaoAction",0);

    _e = 0.0f;     	       //权值
    _gamma = 0.0f;                //小车的行驶时间
    _pos = vector2f(0.0f,0.0f);   //小车当前的位置
    _vel = vector2f(0.0f,0.0f);   //小车当前的速度
    _nvel = vector2f(0.0,0.0f);   //小车下一时刻的速度
    _acc = vector2f(0.0f,0.0f);   //小车的加速度
    _t_acc = vector2f(0.0f,0.0f); //小车最优加速
    _DECMAX = MAX_TRANSLATION_DEC;  //小车最大的减速度
    _AECMAX = MAX_TRANSLATION_ACC;  //小车最大的加速度
    _VAMX = MAX_TRANSLATION_SPEED;//小车最大的速度
    _stoptime = 0.0f;
    _C = 0.0f;                    //程序执行一个周期的时间
    _CNum = 0.0f;
    _flag = 0;
    for (int i = 0; i < PARAM::Field::MAX_PLAYER; i++) {
        last_acc[i] = vector2f(0.0f, 0.0f);
        last_e[i] = 1.0f;         // 1.0表示上周期的加速度对评分无影响
        find_flag[i] = false;
        priority[i] = 0;          // 默认小车的优先级为0
    }
}
/************************************************************************/
/*  modified 2012-2-14 by HQS
/*  modified 2014-03-07 by YYS
更新小车的优先级, t_priority 越大优先级越高
/************************************************************************/
void CDynamicSafetySearch::refresh_priority(const int player, const int t_priority, const vector2f target, const vector2f pos) {
    if (t_priority > 0) {
        priority[player] = t_priority;
        return;
    }
    else if (t_priority == 0) {
        vector2f dis = target - pos;
        if (dis.length() <= 3 * PARAM::Field::MAX_PLAYER_SIZE && dis.length() > PARAM::Field::MAX_PLAYER_SIZE) { // 如果与目标点的距离小于3个车身且大于1个车身
            priority[player] = 1;
        }
        else if (dis.length() <= PARAM::Field::MAX_PLAYER_SIZE) { // 与目标点的距离小于一个车身，表示离终点很近可以直接过去
            priority[player] = -1;
        }
        else {
            priority[player] = 0;
        }
    }
    if (DEBUGFLAG) {
        cout<<"player:"<<player<<'\t'<<"priority: "<<priority[player-1]<<endl;
    }
}
/************************************************************************/
/* modified 2011-12-26 by HQS
/* modified 2014-03-07 by YYS
 评价函数，评价函数与上周期的评分，以及上周期的加速度方向和大小有关
 为了保证加速度连续性
 tempAcc 为传入的加速度，player 为传入的车号
 返回值为0~1的值
/************************************************************************/
float CDynamicSafetySearch::Evaluation(const vector2f tempAcc, const int player) { // 计算权值函数
    vector2f best_acc = _acc;
    float val1, val2;
    float coef;
    coef = 1.0f - last_e[player];
    val1 = fabs(angle_all(best_acc.x, best_acc.y) - angle_all(tempAcc.x, tempAcc.y));
    if (val1 > PI) {
        val1 = 2 * PI - val1;
    }
    val1 = val1 * val1 + 0.5f * (PI * PI - val1 * val1) / (_AECMAX * _AECMAX) * fabs(best_acc.sqlength() - tempAcc.sqlength());
    val1 = min(val1, PI * PI);
    val2 = fabs(angle_all(last_acc[player].x, last_acc[player].y) - angle_all(tempAcc.x, tempAcc.y));
    if (val2 > PI) {
        val2 = 2 * PI - val2;
    }
    val2 = val2 * val2 + 0.3f * (PI * PI - val2 * val2) / (_AECMAX * _AECMAX) * fabs(_AECMAX * _AECMAX - tempAcc.sqlength());
    val2 = min(val2, PI * PI);
    float temp_e =  ((val1 + coef * val2) / (PI * PI + coef * PI * PI));
    return temp_e; // 包含反馈
}
/************************************************************************/
/* modified 2011-12-22 by HQS
/* modified 2014-03-07 by YYS
此为主函数，输入小车的车号和下周期最优速度，返回小车下一时刻的速度
主要参数说明：
Iter:表示随机查找加速度的个数，个数越多平均速度越慢，个数越少可能找不到最
crash_car_dir和crash_car_num表示最近被撞小车的角度与车号
_C：表示程序一个周期的时间
_CNum：表示程序的预测小车运行的周期
/************************************************************************/
CVector CDynamicSafetySearch::SafetySearch(const int player, CVector Vnext, const CVisionModule* pVision, const int t_priority, const CGeoPoint target, const int flags, const float stoptime, double max_acc) { // 获得下一帧的速度
    if(DRAW_DEBUG_MSG) GDebugEngine::Instance()->gui_debug_msg(pVision->ourPlayer(player).Pos(), QString("vel: %1").arg(pVision->ourPlayer(player).Vel().mod()).toLatin1(), COLOR_PURPLE);
    if((pVision->ourPlayer(player).Vel().mod() < MIN_SPEED_OPEN_DSS &&
            !(flags & PlayerStatus::DODGE_BALL))) { ///速度较小的时候强行关掉DSS
        return Vnext;
    }

    CVector meVel = pVision->ourPlayer(player).Vel();
    double factor = (MAX_TRANSLATION_SPEED == MIN_SPEED_OPEN_DSS ? 0.0 : (meVel.mod() - MIN_SPEED_OPEN_DSS) / (MAX_TRANSLATION_SPEED - MIN_SPEED_OPEN_DSS));
    if(flags & PlayerStatus::MIN_DSS) factor = 0.0;
    TEAMMATE_AVOID_DIST = MIN_TEAMMATE_AVOID_DIST + factor * (MAX_TEAMMATE_AVOID_DIST - MIN_TEAMMATE_AVOID_DIST);
    OPP_AVOID_DIST = MIN_OPP_AVOID_DIST + factor * (MAX_OPP_AVOID_DIST - MIN_OPP_AVOID_DIST);
    BALL_AVOID_DIST = MAX_BALL_AVOID_DIST;
    if(DRAW_DEBUG_MSG)  {
        GDebugEngine::Instance()->gui_debug_msg(pVision->ourPlayer(player).Pos() + CVector(0, 200*10), QString("DSS").toLatin1(), COLOR_PURPLE);
        GDebugEngine::Instance()->gui_debug_msg(pVision->ourPlayer(player).Pos() + CVector(0, 400*10), QString("Teammate: %1").arg(TEAMMATE_AVOID_DIST).toLatin1(), COLOR_PURPLE);
        GDebugEngine::Instance()->gui_debug_msg(pVision->ourPlayer(player).Pos() + CVector(0, 600*10), QString("Opp     : %1").arg(OPP_AVOID_DIST).toLatin1(), COLOR_PURPLE);
        GDebugEngine::Instance()->gui_debug_msg(pVision->ourPlayer(player).Pos() + CVector(0, 800*10), QString("Ball    : %1").arg(BALL_AVOID_DIST).toLatin1(), COLOR_PURPLE);
    }
    GDebugEngine::Instance()->gui_debug_arc(pVision->ourPlayer(player).Pos(), 4*10, 0.0, 360, COLOR_PURPLE);

    if (max_acc > 1) {
        _AECMAX = max_acc;
    }
    _stoptime = stoptime;
    _flag = flags; // 运动标签
    int Iter = 15; // 表示随机查找的次数
    vector2f crashed_car_dir = vector2f(0.0f, 0.0f);
    int crash_car_num = 0;
    vector2f tempAcc = vector2f(0.0, 0.0f);
    _pos = CGeoPoint2vector2f(pVision->ourPlayer(player).Pos());
    refresh_priority(player, t_priority, CGeoPoint2vector2f(target), _pos);
    srand(time(NULL)); // 随机数初始化
    _nvel = CVector2vector2f(Vnext);
    _vel = CVector2vector2f(pVision->ourPlayer(player).Vel());
    _CNum = max(int(_vel.length() / MAX_DEC_TO_STOP * PARAM::Vision::FRAME_RATE), MIN_PREDICT_FRAME_NUM); // 对于一车静止，一车运动的情况，提前量比较少 // 保证下一个周期内不会碰撞
    _C = 1.0f / PARAM::Vision::FRAME_RATE; // 程序执行一个周期的时间
    _acc = (_nvel - _vel) / _C;
    _gamma = _C * _CNum;
    _e = 1.0f;		 // 权值初始化

    int type = 1; // type:0表示我方车，1表示敌方车
    if (priority[player] == -1 || CheckAccel(player, _acc, pVision, crashed_car_dir, crash_car_num, type, _C) == SAFE) {
        _t_acc = _acc;
        _e = 1.0f;
        find_flag[player] = true;
        //GDebugEngine::Instance()->gui_debug_msg(pVision->ourPlayer(player).Pos(), "Me", COLOR_YELLOW);
    }
    else {
        float angle_start, angle_end, max_radius, t_mid_angle;
        float safe_angle_start, safe_angle_end;
        float avoid_dist, avoid_angle;
        if (type == OURPLAYER) {
            avoid_dist = max(TEAMMATE_AVOID_DIST, (crashed_car_dir.length() / 2));
            avoid_angle = asin((TEAMMATE_AVOID_DIST) / (avoid_dist));
        }
        else if(type == THEIRPLAYER) {
            avoid_dist = max((TEAMMATE_AVOID_DIST + OPP_AVOID_DIST), (crashed_car_dir.length()));
            avoid_angle = asin((TEAMMATE_AVOID_DIST + OPP_AVOID_DIST) / (avoid_dist));
        }
        else if (type == BALL) {
            avoid_dist = max((TEAMMATE_AVOID_DIST + BALL_AVOID_DIST), (crashed_car_dir.length()));
            avoid_angle = asin((TEAMMATE_AVOID_DIST + BALL_AVOID_DIST) / (avoid_dist));
        }

        max_radius = _AECMAX;
        t_mid_angle = angle_all(crashed_car_dir.x, crashed_car_dir.y);
        angle_start = t_mid_angle - avoid_angle;
        angle_end = t_mid_angle + avoid_angle;
        safe_angle_start = angle_end;
        safe_angle_end = angle_start + 2 * PI;

        if (DEBUGFLAG) {
            vector2f tl1, tl2;
            tl1 = vector2f(cos(safe_angle_start), sin(safe_angle_start));
            tl2 = vector2f(cos(safe_angle_end), sin(safe_angle_end));
            GDebugEngine::Instance()->gui_debug_line(vector2f2CGeoPoint(_pos), vector2f2CGeoPoint(_pos) + vector2f2CVector(tl1 * avoid_dist), COLOR_RED);
            GDebugEngine::Instance()->gui_debug_line(vector2f2CGeoPoint(_pos), vector2f2CGeoPoint(_pos) + vector2f2CVector(tl2 * avoid_dist), COLOR_BLACK);
        }
        float temp_e = 0.0f; // 临时的权值
        int iter_i = 0;

        tempAcc = last_acc[player];
        if (find_flag[player] == true && CheckAccel(player, tempAcc, pVision, crashed_car_dir, crash_car_num, type, _C) == SAFE) { // 判断上次的加速度是否可行
            _t_acc = tempAcc;
            _e = Evaluation(tempAcc, player);
        }
        else {
            find_flag[player] = false;
        }

        for (iter_i = 0; iter_i < Iter; iter_i++) {//普通处理
            tempAcc = RandomAcc(safe_angle_start, safe_angle_end, max_radius);
            if (DEBUGFLAG) {
                GDebugEngine::Instance()->gui_debug_line(vector2f2CGeoPoint(_pos), vector2f2CGeoPoint(_pos) + vector2f2CVector(tempAcc), COLOR_YELLOW);
            }
            temp_e = Evaluation(tempAcc, player);
            if( temp_e <= _e && CheckAccel(player, tempAcc, pVision, crashed_car_dir, crash_car_num, type, _C) == SAFE)
            {
                _t_acc = tempAcc;
                _e = temp_e;
                find_flag[player] = true;
            }
        }

        if (DEBUGFLAG) {
            GDebugEngine::Instance()->gui_debug_arc(vector2f2CGeoPoint(_pos), TEAMMATE_AVOID_DIST * 5, 0.0f, 360.0f, COLOR_YELLOW);
        }
        if (find_flag[player] == false) {
            GDebugEngine::Instance()->gui_debug_msg(target, "NOT FIND", 1);
        }

    }
    if(fabs(Utils::Normalize(_t_acc.angle() - _acc.angle())) > PI / 12.0) {
        find_flag[player] = false;
        if(DRAW_DEBUG_MSG) GDebugEngine::Instance()->gui_debug_msg(pVision->ourPlayer(player).Pos() + CVector(0, 1000), QString("Invalid result!!!").toLatin1(), COLOR_RED);
    }

    if (find_flag[player] == false) {
        _nvel = vector2f(0.0f, 0.0f);
        if(DRAW_DEBUG_MSG) GDebugEngine::Instance()->gui_debug_msg(pVision->ourPlayer(player).Pos() + CVector(0, 1200), QString("STOP!!!").toLatin1(), COLOR_RED);
        last_acc[player] = (_nvel - _vel) / _C;
        last_e[player] = 1.0f;
    }
    else {
        last_e[player] = _e;
        last_acc[player] = _t_acc;
        _nvel = _t_acc * _C + _vel;
    }

    if (DRAW_DEBUG_MSG) {
        GDebugEngine::Instance()->gui_debug_line(vector2f2CGeoPoint(_pos), vector2f2CGeoPoint(_pos) + vector2f2CVector(_acc), COLOR_GREEN);
        GDebugEngine::Instance()->gui_debug_line(vector2f2CGeoPoint(_pos), vector2f2CGeoPoint(_pos) + vector2f2CVector(_t_acc), COLOR_BLUE);
        if (CheckAccel(player, _t_acc, pVision, crashed_car_dir, crash_car_num, type, EPSILON) == UNSAFE) {
            GDebugEngine::Instance()->gui_debug_msg(CGeoPoint(-20.0*10, -20.0*10), "ERROR", 1);
            CheckAccel(player, _t_acc, pVision, crashed_car_dir, crash_car_num, type, EPSILON);
        }
    }
    return vector2f2CVector(_nvel);
}
/************************************************************************/
/* modified 2011-12-26 by HQS                                           */
/* modified 2014-03-09 by YYS                                           */
/* limitTime 忽略0~limitTime内的碰撞无效								    */
/* type 标记不同类型的车。包括我方，对方，和当前小车                         */
/* 返回是否与其它小车相撞以及返回最近相撞的车号和车的方向                     */
/* limitTime在时间范围内不考虑相撞                                        */
/************************************************************************/
bool compare_order(const std::pair<int, double>& a, const std::pair<int, double>& b) {
    return a.second > b.second;
}

bool CDynamicSafetySearch::CheckAccel (const int player, vector2f acc, const CVisionModule* pVision, vector2f& crash_car_dir, int& crash_car_num, int& type, const float limitTime) {
    // type: 0表示我方车, 1表示敌方车, 2表示现在规划的小车, 3表示球
    std::vector<pair<int, double>> order;
    std::pair<int, double> temp;
    CGeoPoint mePos = pVision->ourPlayer(player).Pos();
    // 己方车
    if (!(_flag & PlayerStatus::NOT_AVOID_OUR_VEHICLE)) { // 躲避我方车
        for(int i = 0; i < PARAM::Field::MAX_PLAYER; i++) { // 将非自己，且有效的车存储到order中
            if(i != player && pVision->ourPlayer(i).Valid()) {
                temp.first = i;
                temp.second = pVision->ourPlayer(i).Pos().dist(mePos);
                if (temp.second < DIST_IGNORE) {
                    order.push_back(temp);
                }
            }
        }
    }
    sort(order.begin(), order.end(), compare_order); // 将己方小车由远到近排序
    while(!order.empty()) {
        const PlayerVisionT& teamate = pVision->ourPlayer(order.back().first);

        vector2f pj = vector2f(teamate.Pos().x(), teamate.Pos().y()); // 位置向量
        vector2f vj = vector2f(teamate.VelX(), teamate.VelY()); // 速度向量
        vector2f Aj = vector2f(teamate.AccX(), teamate.AccY());

        type = OURPLAYER;
        if(CheckRobot(player, _pos, _vel, acc, order.back().first, pj, vj, Aj, type, limitTime) == UNSAFE) { // 判断是否相撞
            crash_car_num = order.back().first;
            float tc;
            if (Aj.sqlength() <= ACCMIN) {
                tc = _C * _CNum;
            }
            else {
                tc = (Aj.norm() * _VAMX - vj).length() / Aj.length();
                tc = min(_C * _CNum, tc);
            }
            crash_car_dir = pj + vector2f(tc * vj.x, tc * vj.y) + vector2f(0.5f * tc * tc * Aj.x, 0.5f * tc * tc * Aj.y) - _pos;
            return UNSAFE; //如果相撞直接返回
        }
        order.pop_back();
    }
    //敌方车
    if (!(_flag & PlayerStatus::NOT_AVOID_THEIR_VEHICLE)) {
        for(int i = 0; i < PARAM::Field::MAX_PLAYER; i++) {
            if(pVision->theirPlayer(i).Valid()) {
                temp.first = i;
                temp.second = pVision->theirPlayer(i).Pos().dist(mePos);
                if (temp.second < DIST_IGNORE) {
                    order.push_back(temp);
                }
            }
        }
    }
    sort(order.begin(), order.end(), compare_order);//将敌方小车由远到近排序
    type = THEIRPLAYER;
    while(!order.empty()) {
        const PlayerVisionT& opp = pVision->theirPlayer(order.back().first);

        vector2f pj = vector2f(opp.Pos().x(), opp.Pos().y());
        vector2f vj = vector2f(opp.VelX(), opp.VelY());
        vector2f Aj = vector2f(opp.AccX(), opp.AccY());
        if(CheckRobot(player, _pos, _vel, acc, order.back().first, pj, vj, Aj, type, limitTime) == UNSAFE) {
            crash_car_num = order.back().first;
            float tc;
            if (Aj.sqlength() <= ACCMIN) {
                tc = _C * _CNum;
            } else {
                tc = (Aj.norm() * _VAMX - vj).length() / Aj.length();
                tc = min(_C * _CNum,tc);
            }
            crash_car_dir = pj + vector2f(tc * vj.x, tc * vj.y) + vector2f(0.5f * tc * tc * Aj.x, 0.5f * tc * tc * Aj.y) - _pos;
            return UNSAFE; // 如果相撞直接返回
        }
        order.pop_back();
    }
    // 球
    type = BALL;
    if(pVision->ball().Valid() && (_flag & PlayerStatus::DODGE_BALL)) {
        vector2f pj = vector2f(pVision->ball().Pos().x(), pVision->ball().Pos().y());
        vector2f vj = vector2f(pVision->ball().VelX(), pVision->ball().VelY());
        const MobileVisionT& last_ball = pVision->ball(pVision->getLastCycle());
        vector2f Aj;
        Aj = (vj - vector2f(last_ball.VelX(), last_ball.VelY())) /_C;
        if(CheckRobot(player, _pos, _vel, acc, -1, pj, vj, Aj, type, limitTime) == UNSAFE) {
            crash_car_num = -1;
            float tc;
            if (Aj.sqlength() <= ACCMIN) {
                tc = _C * _CNum;
            }
            else {
                tc = (Aj.norm() * _VAMX - vj).length() / Aj.length();
                tc = min(_C * _CNum, tc);
            }
            crash_car_dir = pj + vector2f(tc * vj.x, tc * vj.y) + vector2f(0.5f * tc * tc * Aj.x, 0.5f * tc * tc * Aj.y) - _pos;
            return UNSAFE; // 如果相撞直接返回
        }
    }
    return SAFE;
}
/************************************************************************/
/* modified 2011-12-22 by HQS
   根据小车的速度和加速构建小车的路径
   返回两个小车是否相撞
/************************************************************************/
bool CDynamicSafetySearch::CheckRobot(const int player, vector2f pi, vector2f vi, vector2f Ai, const int obstacle, vector2f pj, vector2f vj, vector2f Aj, int type, const float limitTime) {
    // 判断两个小车之间是否相互碰撞
    Trajectory Pi, Pj;
    float tradius = TEAMMATE_AVOID_DIST * sqrt(1 + 2 * vi.length() / ACCURATE_AOID); // 防止两车初始位置已开始相碰
//    float tend = 0.0f; // 小车终止时刻
    Pi._limitTime = limitTime;
    Pj._limitTime = limitTime;
    Pi.MakeTrajectory(player, pi, vi, Ai, 0.0f, _gamma, _stoptime, tradius ,_DECMAX, THISPLAYER); // 构建自己的路径
    float tc = min(vj.length() / _DECMAX, _C * _CNum);
    if (type == THEIRPLAYER) {
        tradius = OPP_AVOID_DIST * sqrt(1 + 2 * vj.length() / ACCURATE_AOID);
    }
    else if (type == OURPLAYER) {
        tradius = TEAMMATE_AVOID_DIST * sqrt(1 + 2 * vj.length() / ACCURATE_AOID) ;
    }
    else if (type == BALL) {
        tradius = BALL_AVOID_DIST * sqrt(1 + vj.length() / 800.0f*10) ;
    }
    Pj.MakeTrajectory(obstacle, pj, vj, Aj, 0.0f, tc, Pi._tend, tradius, _DECMAX, type); // 构建障碍物的路径
    return Pi.CheckTrajectory(Pj);
}
/************************************************************************/
/* modified 2011-12-22 by HQS                        		   */
/* 在加速度空间中随机生成加速度
   start_angle和end_angle表示在规定范围内产生随机加速度
   max_radius表示加速空间中最大的加速度
/************************************************************************/
vector2f CDynamicSafetySearch::RandomAcc(float start_angle,float end_angle,float max_radius) {
    // 最大速度限制圆的半径远大于加速度空间圆的半径
    float theta,coef;
    float ax;
    float ay;
    float tc = _gamma; //预测时间长度
    theta = (float(rand()) / RAND_MAX) * (end_angle - start_angle) + start_angle;//产生0~1的随机数
    coef = (float(rand()) / RAND_MAX);
    ax = max_radius * cos(theta) * coef;
    ay = max_radius * sin(theta) * coef;

    if ((((ax + _vel.x / tc) * (ax + _vel.x / tc) + (ay + _vel.y / tc) * (ay + _vel.y / tc)) - _VAMX * _VAMX / (tc * tc) <= 0)) {
        return vector2f(ax, ay);
    }
    return vector2f(0.0f, 0.0f);
}

int compare (const void * a, const void * b) {
    if(*(const float*)a < *(const float*)b) {
        return -1;
    }
    return *(const float*)a > *(const float*)b;
}

inline vector2f CDynamicSafetySearch::CVector2vector2f(const CVector vec1) {
    return vector2f(vec1.x(), vec1.y());
}
inline vector2f CDynamicSafetySearch::CGeoPoint2vector2f(const CGeoPoint pos) {
    return vector2f(pos.x(), pos.y());
}
inline CVector CDynamicSafetySearch::vector2f2CVector(const vector2f vec1) {
    return CVector(vec1.x, vec1.y);
}
inline CGeoPoint CDynamicSafetySearch::vector2f2CGeoPoint(const vector2f pos) {
    return CGeoPoint(pos.x, pos.y);
}
/************************************************************************/
/* modified 2011-12-22 by HQS               			    */
/* 根据不同类型的小车生成不同种类的轨迹
   qlength表示轨迹的个数,最多3段
/************************************************************************/
void Trajectory::MakeTrajectory(const int player, const vector2f pos, const vector2f vel, const vector2f acc, const float t0, const float tc, const float tend, const float Radius, const float DMAX, const int type) {
    // 储存轨迹
    _qlength = 0;
    if (type == THISPLAYER) {
        _type = THISPLAYER;
        _carnum = player;
        double x0 = pos.x;
        double y0 = pos.y;
        double vx0 = vel.x;
        double vy0 = vel.y;
        double t1 = t0 + tc;
        if (t1 > tend) {
            // 当小车已经到达目的地时,直接减速
            double v0 = sqrt(vx0 * vx0 + vy0 * vy0);
            double ax0 =  - vx0 / v0 * DMAX;
            double ay0 =  - vy0 / v0 * DMAX;
            t1 = tend;
            q[_qlength].init(CCar(CPoint(x0, y0), vx0, vy0, ax0, ay0, Radius), t0, t1);
            _qlength++;
            _tend = t1;
        }
        else {
            double ax0 = acc.x;
            double ay0 = acc.y;
            q[_qlength].init(CCar(CPoint(x0, y0), vx0, vy0, ax0, ay0, Radius), t0, t1);
            _qlength++;
            double x1 = x0 + vx0 * tc + 0.5f * ax0 * tc * tc;
            double y1 = y0 + vy0 * tc + 0.5f * ay0 * tc * tc;
            double vx1 = vx0 + ax0 * tc;
            double vy1 = vy0 + ay0 * tc;
            double v1 = sqrt(vx1 * vx1 + vy1 * vy1);
            if (v1 <= VELMIN) { // 车已停
                _tend = t1;
                _qlength = 1;
                return;
            }
            double ax1 =  - vx1/v1 * DMAX;
            double ay1 =  - vy1/v1 * DMAX;
            double t2 = t1 + v1/DMAX;
            if (t2 > tend) {
                t2 = tend;
            }
            q[_qlength].init(CCar(CPoint(x1, y1), vx1, vy1, ax1, ay1, Radius), t1, t2);
            _qlength++;
            _tend = t2; //确定结束的时间
        }
    }
    else if (type == OURPLAYER) {
        _type = OURPLAYER;
        _carnum = player;
        if (fabs(vel.x) < VELMIN && fabs(vel.y) < VELMIN && fabs(acc.x) < ACCMIN && fabs(acc.y) < ACCMIN) { // 小车静止
            double t1 = tend;
            double x0 = pos.x;
            double y0 = pos.y;
            double vx0 = 0.0f;
            double vy0 = 0.0f;
            double ax0 = 0.0f;
            double ay0 = 0.0f;
            q[_qlength].init(CCar(CPoint(x0, y0), vx0, vy0, ax0, ay0, Radius), t0, t1);
            _qlength++;
        }
        else {
            double t1 = t0 + tc;
            double x0 = pos.x;
            double y0 = pos.y;
            double vx0 = vel.x;
            double vy0 = vel.y;
            double ax0 = acc.x;
            double ay0 = acc.y;
            if (t1 >= tend) {
                t1 = tend;
                q[_qlength].init(CCar(CPoint(x0, y0), vx0, vy0, ax0, ay0, Radius), t0, t1);
                _qlength++;
                return;
            }
            q[_qlength].init(CCar(CPoint(x0, y0), vx0, vy0, ax0, ay0, Radius), t0, t1);
            _qlength++;
            double x1 = x0 + vx0 * tc + 0.5f * ax0 * tc * tc;
            double y1 = y0 + vy0 * tc + 0.5f * ay0 * tc * tc;
            double vx1 = vx0 + ax0 * tc;
            double vy1 = vy0 + ay0 * tc;
            double v1 = sqrt(vx1 * vx1 + vy1 * vy1);
            if (fabs(v1) <= EPSILON) { // 小车静止
                double t2 = tend;
                q[_qlength].init(CCar(CPoint(x1, y1), 0.0f, 0.0f, 0.0f, 0.0f, Radius), t1, t2);
                _qlength++;
                return;
            }
            double ax1 =  - vx1 / v1 * DMAX;
            double ay1 =  - vy1 / v1 * DMAX;
            double t2 = t1 + v1 / DMAX;
            if (t2 >= tend) {
                t2 = tend;
                q[_qlength].init(CCar(CPoint(x1, y1), vx1, vy1, ax1, ay1, Radius), t1, t2);
                _qlength = 2;
            }
            else {
                q[_qlength].init(CCar(CPoint(x1, y1), vx1, vy1, ax1, ay1, Radius), t1, t2);
                _qlength++;
                double x2 = x1 + vx1 * (t2 - t1) + 0.5f * ax1 * (t2 - t1) * (t2 - t1);
                double y2 = y1 + vy1 * (t2 - t1) + 0.5f * ay1 * (t2 - t1) * (t2 - t1);
                double t3 = tend; // 经过1s小车肯定能静止
                q[_qlength].init(CCar(CPoint(x2, y2), 0.0f, 0.0f, 0.0f, 0.0f, Radius), t2, t3);
                _qlength++;
            }
        }
    }
    else { // 对于敌方小车，只考虑他们匀速变化
        _type = type;
        _carnum = player;
        double t1 = tend;
        double x0 = pos.x;
        double y0 = pos.y;
        double vx0 = vel.x;
        double vy0 = vel.y;
        double ax0 = 0.0f;
        double ay0 = 0.0f;
        q[_qlength].init(CCar(CPoint(x0, y0), vx0, vy0, ax0, ay0, Radius), t0, t1);
        _qlength++;
    }
}
/************************************************************************/
/* modified 2011-12-22 by HQS                                           */
/* 判断两个轨迹是否相撞
/************************************************************************/
bool Trajectory::CheckTrajectory(const Trajectory& T1) { // 检测两个轨迹之间是否相互碰撞
    if (T1._type == OURPLAYER) {
        if (((T1.q[0].pX - q[0].pX) * q[0].pVelX + (T1.q[0].pY - q[0].pY) * q[0].pVelY) <= 0 ) { // 表示己方小车在速度方向的后面，则表示安全
            return SAFE;
        }
        else { // 否则就安正常的判断流程判断
            for (int i = 0; i < _qlength; i++) {
                for(int j = 0; j < T1._qlength; j++) {
                    CQuadraticEquation q1 = q[i];
                    CQuadraticEquation q2 = T1.q[j];
                    CQuarticEquation q3 = q1 - q2;
                    if (q3.getIsCrash()) {
                        vector2f tdir = vector2f((q2.pX - q1.pX), (q2.pY - q1.pY));
                        vector2f tdirgo = vector2f(q1.ax, q1.ay);
                        float dir_angle = 1.0f;
                        if (tdir.length() < EPSILON || tdirgo.length() < EPSILON) {
                            dir_angle = 0.0f;
                        }
                        else {
                            dir_angle = cosine(tdir, tdirgo);
                        }
                        /*cout<<"time:"<<q3.getRoot()<<endl;*/
                        //判断小车是否相撞，需要考虑碰撞的时间，以及碰撞小车的速度情况
                        if ((q3.getRoot() >= _limitTime) || (q3.getRoot() <= _limitTime && i == 0 && dir_angle > 0.3f)) {
                            return UNSAFE;
                        }
                    }
                }
            }
        }
    }
    else {
        for(int i = 0; i < _qlength; i++) {
            for( int j = 0; j < T1._qlength; j++) {
                CQuadraticEquation q1 = q[i];
                CQuadraticEquation q2 = T1.q[j];
                CQuarticEquation q3 = q1 - q2;
                if (q3.getIsCrash()) {
                    vector2f tdir = vector2f((q2.pX - q1.pX), (q2.pY - q1.pY));
                    vector2f tdirgo = vector2f(q1.ax, q1.ay);
                    float dir_angle = 1.0f;
                    if (tdir.length() < EPSILON || tdirgo.length() < EPSILON) {
                        dir_angle = 0.0f;
                    }
                    else {
                        dir_angle = cosine(tdir, tdirgo);
                    }
                    //判断小车是否相撞，需要考虑碰撞的时间，以及碰撞小车的速度情况
                    if ((q3.getRoot() >= _limitTime) || (q3.getRoot() <= _limitTime && i == 0 && dir_angle > 0.3f)) {
                        return UNSAFE;
                    }
                }
            }
        }
    }
    return SAFE;
}
