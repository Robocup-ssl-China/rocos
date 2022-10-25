#include "ContactChecker.h"
#include "VisionModule.h"

namespace{
	double judge_one[2]={1,1};
	double judge_two[2]={-2,-2};
	double judge_three[2]={-2,-2};
	double judge_four[2]={15,15};
}

ContactChecker::ContactChecker():
    _lastContactNum(0),
    _lastContactReliable(true),
    _previousNum(0),
    _outFieldCycle(0),
    speed_before(0),
    speed_after(0),
    speed_diff_percept(0),
    angle_to(0),
    cycle(0),
    _isBallMoving(0),
    _isBallOutField(0),
    _isJudged(0){}
bool isPointOutField(CGeoPoint pos)
{
	if (pos.x()<PARAM::Field::PITCH_LENGTH/2.0&&pos.x()>-PARAM::Field::PITCH_LENGTH/2.0
		&&pos.y()<PARAM::Field::PITCH_WIDTH/2.0&&pos.y()>-PARAM::Field::PITCH_WIDTH/2.0){
		return false;
	}
	else {
		return true;
	}
}
void ContactChecker::OutFieldJudge(const CVisionModule* pVision)
{
	MobileVisionT last_ball=pVision->ball(pVision->getCycle()-4);
	if (pVision->ball().Valid()&&isPointOutField(pVision->ball().Pos())){
		_isBallOutField=true;
	}
	if (!pVision->ball().Valid()&&last_ball.Valid()&&!isPointOutField(last_ball.Pos()))
	{
		CGeoPoint out_pos=last_ball.Pos()+last_ball.Vel()/15.0;
		if (isPointOutField(out_pos))
		{
			_isBallOutField=true;
		}
	}
	if (_outFieldCycle==0&&_isBallOutField)
	{
		_outFieldCycle=pVision->getCycle();
	}
	/*{
		int counter=0;
		for (int i=0;i<=10;i++)
		{
			if(!isPointOutField(Ball(Cycle()-i).Pos()))
			{
				counter++;
			}
		}
		if (counter>=10&&Ball().Valid()==true&&!isPointOutField(Ball().Pos()))
		{
			_outFieldCycle=0;
		}		
		return false;
	}*/
}
void ContactChecker::prepareJudge(const CVisionModule* pVision)
{
	cycle=pVision->getCycle();
    _ball=pVision->ball();
    _lastball=pVision->ball(cycle-5);
	AllPlayer.clear();
	for (int i=1;i<=6;i++) {
		AllPlayer.push_back(pVision->ourPlayer(i));
	}
	for (int i=1;i<=6;i++) {
		AllPlayer.push_back(pVision->theirPlayer(i));
	}
	_last_contact.frame=0;
	_last_contact.robotnum=0;
	_last_contact.type=0;

	ball_direction_before=pVision->ball(cycle-3).Vel();
	ball_direction_after=pVision->ball().Vel();
	speed_before = ball_direction_before.mod();
	speed_after = ball_direction_after.mod();
	speed_diff_percept = (pVision->ball().Vel()-pVision->ball(cycle-3).Vel()).mod();
	angle_to = (pVision->ball().Vel().dir()-pVision->ball(cycle-3).Vel().dir())/PARAM::Math::PI*180;

	_isJudged=false;
}

void ContactChecker::judgeOne()
{
	if (true){
		int number=0;
		int color=0;
		for (std::vector<PlayerVisionT>::iterator it = AllPlayer.begin(); it != AllPlayer.end(); it++) {
			number++;
			if (it->Valid()){
				if (number>6){color=1;}
				if (_ball.Pos().dist(it->RawPos()) <=(PARAM::Vehicle::V2::PLAYER_SIZE + PARAM::Field::BALL_SIZE+judge_one[color])) {
					_isJudged = true;
					_last_contact.frame = cycle;
					_last_contact.robotnum =number;
					_last_contact.type = 1; 
				}
			}
		}
	}
}

