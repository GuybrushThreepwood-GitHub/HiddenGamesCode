
/*===================================================================
	File: TextureResources.cpp
	Game: AirCadets

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"
#include "MathBase.h"
#include "RenderBase.h"
#include "CollisionBase.h"
#include "ModelBase.h"
#include "ScriptBase.h"

#include "H1.h"

#include "Resources/ResourceHelper.h"
#include "Resources/TextureResources.h"

#include "ScriptAccess/ScriptDataHolder.h"
#include "ScriptAccess/ScriptAccess.h"

namespace
{
	struct lstr
	{
		bool operator() (std::string s1, std::string s2) const
		{
			return( (s1.compare(s2)) < 0);
		}
	};

	// texture pointer store
	struct TextureStore
	{
		int refCount;
		GLuint texId;
		GLuint* pTexId;
	};

	typedef std::map< std::string, TextureStore, struct lstr> TNameTextureMap;
	typedef std::pair< std::string, TextureStore> TNameTexturePair;

	typedef std::map< int, res::TextureResourceStore > TIntTextureResourceMap;
	typedef std::pair< int, res::TextureResourceStore > TIntTextureResourcePair;

	// maps
	TIntTextureResourceMap ResourceMap;
	TNameTextureMap TextureMap;
}

/////////////////////////////////////////////////////
/// Method: CreateTextureResourceMap
/// Params: None
///
/////////////////////////////////////////////////////
void res::CreateTextureResourceMap()
{
	ResourceMap.clear();

#ifdef _DEBUG
	if( script::LuaFunctionCheck( "RegisterTextureResources" ) == 0 )
#endif // _DEBUG
	{
		int result = 0;

		int errorFuncIndex;
		errorFuncIndex = script::GetErrorFuncIndex();

		// get the function
		lua_getglobal( script::LuaScripting::GetState(), "RegisterTextureResources" );

		result = lua_pcall( script::LuaScripting::GetState(), 0, 1, errorFuncIndex );

		// LUA_ERRRUN --- a runtime error. 
		// LUA_ERRMEM --- memory allocation error. For such errors, Lua does not call the error handler function. 
		// LUA_ERRERR --- error while running the error handler function. 

		if( result == LUA_ERRRUN || result == LUA_ERRMEM || result == LUA_ERRERR )
		{
			DBGLOG( "RESOURCES: *ERROR* Calling function '%s' failed\n", "RegisterTextureResources" );
			DBGLOG( "\tLUA_TRACEBACK: %s\n", lua_tostring( script::LuaScripting::GetState(), -1 ) );
			
			DBG_ASSERT_MSG( 0, "RESOURCES: *ERROR* Calling function '%s' failed", "RegisterTextureResources" );
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
					
					if( innerCount == 5 )
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

						// preload
						lua_rawgeti( script::LuaScripting::GetState(), -1, paramIndex++ );
						bool preload = ( lua_toboolean( script::LuaScripting::GetState(), -1 ) != 0 );

						lua_pop( script::LuaScripting::GetState(), 1 );

						// magFilter
						lua_rawgeti( script::LuaScripting::GetState(), -1, paramIndex++ );
						GLenum magFilter =  static_cast<int>( lua_tonumber( script::LuaScripting::GetState(), -1 ) );

						lua_pop( script::LuaScripting::GetState(), 1 );

						// minFilter
						lua_rawgeti( script::LuaScripting::GetState(), -1, paramIndex++ );
						GLenum minFilter =  static_cast<int>( lua_tonumber( script::LuaScripting::GetState(), -1 ) );

						lua_pop( script::LuaScripting::GetState(), 1 );

						// load the texture

						bool mipmaps = false;
						if( minFilter != GL_NEAREST &&
							minFilter != GL_LINEAR )
							mipmaps = true;

						TextureResourceStore resourceInfo;
						resourceInfo.resId				= resId;
						resourceInfo.resourceFilename	= resName;
						resourceInfo.magFilter			= magFilter;
						resourceInfo.minFilter			= minFilter;
						resourceInfo.preLoad			= preload;
						if( preload )
							resourceInfo.texId			= res::LoadTextureName( resName, mipmaps, magFilter, minFilter, core::app::GetLowPowerMode()  );
						else
							resourceInfo.texId			= renderer::INVALID_OBJECT;

						ResourceMap.insert( TIntTextureResourcePair( resId, resourceInfo ) );
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
/// Method: ClearTextureResources
/// Params: None
///
/////////////////////////////////////////////////////
void res::ClearTextureResources()
{
	TIntTextureResourceMap::iterator resIt;

	resIt = ResourceMap.begin();

	while( resIt != ResourceMap.end() )
	{
		if( resIt->second.texId != renderer::INVALID_OBJECT )
			glDeleteTextures( 1, &resIt->second.texId );

		resIt++;
	}

	ResourceMap.clear();
	renderer::ClearTextureMap();
}

/////////////////////////////////////////////////////
/// Method: GetTextureResource
/// Params: [in]index
///
/////////////////////////////////////////////////////
const res::TextureResourceStore* res::GetTextureResource( int index )
{
	TIntTextureResourceMap::iterator resIt;
	
	resIt = ResourceMap.find( index );

	if( resIt != ResourceMap.end() )
		return( &resIt->second );

	return(0);
}

/////////////////////////////////////////////////////
/// Method: GetTextureResourceMap
/// Params: None
///
/////////////////////////////////////////////////////
const std::map< int, res::TextureResourceStore >& res::GetTextureResourceMap()
{
	return ResourceMap;
}

/////////////////////////////////////////////////////
/// Method: LoadHiResTexture
/// Params: [in]index
///
/////////////////////////////////////////////////////
int res::LoadTexture( int index )
{
	file::TFile fileData;
	
	TextureResourceStore* pResourceInfo = 0;

	TIntTextureResourceMap::iterator resIt;
	resIt = ResourceMap.find( index );
	if( resIt != ResourceMap.end() )
	{
		pResourceInfo = &resIt->second;

		file::CreateFileStructure( pResourceInfo->resourceFilename, &fileData );

		bool mipmaps = false;
		if( pResourceInfo->minFilter != GL_NEAREST &&
			pResourceInfo->minFilter != GL_LINEAR )
				mipmaps = true;

		pResourceInfo->texId = res::LoadTextureName( pResourceInfo->resourceFilename, mipmaps, pResourceInfo->magFilter, pResourceInfo->minFilter, core::app::GetLowPowerMode()  );

		return(0);
	}
	
	return(-1);
}


