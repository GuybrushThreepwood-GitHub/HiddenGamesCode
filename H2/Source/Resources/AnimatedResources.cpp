
/*===================================================================
	File: AnimatedResources.cpp
	Game: Cabby

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"
#include "MathBase.h"
#include "RenderBase.h"
#include "CollisionBase.h"
#include "ModelBase.h"
#include "ScriptBase.h"

#include "Resources/AnimatedResources.h"

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

	// model pointer store
	struct ModelStore
	{
		int refCount;
		mdl::ModelHGA* pModel;
	};

	typedef std::map< std::string, ModelStore, struct lstr> TNameModelMap;
	typedef std::pair< std::string, ModelStore> TNameModelPair;

	// model info store
	struct AnimatedResourceStore
	{
		const char* scriptFile;
		const char* funcCall;
	};

	typedef std::map< int, AnimatedResourceStore > TIntResourceMap;
	typedef std::pair< int, AnimatedResourceStore > TIntResourcePair;

	// maps
	TIntResourceMap ResourceMap;
	TNameModelMap ModelMap;
}

/////////////////////////////////////////////////////
/// Method: CreateAnimatedResourceMap
/// Params: None
///
/////////////////////////////////////////////////////
void res::CreateAnimatedResourceMap()
{
	ResourceMap.clear();

#ifdef _DEBUG
	if( script::LuaFunctionCheck( "RegisterAnimatedResources" ) == 0 )
#endif // _DEBUG
	{
		int result = 0;

		int errorFuncIndex;
		errorFuncIndex = script::GetErrorFuncIndex();

		// get the function
		lua_getglobal( script::LuaScripting::GetState(), "RegisterAnimatedResources" );

		result = lua_pcall( script::LuaScripting::GetState(), 0, 1, errorFuncIndex );

		// LUA_ERRRUN --- a runtime error. 
		// LUA_ERRMEM --- memory allocation error. For such errors, Lua does not call the error handler function. 
		// LUA_ERRERR --- error while running the error handler function. 

		if( result == LUA_ERRRUN || result == LUA_ERRMEM || result == LUA_ERRERR )
		{
			DBGLOG( "RESOURCES: *ERROR* Calling function '%s' failed\n", "RegisterAnimatedResources" );
			DBGLOG( "\tLUA_TRACEBACK: %s\n", lua_tostring( script::LuaScripting::GetState(), -1 ) );
			
			DBG_ASSERT_MSG( 0, "RESOURCES: *ERROR* Calling function '%s' failed", "RegisterAnimatedResources" );
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

						// id first
						lua_rawgeti( script::LuaScripting::GetState(), -1, paramIndex++ );
						int resId = static_cast<int>( lua_tonumber( script::LuaScripting::GetState(), -1 ) );

						lua_pop( script::LuaScripting::GetState(), 1 );

						// filename
						lua_rawgeti( script::LuaScripting::GetState(), -1, paramIndex++ );
						const char* scriptFile = lua_tostring( script::LuaScripting::GetState(), -1 );

						lua_pop( script::LuaScripting::GetState(), 1 );

						// function call
						lua_rawgeti( script::LuaScripting::GetState(), -1, paramIndex++ );
						const char* funcCall = lua_tostring( script::LuaScripting::GetState(), -1 );

						lua_pop( script::LuaScripting::GetState(), 1 );

						AnimatedResourceStore resourceInfo;
						resourceInfo.scriptFile = scriptFile;
						resourceInfo.funcCall = funcCall;

						ResourceMap.insert( TIntResourcePair( resId, resourceInfo ) );
					}
				}
				lua_pop( script::LuaScripting::GetState(), 1 );	
			}
			lua_pop( script::LuaScripting::GetState(), 1 );
		}
	}
}

/////////////////////////////////////////////////////
/// Method: ClearAnimatedModelMap
/// Params: None
///
/////////////////////////////////////////////////////
void res::ClearAnimatedModelMap()
{
	TNameModelMap::iterator it;

	it = ModelMap.begin();
	
	while( it != ModelMap.end() )
	{
#ifdef _DEBUG
		DBGLOG( "*WARNING* Model %s still active\n", it->first.c_str() );
#endif // _DEBUG

		delete it->second.pModel;

		it++;
	}

	ModelMap.clear();
}

/////////////////////////////////////////////////////
/// Method: LoadAnimatedModel
/// Params: [in]index
///
/////////////////////////////////////////////////////
mdl::ModelHGA* res::LoadAnimatedModel( int index, bool allowSmoothing )
{
	mdl::ModelHGA* pModel = 0;
	AnimatedResourceStore* pResourceInfo = 0;
	file::TFile fileData;

	// try and find the script and func to call
	TIntResourceMap::iterator resIt;
	resIt = ResourceMap.find( index );
	if( resIt != ResourceMap.end() )
	{
		pResourceInfo = &resIt->second;
	}
	else
	{
		DBG_ASSERT_MSG( 0, "Animated Model resource id: %d - not found", index );
		return(0);
	}

	// have the resource info, load the script
	script::LoadScript( pResourceInfo->scriptFile );

	// now call the object creation func
#ifdef _DEBUG
	if( script::LuaFunctionCheck( pResourceInfo->funcCall ) == 0 )
#endif // _DEBUG
	{
		int result = 0;

		const char* bindFile = 0;
		float cullRadius = 1.0f;
		GLenum magFilter = GL_NEAREST;
		GLenum minFilter = GL_NEAREST;

		int errorFuncIndex;
		errorFuncIndex = script::GetErrorFuncIndex();

		// get the function
		lua_getglobal( script::LuaScripting::GetState(), pResourceInfo->funcCall );

		// returns 2 tables
		result = lua_pcall( script::LuaScripting::GetState(), 0, 2, errorFuncIndex );

		// LUA_ERRRUN --- a runtime error. 
		// LUA_ERRMEM --- memory allocation error. For such errors, Lua does not call the error handler function. 
		// LUA_ERRERR --- error while running the error handler function. 

		if( result == LUA_ERRRUN || result == LUA_ERRMEM || result == LUA_ERRERR )
		{
			DBGLOG( "RESOURCES: *ERROR* Calling function '%s' failed\n", pResourceInfo->funcCall );
			DBGLOG( "\tLUA_TRACEBACK: %s\n", lua_tostring( script::LuaScripting::GetState(), -1 ) );
			
			DBG_ASSERT_MSG( 0, "RESOURCES: *ERROR* Calling function '%s' failed", pResourceInfo->funcCall );
			return(0);
		}

		// should be a table of tables
		// BIND RESOURCE
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
					
					if( innerCount == 4 )
					{
						// filename
						lua_rawgeti( script::LuaScripting::GetState(), -1, 1 );
						bindFile = lua_tostring( script::LuaScripting::GetState(), -1 );

						lua_pop( script::LuaScripting::GetState(), 1 );

						// cull radius
						lua_rawgeti( script::LuaScripting::GetState(), -1, 2 );
						cullRadius =  static_cast<float>( lua_tonumber( script::LuaScripting::GetState(), -1 ) );

						lua_pop( script::LuaScripting::GetState(), 1 );

						// magFilter
						lua_rawgeti( script::LuaScripting::GetState(), -1, 3 );
						magFilter =  static_cast<int>( lua_tonumber( script::LuaScripting::GetState(), -1 ) );

						lua_pop( script::LuaScripting::GetState(), 1 );

						// minFilter
						lua_rawgeti( script::LuaScripting::GetState(), -1, 4 );
						minFilter =  static_cast<int>( lua_tonumber( script::LuaScripting::GetState(), -1 ) );

						lua_pop( script::LuaScripting::GetState(), 1 );
					}
				}
				lua_pop( script::LuaScripting::GetState(), 1 );	
			}
			lua_pop( script::LuaScripting::GetState(), 1 );
		}

		file::CreateFileStructure( bindFile, &fileData );

		// found out if this model has been loaded already and early out
		TNameModelMap::iterator it;
		it = ModelMap.find( std::string(fileData.szFile) );

		// did iterator not hit the end
		if( it != ModelMap.end() )
		{
			it->second.refCount++;
			return( it->second.pModel );
		}		

		// not found create it
		
		// check for smooth filter
		ScriptDataHolder* pScriptData = GetScriptDataHolder();
		ScriptDataHolder::DevScriptData& DevData = pScriptData->GetDevData();
		if( allowSmoothing && DevData.smoothFiltering )
		{
			if( magFilter == GL_NEAREST )
				magFilter = GL_LINEAR;

			if( minFilter == GL_NEAREST )
				minFilter = GL_LINEAR;
			else if( (minFilter == GL_NEAREST_MIPMAP_NEAREST) || 
					(minFilter == GL_LINEAR_MIPMAP_NEAREST) || 
					(minFilter == GL_NEAREST_MIPMAP_LINEAR) )
				minFilter = GL_LINEAR_MIPMAP_LINEAR;
		}
        
        /*if( core::app::IstvOS() )
        {
            if( magFilter == GL_LINEAR )
                magFilter = GL_NEAREST;
            
            if( minFilter == GL_LINEAR)
                minFilter = GL_NEAREST;
            else
                minFilter = GL_NEAREST_MIPMAP_NEAREST;
        }*/

		pModel = new mdl::ModelHGA;
		if( pModel->Load( bindFile, magFilter, minFilter ) )
		{
			DBGLOG( "Model %s file not found\n", bindFile );

			DBG_ASSERT_MSG( 0, "Model %s file not found\n", bindFile );

			delete pModel;

			return(0);
		}

		pModel->SetModelGeneralFlags(0);
		//pModel->SetupAndFree();

		pModel->modelSphere.fRadius = cullRadius;

		ModelStore addModel;
		addModel.refCount = 1;
		addModel.pModel = pModel;

		ModelMap.insert( TNameModelPair( std::string(fileData.szFile), addModel ) );

		// ANIMATION DATA
		if( lua_istable( script::LuaScripting::GetState(), -1 ) )
		{
			int i=0;
			int n = luaL_len( script::LuaScripting::GetState(), -1 );

			const char* animName = 0;
			bool loopFlag = false;

			// go through all the tables in this table
			for( i = 1; i <= n; ++i )
			{
				lua_rawgeti( script::LuaScripting::GetState(), -1, i );
				if( lua_istable( script::LuaScripting::GetState(), -1 ) )
				{
					int innerCount = luaL_len( script::LuaScripting::GetState(), -1 );
					
					if( innerCount == 3 )
					{
						// filename
						lua_rawgeti( script::LuaScripting::GetState(), -1, 1 );
						animName = lua_tostring( script::LuaScripting::GetState(), -1 );

						lua_pop( script::LuaScripting::GetState(), 1 );

						// animation id
						lua_rawgeti( script::LuaScripting::GetState(), -1, 2 );
						unsigned int animId =  static_cast<unsigned int>( lua_tonumber( script::LuaScripting::GetState(), -1 ) );

						lua_pop( script::LuaScripting::GetState(), 1 );

						// loop flag
						lua_rawgeti( script::LuaScripting::GetState(), -1, 3 );
						loopFlag = static_cast<bool>( lua_toboolean( script::LuaScripting::GetState(), -1 ) !=0 );

						lua_pop( script::LuaScripting::GetState(), 1 );

						// load each animation
						pModel->LoadAnimation( animName, animId, loopFlag );
					}
				}
				lua_pop( script::LuaScripting::GetState(), 1 );	
			}
			lua_pop( script::LuaScripting::GetState(), 1 );
		}
	}

	return(pModel);
}