void ContactChecker::judgeTwo()
{
	if (speed_diff_percept > 100 || angle_to > 15 && !_isJudged) {
		int number=0;
		int color=0;
		double last_touched_dist_tmp=30;
		for (std::vector<PlayerVisionT>::iterator it = AllPlayer.begin(); it != AllPlayer.end(); it++) {
			number++;
			if (it->Valid())
			{
				if (number>6){color=1;}
				CGeoCirlce circle (it->RawPos(),PARAM::Vehicle::V2::PLAYER_SIZE+PARAM::Field::BALL_SIZE+judge_three[color]);
				CGeoPoint p1 = _lastball.Pos();
				CGeoPoint p2 = _lastball.Pos()+(_lastball.Vel());
				CGeoSegment ball_line(p1,p2);
				CGeoSegmentCircleIntersection intersection(ball_line,circle);
				CGeoPoint intersections[2]={intersection.point1(),intersection.point2()};
				if (intersection.intersectant()) {
					//cout<<_lastball.Pos()<<endl;
					//cout<<"insecNum"<<number<<endl;
					for (unsigned int j = 0; j < intersection.size(); ++j) {
						if (_lastball.Pos().dist(intersections[j]) < last_touched_dist_tmp&&_lastball.Pos().dist(it->Pos())<25) {
							last_touched_dist_tmp = _lastball.Pos().dist(intersections[j]);
							_isJudged = true;
							_last_contact.frame = cycle;
							_last_contact.robotnum=number;
							_last_contact.type = 2; 
						}
					}
				}
			}

		}
	}
}

void ContactChecker::judgeThree()
{
	if (speed_diff_percept >100|| angle_to > 15 && !_isJudged) {
		int number=0;
		int color=0;
		double last_touched_dist_tmp=20;
		for (std::vector<PlayerVisionT>::iterator it = AllPlayer.begin(); it != AllPlayer.end(); it++) {
			number++;
			if (it->Valid())
			{
				if (number>6){color=1;}
				CGeoCirlce circle (it->RawPos(),PARAM::Vehicle::V2::PLAYER_SIZE+PARAM::Field::BALL_SIZE+judge_three[color]);
				CGeoPoint p1 =_ball.Pos();
				CGeoPoint p2 =_ball.Pos()+(-_ball.Vel());
				CGeoSegment ball_line(p1,p2); 
				CGeoSegmentCircleIntersection intersection(ball_line,circle);
				CGeoPoint intersections[2]={intersection.point1(),intersection.point2()};
				if (intersection.intersectant()) {
					for (int j = 0; j < intersection.size(); ++j) {
						if (_ball.Pos().dist(intersections[j]) < last_touched_dist_tmp&&_ball.Pos().dist(it->Pos())<25) {
							last_touched_dist_tmp = _ball.Pos().dist(intersections[j]);
							_isJudged = true;
							_last_contact.frame = cycle;
							_last_contact.robotnum=number;
							_last_contact.type = 3; 
						}
					}
				}
			}

		}
	}
}
void ContactChecker::judgeFour()
{

	if (speed_diff_percept >200|| angle_to > 20&& _isBallMoving && !_isJudged) {
		int number=0;
		double least_dist=0;
		int color=0;
		for (std::vector<PlayerVisionT>::iterator it = AllPlayer.begin(); it != AllPlayer.end(); it++) {
			number++;
			if (it->Valid()&&_ball.Valid())
			{
				if (least_dist==0)
				{
					least_dist=_ball.Pos().dist(it->Pos());
				}
				else{
					if (_ball.Pos().dist(it->Pos())<least_dist)
					{
						least_dist=_ball.Pos().dist(it->Pos());
						_isJudged = true;
						_last_contact.frame = cycle;
						_last_contact.robotnum=number;
						_last_contact.type = 4;
					}
				}						
			}
		}
		if (_last_contact.robotnum>6){color=1;}
		if (least_dist>judge_four[color])
		{
			_last_contact.robotnum=0;
		}
	}
}
void ContactChecker::judgeFive()
{
	if (speed_after>200&& !_isJudged) {
 		int number=0;
		int color=0;
		double last_touched_dist_tmp=speed_after/15;
		for (std::vector<PlayerVisionT>::iterator it = AllPlayer.begin(); it != AllPlayer.end(); it++) {
			number++;
			if (it->Valid())
			{
				if (number>6){color=1;}
				CGeoCirlce circle (it->RawPos(),PARAM::Vehicle::V2::PLAYER_SIZE);
				CGeoPoint p1 = _ball.Pos();
				CGeoPoint p2 = _ball.Pos()+(_ball.Vel());
				CGeoSegment ball_line(p1,p2);
				CGeoSegmentCircleIntersection intersection(ball_line,circle);
				CGeoPoint intersections[2]={intersection.point1(),intersection.point2()};
				if (intersection.intersectant()) {
					for (unsigned int j = 0; j < intersection.size(); ++j) {
						if (_ball.Pos().dist(intersections[j]) < last_touched_dist_tmp) {
							last_touched_dist_tmp = _ball.Pos().dist(intersections[j]);
							_isJudged = true;
							_last_contact.frame = cycle;
							_last_contact.robotnum=number;
							_last_contact.type = 5; 
						}
					}
				}
			}

		}
	}
}

