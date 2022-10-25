/// FileName : 		KickStatus.h
/// 				declaration and implementation file
/// Description :	It supports flat and chip kick interface for ZJUNlict.
///	Keywords :		flat, chip, kick
/// Organization : 	ZJUNlict@Small Size League
/// Author : 		cliffyin
/// E-mail : 		cliffyin@zju.edu.cn
///					cliffyin007@gmail.com
/// Create Date : 	2011-07-25
/// Modified Date :	2011-07-25 
/// History :

#ifndef _KICK_STATUS_H_
#define _KICK_STATUS_H_

#include "staticparams.h"
#include <singleton.h>

/// <summary>	Kick status, for kick handling.  </summary>
///
/// <remarks>	cliffyin, 2011/7/25. </remarks>

class CKickStatus {
public:

	/// <summary>	Default constructor. </summary>
	///
	/// <remarks>	cliffyin, 2011/7/25. </remarks>

	CKickStatus() {
		clearAll();
	}

	/// <summary>	Sets flat kick. </summary>
	///
	/// <remarks>	cliffyin, 2011/7/25. </remarks>
	///
	/// <param name="num">  	Specified vehicle. </param>
	/// <param name="power">	Specified kick power. </param>

	void setKick(int num, double power) { _normal[num] = power; _chip[num] = _pass[num] = 0; _kicker = num; _needKick[num] = true; }

	/// <summary>	Sets chip kick. </summary>
	///
	/// <remarks>	cliffyin, 2011/7/25. </remarks>
	///
	/// <param name="num">  	Specified vehicle. </param>
	/// <param name="power">	Specified kick power. </param>

	void setChipKick(int num, double power) { 
		_normal[num] = _pass[num] = 0;
		_chip[num] = power;
		_kicker = num; 
		_needKick[num] = true; 
	}

	/// <summary>	Sets both kick. </summary>
	///
	/// <remarks>	cliffyin, 2011/7/25. </remarks>
	///
	/// <param name="num"> 	Specified vehicle. </param>
	/// <param name="kick">	The kick distance. </param>
	/// <param name="chip">	The chip distance. </param>

	void setBothKick(int num, double kick, double chip) { _normal[num] = kick; _chip[num] = chip; _pass[num] = 0; _kicker = num; _needKick[num] = true; }

	/// <summary>	Sets all kick. </summary>
	///
	/// <remarks>	cliffyin, 2011/7/25. </remarks>
	///
	/// <param name="num"> 	Specified vehicle. </param>
	/// <param name="kick">	The kick power. </param>
	/// <param name="chip">	The chip distance. </param>
	/// <param name="pass">	The pass distance. </param>

	void setAllKick(int num, double kick, double chip, double pass) { _normal[num] = kick; _chip[num] = chip; _pass[num] = pass; _kicker = num; _needKick[num] = true; }

	/// <summary>	Check If Need kick. </summary>
	///
	/// <remarks>	cliffyin, 2011/7/25. </remarks>
	///
	/// <param name="num">	Specified vehicle. </param>
	///
	/// <returns>	true if it succeeds, false if it fails. </returns>

	bool needKick(int num) const { return _needKick[num]; }

	/// <summary>	Gets the kick power. </summary>
	///
	/// <remarks>	cliffyin, 2011/7/25. </remarks>
	///
	/// <returns>	The kick power. </returns>

	double getKickPower(int num) const { return _normal[num]; }

	/// <summary>	Gets the chip kick distance. </summary>
	///
	/// <remarks>	cliffyin, 2011/7/25. </remarks>
	///
	/// <returns>	The chip kick distance. </returns>

	double getChipKickDist(int num) const { return _chip[num]; }

	/// <summary>	Gets the pass distance. </summary>
	///
	/// <remarks>	cliffyin, 2011/7/25. </remarks>
	///
	/// <returns>	The pass distance. </returns>

	double getPassDist(int num) const { return _pass[num]; }

	/// <summary>	Gets the kiker. </summary>
	///
	/// <remarks>	cliffyin, 2011/7/25. </remarks>
	///
	/// <returns>	The kiker num. </returns>

	int getKiker() const { return _kicker; } 

	/// <summary>	Clears all interface, called in ActionModule. </summary>
	///
	/// <remarks>	cliffyin, 2011/7/25. </remarks>

	void clearAll() {
		_kicker = 0; 
		for (int vecNum = 0; vecNum < PARAM::Field::MAX_PLAYER; ++ vecNum) {
			_needKick[vecNum] = false;
			_normal[vecNum] = 0;
			_chip[vecNum] = 0;
			_pass[vecNum] = 0;
		}
		_forceClose = false;
		_forceCloseCycle = 0;
	}

	void resetKick2ForceClose(bool forceClose = false, int forceCloseCycle = 0){
		_forceClose = forceClose;
		_forceCloseCycle = forceCloseCycle;
	}

	void updateForceClose(int currentCycle){
		if(currentCycle - _forceCloseCycle >= 7){
			_forceClose = false;
		}
	}

	bool isForceClosed(){
		return _forceClose;
	}

private:

	/// <summary> The kicker </summary>
	int _kicker;		// 当前设定的击球队员

	/// <summary> The normal kick power </summary>
	double _normal[PARAM::Field::MAX_PLAYER];		// 击球的速度

	/// <summary> The chip kick distance </summary>
	double _chip[PARAM::Field::MAX_PLAYER];		// 挑球的距离

	/// <summary> The pass kick distance</summary>
	double _pass[PARAM::Field::MAX_PLAYER];		// 传球的距离

	/// <summary> Need kick flag </summary>
	bool _needKick[PARAM::Field::MAX_PLAYER];

	bool _forceClose;

	int _forceCloseCycle;
};

/// <summary>	Defines an alias representing the kick status. . </summary>
///
/// <remarks>	cliffyin, 2011/7/25. </remarks>

typedef NormalSingleton< CKickStatus > KickStatus;

/// <summary>	Defines an alias representing their kick status. . </summary>
///
/// <remarks>	cliffyin, 2011/7/25. </remarks>

#endif // ~_KICK_STATUS_H_
