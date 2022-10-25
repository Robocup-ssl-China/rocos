#ifndef _SERVER_INTERFACE_H_
#define _SERVER_INTERFACE_H_
#include <server.h>
/************************************************************************/
/*                      和Server的通讯接口                              */
/************************************************************************/
struct RefRecvMsg{
	int blueGoal;
	int yellowGoal;
	int timeRemain;
	int blueGoalie;
	int yellowGoalie;
};
class COptionModule;
class CServerInterface{
public:
	typedef VisualInfoT VisualInfo;
};
#endif //_SERVER_INTERFACE_H_
