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
#include "path_planner.h"
#include <GDebugEngine.h>
#include <exception>
#include <time.h>
#include <float.h>

namespace{
    #define SAMEPOINT 18  //用于判断两点是否同一点
    #define STILLCOUNT 3 //用于判断路径是否已经达到最优了

    inline int boost_lrand48() {
        return rand();
    }

    inline float boost_drand48() { // 返回 [0, 1]
        return float(boost_lrand48() % 10000) / 10000;
    }

    const bool plan_print = false;
    const float out_of_obs_dot = 0.2;

    inline float sdrand48() { return(2 * boost_drand48() - 1); }  //返回 [-1, 1]

    void drawPath(state* p, const state& t) {
        if (p) {
            GDebugEngine::Instance()->gui_debug_line(CGeoPoint(t.pos.x, t.pos.y), CGeoPoint(p->pos.x, p->pos.y), COLOR_WHITE);
        }
        while (p && p->parent) {
            GDebugEngine::Instance()->gui_debug_line(CGeoPoint(p->parent->pos.x, p->parent->pos.y), CGeoPoint(p->pos.x, p->pos.y), COLOR_WHITE);
            p = p->parent;
        }
    }
}

path_planner::path_planner() {
    srand(time(0));
    init_ = false;
}

state path_planner::random_state() {  // 生成随机状态
    state s;
    if(!searchInCircle_)
        s.pos = vector2f((PARAM::Field::PITCH_LENGTH / 2 + PARAM::Field::GOAL_DEPTH) * sdrand48(), PARAM::Field::PITCH_LENGTH / 2 * sdrand48());
    else {
        double r = boost_drand48() * circleRadius;
        double angle = sdrand48() * PARAM::Math::PI;
        s.pos = vector2f(circleCenter.x + r * cos(angle), circleCenter.y + r * sin(angle));
    }
    s.parent = NULL;
    return(s);
}

// 初始化rrt信息，包括最大节点数，waypoints中储存的节点数，以goal为目标的概率，以waypoint中的点为目标的概率，步长.
// 在调用时采用如下参数path_planner[i].init(150, 80, 0.15, 0.65, PARAM::Field::MAX_PLAYER_SIZE, initial);
void path_planner::init(int _max_nodes, int _num_waypoints, float _goal_target_prob, float _waypoint_target_prob, float _step_size, state _goal, bool searchInCircle, vector2f circleCenter, double searchCircleRadius) {
    if ( init_ && !searchInCircle) {
        return;
    }
    vector2f minv;
    vector2f maxv;

    max_nodes = _max_nodes;
    num_waypoints = _num_waypoints;
    goal_target_prob = _goal_target_prob;
    waypoint_target_prob = _waypoint_target_prob;
    waypoint_target_prob_old = _waypoint_target_prob;
    step_size = _step_size;
    num_pathpoint = 0;
    last_num_pathpoint = 0;
    pathlength = 0;
    last_pathlength = 99999999.0f;
    path_target_length = 0;
    last_path_target_length = 999999999.0f;
    last_path_target_collision = false;
    path_target_collision = false;
    still_count = 0;
    out_vel = vector2f(0.0, 0.0);
    searchInCircle_ = searchInCircle;
    this->circleCenter = circleCenter;
    this->circleRadius = searchCircleRadius;

    // 将waypoint数组初始化为随机点
    for(int i = 0; i < num_waypoints; i++) {
        waypoint[i] = random_state();
    }

    //长方形(minv, maxv), 场地左下角和右上角
    if(!searchInCircle) {
        minv = vector2f(-PARAM::Field::PITCH_LENGTH / 2 - PARAM::Field::GOAL_DEPTH, -PARAM::Field::PITCH_WIDTH / 2);
        maxv = vector2f(PARAM::Field::PITCH_LENGTH / 2 + PARAM::Field::GOAL_DEPTH / 2, PARAM::Field::PITCH_WIDTH / 2);
    } else {
        minv = vector2f(circleCenter.x - searchCircleRadius, circleCenter.y - searchCircleRadius);
        maxv = vector2f(circleCenter.x + searchCircleRadius, circleCenter.y + searchCircleRadius);
        if(searchCircleRadius < step_size)
            cout << "Oh shit!!! Search radius is smaller than search step!!! ---path_planner.cpp" << endl;
        if((circleCenter - _goal.pos).length() > searchCircleRadius)
            cout << "Oh shit!!! Target is out of circle!!! ---path_planner.cpp" << endl;
    }
    tree.setdim(minv, maxv, 16, 8); // 重置一棵树
    init_ = true;
}

