#include "RobotCapability.h"
#include "parammanager.h"

namespace{
	int SAO_ACTION = 0;
	class OmniAuto{
	protected:
    	OmniAuto()
        {
        ZSS::ZParamManager::instance()->loadParam(TRANSLATION_SPEED_LIMIT,"CGotoPositionV2/NormalSpeedLimit",4000); //下发速度最大限制，守门员、后卫除外
        ZSS::ZParamManager::instance()->loadParam(MAX_TRANSLATION_SPEED,"CGotoPositionV2/MNormalSpeed",3000);
        ZSS::ZParamManager::instance()->loadParam(MAX_TRANSLATION_ACC,"CGotoPositionV2/MNormalAcc",4500);
        ZSS::ZParamManager::instance()->loadParam(MAX_TRANSLATION_DEC,"CGotoPositionV2/MNormalDec",4500);
        ZSS::ZParamManager::instance()->loadParam(MAX_TRANSLATION_SPEED_BACK,"CGotoPositionV2/MBackSpeed",3000);
        ZSS::ZParamManager::instance()->loadParam(MAX_TRANSLATION_ACC_BACK,"CGotoPositionV2/MBackAcc",4500);
        ZSS::ZParamManager::instance()->loadParam(MAX_TRANSLATION_DEC_BACK,"CGotoPositionV2/MBackDec",4500);
        ZSS::ZParamManager::instance()->loadParam(MAX_TRANSLATION_SPEED_GOALIE,"CGotoPositionV2/MGoalieSpeed",3000);
        ZSS::ZParamManager::instance()->loadParam(MAX_TRANSLATION_ACC_GOALIE,"CGotoPositionV2/MGoalieAcc",4500);
        ZSS::ZParamManager::instance()->loadParam(MAX_TRANSLATION_DEC_GOALIE,"CGotoPositionV2/MGoalieDec",4500);
	    ZSS::ZParamManager::instance()->loadParam(MAX_ROTATION_SPEED,"CGotoPositionV2/RotationSpeed",15);
	    ZSS::ZParamManager::instance()->loadParam(MAX_ROTATION_ACC,"CGotoPositionV2/RotationAcc",15);
        ZSS::ZParamManager::instance()->loadParam(MAX_ROTATION_DEC,"CGotoPositionV2/RotationAcc",15);
        ZSS::ZParamManager::instance()->loadParam(SAO_ACTION,"Alert/SaoAction",0);
        ZSS::ZParamManager::instance()->loadParam(MAX_RUSH_TRANSLATION_SPEED,"CGotoPositionV2/MRushTranslationSpeed", 4000);

			//if (SAO_ACTION == 2) {
			//	MAX_TRANSLATION_SPEED *= 1.5;
			//	MAX_TRANSLATION_ACC *= 1.5;
			//	MAX_TRANSLATION_DEC *= 1.5;
			//	MAX_TRANSLATION_SPEED_GOALIE *= 1.5;
			//	MAX_TRANSLATION_ACC_GOALIE *= 1.5;
			//	MAX_TRANSLATION_DEC_GOALIE *= 1.5;
			//	MAX_TRANSLATION_SPEED_BACK *= 1.5;
			//	MAX_TRANSLATION_ACC_BACK *= 1.5;
			//	MAX_TRANSLATION_DEC_BACK *= 1.5;
			//}
		}

		double maxSpeedGoalie(const double angle) const { return MAX_TRANSLATION_SPEED_GOALIE; }
		double maxAccelerationGoalie(const CVector& velocity, const double angle) const { return MAX_TRANSLATION_ACC_GOALIE; }
		double maxDecGoalie(const CVector& velocity, const double angle) const { return MAX_TRANSLATION_DEC_GOALIE; }

		double maxSpeedBack(const double angle) const { return MAX_TRANSLATION_SPEED_BACK; }
		double maxAccelerationBack(const CVector& velocity, const double angle) const { return MAX_TRANSLATION_ACC_BACK; }
		double maxDecBack(const CVector& velocity, const double angle) const { return MAX_TRANSLATION_DEC_BACK; }

        double maxSpeed(const double angle) const { return TRANSLATION_SPEED_LIMIT; }
        double maxAcceleration(const CVector& velocity, const double angle) const { return MAX_TRANSLATION_ACC; }
        double maxDec(const CVector& velocity, const double angle) const { return MAX_TRANSLATION_DEC; }

		double maxAngularSpeed() const { return MAX_ROTATION_SPEED; }
		double maxAngularAcceleration() const { return MAX_ROTATION_ACC; }
		double maxAngularDec() const { return MAX_ROTATION_DEC; }

        double maxRushTranslationSpeed() const { return MAX_RUSH_TRANSLATION_SPEED; }
		
	private:
        double TRANSLATION_SPEED_LIMIT;//最大速度

