
/*===================================================================
	File: WorldAccess.h
	Game: H4

	(C)Hidden Games
=====================================================================*/

#ifndef __WORLDACCESS_H__
#define __WORLDACCESS_H__

void RegisterStageFunctions( ScriptDataHolder& dataHolder );

int ScriptSetCameraData( lua_State* pState );

int ScriptSetStartStage( lua_State* pState );

int ScriptAddToStages( lua_State* pState );

int ScriptSetActiveStage( lua_State* pState );

int ScriptChangeToStage( lua_State* pState );

int ScriptLaunchMicroGame( lua_State* pState );

int ScriptEndTheGame( lua_State* pState );

#endif // __WORLDACCESS_H__
