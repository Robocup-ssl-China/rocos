#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include "LuaModule.h"
#include "geometry.h"
#include "TaskMediator.h"
#include "BufferCounter.h"
#include "Global.h"
#include <QString>

#ifndef _WIN32
#include <libgen.h>
#endif

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#include "tolua++.h"
TOLUA_API int  tolua_zeus_open (lua_State* tolua_S);

extern "C" {
	typedef struct 
	{
		const char *name;
		int (*func)(lua_State *);
	}luaDef;
}

extern luaDef GUIGlue[];

namespace{
	bool IS_SIMULATION = false;
    bool LUA_DEBUG = true;
}

CLuaModule::CLuaModule():m_pScriptContext(nullptr)
{
	m_pErrorHandler = NULL;

	m_pScriptContext = lua_open();
	luaL_openlibs(m_pScriptContext);
	tolua_zeus_open(m_pScriptContext);
    InitLuaGlueFunc();
    ZSS::ZParamManager::instance()->loadParam(LUA_DEBUG,"Debug/A_LuaDebug",true);

}

void CLuaModule::InitLuaGlueFunc()
{
	for(int i=0; GUIGlue[i].name; i++) {
		AddFunction(GUIGlue[i].name, GUIGlue[i].func);
	}
}

CLuaModule::~CLuaModule()
{
	if(m_pScriptContext)
		lua_close(m_pScriptContext);
}

static std::string findScript(const char *pFname)
{
	FILE *fTest;
#if defined(_WIN32)
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];
    _splitpath( pFname, drive, dir, fname, ext );
    std::string scriptsString = "Scripts\\";
#else
    std::string pname = pFname;
    int l = pname.length();
    char name[l+1];
    strcpy(name,pname.c_str());
    std::string drive = "";
    std::string dir = dirname(name);
    std::string fname = basename(name);// Mark : not work /error
    std::string ext = "";
    std::string scriptsString = "Scripts/";
#endif
    std::string strTestFile(pFname);
    fTest = fopen(strTestFile.c_str(), "r");
    if(fTest == NULL)
    {
        //not that one...
        strTestFile = (std::string) drive + dir + scriptsString + fname + ".lua";
        fTest = fopen(strTestFile.c_str(), "r");
    }
	if(fTest == NULL)
	{
		//not that one...
        strTestFile = (std::string) drive + dir + scriptsString + fname + ".LUA";
		fTest = fopen(strTestFile.c_str(), "r");
	}

	if(fTest == NULL)
	{
		//not that one...
		strTestFile = (std::string) drive + dir + fname + ".LUB";
		fTest = fopen(strTestFile.c_str(), "r");
	}

	if(fTest == NULL)
	{
		//not that one...
		//not that one...
		strTestFile = (std::string) drive + dir + fname + ".LUA";
		fTest = fopen(strTestFile.c_str(), "r");
	}

	if(fTest != NULL)
	{
		fclose(fTest);
	}

	return strTestFile;
}

bool CLuaModule::RunScript(const char *pFname)
{
    std::string strFilename = findScript(pFname);
    const char *pFilename = strFilename.c_str();

	if (0 != luaL_loadfile(m_pScriptContext, pFilename) || 0 != lua_pcall(m_pScriptContext, 0, LUA_MULTRET, 0))
    {
        double x = (-PARAM::Field::PITCH_LENGTH/2)+50;
		double y = (PARAM::Field::PITCH_WIDTH/2);
		double y_step = -150;
        qDebug() << QString("Lua Error - Script Run\nScript Name:%1\nError Message:%2\n").arg(pFilename).arg(luaL_checkstring(m_pScriptContext, -1));
        GDebugEngine::Instance()->gui_debug_msg_fix(CGeoPoint(x,y+y_step*1),QString("Lua Error - Script Load").toLatin1(),COLOR_RED,0,100);
        GDebugEngine::Instance()->gui_debug_msg_fix(CGeoPoint(x,y+y_step*2),QString("Name:%1").arg(pFilename).toLatin1(),COLOR_RED,0,100);
        GDebugEngine::Instance()->gui_debug_msg_fix(CGeoPoint(x,y+y_step*3),QString("Error Message:%1").arg(luaL_checkstring(m_pScriptContext, -1)).toLatin1(),COLOR_RED,0,100);
        GDebugEngine::Instance()->send(!ZSS::ZParamManager::instance()->value("ZAlert/IsYellow").toBool());
        if(LUA_DEBUG){
            std::cout << "Press enter to continue ...";
            std::cin.get();
        }
            return false;
    }
	return true;
}

