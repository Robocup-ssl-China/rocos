/// FileName : 		DribbleStatus.h
/// 				declaration and implementation file
/// Description :	It supports dribble setting interface for ZJUNlict.
///	Keywords :		dribble
/// Organization : 	ZJUNlict@Small Size League
/// Author : 		cliffyin
/// E-mail : 		cliffyin@zju.edu.cn
///					cliffyin007@gmail.com
/// Create Date : 	2011-07-25
/// Modified Date :	2011-07-25 
/// History :

#ifndef _DRIBBLE_STATUS_H_
#define _DRIBBLE_STATUS_H_
/**
* CDribbleStatus.
* 记录当前带球的状态
*/

#include "staticparams.h"
#include <geometry.h>
#include <singleton.h>

/// <summary>	Dribble status, for dribble handling.  </summary>
///
/// <remarks>	cliffyin, 2011/7/25. </remarks>

class CDribbleStatus {
public:

	/// <summary>	Default constructor. </summary>
	///
	/// <remarks>	cliffyin, 2011/7/25. </remarks>

	CDribbleStatus()
	{
		_ballController = 0;
		for (int vecNum = 0; vecNum < PARAM::Field::MAX_PLAYER; ++ vecNum) {
			_needReleaseBall[vecNum] = _dribbleOn[vecNum] = _dribbleStartPosValid[vecNum] = false;
			_dribbleCommand[vecNum] = 0;
		}
	}

	/// <summary>	Check If Dribble on. </summary>
	///
	/// <remarks>	cliffyin, 2011/7/25. </remarks>
	///
	/// <param name="number">	The vehicle num. </param>
	///
	/// <returns>	true if it succeeds, false if it fails. </returns>

	bool dribbleOn(int number) const { return _dribbleOn[number]; }

	/// <summary>	Gets the ball controller. </summary>
	///
	/// <remarks>	cliffyin, 2011/7/25. </remarks>
	///
	/// <returns>	. </returns>

	int ballController() const { return _ballController; }

	/// <summary>	Check If Ball controlled. </summary>
	///
	/// <remarks>	cliffyin, 2011/7/25. </remarks>
	///
	/// <param name="number">	The vehicle num. </param>
	///
	/// <returns>	true if it succeeds, false if it fails. </returns>

	bool ballControlled(int number) const { return _ballController == number; }

	/// <summary>	Dribble start position valid. </summary>
	///
	/// <remarks>	cliffyin, 2011/7/25. </remarks>
	///
	/// <param name="number">	The vehicle num. </param>
	///
	/// <returns>	true if it succeeds, false if it fails. </returns>

	bool dribbleStartPosValid(int number) const { return _dribbleStartPosValid[number]; }

	/// <summary>	Gets the last ball controller. </summary>
	///
	/// <remarks>	cliffyin, 2011/7/25. </remarks>
	///
	/// <returns>	. </returns>

	int lastBallController() const {return _lastBallController;}

	/// <summary>	Check If the last ball controller. </summary>
	/// <summary>	Gets the last ball controller. </summary>
	///
	/// <remarks>	cliffyin, 2011/7/25. </remarks>
	///
	/// <param name="number">	The vehicle num. </param>
	///
	/// <returns>	true if it succeeds, false if it fails. </returns>
	/// <returns>	. </returns>

	bool lastBallController (int number) const {return _lastBallController == number;}

	/// <summary>	Sets the last ball controller. </summary>
	///
	/// <remarks>	cliffyin, 2011/7/25. </remarks>
	///
	/// <param name="number">	The vehicle num. </param>

	void setLastBallController(int number){ _lastBallController = number;}

	/// <summary>	Dribble start position. </summary>
	///
	/// <remarks>	cliffyin, 2011/7/25. </remarks>
	///
	/// <param name="number">	The vehicle num. </param>
	///
	/// <returns>	. </returns>