        double MAX_TRANSLATION_SPEED;
        double MAX_TRANSLATION_ACC;	//最大加速度
		double MAX_TRANSLATION_DEC;	//最大减速度
		double MAX_ROTATION_SPEED;			//最大角速度
		double MAX_ROTATION_ACC;	//最大角加速度
		double MAX_ROTATION_DEC;	//最大角减速度

		/// 守门员专用
		double MAX_TRANSLATION_SPEED_GOALIE;
		double MAX_TRANSLATION_ACC_GOALIE;
		double MAX_TRANSLATION_DEC_GOALIE;

		/// 后卫专用
		double MAX_TRANSLATION_SPEED_BACK;
		double MAX_TRANSLATION_ACC_BACK;
		double MAX_TRANSLATION_DEC_BACK;


        double MAX_RUSH_TRANSLATION_SPEED;
	};
	
	// 挑球的特性
	class ChipKick{ // 有挑球装置
	protected:
		ChipKick() : ChipKickEnabled(true), MinChipKickObstacleDist(10), MaxChipKickDist(300), MaxChipShootDist(180), 
						MinChipShootExtra(20)
		{}
		double minChipKickObstacleDist() const { return ChipKickEnabled ? MinChipKickObstacleDist : 100000; }
		double maxChipKickDist() const { return MaxChipKickDist; }
		double maxChipShootDist() const { return MaxChipShootDist; }
		double minChipShootExtra(const double dist) const { return MinChipShootExtra; }
	private:
		bool ChipKickEnabled; // 是否开启ChipKick
		double MinChipKickObstacleDist;
		double MaxChipKickDist;
		double MaxChipShootDist;
		double MinChipShootExtra;
	};
	
	// 击球的特性
	class KickAuto{
	protected:
		KickAuto() : MinShootAngleWidth(PARAM::Field::BALL_SIZE*1.5)
		{}
		double minShootAngleWidth() const { return MinShootAngleWidth; }
	private:
		double MinShootAngleWidth;
	};

	template < class MovingCap, class KickCap, class ChipKickCap>
	class ConcretRobotCapability : public CRobotCapability, MovingCap, KickCap, ChipKickCap{
	public:
		virtual double minShootAngleWidth() const { return KickCap::minShootAngleWidth(); }

		virtual double minChipKickObstacleDist() const { return ChipKickCap::minChipKickObstacleDist(); }
		virtual double maxChipKickDist() const { return ChipKickCap::maxChipKickDist(); }
		virtual double maxChipShootDist() const { return ChipKickCap::maxChipShootDist(); }
		virtual double minChipShootExtra(const double dist) const { return ChipKickCap::minChipShootExtra(dist); }
		
		virtual double maxAccelerationGoalie(const CVector& velocity, const double angle) const { return MovingCap::maxAccelerationGoalie(velocity, angle); }
		virtual double maxDecGoalie(const CVector& velocity, const double angle) const { return MovingCap::maxDecGoalie(velocity, angle);}
		virtual double maxSpeedGoalie(const double angle) const { return MovingCap::maxSpeedGoalie(angle); }

		virtual double maxAccelerationBack(const CVector& velocity, const double angle) const { return MovingCap::maxAccelerationBack(velocity, angle); }
		virtual double maxDecBack(const CVector& velocity, const double angle) const { return MovingCap::maxDecBack(velocity, angle);}
		virtual double maxSpeedBack(const double angle) const { return MovingCap::maxSpeedBack(angle); }

		virtual double maxAcceleration(const CVector& velocity, const double angle) const { return MovingCap::maxAcceleration(velocity, angle); }
		virtual double maxDec(const CVector& velocity, const double angle) const { return MovingCap::maxDec(velocity, angle);}
		virtual double maxSpeed(const double angle) const { return MovingCap::maxSpeed(angle); }

		virtual double maxAngularAcceleration() const { return MovingCap::maxAngularAcceleration(); }
		virtual double maxAngularDec() const { return MovingCap::maxAngularDec();}
		virtual double maxAngularSpeed() const { return MovingCap::maxAngularSpeed(); }

        virtual double maxRushTranslationSpeed() const { return MovingCap::maxRushTranslationSpeed(); }
	};
}
CRobotCapFactory::CRobotCapFactory()
{
	// 初始化每个车的能力，注意,每次比赛前要检查此处! 看是否正确
	typedef ConcretRobotCapability< OmniAuto, KickAuto, ChipKick > VehicleAuto;
	
	_robotCaps.push_back(new VehicleAuto); // 类型0

	// 小车的类型, 0表示10系的, 1表示9系的
	for( int i=0; i<PARAM::Field::MAX_PLAYER; ++i ){
		LeftRobotType[i] = 0;
		RightRobotType[i] = 0;
	}
}

