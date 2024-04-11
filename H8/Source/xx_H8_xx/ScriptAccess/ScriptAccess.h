
/*===================================================================
	File: ScriptAccess.h
	Game: H8

	(C)Hidden Games
=====================================================================*/

#ifndef __SCRIPTACCESS_H__
#define __SCRIPTACCESS_H__

#include "ScriptBase.h"
#include "ScriptAccess/ScriptDataHolder.h"

void RegisterScriptFunctions( ScriptDataHolder& dataHolder );

int ScriptSetDevData( lua_State* pState );

int ScriptSetProfileState( lua_State* pState );

ScriptDataHolder* GetScriptDataHolder();

#endif // __SCRIPTACCESS_H__

