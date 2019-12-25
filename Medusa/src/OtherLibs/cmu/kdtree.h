/*========================================================================
    KDTree.h : Template for fast online creation KD Trees
  ------------------------------------------------------------------------
    Copyright (C) 1999-2002  James R. Bruce
    School of Computer Science, Carnegie Mellon University
  ------------------------------------------------------------------------
    This software is distributed under the GNU General Public License,
    version 2.  If you do not have a copy of this licence, visit
    www.gnu.org, or write: Free Software Foundation, 59 Temple Place,
    Suite 330 Boston, MA 02111-1307 USA.  This program is distributed
    in the hope that it will be useful, but WITHOUT ANY WARRANTY,
    including MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  ========================================================================*/

#ifndef __KD_TREE_H__
#define __KD_TREE_H__

#include "fast_alloc.h"
#include "vector.h"
#include <GDebugEngine.h>

#define KDT_TEMP template <class state>
#define KDT_FUN  KDTree<state>

// KDTree: 只有叶节点才会保留state位置信息,而非叶节点只是对场地进行方块式的划分,没有实际的位置点分布;
KDT_TEMP
class KDTree{
	struct node{
		vector2f minv,maxv; // bounding box of subtree
		state *states;      // list of states stored at this node
		int num_states;     // number of states of this subtree

		union{
			node *child[2]; // children of this tree
			node *next;
		};
	};

	node *root;
	int leaf_size,max_depth;
	int tests;
	fast_allocator<node> anode;

protected:
	inline bool inside(vector2f &minv,vector2f &maxv,state &s);
	inline float box_distance(vector2f &minv,vector2f &maxv,vector2f &p);

	void split(node *t,int split_dim);
	state *nearest(node *t,state *best,float &best_dist,vector2f &x);
	void clear(node *t);

	void drawATree(node* anode);
public:
	KDTree() {root=NULL; leaf_size=max_depth=0;}
    
	bool setdim(vector2f &minv,vector2f &maxv,int nleaf_size,int nmax_depth);
	bool add(state *s);
	void clear();
	state *nearest(float &dist,vector2f &x);

public:
	void drawTree();
};

// 判断s是否在方形(minv,maxv)之间
KDT_TEMP
inline bool KDT_FUN::inside(vector2f &minv,vector2f &maxv,state &s)
{
	return(s.pos.x>minv.x && s.pos.y>minv.y 
		&& s.pos.x<maxv.x && s.pos.y<maxv.y);
}

// p与将点p限制在方形(minv,maxv)时的对应点之间的距离
KDT_TEMP
inline float KDT_FUN::box_distance(vector2f &minv,vector2f &maxv,vector2f &p)
{
	float dx,dy;

	dx = p.x - bound(p.x,minv.x,maxv.x);
	dy = p.y - bound(p.y,minv.y,maxv.y);

	return(sqrt(dx*dx + dy*dy));
}

// 将节点t处的states根据维度split_dim进行二分,并放置到两棵子树后面(而自身便不带states);
KDT_TEMP
void KDT_FUN::split(node *t,int split_dim)
{
	node *a,*b;
	state *p,*n;
	float split_val;

	// make new nodes
	a = anode.alloc();
	b = anode.alloc();
	if(!a || !b) return;

	a->child[0] = b->child[0] = NULL;
	a->child[1] = b->child[1] = NULL;
	a->states = b->states = NULL;
	a->num_states = b->num_states = 0;

	// determine split value
	a->minv = b->minv = t->minv;
	a->maxv = b->maxv = t->maxv;

	if(split_dim == 0){
		split_val = (t->minv.x + t->maxv.x) / 2;
		a->maxv.x = b->minv.x = split_val;
	}else{
		split_val = (t->minv.y + t->maxv.y) / 2;
		a->maxv.y = b->minv.y = split_val;
	}

	// 对t节点上所有states进行分类,归属a或b去;
	// separate children based on split_val
	n = t->states;
	while(p = n){
		n = n->next;

		// 将p的值与split_val比较,相应的分到a子树或b子树后面去
		if(((split_dim == 0) ? p->pos.x : p->pos.y) < split_val){
			// 将p插入到a后面
			p->next = a->states;
			a->states = p;
			a->num_states++;
		}else{
			// 插入到b后面
			p->next = b->states;
			b->states = p;
			b->num_states++;
		}
	}

	// 将a,b子树插到t后面
	// insert into tree
	t->states = NULL;	// 自身不带states,所有states都在child[0,1]后面
	t->child[0] = a;
	t->child[1] = b;
}