bool CLuaModule::RunString(const char *pCommand)
{
	if (0 != luaL_loadbuffer(m_pScriptContext, pCommand, strlen(pCommand), NULL))
	{
		if(m_pErrorHandler)
		{
			char buf[256];
			sprintf(buf, "Lua Error - String Load\nString:%s\nError Message:%s\n", pCommand, luaL_checkstring(m_pScriptContext, -1));
			m_pErrorHandler(buf);
		}

		return false;
	}
	if (0 != lua_pcall(m_pScriptContext, 0, LUA_MULTRET, 0))
	{
		if(m_pErrorHandler)
		{
			char buf[256];
			sprintf(buf, "Lua Error - String Run\nString:%s\nError Message:%s\n", pCommand, luaL_checkstring(m_pScriptContext, -1));
			m_pErrorHandler(buf);
		}

		return false;
	}
	return true;
}

const char *CLuaModule::GetErrorString(void)
{
	return luaL_checkstring(m_pScriptContext, -1);
}


bool CLuaModule::AddFunction(const char *pFunctionName, LuaFunctionType pFunction)
{
	lua_register(m_pScriptContext, pFunctionName, pFunction);
	return true;
}

const char *CLuaModule::GetStringArgument(int num, const char *pDefault)
{
	return luaL_optstring(m_pScriptContext, num, pDefault);

}

double CLuaModule::GetNumberArgument(int num, double dDefault)
{
	return luaL_optnumber(m_pScriptContext, num, dDefault);
}

bool CLuaModule::GetBoolArgument(int num)
{
	return lua_toboolean(m_pScriptContext, num);
}

CGeoPoint* CLuaModule::GetPointArgument(int num)
{
	return (CGeoPoint*)(lua_touserdata(m_pScriptContext, num));
}

void CLuaModule::PushString(const char *pString)
{
	lua_pushstring(m_pScriptContext, pString);
}

void CLuaModule::PushNumber(double value)
{
	lua_pushnumber(m_pScriptContext, value);
}

void CLuaModule::PushBool(bool value)
{
	lua_pushboolean(m_pScriptContext, value);
}

// extern "C" int Skill_SmartGotoPoint(lua_State *L)
// {
// 	TaskT playerTask;
// 	int runner = LuaModule::Instance()->GetNumberArgument(1, NULL);
// 	playerTask.executor = runner;
// 	double x = LuaModule::Instance()->GetNumberArgument(2, NULL);
// 	double y = LuaModule::Instance()->GetNumberArgument(3, NULL);
// 	playerTask.player.pos = CGeoPoint(x,y);
// 	playerTask.player.rotvel = 0;
// 	playerTask.player.angle = LuaModule::Instance()->GetNumberArgument(4, NULL);
// 	playerTask.player.flag = LuaModule::Instance()->GetNumberArgument(5, NULL);
// 	playerTask.ball.Sender = LuaModule::Instance()->GetNumberArgument(6,NULL);
// 	playerTask.player.max_acceleration = LuaModule::Instance()->GetNumberArgument(7,NULL);
//     double vx = LuaModule::Instance()->GetNumberArgument(8, 0.0);
//     double vy = LuaModule::Instance()->GetNumberArgument(9, 0.0);
//     playerTask.player.vel = CVector(vx,vy);

// 	CPlayerTask* pTask = TaskFactoryV2::Instance()->SmartGotoPosition(playerTask);
// 	TaskMediator::Instance()->setPlayerTask(runner, pTask, 1);
	
// 	return 0;
// }

// extern "C" int Skill_GoCmuRush(lua_State *L)
// {
// 	TaskT playerTask;
// 	playerTask.player.is_specify_ctrl_method = true;
//     playerTask.player.specified_ctrl_method = CMU_TRAJ;
//     int runner = int(LuaModule::Instance()->GetNumberArgument(1, 0));
// 	playerTask.executor = runner;
//     double x = LuaModule::Instance()->GetNumberArgument(2, 0);
//     double y = LuaModule::Instance()->GetNumberArgument(3, 0);
// 	playerTask.player.pos = CGeoPoint(x,y);
//     playerTask.player.angle = LuaModule::Instance()->GetNumberArgument(4, 0);
//     playerTask.player.flag = int(LuaModule::Instance()->GetNumberArgument(5, 0));
//     playerTask.ball.Sender = int(LuaModule::Instance()->GetNumberArgument(6,0));
//     playerTask.player.max_acceleration = LuaModule::Instance()->GetNumberArgument(7,0);
//     double vx = LuaModule::Instance()->GetNumberArgument(9,0.0);
//     double vy = LuaModule::Instance()->GetNumberArgument(10,0.0);
// 	playerTask.player.max_speed = LuaModule::Instance()->GetNumberArgument(11,0.0);
// 	playerTask.player.force_manual_set_running_param = LuaModule::Instance()->GetBoolArgument(12);
//     playerTask.player.vel = CVector(vx,vy);

// 	CPlayerTask* pTask = TaskFactoryV2::Instance()->SmartGotoPosition(playerTask);
// 	TaskMediator::Instance()->setPlayerTask(runner, pTask, 1);