	const CGeoPoint& dribbleStartPos(int number) const { return _dribbleStartPos[number]; }

	/// <summary>	Dribble start cycle. </summary>
	///
	/// <remarks>	cliffyin, 2011/7/25. </remarks>
	///
	/// <param name="number">	The vehicle num. </param>
	///
	/// <returns>	. </returns>

	int dribbleStartCycle(int number) const { return _dribbleStartCycle[number]; }

	/// <summary>	Sets dribble on. </summary>
	///
	/// <remarks>	cliffyin, 2011/7/25. </remarks>
	///
	/// <param name="number">	The vehicle num. </param>
	/// <param name="cycle"> 	The cycle. </param>
	/// <param name="pos">   	The dribble start position. </param>

	void setDribbleOn(int number, int cycle, const CGeoPoint& pos)
	{ 
		if (! _dribbleOn[number]) {
			_dribbleOn[number] = true;
		}

		if (! _dribbleStartPosValid[number]) {
			_dribbleStartPos[number] = pos;
			_dribbleStartCycle[number] = cycle;
			_dribbleStartPosValid[number] = true;
		}
	}

	/// <summary>	Sets dribble off. </summary>
	///
	/// <remarks>	cliffyin, 2011/7/25. </remarks>
	///
	/// <param name="number">	The vehicle num. </param>

	void setDribbleOff(int number)
	{ 
		_dribbleOn[number] = false;
		_dribbleStartPosValid[number] = false;
	}

	/// <summary>	Sets dribble start position lost. </summary>
	///
	/// <remarks>	cliffyin, 2011/7/25. </remarks>
	///
	/// <param name="number">	The vehicle num. </param>

	void setDribbleStartPosLost(int number){ _dribbleStartPosValid[number] = false; }

	/// <summary>	Sets ball controlled. </summary>
	///
	/// <remarks>	cliffyin, 2011/7/25. </remarks>
	///
	/// <param name="number">	The vehicle num. </param>
	/// <param name="p">	 	The start control pos. </param>

	void setBallControlled(int number, const CGeoPoint& p)
	{ 
		if (_ballController != number) {
			_controlStartPos = p;
			_ballController = number;
		}
	}

	/// <summary>	Sets ball lost. </summary>
	///
	/// <remarks>	cliffyin, 2011/7/25. </remarks>
	///
	/// <param name="number">	The vehicle num. </param>

	void setBallLost(int number)
	{ 
		if( _ballController == number ){
			_ballController = 0; 
		}
	}

	/// <summary>	Gets the ball control star position. </summary>
	///
	/// <remarks>	cliffyin, 2011/7/25. </remarks>
	///
	/// <returns>	. </returns>

	const CGeoPoint& ballControlStarPos() const { return _controlStartPos; }

	/// <summary>	Need release ball. </summary>
	///
	/// <remarks>	cliffyin, 2011/7/25. </remarks>
	///
	/// <param name="number">	The vehicle num. </param>
	///
	/// <returns>	true if it succeeds, false if it fails. </returns>

	bool needReleaseBall(int number) const { return _needReleaseBall[number]; }

	/// <summary>	Sets need not release ball. </summary>
	///
	/// <remarks>	cliffyin, 2011/7/25. </remarks>
	///
	/// <param name="number">	The vehicle num. </param>

	void setNeedNotReleaseBall(int number) { _mayNeedReleaseBall[number] = _needReleaseBall[number] = false; }

	/// <summary>	Sets may need release ball. </summary>
	///
	/// <remarks>	cliffyin, 2011/7/25. </remarks>
	///
	/// <param name="number">   	The vehicle num. </param>
	/// <param name="pos">			The dribble start position. </param>
	/// <param name="dribbling">	true to dribbling. </param>

