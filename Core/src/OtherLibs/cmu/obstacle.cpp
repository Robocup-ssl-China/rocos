/* LICENSE:
  =========================================================================
    CMDragons'02 RoboCup F180 Source Code Release
  -------------------------------------------------------------------------
    Copyright (C) 2002 Manuela Veloso, Brett Browning, Mike Bowling,
                       James Bruce; {mmv, brettb, mhb, jbruce}@cs.cmu.edu
    School of Computer Science, Carnegie Mellon University
  -------------------------------------------------------------------------
    This software is distributed under the GNU General Public License,
    version 2.  If you do not have a copy of this licence, visit
    www.gnu.org, or write: Free Software Foundation, 59 Temple Place,
    Suite 330 Boston, MA 02111-1307 USA.  This program is distributed
    in the hope that it will be useful, but WITHOUT ANY WARRANTY,
    including MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  ------------------------------------------------------------------------- */
#include <stdio.h>
#include "vector.h"
#include "constants.h"
#include "obstacle.h"
#include "PlayInterface.h"
#include <TaskMediator.h>
#include "staticparams.h"
#include <math.h>
#include <utils.h>
#include <geometry.h>
#include "PredictTrajectory.h"

#define OURPLAYER 0
#define THEIRPLAYER 1
#define THISPLAYER 2
namespace  {
    const double FRAME_PERIOD = 1.0 / PARAM::Vision::FRAME_RATE;
    const double TEAMMATE_AVOID_DIST = PARAM::Vehicle::V2::PLAYER_SIZE + 4.0f;
    const double OPP_AVOID_DIST = PARAM::Vehicle::V2::PLAYER_SIZE + 5.5f;
    const double BALL_AVOID_DIST = PARAM::Field::BALL_SIZE + 5.0f;
    const float DEC_MAX = 450;
    const float ACCURATE_AVOID = 250;

}
//====================================================================//
//    Obstacle class implementation
//====================================================================//
// 返回 点到这种长条形障碍物的最近点和距离

void circle_rect(vector2f p1,vector2f p2, vector2f normv, float radius,vector2f *c)
{// p1一条边的中点，p2平行边的中点，normv另一条边的方向矢量，radius宽度的一半
	*(c+0) = p1 + normv * radius  ;
	*(c+1) = p1 +normv * (-radius)  ;
	*(c+2) = p2 + normv *(-radius)  ;
	*(c+3) = p2 + normv * radius  ;
}
void drawRectangle(const vector2f &seg_start, const vector2f &seg_end) {
    double x1 = seg_start.x;
    double y1 = seg_start.y;
    double x2 = seg_end.x;
    double y2 = seg_end.y;
    CGeoPoint p1(x1, y1);
    CGeoPoint p2(x1, y2);
    CGeoPoint p3(x2, y1);
    CGeoPoint p4(x2, y2);

    GDebugEngine::Instance()->gui_debug_line(p1, p2, COLOR_GREEN);
    GDebugEngine::Instance()->gui_debug_line(p4, p2, COLOR_GREEN);
    GDebugEngine::Instance()->gui_debug_line(p1, p3, COLOR_GREEN);
    GDebugEngine::Instance()->gui_debug_line(p3, p4, COLOR_GREEN);
}

float obstacle::closest_point(state s, vector2f& nearestPoint) {
	vector2f d;
    float minDist;
    if (this->type != OBS_RECTANGLE) {
        if (Vector::distance(seg_start, seg_end) > EPSILON) {  // 长条圆
            nearestPoint = Vector::point_on_segment(seg_start, seg_end, s.pos, 1);
            minDist = Vector::distance(nearestPoint, s.pos);
        }
        else { // 圆
            d = (s.pos - seg_start);  // 向量
            nearestPoint = seg_start + d.norm(radius); // 最近点
            minDist = d.length();     // 距离
        }
    }
    else { // rectangle
        CGeoPoint sPos = CGeoPoint(s.pos.x, s.pos.y);

        double x_1 = this->seg_start.x;
        double y_1 = this->seg_start.y;
        double x_2 = this->seg_end.x;
        double y_2 = this->seg_end.y;

        if (fabs(sPos.x() - x_1) + fabs(sPos.x() - x_2) - fabs(x_1 - x_2) < EPSILON && fabs(sPos.y() - y_1) + fabs(sPos.y() - y_2) - fabs(y_1 - y_2) < EPSILON) {
            return 0;
        }

        CGeoSegment l1(CGeoPoint(x_1, y_1), CGeoPoint(x_1, y_2));
        CGeoSegment l2(CGeoPoint(x_1, y_2), CGeoPoint(x_2, y_2));
        CGeoSegment l3(CGeoPoint(x_2, y_2), CGeoPoint(x_2, y_1));
        CGeoSegment l4(CGeoPoint(x_1, y_1), CGeoPoint(x_2, y_1));
        minDist = min(l1.dist2Point(sPos), min(l2.dist2Point(sPos), min(l3.dist2Point(sPos), l4.dist2Point(sPos))));
    }
	return(minDist);
}