//返回两点间的距离
inline float path_planner::distance(state &state0, state &state1) {
    return(Vector::distance(state0.pos, state1.pos));
}

// 将节点n加入kdtree,并返回新加入的节点
state *path_planner::add_node(state newExtend, state *parent) {
    // 传入的n是扩展后的节点也就是nearest+step之后的节点
    // parent是nearest节点
    if(num_nodes >= max_nodes) {
        return(NULL);
    }
    newExtend.parent = parent;
    node[num_nodes] = newExtend;
    tree.add(&node[num_nodes]); //state和cmu的node的转换
    num_nodes++;
    return(&node[num_nodes - 1]);
}

/*  选择扩张目标点
    选择扩张的朝向点，产生0到1的随机数p
    targetType = 0、有goal_target_prob（0.15）的概率把goal作为target
                 1、有goal_waypoint_prob（0.65）的概率把waypoints中的点作为target
                 2、有1-0.65-0.15=0.20的概率，以随机点为target */
state path_planner::choose_target(int &targetType) {
    double random = boost_drand48();
    int i;
    if (random < goal_target_prob) { // 0.15
        targetType = 0;
        return(goal);
    }
    else if (random < goal_target_prob + waypoint_target_prob) { // 0.65
        targetType = 1;
        i = boost_lrand48() % num_waypoints;
        return(waypoint[i]); // 随机取waypoint中的点
    }
    else { // 0.20
        targetType = 2;
        return(random_state());
    }
}

state *path_planner::find_nearest(state target) { // 找到距离target最近的已有节点
    state *nearest;
    float nearestDist = 0;
    // 调用kdtree.cpp中的neareast,得到位置点树中离target最近的点
    nearest = tree.nearest(nearestDist, target.pos);

    if (!nearest) {  // nearest == NULL
        // NOTE: something bad must have happened if we get here.
        // find closest current state
        nearest = &node[0];
        float d, nd;
        nd = distance(*nearest, target);
        for (int i = 0; i < num_nodes; i++) {
            d = distance(node[i], target);
            if (d < nd) {
                nearest = &node[i];
                nd = d;
            }
        }
    }
    return(nearest);
}

int path_planner::extend(state *nearest, state target) {
    // Extend所扩张的点是nearest与target连线上,距离nearest距离为step_size的点,还要保证这个点不会撞到障碍物.
    // rrt的扩张,即产生新节点
    state newExtend;
    vector2f step;
    float stepLength;
    // 确定向量step的方向为target与nearest连线方向
    step = target.pos - nearest->pos;
    stepLength = step.length();
    if(stepLength < step_size) {
        return(0);
    }
    step *= step_size / stepLength; // 向量单位化，再赋予它长度step_size

    newExtend.pos = nearest->pos + step;    // newExtend就是新节点

    // 接下来检查起始点s是否会接触障碍物
    // obs->check()定义在obstacle.cpp 中,若有障碍，返回0，无障碍返回1；
    if(searchInCircle_ && obs->check(*nearest, newExtend) && (newExtend.pos - circleCenter).length() < circleRadius) {
        nearest = add_node(newExtend, nearest);
    } else if(!searchInCircle_ && obs->check(*nearest, newExtend)) {
        nearest = add_node(newExtend, nearest);
    }
    // add_node()完成后，将newExtend加入到rrt中,存储在node[num_nodes-1]处
    return(num_nodes); // num_nodes已经在add_node()中自增
}

