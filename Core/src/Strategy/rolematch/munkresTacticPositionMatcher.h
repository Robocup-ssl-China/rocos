#ifndef _MUMKRES_TACTIC_POSITION_MATCHER_H_
#define _MUMKRES_TACTIC_POSITION_MATCHER_H_

#include <geometry.h>
#include <string>
#include <vector>
#include <VisionModule.h>

using namespace std;

/// <summary>	Munkres tactic position.  </summary>
/// 定义为 战术跑位点，带动态属性
/// <remarks>	ZjuNlict, 2011-7-28. </remarks>

struct MunkresTacticPosition {

	/// <summary>	Constructor. </summary>
	///
	/// <remarks>	ZjuNlict, 2011-7-28. </remarks>
	///
	/// <param name="pos">   	The position. </param>
	/// <param name="vel">   	(optional) the velocity. </param>
	/// <param name="radius">	(optional) the radius. </param>
	MunkresTacticPosition(){}
	MunkresTacticPosition(const CGeoPoint& pos, const CVector vel = CVector(0,0), const double radius = 0) {
		_pos = pos;
		_vel = vel;
		_radius = radius;
	}

	/// <summary>	Equality operator. </summary>
	///
	/// <remarks>	ZjuNlict, 2011-7-28. </remarks>
	///
	/// <param name="mtp">	Another MunkresTacticPosition object. </param>
	///
	/// <returns>	true if the parameters are considered equivalent. </returns>

	bool operator == (const MunkresTacticPosition& mtp) {
		return _pos.x() == mtp._pos.x()
				&& _pos.y() == mtp._pos.y()
				&& _vel.x() == mtp._vel.x()
				&& _vel.y() == mtp._vel.y()
				&& _radius == mtp._radius;
	}
					
	CGeoPoint _pos;
	CVector _vel;
	double _radius;
};

/************************************************************************/
/* A1(==>A2) <==> B1(==>B2)                                             */
/************************************************************************/
/// 算法输入使用
// 角色名-战术跑位点 匹配对
typedef std::pair< std::string, MunkresTacticPosition> Role2TacPosPair;
// 角色名-战术跑位点 匹配对 组成的链：作为匹配输入的结构 A1
typedef std::vector< Role2TacPosPair > Role2TacPosList;

// 行号-角色名-战术跑位点 匹配对
typedef std::pair< int, Role2TacPosPair> Row2Role2TacPosPair;
// 行号-角色名-战术跑位点 匹配对 组成的链：作为匹配输入到匈牙利算法输入处理的结构 A2
typedef std::vector< Row2Role2TacPosPair > Row2Role2TacPosPairList;

// 车号 组成的链：作为匹配输入的结构 B1
typedef std::vector < int > VehNumList;

// 列号-车号 匹配对
typedef std::pair< int , int > Col2VehNumPair;
// 列号-车号 匹配对 组成的链：作为匹配输入到匈牙利算法输入处理的结构 B2
typedef std::vector< Col2VehNumPair > Col2VehNumPairList;

/// 算法输出使用
// 角色名-车号 匹配对
typedef std::pair< std::string, int > Role2VehNumPair;
// 角色名-车号 匹配对 组成的链：作为匈牙利算法输出结果的结构 C
typedef std::vector< Role2VehNumPair > Role2VehNumList;

/// <summary>	Munkres tactic position matcher.  </summary>
///
/// <remarks>	ZjuNlict, 2011-7-28. </remarks>

class CMunkresTacticPositionMatcher {
public:

	/// <summary>	Constructor. </summary>
	///
	/// <remarks>	ZjuNlict, 2011-7-28. </remarks>
	///
	/// <param name="pVision">		  	The vision module. </param>
	/// <param name="role2TacPosList">	List of role 2 tac positions. </param>
	/// <param name="vehNumList">	  	List of veh numbers. </param>

	CMunkresTacticPositionMatcher(const CVisionModule* pVision, const Role2TacPosList& role2TacPosList, const VehNumList& vehNumList);

	/// <summary>	Finaliser. </summary>
	///
	/// <remarks>	ZjuNlict, 2011-7-28. </remarks>

	~CMunkresTacticPositionMatcher();

	/// <summary>	Executes the match operation. </summary>
	///
	/// <remarks>	ZjuNlict, 2011-7-28. </remarks>

	void doMatch();

	/// <summary>	Gets the match result. </summary>
	///
	/// <remarks>	ZjuNlict, 2011-7-28. </remarks>
	///
	/// <returns>	The match result. </returns>
	const Role2VehNumList& getMatchResult();

private:
	void inputHandle();
	void match();

private:
	// input data : A1 - B1
	Role2TacPosList _role2TacPosList;
	VehNumList _vehNumList;

	// input transfer : A2 - B2
	Row2Role2TacPosPairList _row2Role2TacPosPairList;
	Col2VehNumPairList _col2VehNumPairList;

	// output res : C
	Role2VehNumList _role2VehNumList;
};

#endif // ~_MUMKRES_TACTIC_POSITION_MATCHER_H_