//机器人与障碍物距离的余量
float obstacle::margin(state s) {
    vector2f p;
	return (closest_point(s, p) - robot_radius - radius);
}

//某个点是否与障碍物相碰; 相碰则返回0, 不相碰则返回1;
bool obstacle::check(state s) {
	return(margin(s) > 0.0);
}


//线段s0~s1是否与障碍物相碰
//modified by Wang in 2018/3/21
bool obstacle::check(state s0, state s1) {
	//不是矩形障碍物的判断
	if (this->type != OBS_RECTANGLE) {
		vector2f p1, p2;
		vector2f d;
		float mindist;
		if (Vector::distance(s0.pos, s1.pos) < EPSILON) {
            return(check(s1));
		}

		if (Vector::distance(seg_start, seg_end) > EPSILON) { // 如果障碍物没有变成长圆形
			mindist = Vector::distance_seg_to_seg(s0.pos, s1.pos, seg_start, seg_end, p1, p2) - robot_radius - radius;

			if (Vector::distance(p1, s0.pos) < 3) {
				mindist = 1;
			}
		}
		else {
			p1 = Vector::point_on_segment(s0.pos, s1.pos, seg_start, 1);

			if (Vector::distance(p1, s0.pos) < 3) {
				mindist = 1; //恰好是线段的开头
			}
			else {
				mindist = Vector::distance(p1, seg_start) - radius - robot_radius;
			}
		}
		return (mindist > 0);
	}
	//若是矩形障碍物的判断
	else if (this->type == OBS_RECTANGLE) {
		CGeoPoint p_s0(s0.pos.x, s0.pos.y);
		CGeoPoint p_s1(s1.pos.x, s1.pos.y);
//        GDebugEngine::Instance()->gui_debug_x(p_s0, COLOR_PURPLE);
//        GDebugEngine::Instance()->gui_debug_x(p_s1, COLOR_PURPLE);
		CGeoSegment mySegment(p_s0, p_s1);

		double x_1 = this->seg_start.x;
		double y_1 = this->seg_start.y;
		double x_2 = this->seg_end.x;
		double y_2 = this->seg_end.y;
        if (x_1 < x_2) {
            x_1 -= robot_radius;
            x_2 += robot_radius;
            if (y_1 < y_2) {
                y_1 -= robot_radius;
                y_2 += robot_radius;
            }
            else {
                y_1 += robot_radius;
                y_2 -= robot_radius;
            }
        }
        else {
            x_1 += robot_radius;
            x_2 -= robot_radius;
            if (y_1 < y_2) {
                y_1 -= robot_radius;
                y_2 += robot_radius;
            }
            else {
                y_1 += robot_radius;
                y_2 -= robot_radius;
            }
        }
		CGeoPoint p_1(x_1, y_1);
		CGeoPoint p_2(x_1, y_2);
		CGeoPoint p_3(x_2, y_1);
        CGeoPoint p_4(x_2, y_2);
//        drawRectangle(vector2f(x_1, y_1), vector2f(x_2, y_2));
        CGeoSegment segment_1(p_1, p_2);
		CGeoSegment segment_2(p_2, p_4);
		CGeoSegment segment_3(p_1, p_3);
		CGeoSegment segment_4(p_3, p_4);

		//若与禁区四条线相交，则与障碍物相碰

        if (mySegment.IsSegmentsIntersect(segment_1) || mySegment.IsSegmentsIntersect(segment_2) || mySegment.IsSegmentsIntersect(segment_3) || mySegment.IsSegmentsIntersect(segment_4)) {
            return false;
        }
        else if (fabs(fabs(s0.pos.x - seg_start.x) + fabs(s0.pos.x - seg_end.x) - fabs(seg_start.x - seg_end.x)) < EPSILON && fabs(fabs(s0.pos.y - seg_start.y) + fabs(s0.pos.y - seg_end.y) - fabs(seg_start.y - seg_end.y)) < EPSILON) {
            return false;
        }
        else if (fabs(fabs(s1.pos.x - seg_start.x) + fabs(s1.pos.x - seg_end.x) - fabs(seg_start.x - seg_end.x)) < EPSILON && fabs(fabs(s1.pos.y - seg_start.y) + fabs(s1.pos.y - seg_end.y) - fabs(seg_start.y - seg_end.y)) < EPSILON) {
            return false;
        }
        else {
            return true;
        }

	}
}

