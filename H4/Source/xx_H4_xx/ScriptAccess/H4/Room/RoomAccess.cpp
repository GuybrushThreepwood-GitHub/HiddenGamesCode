
/*===================================================================
	File: roomAccess.cpp
	Game: H4

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"
#include "MathBase.h"
#include "RenderBase.h"
#include "SoundBase.h"
#include "CollisionBase.h"
#include "ModelBase.h"
#include "InputBase.h"
#include "ScriptBase.h"

#include "Effects/Emitter.h"
#include "ScriptAccess/ScriptDataHolder.h"
#include "GameSystems.h"

#include "ScriptAccess/H4/Room/RoomAccess.h"

LuaGlobal roomGlobals[] = 
{
	// instance type
	{ "TYPE_LIMITEDINSTANCE",				static_cast<double>(Level::TYPE_LIMITEDINSTANCE),			LUA_TNUMBER },
	
	{ "TYPE_EXT_EMPTY",						static_cast<double>(Level::TYPE_EXT_EMPTY),					LUA_TNUMBER },
	{ "TYPE_EXT_AMMO",						static_cast<double>(Level::TYPE_EXT_AMMO),					LUA_TNUMBER },
	{ "TYPE_EXT_HEALTH",					static_cast<double>(Level::TYPE_EXT_HEALTH),				LUA_TNUMBER },
	{ "TYPE_EXT_ZOMBIE",					static_cast<double>(Level::TYPE_EXT_ZOMBIE),				LUA_TNUMBER },
	{ "TYPE_EXT_LOCKED",					static_cast<double>(Level::TYPE_EXT_LOCKED),				LUA_TNUMBER }
};

/////////////////////////////////////////////////////
/// Function: RegisterRoomFunctions
/// Params: None
///
/////////////////////////////////////////////////////
void RegisterRoomFunctions()
{
	int i=0;

	// setup globals
	for( i=0; i < sizeof(roomGlobals)/sizeof(LuaGlobal); ++i )
	{
		lua_pushnumber( script::LuaScripting::GetState(), roomGlobals[i].nConstantValue );
		lua_setglobal( script::LuaScripting::GetState(), roomGlobals[i].szConstantName );
	}

	script::LuaScripting::GetInstance()->RegisterFunction( "SetupLimitedInstanceRooms",	ScriptSetupLimitedInstanceRooms );
	script::LuaScripting::GetInstance()->RegisterFunction( "AddLimitedInstanceRoom",		ScriptAddLimitedInstanceRoom );

	script::LuaScripting::GetInstance()->RegisterFunction( "SetupExtendedInstanceRooms",	ScriptSetupExtendedInstanceRooms );
	script::LuaScripting::GetInstance()->RegisterFunction( "AddExtendedInstanceRoom",		ScriptAddExtendedInstanceRoom );
}

/////////////////////////////////////////////////////
/// Function: ScriptSetupLimitedInstanceRooms
/// Params: [in]pState
///
/////////////////////////////////////////////////////
int ScriptSetupLimitedInstanceRooms( lua_State* pState )
{
	if( lua_isstring( pState, 1 ) )
	{ 
		const char* setupFunc	= lua_tostring( pState, 1 ); 

		// call it
		if( setupFunc )
		{
			int result = 0;
			int errorFuncIndex;
			errorFuncIndex = script::GetErrorFuncIndex();

			lua_pop( pState, 1 );

			lua_getglobal( pState, setupFunc );

			result = lua_pcall( pState, 0, 1, errorFuncIndex );

			// LUA_ERRRUN --- a runtime error. 
			// LUA_ERRMEM --- memory allocation error. For such errors, Lua does not call the error handler function. 
			// LUA_ERRERR --- error while running the error handler function. 

			if( result == LUA_ERRRUN || result == LUA_ERRMEM || result == LUA_ERRERR )
			{
				DBGLOG( "*ERROR* Calling function '%s' failed\n", "ScriptSetupLimitedInstanceRooms" );
				DBGLOG( "\tLUA_TRACEBACK: %s\n", lua_tostring( pState, -1 ) );
				
				DBG_ASSERT_MSG( 0, "*ERROR* Calling function '%s' failed", "ScriptSetupLimitedInstanceRooms"  );

				return(1);
			}
		}
	}

	return(0);
}

/////////////////////////////////////////////////////
/// Function: ScriptAddLimitedInstanceRoom
/// Params: [in]pState
///
/////////////////////////////////////////////////////
int ScriptAddLimitedInstanceRoom( lua_State* pState )
{
	if( lua_istable( pState, 1 ) )
	{
		//int roomType			= static_cast<int>( script::LuaGetNumberFromTableItem( "roomType", 1 ) ); 
		const char* modelFile	= script::LuaGetStringFromTableItem( "modelFile", 1 ); 
		const char* b2dFile		= script::LuaGetStringFromTableItem( "physicsFileB2D", 1 );
		const char* typesFile	= script::LuaGetStringFromTableItem( "typesFile", 1 );

		Level& level = GameSystems::GetInstance()->GetLevel();
		level.AddLimitedInstanceRoom( modelFile, b2dFile, typesFile );
	}

	return(0);
}

/////////////////////////////////////////////////////
/// Function: ScriptSetupExtendedInstanceRooms
/// Params: [in]pState
///
/////////////////////////////////////////////////////
int ScriptSetupExtendedInstanceRooms( lua_State* pState )
{
	if( lua_isstring( pState, 1 ) )
	{ 
		const char* setupFunc	= lua_tostring( pState, 1 ); 

		// call it
		if( setupFunc )
		{
			int result = 0;
			int errorFuncIndex;
			errorFuncIndex = script::GetErrorFuncIndex();

			lua_getglobal( pState, setupFunc );

			result = lua_pcall( pState, 0, 1, errorFuncIndex );

			// LUA_ERRRUN --- a runtime error. 
			// LUA_ERRMEM --- memory allocation error. For such errors, Lua does not call the error handler function. 
			// LUA_ERRERR --- error while running the error handler function. 

			if( result == LUA_ERRRUN || result == LUA_ERRMEM || result == LUA_ERRERR )
			{
				DBGLOG( "*ERROR* Calling function '%s' failed\n", "ScriptSetupExtendedInstanceRooms" );
				DBGLOG( "\tLUA_TRACEBACK: %s\n", lua_tostring( pState, -1 ) );
								
				DBG_ASSERT_MSG( 0, "*ERROR* Calling function '%s' failed", "ScriptSetupExtendedInstanceRooms"  );

				return(1);
			}
		}
	}

	return(0);
}

/////////////////////////////////////////////////////
/// Function: ScriptAddExtendedInstanceRoom
/// Params: [in]pState
///
/////////////////////////////////////////////////////
int ScriptAddExtendedInstanceRoom( lua_State* pState )
{
	if( lua_istable( pState, 1 ) )
	{
		int roomType			= static_cast<int>( script::LuaGetNumberFromTableItem( "roomType", 1 ) ); 
		const char* modelFile	= script::LuaGetStringFromTableItem( "modelFile", 1 ); 
		const char* b2dFile		= script::LuaGetStringFromTableItem( "physicsFileB2D", 1 );
		const char* typesFile	= script::LuaGetStringFromTableItem( "typesFile", 1 );

		Level& level = GameSystems::GetInstance()->GetLevel();
		level.AddExtendedInstanceRoom( roomType, modelFile, b2dFile, typesFile );
	}

	return(0);
}