void ContactChecker::ballMovingJudge(const CVisionModule* pVision)
{
	_isBallMoving=false;
	{
		int couter=0;
		for(int i=0;i<=5;i++)
		{
			if (pVision->ball(pVision->getCycle()-i).Valid()&&pVision->ball(pVision->getCycle()-i).Vel().mod()>10)
			{
				couter++;
			}
		}
		if (couter>=5)
		{
			_isBallMoving=true;
		}
	}
}


void ContactChecker::internalReliableJudge()
{
	if (!_ball.Valid())
	{
		//_lastContactReliable=0;
	}
	//两个置信度判断	
	if (_lastContactNum != 0){

		if (_ball.Pos().dist(AllPlayer[_lastContactNum].Pos())<7)
		{
			setContactNum(0);
			_lastContactReliable=0;
		}

		//避免擦碰
		if (speed_after >200&& angle_to<5&&_lastContactNum!=_previousNum){
			if ((_lastContactNum<6&&_previousNum>=6)||(_lastContactNum>=6&&_previousNum<6))
			{
				_lastContactReliable=0;
				setContactNum(0);
			}

		}
		//避免挤在一起
        if (_last_contact.robotnum>=7&&_last_contact.robotnum<= PARAM::Field::MAX_PLAYER){
			for (std::vector<PlayerVisionT>::iterator it = AllPlayer.begin(); it <=AllPlayer.begin()+5; it++) {
				if (_ball.Pos().dist(it->Pos())<(PARAM::Field::BALL_SIZE+PARAM::Vehicle::V2::PLAYER_SIZE+5))
				{
					setContactNum(0);
					_lastContactReliable=0;break;
				}
			}
		}
		if (_last_contact.robotnum>=1&&_last_contact.robotnum<=6){
			for (std::vector<PlayerVisionT>::iterator it = AllPlayer.begin()+6; it <=AllPlayer.begin()+11; it++) {
				if (_ball.Pos().dist(it->Pos())<(PARAM::Field::BALL_SIZE+PARAM::Vehicle::V2::PLAYER_SIZE+5))
				{
					setContactNum(0);
					_lastContactReliable=0;break;
				}
			}
		}

	} 
}

void ContactChecker::ContactCheck(const CVisionModule* pVision)
{
	//cout<<pVision->Cycle()<<" speed"<<_ball.Vel().mod()<<" "<<_ball.Vel().dir()<<endl;
	//cout<<"raw"<<pVision->RawBall().X()<<endl;
	if (_ball.Valid())
	{
		judgeOne();
		judgeTwo();
		judgeThree();
		judgeFour();
		judgeFive();
	}
	//cout<<"last_contact"<<_last_contact.type<<endl;
	//cout<<"last_contact_Num"<<_last_contact.robotnum<<endl;
	_previousNum=_lastContactNum;
	if(_last_contact.robotnum != 0){
		_lastContactNum = _last_contact.robotnum;
		_lastContactReliable=true;
		/*_ballContact.push_back(_last_contact);
		if(_ballContact.size()>20) _ballContact.erase(_ballContact.begin());*/
	}
	internalReliableJudge();
}
void ContactChecker::externReliableJudge()
{	
	//if (_outFieldCycle!=0&&!_ballContact.empty()){   
	//	//避免漏判
	//	vector<Last_Contact>::iterator it=_ballContact.end()-1;
	//	if (_outFieldCycle - it->frame>120){
	//		_lastContactReliable=0;
	//	}
	//}

}
void ContactChecker::refereeJudge(const CVisionModule* pVision)
{
	prepareJudge(pVision);
	OutFieldJudge(pVision);
    if (pVision->getCurrentRefereeMsg()!="GameStop"&&pVision->getCurrentRefereeMsg()!="GameHalt"&&pVision->rawBall().Valid()){
		ContactCheck(pVision);	
	}			
	//if ( pVision->_refereeMsg <2&&pVision->_lastRefereeMsg>=2) {
	//	if (!_isBallOutField)
	//	{
	//		//setContactReliable(false);
	//		//_lastContactReliable=false;
	//	}
	//}
    if ((pVision->getCurrentRefereeMsg()!="GameStop"||pVision->getCurrentRefereeMsg()!="GameHalt")
        &&(pVision->getLastRefereeMsg()=="GameStop"||pVision->getLastRefereeMsg()=="GameHalt"))
	{
		_outFieldCycle=0;
		_isBallOutField=false;
		setContactNum(0);
		//_lastContactNum=0;
		setContactReliable(true);
	}

	if (getContactReliable()&&getContactNum()!=0){
		_lastContactNum=getContactNum();
	}else{
		_lastContactNum=0;
	}	
}
