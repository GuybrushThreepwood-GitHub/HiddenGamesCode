
/*===================================================================
	File: ModelResources.h
	Game: H8

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"
#include "MathBase.h"
#include "RenderBase.h"
#include "CollisionBase.h"
#include "ModelBase.h"
#include "ScriptBase.h"

#include "Resources/ModelResources.h"
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

	// model pointer store
	struct ModelStore
	{
		int refCount;
		mdl::ModelHGM* pModel;
	};

	typedef std::map< std::string, ModelStore, struct lstr> TNameModelMap;
	typedef std::pair< std::string, ModelStore> TNameModelPair;

	typedef std::map< int, res::ModelResourceStore > TIntResourceMap;
	typedef std::pair< int, res::ModelResourceStore > TIntResourcePair;

	// maps
	TIntResourceMap ResourceMap;
	TNameModelMap ModelMap;
}

/////////////////////////////////////////////////////
/// Function: LoadTextureAnimation
/// Params: [in]animFile
///
/////////////////////////////////////////////////////
void LoadTextureAnimation( mdl::ModelHGM* pModel, const char* animFile );
void LoadTextureAnimation( mdl::ModelHGM* pModel, const char* animFile )
{
	mdl::TextureAnimationBlock* block = 0;

	if( script::LoadScript( animFile ) == 0 )
	{
#ifdef _DEBUG
		if( script::LuaFunctionCheck( "GetTextureAnimations" ) == 0 )
#endif // _DEBUG
		{
			int result = 0;
			int i=0,j=0;

			int errorFuncIndex;
			errorFuncIndex = script::GetErrorFuncIndex();

			// get the function
			lua_getglobal( script::LuaScripting::GetState(), "GetTextureAnimations" );

			result = lua_pcall( script::LuaScripting::GetState(), 0, 1, errorFuncIndex );

			// LUA_ERRRUN --- a runtime error. 
			// LUA_ERRMEM --- memory allocation error. For such errors, Lua does not call the error handler function. 
			// LUA_ERRERR --- error while running the error handler function. 

			if( result == LUA_ERRRUN || result == LUA_ERRMEM || result == LUA_ERRERR )
			{
				DBGLOG( "RESOURCES: *ERROR* Calling function '%s' failed\n", "GetTextureAnimations" );
				DBGLOG( "\tLUA_TRACEBACK: %s\n", lua_tostring( script::LuaScripting::GetState(), -1 ) );
			
				DBG_ASSERT_MSG( 0, "RESOURCES: *ERROR* Calling function '%s' failed", "GetTextureAnimations" );
				return;
			}

			// should be a table
			if( lua_istable( script::LuaScripting::GetState(), -1 ) )
			{
				int n = luaL_len( script::LuaScripting::GetState(), -1 );
				
				block = new mdl::TextureAnimationBlock[n];
				DBG_ASSERT( block != 0 );

				// go through all the tables in this table
				for( i = 1; i <= n; ++i )
				{
					// clear
					std::memset( &block[i-1], 0, sizeof(mdl::TextureAnimationBlock) );

					lua_rawgeti( script::LuaScripting::GetState(), -1, i );
					if( lua_istable( script::LuaScripting::GetState(), -1 ) )
					{
						int paramIndex = 1;

						// animation table lookup name
						lua_rawgeti( script::LuaScripting::GetState(), -1, paramIndex++ );
						const char* animTableName = lua_tostring( script::LuaScripting::GetState(), -1 );
						lua_pop( script::LuaScripting::GetState(), 1 );

						// now load the table data for this anim
						lua_getglobal( script::LuaScripting::GetState(), animTableName );

						// push sub mesh id table
						lua_pushstring( script::LuaScripting::GetState(), "subMeshIds" );
						lua_gettable( script::LuaScripting::GetState(), -2 );

						if( lua_istable( script::LuaScripting::GetState(), -1 ) )
						{
							// grab numbers
							block[i-1].numSubmeshIds = luaL_len( script::LuaScripting::GetState(), -1 );
							block[i-1].pSubMeshList = new mdl::TextureAnimationBlock::MeshData[block[i-1].numSubmeshIds];
							DBG_ASSERT( block[i-1].pSubMeshList != 0 );

							paramIndex = 1;
							for( j = 1; j <= block[i-1].numSubmeshIds; ++j )
							{
								// animation table lookup name
								lua_rawgeti( script::LuaScripting::GetState(), -1, paramIndex++ );
								int subMesh = lua_tointeger( script::LuaScripting::GetState(), -1 );
								lua_pop( script::LuaScripting::GetState(), 1 );

								block[i-1].pSubMeshList[ j-1 ].meshId = subMesh;
								block[i-1].pSubMeshList[ j-1 ].pMesh = pModel->GetMeshFromIndex( subMesh );
							}
						}
						lua_pop( script::LuaScripting::GetState(), 2 ); // pop string and global
						
						// now load the table data for this anim
						lua_getglobal( script::LuaScripting::GetState(), animTableName );

						// push uv animation table
						lua_pushstring( script::LuaScripting::GetState(), "animationFrames" );
						lua_gettable( script::LuaScripting::GetState(), -2 );

						if( lua_istable( script::LuaScripting::GetState(), -1 ) )
						{
							// grab table list
							block[i-1].numAnimationFrames = luaL_len( script::LuaScripting::GetState(), -1 );
							block[i-1].animationData = new mdl::TextureAnimationBlock::AnimationData[block[i-1].numAnimationFrames];
							DBG_ASSERT( block[i-1].animationData != 0 );

							for( j = 1; j <= block[i-1].numAnimationFrames; ++j )
							{
								lua_rawgeti( script::LuaScripting::GetState(), -1, j );
								if( lua_istable( script::LuaScripting::GetState(), -1 ) )
								{
									// grab numbers
									paramIndex = 1;

									lua_rawgeti( script::LuaScripting::GetState(), -1, paramIndex++ );
									float time = static_cast<float>( lua_tonumber( script::LuaScripting::GetState(), -1 ) );
									lua_pop( script::LuaScripting::GetState(), 1 );

									lua_rawgeti( script::LuaScripting::GetState(), -1, paramIndex++ );
									int textureLookup = static_cast<int>( lua_tonumber( script::LuaScripting::GetState(), -1 ) );
									lua_pop( script::LuaScripting::GetState(), 1 );

									block[i-1].animationData[j-1].time = time;
									block[i-1].animationData[j-1].textureLookup = textureLookup;
									const res::TextureResourceStore* tr = res::GetTextureResource( textureLookup );
									DBG_ASSERT( tr != 0 );
									if( tr->texId == renderer::INVALID_OBJECT )
									{
										res::LoadTexture( textureLookup );
									}
									block[i-1].animationData[j-1].textureId = tr->texId;
								}
								lua_pop( script::LuaScripting::GetState(), 1 ); // pop uv table off
							}
						}
						lua_pop( script::LuaScripting::GetState(), 2 ); // pop animFrames string and global
					}
					lua_pop( script::LuaScripting::GetState(), 1 ); // pop animation table
				}
				lua_pop( script::LuaScripting::GetState(), 1 );

				// pass it to the model
				pModel->LoadTextureAnimations( n, block );
			}
		}
	}
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

						// texture animation filename
						lua_rawgeti( script::LuaScripting::GetState(), -1, paramIndex++ );
						const char* textureAnimFile = lua_tostring( script::LuaScripting::GetState(), -1 );

						lua_pop( script::LuaScripting::GetState(), 1 );

						// magFilter
						lua_rawgeti( script::LuaScripting::GetState(), -1, paramIndex++ );
						GLenum magFilter =  static_cast<int>( lua_tonumber( script::LuaScripting::GetState(), -1 ) );

						lua_pop( script::LuaScripting::GetState(), 1 );

						// minFilter
						lua_rawgeti( script::LuaScripting::GetState(), -1, paramIndex++ );
						GLenum minFilter =  static_cast<int>( lua_tonumber( script::LuaScripting::GetState(), -1 ) );

						lua_pop( script::LuaScripting::GetState(), 1 );

						// texture start
						/*lua_rawgeti( script::LuaScripting::GetState(), -1, paramIndex++ );
						int textureStartIndex =  static_cast<int>( lua_tonumber( script::LuaScripting::GetState(), -1 ) );

						lua_pop( script::LuaScripting::GetState(), 1 );

						// texture count
						lua_rawgeti( script::LuaScripting::GetState(), -1, paramIndex++ );
						int textureCount =  static_cast<int>( lua_tonumber( script::LuaScripting::GetState(), -1 ) );

						lua_pop( script::LuaScripting::GetState(), 1 );*/

						res::ModelResourceStore resourceInfo;

						resourceInfo.modelName = resName;
						resourceInfo.textureAnimFile = textureAnimFile;
						resourceInfo.magFilter = magFilter;
						resourceInfo.minFilter = minFilter;
						//resourceInfo.textureStartIndex = textureStartIndex;
						//resourceInfo.textureCount = textureCount;
						//resourceInfo.hasDamageTexture = false;

						/*if( innerCount == 8 )
						{
							bool hasDamageTexture = false;

							// damage model
							lua_rawgeti( script::LuaScripting::GetState(), -1, paramIndex++ );
							hasDamageTexture = static_cast<bool>( lua_toboolean( script::LuaScripting::GetState(), -1 ) != 0 );

							lua_pop( script::LuaScripting::GetState(), 1 );

							resourceInfo.hasDamageTexture = hasDamageTexture;
						}*/

						// add
						ResourceMap.insert( TIntResourcePair( resId, resourceInfo ) );
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
/// Method: GetModelResource
/// Params: [in]index
///
/////////////////////////////////////////////////////
const res::ModelResourceStore* res::GetModelResource( int index )
{
	TIntResourceMap::iterator resIt;
	
	resIt = ResourceMap.find( index );

	if( resIt != ResourceMap.end() )
		return( &resIt->second );

	return(0);
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
	/*ScriptDataHolder* pScriptData = GetScriptDataHolder();
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
	}*/

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
	unsigned int i=0;
	res::ModelResourceStore* pResourceInfo = 0;

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
	/*ScriptDataHolder* pScriptData = GetScriptDataHolder();
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
	}*/

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

	// load texture animation file if defined
	if( !core::IsEmptyString( pResourceInfo->textureAnimFile ) )
	{
		LoadTextureAnimation( pModel, pResourceInfo->textureAnimFile );
	}

	pModel->SetModelGeneralFlags(0);
	pModel->SetupAndFree();

	// enable all texture animations
	if( !core::IsEmptyString( pResourceInfo->textureAnimFile ) )
	{
		for( i=0; i < pModel->GetNumAnimationBlocks(); ++i )
			pModel->SetTextureAnimation(i);
	}

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
