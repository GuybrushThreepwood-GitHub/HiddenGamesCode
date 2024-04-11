
/*===================================================================
	File: SpriteResources.cpp
	Game: Cabby

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"
#include "MathBase.h"
#include "RenderBase.h"
#include "CollisionBase.h"
#include "ModelBase.h"
#include "ScriptBase.h"

#include "Resources/SpriteResources.h"

namespace
{
	typedef std::map< int, res::SpriteResourceStore > TIntSpriteMap;
	typedef std::pair< int, res::SpriteResourceStore > TIntSpritePair;

	// maps
	TIntSpriteMap ResourceMap;
}

/////////////////////////////////////////////////////
/// Method: CreateSpriteResourceMap
/// Params: None
///
/////////////////////////////////////////////////////
void res::CreateSpriteResourceMap()
{
	ResourceMap.clear();

#ifdef _DEBUG
	if( script::LuaFunctionCheck( "RegisterSpriteResources" ) == 0 )
#endif // _DEBUG
	{
		int result = 0;

		int errorFuncIndex;
		errorFuncIndex = script::GetErrorFuncIndex();

		// get the function
		lua_getglobal( script::LuaScripting::GetState(), "RegisterSpriteResources" );

		result = lua_pcall( script::LuaScripting::GetState(), 0, 1, errorFuncIndex );

		// LUA_ERRRUN --- a runtime error. 
		// LUA_ERRMEM --- memory allocation error. For such errors, Lua does not call the error handler function. 
		// LUA_ERRERR --- error while running the error handler function. 

		if( result == LUA_ERRRUN || result == LUA_ERRMEM || result == LUA_ERRERR )
		{
			DBGLOG( "RESOURCES: *ERROR* Calling function '%s' failed\n", "RegisterSpriteResources" );
			DBGLOG( "\tLUA_TRACEBACK: %s\n", lua_tostring( script::LuaScripting::GetState(), -1 ) );
			
			DBG_ASSERT_MSG( 0, "RESOURCES: *ERROR* Calling function '%s' failed", "RegisterSpriteResources" );
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
					
					if( innerCount == 8 )
					{
						int paramIndex = 1;

						// id first
						lua_rawgeti( script::LuaScripting::GetState(), -1, paramIndex++ );
						int resId = static_cast<int>( lua_tonumber( script::LuaScripting::GetState(), -1 ) );

						lua_pop( script::LuaScripting::GetState(), 1 );

						// texture resource id
						lua_rawgeti( script::LuaScripting::GetState(), -1, paramIndex++ );
						int textureResId = static_cast<int>( lua_tonumber( script::LuaScripting::GetState(), -1 ) );

						lua_pop( script::LuaScripting::GetState(), 1 );

						// u
						lua_rawgeti( script::LuaScripting::GetState(), -1, paramIndex++ );
						float pixelPosX =  static_cast<float>( lua_tonumber( script::LuaScripting::GetState(), -1 ) );

						lua_pop( script::LuaScripting::GetState(), 1 );

						// v
						lua_rawgeti( script::LuaScripting::GetState(), -1, paramIndex++ );
						float pixelPosY =  static_cast<float>( lua_tonumber( script::LuaScripting::GetState(), -1 ) );

						lua_pop( script::LuaScripting::GetState(), 1 );

						// w
						lua_rawgeti( script::LuaScripting::GetState(), -1, paramIndex++ );
						float pixelWidth =  static_cast<float>( lua_tonumber( script::LuaScripting::GetState(), -1 ) );

						lua_pop( script::LuaScripting::GetState(), 1 );

						// h
						lua_rawgeti( script::LuaScripting::GetState(), -1, paramIndex++ );
						float pixelHeight =  static_cast<float>( lua_tonumber( script::LuaScripting::GetState(), -1 ) );

						lua_pop( script::LuaScripting::GetState(), 1 );

						// texture width
						lua_rawgeti( script::LuaScripting::GetState(), -1, paramIndex++ );
						float texWidth =  static_cast<float>( lua_tonumber( script::LuaScripting::GetState(), -1 ) );

						lua_pop( script::LuaScripting::GetState(), 1 );

						// texture height
						lua_rawgeti( script::LuaScripting::GetState(), -1, paramIndex++ );
						float texHeight =  static_cast<float>( lua_tonumber( script::LuaScripting::GetState(), -1 ) );

						lua_pop( script::LuaScripting::GetState(), 1 );

						DBG_ASSERT( texWidth != 0 );
						DBG_ASSERT( texHeight != 0 );

						SpriteResourceStore resourceInfo;
						resourceInfo.textureResId = textureResId;
						resourceInfo.uOffset = pixelPosX / texWidth;
						resourceInfo.vOffset = pixelPosY / texHeight;
						resourceInfo.w = pixelWidth / texWidth;
						resourceInfo.h = pixelHeight / texHeight;

						ResourceMap.insert( TIntSpritePair( resId, resourceInfo ) );
					}
				}
				lua_pop( script::LuaScripting::GetState(), 1 );	
			}
			lua_pop( script::LuaScripting::GetState(), 1 );
		}
	}
}

/////////////////////////////////////////////////////
/// Method: ClearSpriteResources
/// Params: None
///
/////////////////////////////////////////////////////
void res::ClearSpriteResources()
{
	TIntSpriteMap::iterator resIt;
	
	resIt = ResourceMap.begin();

	while( resIt != ResourceMap.end() )
	{
		//renderer::RemoveTexture( );
		resIt++;
	}

	ResourceMap.clear();
}

/////////////////////////////////////////////////////
/// Method: GetSpriteResource
/// Params: [in]index
///
/////////////////////////////////////////////////////
const res::SpriteResourceStore* res::GetSpriteResource( int index )
{
	TIntSpriteMap::iterator resIt;
	
	resIt = ResourceMap.find( index );

	if( resIt != ResourceMap.end() )
		return( &resIt->second );

	return(0);
}
