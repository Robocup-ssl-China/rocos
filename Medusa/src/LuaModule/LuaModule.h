#ifndef _LUA_MODULE_H_
#define _LUA_MODULE_H_

#include "singleton.h"
#include <map>
#include <vector>
#include <geometry.h>
using namespace std;
struct lua_State;

#define LuaGlue extern "C" int
extern "C" {
	typedef int (*LuaFunctionType)(struct lua_State *pLuaState);
}

class CLuaModule{
public:
	CLuaModule();
	virtual ~CLuaModule();
	void		InitLuaGlueFunc();
	bool		RunScript(const char *pFilename);
	bool		RunString(const char *pCommand);
	const char *GetErrorString(void);
	bool		AddFunction(const char *pFunctionName, LuaFunctionType pFunction);
	const char *GetStringArgument(int num, const char *pDefault=NULL);
	double		GetNumberArgument(int num, double dDefault=0.0);
	CGeoPoint*	GetPointArgument(int num);
	bool		GetBoolArgument(int num);
	void		PushString(const char *pString);
	void		PushNumber(double value);
	void		PushBool(bool value);

	void		SetErrorHandler(void(*pErrHandler)(const char *pError)) {m_pErrorHandler = pErrHandler;}

	lua_State	*GetLuaState(void)		{return m_pScriptContext;}
private:
	lua_State	*m_pScriptContext;

	void(*m_pErrorHandler)(const char *pError);

};

typedef NormalSingleton<CLuaModule> LuaModule;

#endif
