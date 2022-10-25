#include "OptionModule.h"
#include "staticparams.h"
#include <weerror.h>
#include "WorldModel.h"
#include "parammanager.h"
namespace{
	int DEFAULT_NUMBER = 1;
	int DEFAULT_TYPE = 2;
    int DEFAULT_COLOR = PARAM::BLUE;
}
COptionModule::COptionModule()
: _side(PARAM::Field::POS_SIDE_LEFT)
{
	_color = DEFAULT_COLOR;
	_number = DEFAULT_NUMBER;
    _type = DEFAULT_TYPE;
    bool isRight;
    bool isYellow;
    ZSS::ZParamManager::instance()->loadParam(isRight,"ZAlert/IsRight",false);
    ZSS::ZParamManager::instance()->loadParam(isYellow,"ZAlert/IsYellow",false);
    _side = isRight ? PARAM::Field::POS_SIDE_RIGHT : PARAM::Field::POS_SIDE_LEFT;
    _color = isYellow ? PARAM::YELLOW : PARAM::BLUE;

	std::cout << "Side : " << ((_side == PARAM::Field::POS_SIDE_LEFT) ? "left" : "right")
              << ", Color : " << ((_color == PARAM::YELLOW) ? "yellow" : "blue") << " is running..." << std::endl;
	WorldModel::Instance()->registerOption(this);
}

COptionModule::~COptionModule(void)
{

}

void COptionModule::ReadOption(int argc, char* argv[])
{
	for( int i=1; i<argc; ++i ){
		switch( argv[i][0] ){
		case 's':
			// side
			if( argv[i][1] == 'l' ){
				_side = PARAM::Field::POS_SIDE_LEFT;
			}else if( argv[i][1] == 'r' ){
				_side = PARAM::Field::POS_SIDE_RIGHT;
			}else{
                _side = PARAM::Field::POS_SIDE_LEFT;
			}
			break;
		case 'c':
			// color
			if( argv[i][1] == 'y' ){
                _color = PARAM::YELLOW;
			}else if( argv[i][1] == 'b' ){
                _color = PARAM::BLUE;
			}else{
                _color = PARAM::BLUE;
			}
			break;
		default:
			break;
		}
	}
}