// 	return 0;
// }

// extern "C" int Skill_Goalie(lua_State *L)
// {
//     TaskT playerTask;
//     int runner = int(LuaModule::Instance()->GetNumberArgument(1, 0));
//     playerTask.executor = runner;
//     CPlayerTask* pTask = TaskFactoryV2::Instance()->Goalie(playerTask);
//     TaskMediator::Instance()->setPlayerTask(runner, pTask, 1);

//     return 0;
// }

// extern "C" int Skill_Touch(lua_State *L)
// {
//     TaskT playerTask;
//     int runner = int(LuaModule::Instance()->GetNumberArgument(1, 0));
//     playerTask.executor = runner;
//     double x = LuaModule::Instance()->GetNumberArgument(2, 0);
//     double y = LuaModule::Instance()->GetNumberArgument(3, 0);
//     playerTask.player.pos = CGeoPoint(x,y);
//     bool useInter = LuaModule::Instance()->GetBoolArgument(4);
//     playerTask.player.is_specify_ctrl_method = useInter;
//     CPlayerTask* pTask = TaskFactoryV2::Instance()->Touch(playerTask);
//     TaskMediator::Instance()->setPlayerTask(runner, pTask, 1);

//     return 0;
// }

// extern "C" int Skill_NoneZeroGoCmuRush(lua_State *L)
// {
// 	TaskT playerTask;
// 	playerTask.player.is_specify_ctrl_method = true;
// 	playerTask.player.specified_ctrl_method = CMU_TRAJ;
// 	int runner = LuaModule::Instance()->GetNumberArgument(1, NULL);
// 	playerTask.executor = runner;
// 	double x = LuaModule::Instance()->GetNumberArgument(2, NULL);
// 	double y = LuaModule::Instance()->GetNumberArgument(3, NULL);
// 	playerTask.player.pos = CGeoPoint(x, y);
// 	playerTask.player.angle = LuaModule::Instance()->GetNumberArgument(4, NULL);
// 	playerTask.player.flag = LuaModule::Instance()->GetNumberArgument(5, NULL);
// 	playerTask.ball.Sender = LuaModule::Instance()->GetNumberArgument(6, NULL);
// 	playerTask.player.max_acceleration = LuaModule::Instance()->GetNumberArgument(7, NULL);
// 	double velX = LuaModule::Instance()->GetNumberArgument(9, NULL);
// 	double velY = LuaModule::Instance()->GetNumberArgument(10, NULL);
//     playerTask.player.vel = CVector(velX, velY);

//     CPlayerTask* pTask = TaskFactoryV2::Instance()->SmartGotoPosition(playerTask);
// 	TaskMediator::Instance()->setPlayerTask(runner, pTask, 1);

// 	return 0;
// }

// extern "C" int Skill_SimpleGotoPoint(lua_State *L)
// {
// 	int runner = LuaModule::Instance()->GetNumberArgument(1, NULL);
// 	double x = LuaModule::Instance()->GetNumberArgument(2, NULL);
// 	double y = LuaModule::Instance()->GetNumberArgument(3, NULL);
// 	double angle = LuaModule::Instance()->GetNumberArgument(4, NULL);
// 	int flag = LuaModule::Instance()->GetNumberArgument(5, NULL);
// 	CPlayerTask* pTask = PlayerRole::makeItSimpleGoto(runner, CGeoPoint(x, y), angle, flag);
// 	TaskMediator::Instance()->setPlayerTask(runner, pTask, 1);

// 	return 0;
// }

// extern "C" int Skill_Stop(lua_State *L)
// {
// 	int runner = LuaModule::Instance()->GetNumberArgument(1, NULL);
// 	CPlayerTask* pTask = PlayerRole::makeItStop(runner);
// 	TaskMediator::Instance()->setPlayerTask(runner, pTask, 1);

// 	return 0;
// }

extern "C" int Register_Role(lua_State *L)
{
	int num = LuaModule::Instance()->GetNumberArgument(1, 0);
	string role = LuaModule::Instance()->GetStringArgument(2, NULL);
	TaskMediator::Instance()->setRoleInLua(num, role);
	return 0;
}

