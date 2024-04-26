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
#include <singleton.hpp>

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

	void setKick(int num, double power) { _normal[num] = power; _chip[num] = 0; _kicker = num; _needKick[num] = true; }

	/// <summary>	Sets chip kick. </summary>
	///
	/// <remarks>	cliffyin, 2011/7/25. </remarks>
	///
	/// <param name="num">  	Specified vehicle. </param>
	/// <param name="power">	Specified kick power. </param>

	void setChipKick(int num, double power) { 
		_normal[num] = 0;
		_chip[num] = power;
		_kicker = num; 
		_needKick[num] = true; 
	}

	// kick_mode: false for flat kick, true for chip kick
	void setDirectKick(int num, bool kick_mode, double power, bool no_calibration = false, double direct_kick_power = 0){
		_normal[num] = kick_mode ? 0 : power;
		_chip[num] = kick_mode ? power : 0;
		_kicker = num;
		_needKick[num] = true;
		// use for kick collect calibration data
		_direct_kick_no_calibration[num] = no_calibration;
		_direct_kick_power[num] = direct_kick_power;
	}
	bool isDirectKickNoCalibration(int num) const { return _direct_kick_no_calibration[num]; }
	double getDirectKickPower(int num) const { return _direct_kick_power[num]; }

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
			_direct_kick_no_calibration[vecNum] = false;
			_direct_kick_power[vecNum] = 0;
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

	/// <summary> Need kick flag </summary>
	bool _needKick[PARAM::Field::MAX_PLAYER];

	bool _direct_kick_no_calibration[PARAM::Field::MAX_PLAYER];
	double _direct_kick_power[PARAM::Field::MAX_PLAYER];

	bool _forceClose;

	int _forceCloseCycle;
};

/// <summary>	Defines an alias representing the kick status. . </summary>
///
/// <remarks>	cliffyin, 2011/7/25. </remarks>

typedef Singleton< CKickStatus > KickStatus;

/// <summary>	Defines an alias representing their kick status. . </summary>
///
/// <remarks>	cliffyin, 2011/7/25. </remarks>

#endif // ~_KICK_STATUS_H_
