#ifndef STATICPARAMS_H
#define STATICPARAMS_H
//change param for 2018 rule
//wait for change camera number
#include <QString>
namespace PARAM {
    const bool DEBUG = false;
    const int CAMERA = 8;
    const int ROBOTNUM = 16;// max allow robot on the playgroung
    const int ROBOTMAXID = 16;
    const int BALLNUM = 20;
    const int BLUE = 0;
    const int YELLOW = 1;
    const int BALLMERGEDISTANCE = 0;
    const int TEAMS = 2;

    namespace Field{
        const int POS_SIDE_LEFT = 1;
        const int POS_SIDE_RIGHT = -1;
        const int MAX_PLAYER = 16;
        const int MAX_BALL_SPEED = 6300;
        /* 长度单位均为厘米,时间单位均为秒,质量单位为克,角度单位为弧度 */
        /* Ball */
        const double BALL_SIZE	= 21.5; // 半径
        const double BALL_DECAY = -0.8; // 阻力对球的加速度和速度成正比,单位为 /s
        /* Player */
        const double MAX_PLAYER_SIZE = 180;

        extern const double PITCH_LENGTH;                 // 场地长
        extern const double PITCH_WIDTH;                  // 场地宽
        extern const double PENALTY_AREA_WIDTH;           // rectangle禁区宽度
        extern const double PENALTY_AREA_DEPTH;            // rectangle禁区深度
        extern const double GOAL_WIDTH;
        extern const double GOAL_DEPTH;

        const double PITCH_MARGIN = 10;                   // 边界宽度
        const double CENTER_CIRCLE_R = 500;               // 中圈半径

        const double GOAL_POST_AVOID_LENGTH = 20;         //伸进场地内门柱的避障长度
        const double GOAL_POST_THICKNESS = 20;           //门柱宽度

        const double PENALTY_MARK_X = 4800;               // 点球点的X坐标
        const double OUTER_PENALTY_AREA_WIDTH = 1950;     // 外围禁区宽度(界外开球时不能站在该线内)
        const double FREE_KICK_AVOID_BALL_DIST = 500;     // 开任意球的时候,对方必须离球这么远
    }
    namespace Rule{
        const int Version = 2019; // 规则的版本/年份
        const double MaxDribbleDist = 500; // 最大带球距离, 0代表没有限制
    }
    namespace Vision{
        const double FRAME_RATE = 73;
    }
    namespace Latency{
        // 下面分开的部分是给server作仿真用的
        const double TOTAL_LATED_FRAME = 4.7;
    }
    namespace Physics {
        const double G = 9.8;
    }
    namespace Math{
        const double PI = 3.14159265358979323846;
    }
    namespace Vehicle{
        namespace V2{
            const double PLAYER_SIZE = 90;
            const double PLAYER_FRONT_TO_CENTER = 80.0;
            const double PLAYER_CENTER_TO_BALL_CENTER = 93;
            const double KICK_ANGLE = PARAM::Math::PI*30/180; // 可以击球的最大相对身体角度
            const double DRIBBLE_ANGLE = PARAM::Math::PI*17/180; // 可以带球的最大相对身体角度
            const double HEAD_ANGLE = 57*PARAM::Math::PI/180; // 前面的开口角度
        }
    }
    namespace AvoidDist{
        //避障所用参数
        const double TEAMMATE_AVOID_DIST = PARAM::Vehicle::V2::PLAYER_SIZE*3;
        const double OPP_AVOID_DIST = PARAM::Field::MAX_PLAYER_SIZE; // 厘米 18
        const double BALL_AVOID_DIST = PARAM::Field::BALL_SIZE/2+20.0; // 厘米3
    }
}
namespace ZSS {
    const QString ZSS_ADDRESS = "233.233.233.233";
    const QString SSL_ADDRESS = "224.5.23.2";
    const QString REF_ADDRESS = "224.5.23.1";
    const QString LOCAL_ADDRESS = "127.0.0.1";
    namespace Jupyter {
        const int CONTROL_BACK_SEND = 10002;
        const int CONTROL_RECEIVE = 10001;
    }
    namespace Medusa {
        const int DEBUG_MSG_SEND[2] = {20001, 20002};
        const int DEBUG_SCORE_SEND[2] = {20003, 20004};
    }
    namespace Sim {
        const int SIM_RECEIVE = 30001;
        const double G = 9.8;
        const double PI = 3.14159265358979323846;
        const int CHIP_ANGLE = 45;
        const int BLUE_STATUS_PORT = 30011;
        const int YELLOW_STATUS_PORT = 30012;
    }
    namespace Athena {
        const int CONTROL_SEND[PARAM::TEAMS] = {50011, 50012};
        const int CONTROL_BACK_RECEIVE[PARAM::TEAMS] = {60001, 60002};
        const int VISION_SEND[PARAM::TEAMS] = {23333, 23334};
        const int SEND_TO_PYTHON = 41001;
        const int DEBUG_MSG_RECEIVE[2] = {20001, 20002};
        const int DEBUG_SCORE_RECEIVE[2] = {20003, 20004};
        const int SIM_SEND = 20011;
        const double FRAME_RATE = 75;
        const double V_SWITCH = 2000;
        const double ACC_SLIDE = -4000;
        const double ACC_ROLL = -325;
        const float TOTAL_LATED_FRAME = 4.7f;
        const double BALL_DELC_CHANGE_POINT = 2500;
        const double BALL_FAST_DEC = 360;
        const double BALL_SLOW_DEC = 27.5;
        namespace Vision {
            const int BALL_STORE_BUFFER = 100;
            const int ROBOT_STORE_BUFFER = 100;
            const int MAINTAIN_STORE_BUFFER = 1000;
        }
    }
}
#endif // STATICPARAMS_H