// 返回障碍物像位置点s的排斥力方向
vector2f obstacle::repulse(state s) {
    if (this->type != OBS_RECTANGLE) {
        vector2f p, v;
        float d1, d2;
        d1 = Vector::distance(seg_start, s.pos);
        d2 = Vector::distance(seg_end, s.pos);
        if (d1 > d2) {
            return ((s.pos - seg_start) / sqrt(d1));
        }
        else {
            return ((s.pos - seg_end) / sqrt(d2));
        }
    }
    else {
        vector2f left = vector2f(seg_end.x, seg_start.y);
        vector2f right = vector2f(seg_end.x, seg_end.y);
        if (fabs(s.pos.y - seg_start.y) < fabs(s.pos.y - seg_end.y)) {
            return ((s.pos - left) / sqrt(Vector::distance(left, s.pos)));
        }
        else {
            return ((s.pos - right) / sqrt(Vector::distance(right, s.pos)));
        }
    }
}

//====================================================================//
//    Obstacles class implementation
//====================================================================//

obstacles::obstacles(float robotRadius) {
    robot_radius = robotRadius;
    num = 0;
    current_mask = 0;
    for( int i = 0; i < MAX_OBSTACLES; ++i ) {
        obs[i].set_robot_radius(robot_radius);
    }
}

void obstacles::draw_long_circle(const vector2f& seg_start,const vector2f& seg_end, const float& radius) {
	vector2f c[4];
	vector2f norm;
	norm = (seg_end - seg_start).perp();

	circle_rect(seg_start,seg_end,norm/norm.length(),radius,c);

	for (int iter = 0; iter < 4; iter++) {
        GDebugEngine::Instance()->gui_debug_line(CGeoPoint(c[iter].x,c[iter].y),CGeoPoint(c[(iter+1)%4].x,c[(iter+1)%4].y),1);
	}
    GDebugEngine::Instance()->gui_debug_arc(CGeoPoint(seg_end.x,seg_end.y),radius,0,360,1);
    GDebugEngine::Instance()->gui_debug_arc(CGeoPoint(seg_start.x,seg_start.y),radius,0,360,1);
}

void obstacles::draw_rectangle(const vector2f &seg_start, const vector2f &seg_end) {
    double x1 = seg_start.x;
    double y1 = seg_start.y;
    double x2 = seg_end.x;
    double y2 = seg_end.y;
    CGeoPoint p1(x1, y1);
    CGeoPoint p2(x1, y2);
    CGeoPoint p3(x2, y1);
    CGeoPoint p4(x2, y2);

    GDebugEngine::Instance()->gui_debug_line(p1, p2, COLOR_GREEN);
    GDebugEngine::Instance()->gui_debug_line(p4, p2, COLOR_GREEN);
    GDebugEngine::Instance()->gui_debug_line(p1, p3, COLOR_GREEN);
    GDebugEngine::Instance()->gui_debug_line(p3, p4, COLOR_GREEN);
}

void obstacles::add_long_circle(vector2f x0, vector2f x1, vector2f v, float r, int mask, bool drawObs, ObstacleType type) {
	if(num >= MAX_OBSTACLES) return;
    obs[num].type = type; // 防守区域
	obs[num].mask = mask;
	obs[num].seg_start = x0;
	obs[num].seg_end = x1;
	obs[num].vel = v;
	obs[num].radius = r;
    if(drawObs)
        draw_long_circle(x0, x1, r);
	num ++;
}

