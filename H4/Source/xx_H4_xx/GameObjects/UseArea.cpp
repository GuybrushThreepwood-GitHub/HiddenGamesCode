
/*===================================================================
	File: UseArea.cpp
	Game: H4

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"
#include "MathBase.h"
#include "CollisionBase.h"

#include "Level/LevelTypes.h"
#include "UseArea.h"

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
UseArea::UseArea( int objectId, lua_State* state, const collision::Sphere& s, float angle )
	: BaseObject( PHYSICSBASICID_UNKNOWN, PHYSICSCASTID_UNKNOWN, objectId, USE_AREA, state )
	, m_Sphere(s)
	, m_Angle(angle)
{
	std::memset( m_FunctionBuffer, 0, sizeof(char)*LUAFUNC_STRBUFFER );
}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
UseArea::~UseArea()
{

}

/////////////////////////////////////////////////////
/// Method: OnCreate
/// Params: None
///
/////////////////////////////////////////////////////
void UseArea::OnCreate()
{
	snprintf( m_FunctionBuffer, LUAFUNC_STRBUFFER, "UseArea%d_OnCreate", GetId() );
	if( script::LuaFunctionCheck( m_FunctionBuffer ) == 0 )
	{
		// function exists call it
		//void* pObject = reinterpret_cast<void *>(this);
		int err = 0;
		int errorFuncIndex;
		errorFuncIndex = script::GetErrorFuncIndex();

		lua_getglobal( GetScriptState(), m_FunctionBuffer );

		//lua_pushlightuserdata( GetScriptState(), pObject );
		err = lua_pcall( GetScriptState(), 0, 0, errorFuncIndex );

		// LUA_ERRRUN --- a runtime error. 
		// LUA_ERRMEM --- memory allocation error. For such errors, Lua does not call the error handler function. 
		// LUA_ERRERR --- error while running the error handler function. 

		if( err == LUA_ERRRUN || err == LUA_ERRMEM || err == LUA_ERRERR )
		{
			DBGLOG( "LUASCRIPTING: *ERROR* Calling script '%s' failed\n", m_FunctionBuffer );
			DBGLOG( "\tLUA_TRACEBACK: %s\n", lua_tostring( script::LuaScripting::GetState(), -1 ) );

			script::StackDump(script::LuaScripting::GetState());

			DBG_ASSERT_MSG( 0, "LUASCRIPTING: *ERROR* Calling script '%s' failed", m_FunctionBuffer );
		}
	}
}

/////////////////////////////////////////////////////
/// Method: OnActivate
/// Params: None
///
/////////////////////////////////////////////////////
void UseArea::OnActivate()
{
	snprintf( m_FunctionBuffer, LUAFUNC_STRBUFFER, "UseArea%d_OnActivate", GetId() );
	if( script::LuaFunctionCheck( m_FunctionBuffer ) == 0 )
	{
		// function exists call it
		//void* pObject = reinterpret_cast<void *>(this);
		int err = 0;
		int errorFuncIndex;
		errorFuncIndex = script::GetErrorFuncIndex();

		lua_getglobal( GetScriptState(), m_FunctionBuffer );

		//lua_pushlightuserdata( GetScriptState(), pObject );
		err = lua_pcall( GetScriptState(), 0, 0, errorFuncIndex );

		// LUA_ERRRUN --- a runtime error. 
		// LUA_ERRMEM --- memory allocation error. For such errors, Lua does not call the error handler function. 
		// LUA_ERRERR --- error while running the error handler function. 

		if( err == LUA_ERRRUN || err == LUA_ERRMEM || err == LUA_ERRERR )
		{
			DBGLOG( "LUASCRIPTING: *ERROR* Calling script '%s' failed\n", m_FunctionBuffer );
			DBGLOG( "\tLUA_TRACEBACK: %s\n", lua_tostring(script::LuaScripting::GetState(), -1 ) );

			script::StackDump(script::LuaScripting::GetState());

			DBG_ASSERT_MSG( 0, "LUASCRIPTING: *ERROR* Calling script '%s' failed", m_FunctionBuffer );
		}
	}
}

/////////////////////////////////////////////////////
/// Method: OnUse
/// Params: None
///
/////////////////////////////////////////////////////
void UseArea::OnUse()
{
	snprintf( m_FunctionBuffer, LUAFUNC_STRBUFFER, "UseArea%d_OnUse", GetId() );
	if( script::LuaFunctionCheck( m_FunctionBuffer ) == 0 )
	{
		// function exists call it
		//void* pObject = reinterpret_cast<void *>(this);
		int err = 0;
		int errorFuncIndex;
		errorFuncIndex = script::GetErrorFuncIndex();

		lua_getglobal( GetScriptState(), m_FunctionBuffer );

		//lua_pushlightuserdata( GetScriptState(), pObject );
		err = lua_pcall( GetScriptState(), 0, 0, errorFuncIndex );

		// LUA_ERRRUN --- a runtime error. 
		// LUA_ERRMEM --- memory allocation error. For such errors, Lua does not call the error handler function. 
		// LUA_ERRERR --- error while running the error handler function. 

		if( err == LUA_ERRRUN || err == LUA_ERRMEM || err == LUA_ERRERR )
		{
			DBGLOG( "LUASCRIPTING: *ERROR* Calling script '%s' failed\n", m_FunctionBuffer );
			DBGLOG( "\tLUA_TRACEBACK: %s\n", lua_tostring( script::LuaScripting::GetState(), -1 ) );

			script::StackDump(script::LuaScripting::GetState());

			DBG_ASSERT_MSG( 0, "LUASCRIPTING: *ERROR* Calling script '%s' failed", m_FunctionBuffer );
		}
	}
}
