
/*===================================================================
	File: Leaderboards.cpp
	Library: Support

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"
#include "ScriptBase.h"

#include "Support/Leaderboards.h"

namespace
{
	struct lstr
	{
		bool operator() (const char* s1, const char* s2) const
		{
			return( strcmp(s1,s2) < 0 );
		}
	};

	typedef std::map< const char*, support::LeaderboardStore*, lstr > TCharLeaderboardMap;
	typedef std::pair< const char*, support::LeaderboardStore* > TCharLeaderboardPair;

	// maps
	TCharLeaderboardMap ResourceMap;
}

/////////////////////////////////////////////////////
/// Method: AppendLeaderboards
/// Params: None
///
/////////////////////////////////////////////////////
void support::AppendLeaderboards( const char* functionCall )
{
	DBG_ASSERT( functionCall != 0 );
	
	ResourceMap.clear();

#ifdef _DEBUG
	if( script::LuaFunctionCheck( functionCall ) == 0 )
#endif // _DEBUG
	{
		int result = 0;

		int errorFuncIndex;
		errorFuncIndex = script::GetErrorFuncIndex();

		// get the function
		lua_getglobal( script::LuaScripting::GetState(), functionCall );

		result = lua_pcall( script::LuaScripting::GetState(), 0, 1, errorFuncIndex );

		// LUA_ERRRUN --- a runtime error. 
		// LUA_ERRMEM --- memory allocation error. For such errors, Lua does not call the error handler function. 
		// LUA_ERRERR --- error while running the error handler function. 

		if( result == LUA_ERRRUN || result == LUA_ERRMEM || result == LUA_ERRERR )
		{
			DBGLOG( "LEADERBOARDS: *ERROR* Calling function '%s' failed\n", functionCall );
			DBGLOG( "\tLUA_TRACEBACK: %s\n", lua_tostring( script::LuaScripting::GetState(), -1 ) );
			
			DBG_ASSERT_MSG( 0, "LEADERBOARDS: *ERROR* Calling function '%s' failed", functionCall );
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
					
					if( innerCount == 3 )
					{
						int paramIndex = 1;

						// name
						lua_rawgeti( script::LuaScripting::GetState(), -1, paramIndex++ );
						const char* name = lua_tostring( script::LuaScripting::GetState(), -1 );

						lua_pop( script::LuaScripting::GetState(), 1 );

						// type
						lua_rawgeti( script::LuaScripting::GetState(), -1, paramIndex++ );
						support::EScoreType type =  static_cast<support::EScoreType>( lua_tointeger( script::LuaScripting::GetState(), -1 ) );

						lua_pop( script::LuaScripting::GetState(), 1 );

						// order
						lua_rawgeti( script::LuaScripting::GetState(), -1, paramIndex++ );
						support::EScoreOrder order =  static_cast<support::EScoreOrder>( lua_tointeger( script::LuaScripting::GetState(), -1 ) );
						
						lua_pop( script::LuaScripting::GetState(), 1 );
						
						// see if this name already exists
						if( support::GetLeaderboard( name ) == 0 )
						{
							LeaderboardStore* resourceInfo = new LeaderboardStore;
							DBG_ASSERT( resourceInfo !=0 );
							
							snprintf( resourceInfo->name, MAX_LEADERBOARD_NAME, "%s", name );
							resourceInfo->type				= type;
							resourceInfo->order				= order;

							ResourceMap.insert( TCharLeaderboardPair( &resourceInfo->name[0], resourceInfo ) );
						}
						else
						{
							// moan but carry on
							DBGLOG( "*WARNING* Leaderboard %s is already within the list", name );
						}
					}
					else
					{
						DBG_ASSERT(0);
					}
				}
				lua_pop( script::LuaScripting::GetState(), 1 );	
			}
			lua_pop( script::LuaScripting::GetState(), 1 );
		}
	}
}

/////////////////////////////////////////////////////
/// Method: ClearLeaderboards
/// Params: None
///
/////////////////////////////////////////////////////
void support::ClearLeaderboards()
{
	TCharLeaderboardMap::iterator resIt;

	resIt = ResourceMap.begin();

	while( resIt != ResourceMap.end() )
	{
		delete resIt->second;
		
		// next
		resIt++;
	}

	ResourceMap.clear();
}

/////////////////////////////////////////////////////
/// Method: GetLeaderboard
/// Params: [in]index
///
/////////////////////////////////////////////////////
const support::LeaderboardStore* support::GetLeaderboard( const char *name )
{
	TCharLeaderboardMap::iterator resIt;
	
	resIt = ResourceMap.find( name );

	if( resIt != ResourceMap.end() )
		return( resIt->second );

	return(0);
}

