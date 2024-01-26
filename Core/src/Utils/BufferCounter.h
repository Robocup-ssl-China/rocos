#ifndef _BUFFER_COUNTER_H_
#define _BUFFER_COUNTER_H_

#include "singleton.hpp"
#include <vector>

using namespace std;

struct BufCnt
{
	int cnt_frame;
	int cnt_num;
	int buf_frame;
	int buf_num;
	bool condition;
};

class CBufferCounter{
public:
	CBufferCounter();
	~CBufferCounter();
	void startCount(int, bool, int, int );
	bool timeOut(int, bool);
	void clear();
	bool isClear(int);
private:
	vector<BufCnt> _bufCnts;
	bool _cleared;
};

typedef Singleton<CBufferCounter> BufferCounter;

#endif // _BUFFER_COUNTER_H_