/////////////////////////////////////////////////////
/// Method: RemoveAnimatedReference
/// Params: [in]pModel
///
/////////////////////////////////////////////////////
void res::RemoveAnimatedReference( mdl::ModelHGA* pModel )
{
	TNameModelMap::iterator it;

	it = ModelMap.begin();
	
	while( it != ModelMap.end() )
	{
		if( it->second.pModel == pModel )
		{
			it->second.refCount--;
			return;
		}

		it++;
	}
}

/////////////////////////////////////////////////////
/// Method: RemoveModel
/// Params: [in]pModel
///
/////////////////////////////////////////////////////
void res::RemoveAnimatedModel( mdl::ModelHGA* pModel )
{
	TNameModelMap::iterator it;

	it = ModelMap.begin();
	
	if( pModel == 0 )
	{
		while( it != ModelMap.end() )
		{
			if( it->second.refCount < 1 )
			{
				delete it->second.pModel;
				ModelMap.erase( it );
				it = ModelMap.begin();
				continue;
			}

			it++;
		}
	}
	else
	{
		while( it != ModelMap.end() )
		{
			if( it->second.pModel == pModel )
			{
				it->second.refCount--;

				if( it->second.refCount < 1 )
				{
					delete it->second.pModel;

					ModelMap.erase( it );

					return;
				}
			}

			it++;
		}
	}
}
