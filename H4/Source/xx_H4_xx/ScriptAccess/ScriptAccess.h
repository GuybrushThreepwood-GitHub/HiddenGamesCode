
/*===================================================================
	File: ScriptAccess.h
	Game: H4

	(C)Hidden Games
=====================================================================*/

#ifndef __SCRIPTACCESS_H__
#define __SCRIPTACCESS_H__

#include "ScriptBase.h"
#include "ScriptAccess/ScriptDataHolder.h"

struct LuaGlobal
{
	const char *szConstantName;
	double nConstantValue;
	int nLuaType;
};

void RegisterScriptFunctions( ScriptDataHolder& dataHolder );

ScriptDataHolder* GetScriptDataHolder();

int ScriptGetHiResMode( lua_State* pState );

int ScriptSetDevData( lua_State* pState );

#endif // __SCRIPTACCESS_H__

