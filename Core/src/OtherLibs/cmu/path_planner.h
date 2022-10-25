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

#ifndef __PATH_PLANNER_H__
#define __PATH_PLANNER_H__

#include "obstacle.h"
#include "kdtree.h"

const int MAX_NODES = 400;      // 最多节点
const int MAX_WAYPTS = 100;     // 最多waypoint数量
const float NEAR_DIST = 20.0;   // 当距离小于20cm时就可以直接过去.

class path_planner {
	state node[MAX_NODES];
	state waypoint[MAX_WAYPTS];
	state last_pathpoint[MAX_WAYPTS];  // 记录路径点
	state pathpoint[MAX_WAYPTS];
	state goal;       // 目标点
	vector2f out_vel; // 最终输出的速度(大小，方向)

	KDTree<state> tree;
	int num_nodes;
	int max_nodes;
	int num_waypoints;
	int last_num_pathpoint;
	int num_pathpoint;
	static const int max_extend = 1;

	float goal_target_prob;
	float waypoint_target_prob;
	float waypoint_target_prob_old;
	float step_size;

	float last_pathlength;    //记录路径长度
	float pathlength;
	float path_target_length;       //记录nearestpoint与障碍物之间的距离
	float last_path_target_length;
	bool path_target_collision;     //记录nearestpoint是否与终点有障碍物
	bool last_path_target_collision;
	int still_count;          //路径没有变化的计数器

	bool init_;
    bool searchInCircle_;
    double circleRadius;
    vector2f circleCenter;

	obstacles *obs;
public:
	path_planner();
    void init(int _max_nodes, int _num_waypoints, float _goal_target_prob, float _waypoint_target_prob, float _step_size, state _goal, bool searchInCircle = false, vector2f circleCenter = vector2f(0.0, 0.0), double searchCircleRadius = 0.0);
	float distance(state &state0, state &state1);
    state random_state();
	state *add_node(state newExtend, state *parent);
	state choose_target(int &targ);
	state *find_nearest(state target);
	int extend(state *nearest, state target);
    vector<state> errt(state initial, state _goal, bool drawtreeflag);
    vector<state> plan(obstacles *_obs, int obs_mask, state initial, vector2f _vel, state _goal);
	void save_newpath();
	void cal_vel();
	vector2f get_vel();
};

#endif /*__PATH_PLANNER_H__*/
