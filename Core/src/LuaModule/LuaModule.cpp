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

extern "C" int Register_Role(lua_State *L)
{
	int num = LuaModule::Instance()->GetNumberArgument(1, 0);
	string role = LuaModule::Instance()->GetStringArgument(2, NULL);
	TaskMediator::Instance()->setRoleInLua(num, role);
	return 0;
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

luaDef GUIGlue[] = 
{
    {"CRegisterRole",		Register_Role},
    {"CGetSettings",        FUNC_GetSettings},
	{NULL, NULL}
};