void obstacles::add_circle(vector2f x0, vector2f v, float r, int mask, bool drawObs, ObstacleType type) {
	if(num >= MAX_OBSTACLES) return;
    obs[num].type = type; // 车辆
	obs[num].mask = mask;
	obs[num].seg_start = x0;
	obs[num].seg_end = x0;
	obs[num].vel = v;
	obs[num].radius = r;
    if(drawObs)
        draw_long_circle(x0, x0, r);
	num ++;
}

//添加矩形障碍物  added by Wang in 2018/3/21
void obstacles::add_rectangle(vector2f x0, vector2f x1, int mask, bool drawObs, ObstacleType type) {
	if (num >= MAX_OBSTACLES) return;
    obs[num].type = type; // 矩形
    obs[num].mask = mask;
	obs[num].seg_start = x0;
	obs[num].seg_end = x1;
    if(drawObs)
        draw_rectangle(x0, x1);
	num++;
}

bool obstacles::check(vector2f p) {
	state s;
	s.pos = p;
	s.parent = NULL;
    return(check(s));
}

bool obstacles::check(vector2f p, int &id) {
	state s;
	s.pos = p;
	s.parent = NULL;
	return(check(s, id));
}

// 返回位置s是否与列表中的障碍物相碰撞; 不相碰返回1;
bool obstacles::check(state s) {
  int i;

  i = 0;
  while(i<num && ((obs[i].mask & current_mask)==0 || obs[i].check(s))) {
	  i++;
  }

  return(i == num);
}

// 返回位置s是否与列表中的障碍物相碰撞,若相碰撞,返回碰撞的障碍物
bool obstacles::check(state s,int &id) {
	int i = 0;
	while(i < num && ((obs[i].mask & current_mask)==0 || obs[i].check(s))) 
		i++;
	if(i < num) {
		id = i;
	}
	return(i == num);
}

//返回s0~s1这一段是否与列表中障碍物有所碰撞
bool obstacles::check(state s0, state s1) {
    int i = 0;
    while(i < num && ((obs[i].mask & current_mask)==0 || obs[i].check(s0, s1))) {
        // printf("%d",obs[i].check(s0,s1));
		i++;
    }
	return(i == num);
}

//返回s0~s1这一段是否与列表中障碍物有所碰撞,并返回碰撞的障碍物下标
bool obstacles::check(state s0,state s1,int &id) {
	int i = 0;
	while(i < num && ((obs[i].mask & current_mask)==0 || obs[i].check(s0, s1))) {
		i++;
	}
	if(i < num) {
		id = i;
	}
	return(i == num);
}

