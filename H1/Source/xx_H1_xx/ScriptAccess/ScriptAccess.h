
/*===================================================================
	File: ScriptAccess.h
	Game: AirCadets

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

int ScriptAddLevelPack( lua_State* pState );
int ScriptSetStartLevel( lua_State* pState );

int ScriptSetDevData( lua_State* pState );

int ScriptSetLevelData( lua_State* pState );
int ScriptSetPlayerData( lua_State* pState );
int ScriptSetPhysicsData( lua_State* pState );

int ScriptSetProfileState( lua_State* pState );

ScriptDataHolder* GetScriptDataHolder();

#endif // __SCRIPTACCESS_H__

