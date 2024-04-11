
/*===================================================================
	File: FontResources.cpp
	Game: Cabby

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"
#include "MathBase.h"
#include "RenderBase.h"
#include "CollisionBase.h"
#include "ModelBase.h"
#include "ScriptBase.h"

#include "Resources/FontResources.h"

namespace
{
	typedef std::map< int, res::FontResourceStore > TIntFontMap;
	typedef std::pair< int, res::FontResourceStore > TIntFontPair;

	// maps
	std::vector<res::FontResourceStore> ResourceVector;
	
	const int MIN_FONT_SIZE = 12;
}

/////////////////////////////////////////////////////
/// Method: CreateFontResourceMap
/// Params: None
///
/////////////////////////////////////////////////////
void res::CreateFontResourceMap()
{
	ResourceVector.clear();

#ifdef _DEBUG
	if( script::LuaFunctionCheck( "RegisterFontResources" ) == 0 )
#endif // _DEBUG
	{
		int result = 0;

		int errorFuncIndex;
		errorFuncIndex = script::GetErrorFuncIndex();

		// get the function
		lua_getglobal( script::LuaScripting::GetState(), "RegisterFontResources" );

		result = lua_pcall( script::LuaScripting::GetState(), 0, 1, errorFuncIndex );

		// LUA_ERRRUN --- a runtime error. 
		// LUA_ERRMEM --- memory allocation error. For such errors, Lua does not call the error handler function. 
		// LUA_ERRERR --- error while running the error handler function. 

		if( result == LUA_ERRRUN || result == LUA_ERRMEM || result == LUA_ERRERR )
		{
			DBGLOG( "RESOURCES: *ERROR* Calling function '%s' failed\n", "RegisterFontResources" );
			DBGLOG( "\tLUA_TRACEBACK: %s\n", lua_tostring( script::LuaScripting::GetState(), -1 ) );
			
			DBG_ASSERT_MSG( 0, "RESOURCES: *ERROR* Calling function '%s' failed", "RegisterFontResources" );
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
						FontResourceStore resourceInfo;

						// id first
						lua_rawgeti( script::LuaScripting::GetState(), -1, paramIndex++ );
						resourceInfo.fontId = static_cast<int>( lua_tonumber( script::LuaScripting::GetState(), -1 ) );

						lua_pop( script::LuaScripting::GetState(), 1 );

						// filename
						lua_rawgeti( script::LuaScripting::GetState(), -1, paramIndex++ );
						resourceInfo.resName = lua_tostring( script::LuaScripting::GetState(), -1 );

						lua_pop( script::LuaScripting::GetState(), 1 );

						resourceInfo.fontRender = 0;

						ResourceVector.push_back( resourceInfo );
					}
				}
				lua_pop( script::LuaScripting::GetState(), 1 );	
			}
			lua_pop( script::LuaScripting::GetState(), 1 );
		}
	}
}

/////////////////////////////////////////////////////
/// Method: ClearFontResources
/// Params: None
///
/////////////////////////////////////////////////////
void res::ClearFontResources()
{
	std::vector<res::FontResourceStore>::iterator resIt;
	
	resIt = ResourceVector.begin();

	while( resIt != ResourceVector.end() )
	{
		if( resIt->fontRender != 0 )
			delete resIt->fontRender;

		resIt++;
	}

	ResourceVector.clear();
}

/////////////////////////////////////////////////////
/// Method: GetFontResource
/// Params: [in]index, [in]fontSize, [in]dropShadow, [in]colour
///
/////////////////////////////////////////////////////
const res::FontResourceStore* res::GetFontResource( int index, int fontSize, bool dropShadow, const math::Vec4Lite& colour, bool fixedWidth )
{
	std::vector<res::FontResourceStore>::iterator resIt = ResourceVector.begin();
	
	if( fontSize < MIN_FONT_SIZE )
		fontSize = MIN_FONT_SIZE;
	
	// go through ALL resource
	while( resIt != ResourceVector.end() )
	{
		if( resIt->fontId == index )
		{
			if( resIt->fontRender != 0 )
			{
				math::Vec2 dims = resIt->fontRender->GetDimensions();

				// everything must match
				if( static_cast<int>(dims.Width) == fontSize )
				{
					return &(*resIt);
				}
			}
		}

		// next
		resIt++;
	}

	// create new font?

	// find original index
	resIt = ResourceVector.begin();

	while( resIt != ResourceVector.end() )
	{
		if( resIt->fontId == index )
		{
			FontResourceStore resourceInfo;
			resourceInfo.fontRender = new renderer::FreetypeFont;

			math::Vec2 finalFontSize(static_cast<float>(fontSize), static_cast<float>(fontSize) );

			resourceInfo.fontRender->SetBlockFillFlag(true);

			if (resourceInfo.fontRender->Load(resIt->resName, 0, 0, finalFontSize, colour, colour, fixedWidth))
			{
				DBG_ASSERT_MSG( 0, "*ERROR* Font file '%s' not found", resIt->resName );
				return 0;
			}

			// insert
			resourceInfo.fontId = resIt->fontId;
			resourceInfo.resName = resIt->resName;

			ResourceVector.push_back( resourceInfo );

			return &ResourceVector[ResourceVector.size()-1];
		}

		resIt++;
	}

	return 0;
}