// extern "C" int Skill_Speed(lua_State *L)
// {
// 	int runner = LuaModule::Instance()->GetNumberArgument(1,NULL);
// 	double speedX = LuaModule::Instance()->GetNumberArgument(2,NULL);
// 	double speedY = LuaModule::Instance()->GetNumberArgument(3,NULL);
// 	double rotSpeed = LuaModule::Instance()->GetNumberArgument(4,NULL);
// 	CPlayerTask* pTask = PlayerRole::makeItRun(runner, speedX, speedY, rotSpeed, 0);
// 	TaskMediator::Instance()->setPlayerTask(runner, pTask, 1);
// 	return 0;
// }
// extern "C" int Skill_OpenSpeed(lua_State *L)
// {
//     int runner = LuaModule::Instance()->GetNumberArgument(1,NULL);
//     double speedX = LuaModule::Instance()->GetNumberArgument(2,NULL);
//     double speedY = LuaModule::Instance()->GetNumberArgument(3,NULL);
//     double rotSpeed = LuaModule::Instance()->GetNumberArgument(4,NULL);
// 	int flag = LuaModule::Instance()->GetNumberArgument(5,0);
//     CPlayerTask* pTask = PlayerRole::makeItOpenRun(runner, speedX, speedY, rotSpeed, flag);
//     TaskMediator::Instance()->setPlayerTask(runner, pTask, 1);
//     return 0;
// }

extern "C" int FUNC_TimeOut(lua_State* L)
{
	bool cond = LuaModule::Instance()->GetBoolArgument(1);
	int buf = LuaModule::Instance()->GetNumberArgument(2, 0);
	int cnt = LuaModule::Instance()->GetNumberArgument(3, 9999);

	if(BufferCounter::Instance()->isClear(vision->getCycle())){
		BufferCounter::Instance()->startCount(vision->getCycle(), cond, buf, cnt);
	}

	if(BufferCounter::Instance()->timeOut(vision->getCycle(), cond)){
		LuaModule::Instance()->PushNumber(1);
	} else{
		LuaModule::Instance()->PushNumber(0);
	}
	return 1;
}

extern "C" int FUNC_GetIsSimulation(lua_State* L)
{
    ZSS::ZParamManager::instance()->loadParam(IS_SIMULATION,"Alert/IsSimulation",false);

	LuaModule::Instance()->PushBool(IS_SIMULATION);
	return 1;
}

extern "C" int FUNC_GetSettings(lua_State* L){
    QString key(LuaModule::Instance()->GetStringArgument(1, NULL));
    QString type(LuaModule::Instance()->GetStringArgument(2, NULL));
    if(type == "Bool"){
        bool temp;
        ZSS::ZParamManager::instance()->loadParam(temp,key);
        LuaModule::Instance()->PushBool(temp);
    }else if(type == "Int"){
        int temp;
        ZSS::ZParamManager::instance()->loadParam(temp,key);
        LuaModule::Instance()->PushNumber(temp);
    }else if(type == "Double"){
        double temp;
        ZSS::ZParamManager::instance()->loadParam(temp,key);
        LuaModule::Instance()->PushNumber(temp);
    }else{
        QString temp;
        ZSS::ZParamManager::instance()->loadParam(temp,key);
        LuaModule::Instance()->PushString(temp.toLatin1());
    }
    return 1;
}

extern "C" int FUNC_PrintString(lua_State* L) {
    const char* str = LuaModule::Instance()->GetStringArgument(1, NULL);
    printf("%s\n",str);
    fflush(stdout);
    return 0;
}
// extern "C" int Skill_SpeedInRobot(lua_State* L){
// 	int runner = LuaModule::Instance()->GetNumberArgument(1,NULL);
// 	double speedX = LuaModule::Instance()->GetNumberArgument(2,NULL);
// 	double speedY = LuaModule::Instance()->GetNumberArgument(3,NULL);
// 	double rotSpeed = LuaModule::Instance()->GetNumberArgument(4,NULL);
// 	CVector localVel(speedX, speedY);
// 	CVector globalVel = localVel.rotate(vision->ourPlayer(runner).Dir());
// 	DribbleStatus::Instance()->setDribbleCommand(runner,3);
// 	CPlayerTask* pTask = PlayerRole::makeItRun(runner, globalVel.x(), globalVel.y(), rotSpeed, 0);
// 	TaskMediator::Instance()->setPlayerTask(runner, pTask, 1);
// 	return 0;
// }

luaDef GUIGlue[] = 
{
    {"CRegisterRole",		Register_Role},
    {"CTimeOut",			FUNC_TimeOut},
	{"CGetIsSimulation",	FUNC_GetIsSimulation},
    {"CGetSettings",        FUNC_GetSettings},
    // {"SmartGotoPos",		Skill_SmartGotoPoint},
	// {"SimpleGotoPos",		Skill_SimpleGotoPoint},
    // {"StopRobot",			Skill_Stop},
    // {"CSpeed",				Skill_Speed},
    // {"COpenSpeed",			Skill_OpenSpeed},
    // {"CGoCmuRush",			Skill_GoCmuRush},
    // {"CGoalie",             Skill_Goalie},
    // {"CTouch",              Skill_Touch},
    // {"CNoneZeroGoCmuRush",	Skill_NoneZeroGoCmuRush},
    // {"CSpeedInRobot",		Skill_SpeedInRobot},
	{NULL, NULL}
};