// ERRT算法*******************************************************************************************************
vector<state> path_planner::errt(state initial, state _goal, bool drawtreeflag) {
/*  0、如果初始点已经距离goal比较近，采用简化方法。否则执行rrt的plan
    1、选择target
    2、寻找nearest
    3、从nearest点处向target扩张(extend)，确定下一个点，若足够接近goal,转到4；否则，重复1。
    4、逆向考察曲线是否可以简化，即逆向考察每一个节点，看它能否和initial直接相通，没有障碍阻挡。
    5、将成功规划出的路径上的点，随机的，部分加入waypoints中。
    6、返回当前所需要的下一个位置 */
    vector<state> targetList;
    state target, *nearest, *nearest_goal;
    float tempDist;
    float newDist;
    goal = _goal;
    // 步骤0: 如果initial与goal足够近时的简便方法: 直接考虑target在initial与goal连线上,尽量靠近goal,不避障碍
    tempDist = Vector::distance(initial.pos, goal.pos);

    if (obs->check(initial, goal)) {
        // 起点与终点连线之间没有碰撞
        pathlength =  0;
        path_target_length = 0;
        path_target_collision = 1;
        num_pathpoint = 2;
        pathpoint[0] = initial;
        pathpoint[1] = goal;
        if (isnan(goal.pos.x) || isnan(goal.pos.y)) {
            printf("ERROR INIT3\n");
        }
        targetList.push_back(goal);
        return(targetList);
    }
    else {
        tree.clear();
        int i, iter_limit;
        int targ_type;
        i = num_nodes = 0;
        nearest = nearest_goal = add_node(initial, NULL);
        tempDist = distance(*nearest, goal);
        // plan rrt规划,直到距离目标点足够近,或者循环次数达到上限,足够近时就可能碰到障碍物
        iter_limit = max_nodes; // 尝试的次数等于的结点数
        // i到达数目或者num_nodes到达数目且nearest_goal离目标点大于设定值时会跳出while
        // i到了,num_nodes到了,或者找到离目标点很近的点了,就会跳出while
        while (i < iter_limit && num_nodes < max_nodes /* && d>NEAR_DIST*/ ) {
            // 步骤1: 选择target
            // targ_type 是个引用，将根据随机产生的概率，返回goal，waypoint, 还是randomstate
            // init中，将waypoint已经全部设置为随机点，randomState
            target = choose_target(targ_type);
            // 步骤2: 寻找nearest(如果target就是goal,则直接使用nearest_goal,其即为最近点; 否则针对target再调用find_nearest()函数找到最近点)
            // find_nearest是寻找树上，离x最近的state，一开始的树是一颗空树
            nearest = (targ_type == 0) ? nearest_goal : find_nearest(target);
            // 步骤3: 从nearest点处向target扩张(extend)，确定下一个点
            // target就是步骤1中选择的三种，nearest就是步骤2中选择的两种..在树中找的,或者初始点
            // extend在nearest和target方向上,按step寻找一个最近点
            // extend中会做 扩展 检查 添加...会调用check和add_node
            // extend中不会碰才会add_node()
            extend(nearest, target); // 若exrend中不碰，则nearest会更新为新扩展的点
            // 得到新加入的点
            // 如果extend中check是安全的,则add_node, node和num_nodes都会被更新
            newDist = distance(node[num_nodes - 1], goal);
            if (newDist < tempDist) { // tempDist初始化是nearest和goal之间的距离，nearest一开始被初始化为initial
                // 新加入的点与目标点足够近,则更新最近点nearest_goal,及最近点与目标点距离tempDist
                nearest_goal = &node[num_nodes - 1];
                tempDist = newDist;
            }
            i++;
        }
        // 步骤4:回溯路径,寻找优化,逆向考察曲线是否可以简化,即逆向考察每一个节点,看它能否和initial直接相通,没有障碍阻挡
        state *p;
        p = nearest_goal; // 刚刚在while中被更新,总体上是离目标点很近的点
        path_target_length = newDist; // 路径到终点的长度...其实是最后一个添加的点到目标点的距离
        path_target_collision = obs->check(goal, *p);
        num_pathpoint = 0;
        pathlength = 0;
        pathpoint[num_pathpoint++] = initial; // 将路径点保存
        // 每次进循环pathpoint[]的第0个元素都会被设置成initial
        // p是在while中找到的nearest_goal
        if (Vector::distance(p->pos, initial.pos) > 1.0) { // 如果最近点不是起点时
            p = NULL;
            while (p != nearest_goal) {
                // 两头找，找到从initial到哪个state不碰，下一个循环检查这个state和哪个state不碰，直到直到移动到nearest_goal
                // 这样保证，pathpoint中的点都是没有冗余的，且没有碰撞的
                p = nearest_goal;
                // 若起始点不相碰,则从nearest_goal回溯找这样一个点: 从起始点initial到该点连线与障碍物不相碰,即可以直接相连;
                while (p != NULL && !obs->check(pathpoint[num_pathpoint - 1], *p)) {
                    p = p->parent; // 如果initial和p连线与障碍物相碰，则将p设置为p的parent，再检查，直到查到一个和initial相连不碰障碍物的点
                }
                pathlength += Vector::distance(pathpoint[num_pathpoint-1].pos, p->pos); // 初始点到不碰p点的距离
                pathpoint[num_pathpoint++] = *p;
//                GDebugEngine::Instance()->gui_debug_x(CGeoPoint(p->pos.x, p->pos.y));
            }
            //pathpoint[num_pathpoint++] = *p; // 此时 *p == nearest_goal
			num_pathpoint--;
            pathlength += path_target_length; // 最终的pathlength还需要加上nearest和goal之间的距离
        }
        pathpoint[num_pathpoint++] = goal;
        if (num_pathpoint > 2) {
			for (int i = 0; i < num_pathpoint; i++) {
                targetList.push_back(pathpoint[i]);
			}
            // 将回溯终点设为可直达目标点,即这一帧的目标点
            target = pathpoint[1]; //initial之后的一个点，不一定是nearest
        }
        else {
            // 回溯点最终找到本身;
            pathlength = 9999;
            path_target_length = 9999;
            path_target_collision = 1;
            num_pathpoint = 2;
            pathpoint[0] = initial;
            pathpoint[1] = goal;
            vector2f tempv = obs->repulse(initial);
            target.pos = (initial.pos + tempv * step_size);
            pathpoint[num_pathpoint] = pathpoint[num_pathpoint - 1];
            pathpoint[num_pathpoint - 1] = target;
            num_pathpoint ++;
            pathlength = Vector::distance(pathpoint[0].pos, pathpoint[1].pos);
            path_target_length = Vector::distance(pathpoint[1].pos, pathpoint[2].pos);
            path_target_collision = obs->check(pathpoint[1], pathpoint[2]);
			targetList.push_back(goal);
			return(targetList);
        }
        // 步骤5:将成功规划出的路径上的点，随机的，部分加入waypoints中。
        if (drawtreeflag) {
            //waypoint设置!!!设置成nearest之前的点，或者是随机一些点。
            int j;
            if((num_pathpoint > 2)/* && (d < NEAR_DIST)*/ || boost_drand48() < 0.1) {
                // 若能到达目标点,或以一较小概率; 则将当前距离目标最近点nearest_goal及到达它的路径上的点加入waypoints
                j = 1;
                while (num_waypoints > 0 && (num_pathpoint - 1) != j) {
                    i = boost_lrand48() % num_waypoints;
                    waypoint[i] = pathpoint[j];
                    waypoint[i].parent = NULL;
                    // 加到可直达目标点到nearest_goal这段加到waypoint中去,之前的略过
                    j++;
                }
                if (0 /*d < NEAR_DIST*/) {
                    waypoint_target_prob = 1.0 - goal_target_prob - 0.01;
                }
            }
            else {
                // 否则,随机生成点,加入进去
                for (j = 0; num_waypoints > 0 && j < num_waypoints; j++) {
                    i = boost_lrand48() % num_waypoints;
                    waypoint[i] = random_state();
                    waypoint_target_prob = waypoint_target_prob_old;
                }
            }
        }

        // 步骤6:返回当前所需要的下一个位置
        return(targetList);
    }
}
vector<state> path_planner::plan(obstacles *_obs, int obs_mask, state initial, vector2f vel, state _goal) {
    vector<state> target_now; // 要返回的目标点，每次只返回一个点，下一次再进来返回另外一个点，planner是全局的
    vector2f lastMotionVector;
    int i;
    float lastMotionVectorLen;

    lastMotionVector = vector2f(0.0, 0.0);
    obs = _obs;
    obs->set_mask(obs_mask);
    if ( last_num_pathpoint > 1 ) { // 一开始不会进这个选择，第二次进来一般会选这个(好像现在也没用了)
        if (Vector::distance(last_pathpoint[last_num_pathpoint - 1].pos, _goal.pos) > SAMEPOINT) { // 更新终点,则重新开始优化
            still_count = 0;
            last_path_target_collision = false;
            last_num_pathpoint = 0;
        }
        else {
            // 判断原始路径是否与现有的障碍物相撞
            for (i = 0; i < last_num_pathpoint - 2; i++) {
                if ( !obs->check(last_pathpoint[i], last_pathpoint[i + 1])) {
                    break; // 跳出代表有碰撞
                }
            }
            if ( i == (last_num_pathpoint - 2) && ((obs->check(last_pathpoint[i], last_pathpoint[i + 1])) /* || (Vector::distance(last_pathpoint[i].pos,last_pathpoint[i+1].pos) < 20) */)) { // 全部没有碰撞或者最后两点距离很近
                last_path_target_collision = true;  // 表示没相撞
            }
            else {
                last_path_target_collision = false; // 表示相撞
                still_count = 0;                    // 持续状态清零
            }

            // 将原始路径数据更新和运动方向的设计
            last_pathpoint[0].pos = initial.pos;    // 更新初始位置
            lastMotionVector = last_pathpoint[1].pos - last_pathpoint[0].pos; // 运动的方向向量
            lastMotionVectorLen = lastMotionVector.length();
            if (lastMotionVectorLen > SAMEPOINT) {
                lastMotionVector = lastMotionVector / lastMotionVectorLen; // 归一化
            }
            else {
                lastMotionVector = vel.norm(300.0); // 如果小于0，则走原来路线
            }

            if ( Vector::distance(last_pathpoint[1].pos, initial.pos) < 25 ) { // 看车的位置是否到达第一点,如果到达,则改变路径(25cm之内表示到达)
                still_count = 0; // 重新寻找最优点
                if ( last_num_pathpoint > 2) { // 如果有多个点时
                    for (i = 1; i < last_num_pathpoint - 1; i++) {
                        last_pathpoint[i] = last_pathpoint[i + 1];
                    }
                    last_num_pathpoint -= 1;
                }
            }
            if (last_num_pathpoint == 1) {
                printf("DEADLY ERROR************************\n");
            }
            last_pathpoint[last_num_pathpoint - 1].pos = _goal.pos;
        }
    }

    target_now = errt(initial, _goal, 1); // ERRT
    return target_now; // ERRT里面形成了到goal的树,并且返回了一个target,为树中,initial之后的那个state
}
