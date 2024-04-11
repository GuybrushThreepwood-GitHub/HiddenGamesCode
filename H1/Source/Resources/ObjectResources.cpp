
/*===================================================================
	File: ObjectResources.cpp
	Game: AirCadets

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"
#include "MathBase.h"
#include "RenderBase.h"
#include "CollisionBase.h"
#include "ModelBase.h"
#include "ScriptBase.h"
#include "SoundBase.h"

#include "Resources/ObjectResources.h"

namespace
{
	typedef std::map< int, res::ObjectResourceStore > TIntObjectMap;
	typedef std::pair< int, res::ObjectResourceStore > TIntObjectPair;

	// maps
	TIntObjectMap ResourceMap;
}

/////////////////////////////////////////////////////
/// Method: CreateObjectResourceMap
/// Params: None
///
/////////////////////////////////////////////////////
void res::CreateObjectResourceMap()
{
	ResourceMap.clear();

#ifdef _DEBUG
	if( script::LuaFunctionCheck( "RegisterObjectResources" ) == 0 )
#endif // _DEBUG
	{
		int result = 0;

		int errorFuncIndex;
		errorFuncIndex = script::GetErrorFuncIndex();

		// get the function
		lua_getglobal( script::LuaScripting::GetState(), "RegisterObjectResources" );

		result = lua_pcall( script::LuaScripting::GetState(), 0, 1, errorFuncIndex );

		// LUA_ERRRUN --- a runtime error. 
		// LUA_ERRMEM --- memory allocation error. For such errors, Lua does not call the error handler function. 
		// LUA_ERRERR --- error while running the error handler function. 

		if( result == LUA_ERRRUN || result == LUA_ERRMEM || result == LUA_ERRERR )
		{
			DBGLOG( "RESOURCES: *ERROR* Calling function '%s' failed\n", "RegisterObjectResources" );
			DBGLOG( "\tLUA_TRACEBACK: %s\n", lua_tostring( script::LuaScripting::GetState(), -1 ) );
			
			DBG_ASSERT_MSG( 0, "RESOURCES: *ERROR* Calling function '%s' failed", "RegisterObjectResources" );
			return;
		}

		// should be a table of tables
		if( lua_istable( script::LuaScripting::GetState(), -1 ) )
		{
			int i=0;
			int n = luaL_len( script::LuaScripting::GetState(), -1 );

			// go through all the tables in this table
			for( i = 1; i <= n; ++i )
			{
				lua_rawgeti( script::LuaScripting::GetState(), -1, i );
				if( lua_istable( script::LuaScripting::GetState(), -1 ) )
				{
					int innerCount = luaL_len( script::LuaScripting::GetState(), -1 );
					
					if( innerCount == 10 )
					{
						int paramIndex = 1;

						// id first
						lua_rawgeti( script::LuaScripting::GetState(), -1, paramIndex++ );
						int resId = static_cast<int>( lua_tonumber( script::LuaScripting::GetState(), -1 ) );

						lua_pop( script::LuaScripting::GetState(), 1 );

						// filename
						lua_rawgeti( script::LuaScripting::GetState(), -1, paramIndex++ );
						const char* resName = lua_tostring( script::LuaScripting::GetState(), -1 );

						lua_pop( script::LuaScripting::GetState(), 1 );

						// destroy flag
						lua_rawgeti( script::LuaScripting::GetState(), -1, paramIndex++ );
						bool destroyFlag = static_cast<bool>( lua_toboolean( script::LuaScripting::GetState(), -1 ) != 0 );

						lua_pop( script::LuaScripting::GetState(), 1 );

						// low
						lua_rawgeti( script::LuaScripting::GetState(), -1, paramIndex++ );
						int lowResId = static_cast<int>( lua_tonumber( script::LuaScripting::GetState(), -1 ) );

						lua_pop( script::LuaScripting::GetState(), 1 );

						// medium
						lua_rawgeti( script::LuaScripting::GetState(), -1, paramIndex++ );
						int mediumResId = static_cast<int>( lua_tonumber( script::LuaScripting::GetState(), -1 ) );

						lua_pop( script::LuaScripting::GetState(), 1 );

						// high
						lua_rawgeti( script::LuaScripting::GetState(), -1, paramIndex++ );
						int highResId = static_cast<int>( lua_tonumber( script::LuaScripting::GetState(), -1 ) );

						lua_pop( script::LuaScripting::GetState(), 1 );

						// disable distance
						lua_rawgeti( script::LuaScripting::GetState(), -1, paramIndex++ );
						float disableDist = static_cast<float>( lua_tonumber( script::LuaScripting::GetState(), -1 ) );

						lua_pop( script::LuaScripting::GetState(), 1 );

						// low distance
						lua_rawgeti( script::LuaScripting::GetState(), -1, paramIndex++ );
						float lowDist = static_cast<float>( lua_tonumber( script::LuaScripting::GetState(), -1 ) );

						lua_pop( script::LuaScripting::GetState(), 1 );

						// medium distance
						lua_rawgeti( script::LuaScripting::GetState(), -1, paramIndex++ );
						float mediumDist = static_cast<float>( lua_tonumber( script::LuaScripting::GetState(), -1 ) );

						lua_pop( script::LuaScripting::GetState(), 1 );

						// high distance
						lua_rawgeti( script::LuaScripting::GetState(), -1, paramIndex++ );
						float highDist = static_cast<float>( lua_tonumber( script::LuaScripting::GetState(), -1 ) );

						lua_pop( script::LuaScripting::GetState(), 1 );
						ObjectResourceStore resourceInfo;
						resourceInfo.resId = resId;
						resourceInfo.physicsFile = resName;
						resourceInfo.destroyFlag = destroyFlag;
						resourceInfo.lowModelId = lowResId;
						resourceInfo.mediumModelId = mediumResId;
						resourceInfo.highModelId = highResId;

						resourceInfo.disableDistance = disableDist;
						resourceInfo.lowDistance = lowDist;
						resourceInfo.mediumDistance = mediumDist;
						resourceInfo.highDistance = highDist;

						ResourceMap.insert( TIntObjectPair( resId, resourceInfo ) );
					}
				}
				lua_pop( script::LuaScripting::GetState(), 1 );	
			}
			lua_pop( script::LuaScripting::GetState(), 1 );
		}
	}
}

/////////////////////////////////////////////////////
/// Method: ClearObjectResources
/// Params: None
///
/////////////////////////////////////////////////////
void res::ClearObjectResources()
{
	TIntObjectMap::iterator resIt;
	
	resIt = ResourceMap.begin();

	while( resIt != ResourceMap.end() )
	{
		//snd::RemoveSound( resIt->second.bufferId );
		resIt++;
	}

	ResourceMap.clear();
}

/////////////////////////////////////////////////////
/// Method: GetObjectResource
/// Params: [in]index
///
/////////////////////////////////////////////////////
const res::ObjectResourceStore* res::GetObjectResource( int index )
{
	TIntObjectMap::iterator resIt;
	
	resIt = ResourceMap.find( index );

	if( resIt != ResourceMap.end() )
		return( &resIt->second );

	return(0);
}