void obstacles::addObs(const CVisionModule *pVision, const TaskT &task, bool drawObs, double oppAvoidDist, double teammateAvoidDist, double ballAvoidDist) {
    int player = task.executor;
    int shootCar = task.ball.Sender;
    CGeoPoint target = task.player.pos;
    int flags = task.player.flag;

    if(flags & PlayerStatus::FREE_KICK)
        add_rectangle(vector2f(PARAM::Field::PITCH_LENGTH / 2, -PARAM::Field::PENALTY_AREA_WIDTH/2 + robot_radius - PARAM::Vehicle::V2::PLAYER_SIZE - 20.0),
                        vector2f(PARAM::Field::PITCH_LENGTH / 2 - PARAM::Field::PENALTY_AREA_DEPTH + robot_radius - PARAM::Vehicle::V2::PLAYER_SIZE - 20.0, PARAM::Field::PENALTY_AREA_WIDTH / 2 - robot_radius + PARAM::Vehicle::V2::PLAYER_SIZE + 20.0), 1, drawObs);
    else if(!(flags & PlayerStatus::NOT_AVOID_PENALTY))
        add_rectangle(vector2f(PARAM::Field::PITCH_LENGTH / 2, -PARAM::Field::PENALTY_AREA_WIDTH/2 + robot_radius - PARAM::Vehicle::V2::PLAYER_SIZE),
                        vector2f(PARAM::Field::PITCH_LENGTH / 2 - PARAM::Field::PENALTY_AREA_DEPTH + robot_radius - PARAM::Vehicle::V2::PLAYER_SIZE, PARAM::Field::PENALTY_AREA_WIDTH / 2 - robot_radius + PARAM::Vehicle::V2::PLAYER_SIZE), 1, drawObs);


    // 如果是躲避射门
    if (flags & PlayerStatus::AVOID_SHOOTLINE) {
        const PlayerVisionT& shooter = pVision->ourPlayer(shootCar);
        // 球门中心
        add_long_circle(vector2f(shooter.Pos().x(), shooter.Pos().y()), vector2f(PARAM::Field::PITCH_LENGTH/2, 0.0f), vector2f(0.0f, 0.0f), 3.0f, 1, drawObs);
        // 球门左门柱
        add_long_circle(vector2f(shooter.Pos().x(), shooter.Pos().y()), vector2f(PARAM::Field::PITCH_LENGTH/2, PARAM::Field::GOAL_WIDTH/2.0), vector2f(0.0f, 0.0f), 3.0f, 1, drawObs);
        // 球门右门柱
        add_long_circle(vector2f(shooter.Pos().x(), shooter.Pos().y()), vector2f(PARAM::Field::PITCH_LENGTH/2, -PARAM::Field::GOAL_WIDTH/2.0), vector2f(0.0f, 0.0f), 3.0f, 1, drawObs);
    }

    // set up teammates as obstacles
    if (!(flags & PlayerStatus::NOT_AVOID_OUR_VEHICLE)) {
        for(int i = 0; i < PARAM::Field::MAX_PLAYER; ++i) {
            const PlayerVisionT& teammate = pVision->ourPlayer(i);
            if((i != player) && teammate.Valid()) {
                if(i == TaskMediator::Instance()->rightBack() ||
                        i == TaskMediator::Instance()->leftBack() ||
                        WorldModel::Instance()->CurrentRefereeMsg() == "GameStop" ||
                        WorldModel::Instance()->CurrentRefereeMsg() == "OurTimeout")
                    add_circle(vector2f(teammate.Pos().x(), teammate.Pos().y()), vector2f(teammate.Vel().x(), teammate.Vel().y()), PARAM::Vehicle::V2::PLAYER_SIZE, 1, drawObs);
                else
                    add_circle(vector2f(teammate.Pos().x(), teammate.Pos().y()), vector2f(teammate.Vel().x(), teammate.Vel().y()), teammateAvoidDist, 1, drawObs);
            }
        }
    }

    // set up opponents as obstacles
    if (!(flags & PlayerStatus::NOT_AVOID_THEIR_VEHICLE)) {
        for(int i=0; i<PARAM::Field::MAX_PLAYER; ++i) {
            const PlayerVisionT& opp = pVision->theirPlayer(i);
            if(opp.Valid()) {
                if((target.dist(opp.Pos()) < PARAM::Field::MAX_PLAYER_SIZE / 2) ) {
                    continue;
                }
                else {
                    add_circle(vector2f(opp.Pos().x(), opp.Pos().y()), vector2f(opp.Vel().x(), opp.Vel().y()), oppAvoidDist, 1, drawObs);
                }
            }
        }
    }

    // 禁区 和 门柱
    if(player != TaskMediator::Instance()->goalie()) {
        // 我方禁区
        if(flags & PlayerStatus::FREE_KICK)
            add_rectangle(vector2f(-PARAM::Field::PITCH_LENGTH / 2, -PARAM::Field::PENALTY_AREA_WIDTH/2 + robot_radius - PARAM::Vehicle::V2::PLAYER_SIZE - 20.0),
                          vector2f(-PARAM::Field::PITCH_LENGTH / 2 + PARAM::Field::PENALTY_AREA_DEPTH - robot_radius + PARAM::Vehicle::V2::PLAYER_SIZE + 20.0, PARAM::Field::PENALTY_AREA_WIDTH / 2 - robot_radius + PARAM::Vehicle::V2::PLAYER_SIZE + 20.0), 1, drawObs);
        else if(!(flags & PlayerStatus::NOT_AVOID_PENALTY))
            add_rectangle(vector2f(-PARAM::Field::PITCH_LENGTH / 2, -PARAM::Field::PENALTY_AREA_WIDTH/2 + robot_radius - PARAM::Vehicle::V2::PLAYER_SIZE),
                          vector2f(-PARAM::Field::PITCH_LENGTH / 2 + PARAM::Field::PENALTY_AREA_DEPTH - robot_radius + PARAM::Vehicle::V2::PLAYER_SIZE, PARAM::Field::PENALTY_AREA_WIDTH / 2 - robot_radius + PARAM::Vehicle::V2::PLAYER_SIZE), 1, drawObs);
    }

    // ball
    if(flags & PlayerStatus::DODGE_BALL) {
        const MobileVisionT& ball = pVision->ball();
        add_circle(vector2f(ball.Pos().x(), ball.Pos().y()), vector2f(ball.Vel().x(), ball.Vel().y()), ballAvoidDist, 1, drawObs);
    }

    if(WorldModel::Instance()->CurrentRefereeMsg() == "GameStop") {
        const MobileVisionT& ball = pVision->ball();
        add_circle(vector2f(ball.Pos().x(), ball.Pos().y()), vector2f(0.0f, 0.0f), 50.0f, 1, drawObs);
    }

    set_mask(1);
}