	void setMayNeedReleaseBall(int number, const CGeoPoint& pos, bool dribbling)
	{ 
		if( PARAM::Rule::MaxDribbleDist > 0 ){
			if( !_mayNeedReleaseBall[number] ){
				if( dribbling ){ // 开始带球了
					_mayNeedReleaseBall[number] = true;
					_mayNeedReleaseBallStartPos[number] = pos;
				}
			}else if((pos - _mayNeedReleaseBallStartPos[number]).mod2() > PARAM::Rule::MaxDribbleDist * PARAM::Rule::MaxDribbleDist ){
				_needReleaseBall[number] = true;
			}
		}else{
			_needReleaseBall[number] = false;
		}
	}

	/// <summary>	Clears the dribble command. </summary>
	///
	/// <remarks>	cliffyin, 2011/7/25. </remarks>

	void clearDribbleCommand(){
		for (int vecNum = 0; vecNum < PARAM::Field::MAX_PLAYER; ++ vecNum) {
			_dribbleCommand[vecNum] = 0;
		}
	}

	/// <summary>	Sets dribble command. </summary>
	///
	/// <remarks>	cliffyin, 2011/7/25. </remarks>
	///
	/// <param name="num">  	Vehicle num. </param>
	/// <param name="power">	The dribble power. </param>

	void setDribbleCommand(int num, unsigned char power){ 
		if(num>=0 && num< PARAM::Field::MAX_PLAYER) _dribbleCommand[num] = power;
	}

	/// <summary>	Gets dribble command. </summary>
	///
	/// <remarks>	cliffyin, 2011/7/25. </remarks>
	///
	/// <param name="num">	Vehicle num. </param>
	///
	/// <returns>	The dribble command. </returns>

	unsigned char getDribbleCommand(int num){ 
		if (num>=0 && num < PARAM::Field::MAX_PLAYER) {
			return _dribbleCommand[num];
		} else {
			return 0;
		}
	}

	/// <summary>	Check If Ball controlled. </summary>
	///
	/// <remarks>	cliffyin, 2011/7/25. </remarks>
	///
	/// <returns>	true if it succeeds, false if it fails. </returns>

	bool ballControlled(){
		for(int i = 0; i < PARAM::Field::MAX_PLAYER; i++){
			if(ballControlled(i)){
				return true;
			}	
		}
		return false;
	}
	
private:

	/// <summary> true to enable, false to disable the dribble </summary>
	bool _dribbleOn[PARAM::Field::MAX_PLAYER]; // 带球装置是否打开

	/// <summary> true to dribble start position valid </summary>
	bool _dribbleStartPosValid[PARAM::Field::MAX_PLAYER]; // 开始带球的位置是否有效

	/// <summary> The dribble start position </summary>
	CGeoPoint _dribbleStartPos[PARAM::Field::MAX_PLAYER]; // 开始带球的位置

	/// <summary> The dribble start cycle </summary>
	int _dribbleStartCycle[PARAM::Field::MAX_PLAYER]; // 开始带球的时间

	/// <summary> true to need release ball </summary>
	bool _needReleaseBall[PARAM::Field::MAX_PLAYER]; // 是否需要把球松开

	/// <summary> true to may need release ball </summary>
	bool _mayNeedReleaseBall[PARAM::Field::MAX_PLAYER]; // 是否需要把球松开

	/// <summary> The may need release ball start position </summary>
	CGeoPoint _mayNeedReleaseBallStartPos[PARAM::Field::MAX_PLAYER]; // 开始需要松开球的位置

	/// <summary> The control start position </summary>
	CGeoPoint _controlStartPos; // 开始控球的位置

    /// <summary> The ball controller </summary>
    int _ballController; // 控球者

	/// <summary> The last ball controller </summary>
	int _lastBallController;

	/// <summary> The dribble command </summary>
	unsigned char _dribbleCommand[PARAM::Field::MAX_PLAYER];
};

typedef NormalSingleton< CDribbleStatus > DribbleStatus;

#endif // ~_DRIBBLE_STATUS_H_
