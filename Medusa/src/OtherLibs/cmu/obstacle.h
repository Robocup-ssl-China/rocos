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

#ifndef __OBSTACLE_H__
#define __OBSTACLE_H__

#include "ObstacleNew.h"
#include "vector.h"
#include "geometry.h"
#include "utils.h"
#include "parammanager.h"
#include <VisionModule.h>
#include "GDebugEngine.h"

#define EPSILON (1.0E-10)
enum ObstacleType{ OBS_CIRCLE, OBS_LONG_CIRCLE, OBS_RECTANGLE}; //车和防守区域对速度的变化规则不同

struct state{
  vector2f pos;
  state *parent;
  state *next;
};

class obstacle{
public:
    float robot_radius; // robot的半径
	obstacle(){}
    obstacle(ObstacleNew obs) {
        this->seg_start = vector2f(obs.getStart().x(), obs.getStart().y());
        this->seg_end = vector2f(obs.getEnd().x(), obs.getEnd().y());
        this->vel = vector2f(obs.getVel().x(), obs.getVel().y());
        this->radius = float(obs.getRadius());
        this->robot_radius = float(obs.getRobotRadius());
        int obsType = obs.getType();
        this->type = ObstacleType(obsType);
        this->mask = 1;
    }
	int type; // type of obstacle
	int mask; // enable mask
	vector2f seg_start; // 长条圆的第一个中心点
	vector2f seg_end;   // 长条圆的第二个中心点
	vector2f vel;  // object velocity
	float radius;  // radius for circle
public:
	void set_robot_radius(float r){ robot_radius = r; }
	float margin(state s);
	float closest_point(state s, vector2f &p);
	bool check(state s);
	bool check(state s0, state s1);
	vector2f repulse(state s);
	float get_robot_radius(){return robot_radius;}
};

const int MAX_OBSTACLES = 24;
// 2*MAX_TEAM_ROBOTS+1+2+4
// robots, ball, defense areas, walls

class obstacles {
	float robot_radius;
	int num; //障碍物数量
	int current_mask;
public:
	obstacle obs[MAX_OBSTACLES];
public:
    obstacles(float robotRadius);
    obstacles(ObstaclesNew obsNew) {
        for (int i = 0; i < obsNew.getNum(); i++) {
            obs[i] = obstacle(obsNew.obs[i]);
        }
        num = obsNew.getNum();
        current_mask = 1;
        robot_radius = float(obsNew.getRobotRadius());
    }
    float getRadius() { return robot_radius; }
	void clear() { num = 0;}
    void addObs(const CVisionModule *pVision, const TaskT &task, bool drawObs, double oppAvoidDist, double teammateAvoidDist, double ballAvoidDist);
    void add_long_circle(vector2f x0, vector2f x1, vector2f v, float r, int mask, bool DRAW_OBSTACLES = false, ObstacleType type = OBS_LONG_CIRCLE);
    void add_circle(vector2f x0, vector2f v, float r, int mask, bool DRAW_OBSTACLES = false, ObstacleType type = OBS_CIRCLE);
    void add_rectangle(vector2f x0, vector2f x1, int mask, bool DRAW_OBSTACLES = false, ObstacleType type = OBS_RECTANGLE);

	void change_world(vector2f s, vector2f p, vector2f v, float maxD); //考虑速度因素影响，修改障碍物的体积
	void set_mask(int mask) { current_mask = mask; }
	bool check(vector2f p);
	bool check(vector2f p, int &id);
	bool check(state s);
	bool check(state s, int &id);
	bool check(state s0, state s1);
	bool check(state s0, state s1, int &id);
    vector2f repulse(state s);
	vector2f repulse(state s, const int &id);
	int get_num() { return num; }
	state dynamic_obstacle(vector2f vel, state initial, state result, vector2f& output_vel);
	void draw_long_circle(const vector2f& seg_start, const vector2f& seg_end, const float& radius);
    void draw_rectangle(const vector2f& seg_start, const vector2f& seg_end);
};
#endif /*__OBSTACLE_H__*/
