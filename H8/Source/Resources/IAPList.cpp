
/*===================================================================
	File: IAPList.cpp
	Game: H8

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"
#include "MathBase.h"
#include "RenderBase.h"
#include "CollisionBase.h"
#include "ModelBase.h"
#include "ScriptBase.h"

#include "Resources/IAPList.h"

#include "ScriptAccess/ScriptDataHolder.h"
#include "ScriptAccess/ScriptAccess.h"

namespace
{
	struct lstr
	{
		bool operator() (const char* s1, const char* s2) const
		{
			return( (strcmp( s1, s2 ) < 0) );
		}
	};


	typedef std::map< int, const char*> TIntStringMap;
	typedef std::pair< int, const char*> TIntStringPair;

	// maps
	TIntStringMap ResourceMap;
}

/////////////////////////////////////////////////////
/// Method: CreateIAPList
/// Params: None
///
/////////////////////////////////////////////////////
void res::CreateIAPList()
{
	ResourceMap.clear();

#ifdef _DEBUG
	if( script::LuaFunctionCheck( "RegisterIAPList" ) == 0 )
#endif // _DEBUG
	{
		int result = 0;

		int errorFuncIndex;
		errorFuncIndex = script::GetErrorFuncIndex();

		// get the function
		lua_getglobal( script::LuaScripting::GetState(), "RegisterIAPList" );

		result = lua_pcall( script::LuaScripting::GetState(), 0, 1, errorFuncIndex );

		// LUA_ERRRUN --- a runtime error. 
		// LUA_ERRMEM --- memory allocation error. For such errors, Lua does not call the error handler function. 
		// LUA_ERRERR --- error while running the error handler function. 

		if( result == LUA_ERRRUN || result == LUA_ERRMEM || result == LUA_ERRERR )
		{
			DBGLOG( "RESOURCES: *ERROR* Calling function '%s' failed\n", "RegisterIAPList" );
			DBGLOG( "\tLUA_TRACEBACK: %s\n", lua_tostring( script::LuaScripting::GetState(), -1 ) );
			
			DBG_ASSERT_MSG( 0, "RESOURCES: *ERROR* Calling function '%s' failed", "RegisterIAPList" );
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

						// string
						lua_rawgeti( script::LuaScripting::GetState(), -1, paramIndex++ );
						const char* scriptString = lua_tostring( script::LuaScripting::GetState(), -1 );

						lua_pop( script::LuaScripting::GetState(), 1 );

						ResourceMap.insert( TIntStringPair( resId, scriptString ) );
					}
				}
				lua_pop( script::LuaScripting::GetState(), 1 );	
			}
			lua_pop( script::LuaScripting::GetState(), 1 );
		}
	}
}

/////////////////////////////////////////////////////
/// Method: ClearIAPList
/// Params: None
///
/////////////////////////////////////////////////////
void res::ClearIAPList()
{
	ResourceMap.clear();
}


/////////////////////////////////////////////////////
/// Method: GetIAPString
/// Params: [in]index
///
/////////////////////////////////////////////////////
const char* res::GetIAPString( int index )
{
	TIntStringMap::iterator it;
	it = ResourceMap.find( index );

	// did iterator not hit the end
	if( it != ResourceMap.end() )
	{
		return( it->second );
	}

	DBG_ASSERT_MSG( 0, "*ERROR* Could not find iap index in map" );
	return(0);
}

/////////////////////////////////////////////////////
/// Method: GetIAPIndex
/// Params: [in]purchaseId
///
/////////////////////////////////////////////////////
int res::GetIAPIndex( const char* purchaseId )
{
	TIntStringMap::iterator it;
	it = ResourceMap.begin();
	
	// did iterator not hit the end
	while( it != ResourceMap.end() )
	{
		if( strcmp( purchaseId, it->second ) == 0 )
			return( it->first );
		
		// next
		it++;
	}
	
	DBG_ASSERT_MSG( 0, "*ERROR* Could not find iap index in map" );
	return(0);
}

/////////////////////////////////////////////////////
/// Method: GetIAPAsList
/// Params: None
///
/////////////////////////////////////////////////////
std::list<support::TPurchase> res::GetIAPAsList()
{
	TIntStringMap::iterator it;
	it = ResourceMap.begin();
	std::list<support::TPurchase> purchaseList;

	// did iterator not hit the end
	while( it != ResourceMap.end() )
	{
		support::TPurchase item;
		item.isValid = false;
		snprintf( item.purchaseId, MAX_PURCHASE_ID_LEN, "%s", it->second );

		purchaseList.push_back( item );

		// next
		it++;
	}

	return purchaseList;
}
