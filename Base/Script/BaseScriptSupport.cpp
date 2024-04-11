
/*===================================================================
	File: BaseScriptSupport.cpp
	Library: Script

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"
#include "MathBase.h"
#include "RenderBase.h"
#include "ScriptBase.h"

#ifdef BASE_SUPPORT_SCRIPTING

/////////////////////////////////////////////////////
/// Function: ScriptLoadScript
/// Params: [in]pState
///
/////////////////////////////////////////////////////
int ScriptLoadScript( lua_State* pState )
{
#ifdef _DEBUG
	CHECK_LUASTATE()
#endif // _DEBUG

	script::LoadScript( lua_tostring( pState, 1 ) );

	return(0);
}

/////////////////////////////////////////////////////
/// Function: ScriptRandomNumber
/// Params: [in]pState
///
/////////////////////////////////////////////////////
int ScriptRandomNumber( lua_State* pState )
{
#ifdef _DEBUG
	CHECK_LUASTATE()
#endif // _DEBUG

	int minVal = static_cast<int>( lua_tonumber( pState, 1 ) );
	int maxVal = static_cast<int>( lua_tonumber( pState, 2 ) );

	int number = math::RandomNumber( minVal, maxVal );

	lua_pushnumber( pState, number );
	return(1);
}

#endif // BASE_SUPPORT_SCRIPTING