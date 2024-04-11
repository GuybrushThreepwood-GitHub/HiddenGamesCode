
/*===================================================================
	File: SoundResources.cpp
	Game: H8

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"
#include "MathBase.h"
#include "RenderBase.h"
#include "CollisionBase.h"
#include "ModelBase.h"
#include "ScriptBase.h"
#include "SoundBase.h"

#include "Resources/SoundResources.h"

namespace
{
	typedef std::map< int, res::SoundResourceStore > TIntSoundMap;
	typedef std::pair< int, res::SoundResourceStore > TIntSoundPair;

	// maps
	TIntSoundMap ResourceMap;
}

/////////////////////////////////////////////////////
/// Method: CreateSoundResourceMap
/// Params: None
///
/////////////////////////////////////////////////////
void res::CreateSoundResourceMap()
{
	ResourceMap.clear();
	snd::Sound sndLoader;

#ifdef _DEBUG
	if( script::LuaFunctionCheck( "RegisterSoundResources" ) == 0 )
#endif // _DEBUG
	{
		int result = 0;

		int errorFuncIndex;
		errorFuncIndex = script::GetErrorFuncIndex();

		// get the function
		lua_getglobal( script::LuaScripting::GetState(), "RegisterSoundResources" );

		result = lua_pcall( script::LuaScripting::GetState(), 0, 1, errorFuncIndex );

		// LUA_ERRRUN --- a runtime error. 
		// LUA_ERRMEM --- memory allocation error. For such errors, Lua does not call the error handler function. 
		// LUA_ERRERR --- error while running the error handler function. 

		if( result == LUA_ERRRUN || result == LUA_ERRMEM || result == LUA_ERRERR )
		{
			DBGLOG( "RESOURCES: *ERROR* Calling function '%s' failed\n", "RegisterSoundResources" );
			DBGLOG( "\tLUA_TRACEBACK: %s\n", lua_tostring( script::LuaScripting::GetState(), -1 ) );
			
			DBG_ASSERT_MSG( 0, "RESOURCES: *ERROR* Calling function '%s' failed", "RegisterSoundResources" );
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
					
					if( innerCount == 2 )
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

						SoundResourceStore resourceInfo;
						resourceInfo.sndFile = resName;

						ResourceMap.insert( TIntSoundPair( resId, resourceInfo ) );
					}
				}
				lua_pop( script::LuaScripting::GetState(), 1 );	
			}
			lua_pop( script::LuaScripting::GetState(), 1 );
		}
	}
}

/////////////////////////////////////////////////////
/// Method: ClearSoundResources
/// Params: None
///
/////////////////////////////////////////////////////
void res::ClearSoundResources()
{
	TIntSoundMap::iterator resIt;
	
	resIt = ResourceMap.begin();

	while( resIt != ResourceMap.end() )
	{
		//snd::RemoveSound( resIt->second.bufferId );
		resIt++;
	}

	ResourceMap.clear();
}

/////////////////////////////////////////////////////
/// Method: GetSoundResource
/// Params: [in]index
///
/////////////////////////////////////////////////////
const char* res::GetSoundResource( int index )
{
	TIntSoundMap::iterator resIt;
	
	resIt = ResourceMap.find( index );

	if( resIt != ResourceMap.end() )
		return( resIt->second.sndFile );

	return(0);
}
