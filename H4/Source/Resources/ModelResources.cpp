
#include "CoreBase.h"
#include "MathBase.h"
#include "RenderBase.h"
#include "CollisionBase.h"
#include "ModelBase.h"
#include "ScriptBase.h"

#include "H4.h"

#include "Resources/ModelResources.h"

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
		mdl::ModelHGM* pModel;
	};

	typedef std::map< std::string, ModelStore, struct lstr> TNameModelMap;
	typedef std::pair< std::string, ModelStore> TNameModelPair;

	// model info store
	struct ModelResourceStore
	{
		const char* modelName;
		GLenum magFilter;
		GLenum minFilter;
	};

	typedef std::map< int, ModelResourceStore > TIntResourceMap;
	typedef std::pair< int, ModelResourceStore > TIntResourcePair;

	// maps
	TIntResourceMap ResourceMap;
	TNameModelMap ModelMap;
}

/////////////////////////////////////////////////////
/// Method: CreateResourceMap
/// Params: None
///
/////////////////////////////////////////////////////
void res::CreateResourceMap()
{
	ResourceMap.clear();

#ifdef _DEBUG
	if( script::LuaFunctionCheck( "RegisterModelResources" ) == 0 )
#endif // _DEBUG
	{
		int result = 0;

		int errorFuncIndex;
		errorFuncIndex = script::GetErrorFuncIndex();

		// get the function
		lua_getglobal( script::LuaScripting::GetState(), "RegisterModelResources" );

		result = lua_pcall( script::LuaScripting::GetState(), 0, 1, errorFuncIndex );

		// LUA_ERRRUN --- a runtime error. 
		// LUA_ERRMEM --- memory allocation error. For such errors, Lua does not call the error handler function. 
		// LUA_ERRERR --- error while running the error handler function. 

		if( result == LUA_ERRRUN || result == LUA_ERRMEM || result == LUA_ERRERR )
		{
			DBGLOG( "RESOURCES: *ERROR* Calling function '%s' failed\n", "RegisterModelResources" );
			DBGLOG( "\tLUA_TRACEBACK: %s\n", lua_tostring( script::LuaScripting::GetState(), -1 ) );
			
			DBG_ASSERT_MSG( 0, "RESOURCES: *ERROR* Calling function '%s' failed", "RegisterModelResources" );
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
					
					if( innerCount == 4 )
					{
						// id first
						lua_rawgeti( script::LuaScripting::GetState(), -1, 1 );
						int resId = static_cast<int>( lua_tonumber( script::LuaScripting::GetState(), -1 ) );

						lua_pop( script::LuaScripting::GetState(), 1 );

						// filename
						lua_rawgeti( script::LuaScripting::GetState(), -1, 2 );
						const char* resName = lua_tostring( script::LuaScripting::GetState(), -1 );

						lua_pop( script::LuaScripting::GetState(), 1 );

						// magFilter
						lua_rawgeti( script::LuaScripting::GetState(), -1, 3 );
						GLenum magFilter =  static_cast<int>( lua_tonumber( script::LuaScripting::GetState(), -1 ) );

						lua_pop( script::LuaScripting::GetState(), 1 );

						// minFilter
						lua_rawgeti( script::LuaScripting::GetState(), -1, 4 );
						GLenum minFilter =  static_cast<int>( lua_tonumber( script::LuaScripting::GetState(), -1 ) );

						lua_pop( script::LuaScripting::GetState(), 1 );

						ModelResourceStore resourceInfo;
						resourceInfo.modelName = resName;
						resourceInfo.magFilter = magFilter;
						resourceInfo.minFilter = minFilter;

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
/// Method: ClearModelMap
/// Params: None
///
/////////////////////////////////////////////////////
void res::ClearModelMap()
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
/// Method: LoadModel
/// Params: [in]modelFile, [in]magFilter, [in]minFilter
///
/////////////////////////////////////////////////////
mdl::ModelHGM* res::LoadModel( const char* modelFile, GLenum magFilter, GLenum minFilter, bool allowSmoothing )
{
	file::TFile fileData;
	file::CreateFileStructure( modelFile, &fileData );

	TNameModelMap::iterator it;
	it = ModelMap.find( std::string(fileData.szFile) );

	// did iterator not hit the end
	if( it != ModelMap.end() )
	{
		it->second.refCount++;
		return( it->second.pModel );
	}

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

	// not found create it
	mdl::ModelHGM* pModel = 0;
	pModel = new mdl::ModelHGM;
	if( pModel->Load( modelFile, magFilter, minFilter, core::app::GetLowPowerMode() ) )
	{
		DBGLOG( "Model %s file not found\n", modelFile );
		return(0);
	}

	pModel->SetModelGeneralFlags(0);
	pModel->SetupAndFree();

	ModelStore addModel;
	addModel.refCount = 1;
	addModel.pModel = pModel;

	ModelMap.insert( TNameModelPair( std::string(fileData.szFile), addModel ) );

	return(pModel);
}


/////////////////////////////////////////////////////
/// Method: LoadModel
/// Params: [in]index
///
/////////////////////////////////////////////////////
mdl::ModelHGM* res::LoadModel( int index, bool allowSmoothing )
{
	file::TFile fileData;
	
	ModelResourceStore* pResourceInfo = 0;

	TIntResourceMap::iterator resIt;
	resIt = ResourceMap.find( index );
	if( resIt != ResourceMap.end() )
	{
		pResourceInfo = &resIt->second;

		file::CreateFileStructure( pResourceInfo->modelName, &fileData );
	}
	else
		return(0);

	TNameModelMap::iterator it;
	it = ModelMap.find( std::string(fileData.szFile) );

	// did iterator not hit the end
	if( it != ModelMap.end() )
	{
		it->second.refCount++;
		return( it->second.pModel );
	}

	// check for smooth filter
	ScriptDataHolder* pScriptData = GetScriptDataHolder();
	ScriptDataHolder::DevScriptData& DevData = pScriptData->GetDevData();
	if( allowSmoothing && DevData.smoothFiltering )
	{
		if( pResourceInfo->magFilter == GL_NEAREST )
			pResourceInfo->magFilter = GL_LINEAR;

		if( pResourceInfo->minFilter == GL_NEAREST )
			pResourceInfo->minFilter = GL_LINEAR;
		else if( (pResourceInfo->minFilter == GL_NEAREST_MIPMAP_NEAREST) || 
				(pResourceInfo->minFilter == GL_LINEAR_MIPMAP_NEAREST) || 
				(pResourceInfo->minFilter == GL_NEAREST_MIPMAP_LINEAR) )
			pResourceInfo->minFilter = GL_LINEAR_MIPMAP_LINEAR;
	}

	// not found create it
	mdl::ModelHGM* pModel = 0;
	pModel = new mdl::ModelHGM;
	if( pModel->Load( fileData.szFilename, pResourceInfo->magFilter, pResourceInfo->minFilter, core::app::GetLowPowerMode() ) )
	{
		DBGLOG( "Model %s file not found\n", fileData.szFilename );

		DBG_ASSERT_MSG( 0, "Model %s file not found\n", fileData.szFilename );

		delete pModel;

		return(0);
	}

	pModel->SetModelGeneralFlags(0);
	pModel->SetupAndFree();

	ModelStore addModel;
	addModel.refCount = 1;
	addModel.pModel = pModel;

	ModelMap.insert( TNameModelPair( std::string(fileData.szFile), addModel ) );

	return(pModel);
}

/////////////////////////////////////////////////////
/// Method: RemoveReference
/// Params: [in]pModel
///
/////////////////////////////////////////////////////
void res::RemoveReference( mdl::ModelHGM* pModel )
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
void res::RemoveModel( mdl::ModelHGM* pModel )
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
