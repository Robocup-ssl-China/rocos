/*========================================================================
    FastAlloc.h :  A pool-based memory allocator for same-sized objects
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

#ifndef __FAST_ALLOC_H__
#define __FAST_ALLOC_H__

#define FSTALC_TEM template <class item_t>
#define FSTALC_FUN fast_allocator<item_t>

// #define DEBUG 1

// 自己来处理一些列表的增删操作: 
// 先自己保留一定大小的空间缓冲,用以存储增删item
// 而不是直接通过内存空间申请释放操作;
FSTALC_TEM
class fast_allocator{
	item_t *free_list;
	int num_alloc,num_free;
public:
	fast_allocator() {free_list=NULL; num_alloc=num_free=0;}
	~fast_allocator();

	item_t *alloc();
	void free(item_t *item);
	void freelist(item_t *item);

	int count_allocated() {return(num_alloc);}
	int count_free() {return(num_free);}
};

FSTALC_TEM
FSTALC_FUN::~fast_allocator()
{
	item_t *p,*q;

	p = free_list;
	while(q = p){
		p = p->next;
		q->next = NULL;
		delete(q);
	}

	free_list = NULL;
	num_alloc = num_free = 0;
}

// 获得一个item
// 如果有freelist,则表示尚有闲置空间,则先从freelist里把这块空间利用过来; 没有的话再创建一个.
FSTALC_TEM
item_t *FSTALC_FUN::alloc()
{
	item_t *p;

	if(free_list){
		p = free_list;
		free_list = p->next;
		p->next = NULL;
		num_free--;
	}else{
		p = new item_t;
	}
	num_alloc++;

	return(p);
}


//将item指针加入到freelist列表头部
FSTALC_TEM
void FSTALC_FUN::free(item_t *item)
{
	item->next = free_list;
	free_list = item;
	num_free++;
	num_alloc--;
}

//将一个item的列表都加入freelist中
FSTALC_TEM
void FSTALC_FUN::freelist(item_t *item)
{
	item_t *n;

	while(item){
		n = item->next;
		item->next = free_list;
		free_list = item;
		item = n;

		num_free++;
		num_alloc--;
	}
}

#endif /*__FAST_ALLOC_H__*/