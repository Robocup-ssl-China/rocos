#include "BufferCounter.h"
#include <iostream>

namespace{
	int startCnt = 0;
	int timeOutCnt = 0;
	int lastStartCycle = 0;
	int lastTimeOutCycle = 0;
}

CBufferCounter::CBufferCounter()
{
	_bufCnts.clear();
}

CBufferCounter::~CBufferCounter()
{

}

void CBufferCounter::startCount(int cycle, bool cond, int buf, int cnt)
{
	if(cycle > lastStartCycle){
		startCnt = 0;
	}

	BufCnt bc;
	bc.condition = cond;
	bc.buf_frame = buf;
	bc.cnt_frame = cnt;
	bc.buf_num = 0;
	bc.cnt_num = 0;
	_bufCnts.push_back(bc);
	
	lastStartCycle = cycle;
	startCnt++;
}

void CBufferCounter::clear()
{
	_bufCnts.clear();
}

bool CBufferCounter::isClear(int cycle)
{
	if(_bufCnts.empty()|| lastStartCycle == 0 || lastStartCycle == cycle){
		return true;
	}
	return false;
}

bool CBufferCounter::timeOut(int cycle, bool cond)
{
	// 如果超过定时时间，直接退出
	if(cycle > lastTimeOutCycle){
		timeOutCnt = 0;
	}

	BufCnt& _curBufCnt = _bufCnts[timeOutCnt];
	_curBufCnt.condition = cond;
	lastTimeOutCycle = cycle;
	timeOutCnt++;
	//std::cout<<timeOutCnt<<" "<<_curBufCnt.cnt_num<<"("<<_curBufCnt.cnt_frame<<") "<<_curBufCnt.buf_num
	//	<<"("<<_curBufCnt.buf_frame<<") "<<endl;
	if( _curBufCnt.cnt_num < _curBufCnt.cnt_frame ){
		_curBufCnt.cnt_num++;
	} else{
	//	clear();
		return true;
	}

	if(_curBufCnt.condition){
		_curBufCnt.buf_num++;
		if(_curBufCnt.buf_num < _curBufCnt.buf_frame){
		//	_curBufCnt.buf_num++;
		} else{
		//	clear();
			return true;
		}
	}
	return false;
}