vector2f obstacles::repulse(state s) {
	vector2f f(0.0, 0.0);
	// 将所有与车距离为9cm的障碍物的排斥向量相加
	for (int i = 0; i < num; i++) {
		if ((obs[i].mask & current_mask) && (obs[i].margin(s)) < 50 ) {
			f += obs[i].repulse(s);
		//	cout<<"NUM: "<<i<<endl;
		}
	}
	if (f.length() > 0.001) {
		f.normalize();
    }
	return(f);
}

vector2f obstacles::repulse(state s, const int &id) {
	vector2f f(0.0, 0.0);

	if ((obs[id].mask & current_mask) && !obs[id].check(s)) {
		f = obs[id].repulse(s);
	}
	return f;
}

void obstacles::change_world(vector2f s,vector2f g, vector2f v, float maxD) { // maxD为最大减速度 该函数现在没用到
    float factor = 0.2f;  //最短距离的变化
    vector2f g_dir, obsmove, obsdist;
    float sr, dt, scale, uncertain_position; // 球障碍的变形
    dt = 1.0 / PARAM::Vision::FRAME_RATE; // 系统为1/60s
    scale = 5.0;
    uncertain_position = 2.0;
    for(int i = 0; i < num; i++) {
        if((obs[i].mask & current_mask)) {
            if (obs[i].type == OBS_CIRCLE) {
                // 圆形变成长圆形
                if ( v.x != 0 || v.y != 0) {
                    sr = v.sqlength() / (2.0 * maxD); //只有当障碍物过近,并且同向时就选进行长圆形变化
                    g_dir = g - s;
                    if ( sr > (Vector::distance(obs[i].seg_start, s) - robot_radius - obs[i].radius)) {
                        sr = sr * factor;
                        obs[i].seg_start = obs[i].seg_start;
                        obs[i].seg_end = obs[i].seg_start + (-v).norm(sr);
                        //draw_long_circle(obs[i].seg_start,obs[i].seg_end,obs[i].radius);
                    }
                /*	else
                    {
                         GDebugEngine::Instance()->gui_debug_arc(CGeoPoint(obs[i].seg_start.x,obs[i].seg_start.y),obs[i].radius,0.0,360.0,1);
                    }*/
                }
                /*else{
                    GDebugEngine::Instance()->gui_debug_arc(CGeoPoint(obs[i].seg_start.x,obs[i].seg_start.y),obs[i].radius,0.0,360.0,1);
                }*/

                /*if (obs[i].vel.x!= 0 || obs[i].vel.y!=0 )
                {
                    obsmove = obs[i].vel * dt * scale;
                    obs[i].seg_start = obs[i].seg_start + obsmove;
                    obs[i].seg_end = obs[i].seg_end + obsmove;
                    obsdist = obsmove * uncertain_position;
                    add_long_circle(obs[i].seg_start + (-obsdist)/4.0,obs[i].seg_start + obsdist*5.0/4.0,vector2f(0.0,0.0),obs[i].radius,1);
                    draw_long_circle(obs[num-1].seg_start,obs[num-1].seg_end,obs[num-1].radius);
                }*/
            }
            else if(obs[i].type == OBS_LONG_CIRCLE) {

                if ( v.x != 0 || v.y != 0) {
                    sr = v.sqlength() / (2 * maxD) * factor / 2.0;
                    obs[i].radius = obs[i].radius + sr;
                }
                /*draw_long_circle(obs[i].seg_start,obs[i].seg_end,obs[i].radius);
                draw_long_circle(obs[i].seg_start,obs[i].seg_end,obs[i].radius+robot_radius);*/
            }
        }
    }
    return;
}