// 重置一棵树
KDT_TEMP
bool KDT_FUN::setdim(vector2f &minv,vector2f &maxv,int nleaf_size,int nmax_depth)
{
	clear();
	if(!root) root = anode.alloc();
	if(!root) return(false);
	mzero(*root);
	root->minv = minv;
	root->maxv = maxv;
	leaf_size = nleaf_size;
	max_depth = nmax_depth;
	return(true);
}

KDT_TEMP
bool KDT_FUN::add(state *s)
{
	node *p;
	int c,level;

	level = 0;
	p = root;

	// 树为空,或者位置s已在树根节点所代表的方形范围内,则返回false
	if(!p || !inside(p->minv,p->maxv,*s)) return(false);

	// go down tree to see where new state should go
	while(p->child[0]){ // implies p->child[1] also
		c = !inside(p->child[0]->minv,p->child[0]->maxv,*s);	// s是否在子树节点0代表的方形里面: 是则跳到子树0,继续找; 否则去子树1中继续找; 一直找到叶节点为止;
		p = p->child[c];
		level++;
	}

	// 将s插入到叶节点状态列表的头部
	// add it to leaf; and split leaf if too many children
	s->next = p->states;
	p->states = s;
	p->num_states++;

	// split leaf if not too deep and too many children for one node
	if(level<max_depth && p->num_states>leaf_size){
		// 根据level的奇偶性对p点的states进行x维度或y维度的拆分
		split(p,level % 2);
	}
	return(true);
}


// 将t子树清空
KDT_TEMP
void KDT_FUN::clear(node *t)
{
	if(!t) return;
	if(t->child[0]) clear(t->child[0]);
	if(t->child[1]) clear(t->child[1]);

	t->child[0] = t->child[1] = NULL;
	t->states = NULL;
	t->num_states = 0;

	anode.free(t);
}

KDT_TEMP
void KDT_FUN::clear()
{
	if(!root) return;

	clear(root->child[0]);
	clear(root->child[1]);

	root->child[0] = root->child[1] = NULL;
	root->states = NULL;
	root->num_states = 0;
}

KDT_TEMP
state *KDT_FUN::nearest(node *t,state *best,float &best_dist,vector2f &x)
{
	float d,dc[2];
	state *p;
	int c;

	// look at states at current node
	// 先寻找t节点上的所有state,找到最小的state
	p = t->states;
	while(p){
		d = Vector::distance(p->pos,x);
		if(d < best_dist){
			best = p;
			best_dist = d;
		}
		tests++;
		p = p->next;
	}

	// 递归的寻找t的子树上是否有更近的点
	// recurse on children (nearest first to maximize pruning)
	if(t->child[0]){ // implies t->child[1]
		dc[0] = box_distance(t->child[0]->minv,t->child[0]->maxv,x);
		dc[1] = box_distance(t->child[1]->minv,t->child[1]->maxv,x);

		// 根据dc[0],dc[1]哪个小,即位置x离哪个子方块更近,来判定那棵子树会有更小值
		c = dc[1] < dc[0]; // c indicates nearest lower bound distance child

		if(dc[ c] < best_dist) best = nearest(t->child[ c],best,best_dist,x);
		if(dc[!c] < best_dist) best = nearest(t->child[!c],best,best_dist,x);
	}

	return(best);
}

KDT_TEMP
state *KDT_FUN::nearest(float &dist,vector2f &x)
{
	state *best;

	best = NULL;
	dist = 4000;

	tests = 0;
	best = nearest(root,best,dist,x);
	// printf("tests=%d dist=%f\n\n",tests,best_dist);

	return(best);
}

// recursively draw a tree
KDT_TEMP
void KDT_FUN::drawATree(node* anode)
{
	if (anode->states){
		std::cout<<"leaf start draw"<<endl;
		state* p = anode->states;
		while( p != NULL && p->next != NULL){
			GDebugEngine::Instance()->gui_debug_line(CGeoPoint(p->pos.x, p->pos.y), CGeoPoint(p->next->pos.x,p->next->pos.y), COLOR_ORANGE);
			p = p->next;
		}
		std::cout<<"leaf end draw"<<endl;
	}
	else if (anode->child[0]){
		std::cout<<"have child..."<<endl;
		drawATree(anode->child[0]);
		drawATree(anode->child[1]);
	}else{
		std::cout<<"wrong layer"<<std::endl;
		return;
	}
}

// 画出整棵树的函数
KDT_TEMP
void KDT_FUN::drawTree()
{
	drawATree(root);
}

#endif /*__KD_TREE_H__*/