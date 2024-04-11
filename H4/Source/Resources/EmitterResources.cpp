
/*===================================================================
	File: EmitterResources.cpp
	Game: H4

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"
#include "MathBase.h"
#include "RenderBase.h"
#include "CollisionBase.h"
#include "ModelBase.h"
#include "ScriptBase.h"

#include "Resources/TextureResources.h"
#include "Resources/SpriteResources.h"

#include "Resources/EmitterResources.h"

namespace
{
	typedef std::map< int, res::EmitterResourceStore > TIntEmitterMap;
	typedef std::pair< int, res::EmitterResourceStore > TIntEmitterPair;

	// maps
	TIntEmitterMap ResourceMap;
}

/////////////////////////////////////////////////////
/// Function: CreateEmitterResourceMap
/// Params: None
///
/////////////////////////////////////////////////////
int LoadAndCallEmitterFunction( const char* filename, const char* funcName, res::EmitterResourceStore& newResource );
int LoadAndCallEmitterFunction( const char* filename, const char* funcName, res::EmitterResourceStore& newResource )
{
	if( script::LuaScripting::GetInstance()->LoadScript( filename ) )
	{
		DBG_ASSERT_MSG( 0, "*ERROR* Could not load emitter script '%s'", filename );
		return(1);
	}

#ifdef _DEBUG
	if( script::LuaFunctionCheck( funcName ) == 0 )
#endif // _DEBUG
	{
		int result = 0;

		int errorFuncIndex;
		errorFuncIndex = script::GetErrorFuncIndex();

		// get the function
		lua_getglobal( script::LuaScripting::GetState(), funcName );

		result = lua_pcall( script::LuaScripting::GetState(), 0, 1, errorFuncIndex );

		// LUA_ERRRUN --- a runtime error. 
		// LUA_ERRMEM --- memory allocation error. For such errors, Lua does not call the error handler function. 
		// LUA_ERRERR --- error while running the error handler function. 

		if( result == LUA_ERRRUN || result == LUA_ERRMEM || result == LUA_ERRERR )
		{
			DBGLOG( "RESOURCES: *ERROR* Calling function '%s' failed\n", "RegisterSpriteResources" );
			DBGLOG( "\tLUA_TRACEBACK: %s\n", lua_tostring( script::LuaScripting::GetState(), -1 ) );

			DBG_ASSERT_MSG( 0, "RESOURCES: *ERROR* Calling function '%s' failed", "RegisterSpriteResources" );
			return(1);
		}

		int paramIndex = -1;

		// should be the emitter table
		if( lua_istable( script::LuaScripting::GetState(), paramIndex ) )
		{
			// can create the emitter block
			newResource.block = new efx::Emitter::EmitterBlock;
			std::memset( newResource.block, 0, sizeof(efx::Emitter::EmitterBlock) );

			// read the table
			paramIndex = -2;

			if( script::DoesTableItemExist( "SpriteId", paramIndex, LUA_TNUMBER ) )
				newResource.block->emitterId		= static_cast<int>( script::LuaGetNumberFromTableItem( "SpriteId", paramIndex, 1 ) ); 


			if( script::DoesTableItemExist( "CameraAlign", paramIndex, LUA_TBOOLEAN ) )
				newResource.block->cameraAlign		= static_cast<bool>( script::LuaGetBoolFromTableItem( "CameraAlign", paramIndex, true ) ); 

			//if( script::DoesTableItemExist( "TextureFile", 1, LUA_TSTRING ) )
			//	newEmitter.emitterTexture	= static_cast<const char*>( script::LuaGetStringFromTableItem( "TextureFile", 1 ) ); 

			if( script::DoesTableItemExist( "UsePoints", paramIndex, LUA_TBOOLEAN ) )
				newResource.block->usePoints			= static_cast<bool>( script::LuaGetBoolFromTableItem( "UsePoints", paramIndex, false ) ); 
			if( script::DoesTableItemExist( "SmoothPoints", paramIndex, LUA_TBOOLEAN ) )
				newResource.block->smoothPoints		= static_cast<bool>( script::LuaGetBoolFromTableItem( "SmoothPoints", paramIndex, false ) ); 
			if( script::DoesTableItemExist( "UseHWPointSprites", paramIndex, LUA_TBOOLEAN ) )
				newResource.block->useHWPointSprites	= static_cast<bool>( script::LuaGetBoolFromTableItem( "UseHWPointSprites", paramIndex, false ) ); 

			if( script::DoesTableItemExist( "MaxParticles", paramIndex, LUA_TNUMBER ) )
				newResource.block->maxParticles		= static_cast<int>( script::LuaGetNumberFromTableItem( "MaxParticles", paramIndex, 1 ) ); 
		
			// depth
			if( script::DoesTableItemExist( "EnableDepth", paramIndex, LUA_TBOOLEAN ) )
				newResource.block->enableDepth		= static_cast<bool>( script::LuaGetBoolFromTableItem( "EnableDepth", paramIndex, false ) ); 
			if( script::DoesTableItemExist( "DepthFunc", paramIndex, LUA_TNUMBER ) )
				newResource.block->depthFunc		= static_cast<GLenum>( script::LuaGetNumberFromTableItem( "DepthFunc", paramIndex, GL_LESS ) ); 

			// blend
			if( script::DoesTableItemExist( "EnableBlending", paramIndex, LUA_TBOOLEAN ) )
				newResource.block->enableBlending	= static_cast<bool>( script::LuaGetBoolFromTableItem( "EnableBlending", paramIndex, false ) ); 
			if( script::DoesTableItemExist( "BlendSrc", paramIndex, LUA_TNUMBER ) )
				newResource.block->blendSrc			= static_cast<GLenum>( script::LuaGetNumberFromTableItem( "BlendSrc", paramIndex, GL_SRC_ALPHA ) );
			if( script::DoesTableItemExist( "BlendDest", paramIndex, LUA_TNUMBER ) )
				newResource.block->blendDest		= static_cast<GLenum>( script::LuaGetNumberFromTableItem( "BlendDest", paramIndex, GL_ONE_MINUS_SRC_ALPHA ) );

			// alpha
			if( script::DoesTableItemExist( "EnableAlpha", paramIndex, LUA_TBOOLEAN ) )
				newResource.block->enableAlpha	= static_cast<bool>( script::LuaGetBoolFromTableItem( "EnableAlpha", paramIndex, false ) );
			if( script::DoesTableItemExist( "AlphaCompare", paramIndex, LUA_TNUMBER ) )
				newResource.block->alphaCompare	= static_cast<GLenum>( script::LuaGetNumberFromTableItem( "AlphaCompare", paramIndex, GL_ALWAYS ) );
			if( script::DoesTableItemExist( "AlphaClamp", paramIndex, LUA_TNUMBER ) )
				newResource.block->alphaClamp	= static_cast<float>( script::LuaGetNumberFromTableItem( "AlphaClamp", paramIndex, 0.0 ) );

			// gravity
			if( script::DoesTableItemExist( "GravityX", paramIndex, LUA_TNUMBER ) )
				newResource.block->gravity.X	= static_cast<float>( script::LuaGetNumberFromTableItem( "GravityX", paramIndex, 0.0 ) );
			if( script::DoesTableItemExist( "GravityY", paramIndex, LUA_TNUMBER ) )
				newResource.block->gravity.Y	= static_cast<float>( script::LuaGetNumberFromTableItem( "GravityY", paramIndex, -1.0 ) );
			if( script::DoesTableItemExist( "GravityZ", paramIndex, LUA_TNUMBER ) )
				newResource.block->gravity.Z	= static_cast<float>( script::LuaGetNumberFromTableItem( "GravityZ", paramIndex, 0.0 ) );

			
			if( script::DoesTableItemExist( "EmitRateMin", paramIndex, LUA_TNUMBER ) )
				newResource.block->emitRate.minVal	= static_cast<float>( script::LuaGetNumberFromTableItem( "EmitRateMin", paramIndex, 1.0 ) );
			if( script::DoesTableItemExist( "EmitRateMax", paramIndex, LUA_TNUMBER ) )
				newResource.block->emitRate.maxVal	= static_cast<float>( script::LuaGetNumberFromTableItem( "EmitRateMax", paramIndex, 1.0 ) );

			if( script::DoesTableItemExist( "LifetimeMin", paramIndex, LUA_TNUMBER ) )
				newResource.block->lifetime.minVal	= static_cast<float>( script::LuaGetNumberFromTableItem( "LifetimeMin", paramIndex, 1.0 ) );
			if( script::DoesTableItemExist( "LifetimeMax", paramIndex, LUA_TNUMBER ) )
				newResource.block->lifetime.maxVal	= static_cast<float>( script::LuaGetNumberFromTableItem( "LifetimeMax", paramIndex, 1.0 ) );

			if( script::DoesTableItemExist( "SizeMin", paramIndex, LUA_TNUMBER ) )
				newResource.block->size.minVal	= static_cast<float>( script::LuaGetNumberFromTableItem( "SizeMin", paramIndex, 1.0 ) );
			if( script::DoesTableItemExist( "SizeMax", paramIndex, LUA_TNUMBER ) )
				newResource.block->size.maxVal	= static_cast<float>( script::LuaGetNumberFromTableItem( "SizeMax", paramIndex, 1.0 ) );
			
			if( script::DoesTableItemExist( "UseWidthHeightDepth", paramIndex, LUA_TBOOLEAN ) )
				newResource.block->useWidthHeightDepth	= static_cast<bool>( script::LuaGetBoolFromTableItem( "UseWidthHeightDepth", paramIndex, false ) );
			if( script::DoesTableItemExist( "Width", paramIndex, LUA_TNUMBER ) )
				newResource.block->dims.Width	= static_cast<float>( script::LuaGetNumberFromTableItem( "Width", paramIndex, 1.0 ) );
			if( script::DoesTableItemExist( "Height", paramIndex, LUA_TNUMBER ) )
				newResource.block->dims.Height	= static_cast<float>( script::LuaGetNumberFromTableItem( "Height", paramIndex, 1.0 ) );
			if( script::DoesTableItemExist( "Depth", paramIndex, LUA_TNUMBER ) )
				newResource.block->dims.Depth	= static_cast<float>( script::LuaGetNumberFromTableItem( "Depth", paramIndex, 0.0 ) );

			if( script::DoesTableItemExist( "SpawnDir1X", paramIndex, LUA_TNUMBER ) )
				newResource.block->spawnDir1.X	= static_cast<float>( script::LuaGetNumberFromTableItem( "SpawnDir1X", paramIndex, -1.0 ) );
			if( script::DoesTableItemExist( "SpawnDir1Y", paramIndex, LUA_TNUMBER ) )
				newResource.block->spawnDir1.Y	= static_cast<float>( script::LuaGetNumberFromTableItem( "SpawnDir1Y", paramIndex, -1.0 ) );
			if( script::DoesTableItemExist( "SpawnDir1Z", paramIndex, LUA_TNUMBER ) )
				newResource.block->spawnDir1.Z	= static_cast<float>( script::LuaGetNumberFromTableItem( "SpawnDir1Z", paramIndex, -1.0 ) );

			if( script::DoesTableItemExist( "SpawnDir2X", paramIndex, LUA_TNUMBER ) )
				newResource.block->spawnDir2.X	= static_cast<float>( script::LuaGetNumberFromTableItem( "SpawnDir2X", paramIndex, 1.0 ) );
			if( script::DoesTableItemExist( "SpawnDir2Y", paramIndex, LUA_TNUMBER ) )
				newResource.block->spawnDir2.Y	= static_cast<float>( script::LuaGetNumberFromTableItem( "SpawnDir2Y", paramIndex, 1.0 ) );
			if( script::DoesTableItemExist( "SpawnDir2Z", paramIndex, LUA_TNUMBER ) )
				newResource.block->spawnDir2.Z	= static_cast<float>( script::LuaGetNumberFromTableItem( "SpawnDir2Z", paramIndex, 1.0 ) );
			
			if( script::DoesTableItemExist( "EmitRadiusX", paramIndex, LUA_TNUMBER ) )
				newResource.block->emitRadius.X	= static_cast<float>( script::LuaGetNumberFromTableItem( "EmitRadiusX", paramIndex, 1.0 ) );
			if( script::DoesTableItemExist( "EmitRadiusY", paramIndex, LUA_TNUMBER ) )
				newResource.block->emitRadius.Y	= static_cast<float>( script::LuaGetNumberFromTableItem( "EmitRadiusY", paramIndex, 1.0 ) );
			if( script::DoesTableItemExist( "EmitRadiusZ", paramIndex, LUA_TNUMBER ) )
				newResource.block->emitRadius.Z	= static_cast<float>( script::LuaGetNumberFromTableItem( "EmitRadiusZ", paramIndex, 1.0 ) );

			if( script::DoesTableItemExist( "StartColour1R", paramIndex, LUA_TNUMBER ) )
				newResource.block->startColour1.R	= static_cast<float>( script::LuaGetNumberFromTableItem( "StartColour1R", paramIndex, 1.0 ) );
			if( script::DoesTableItemExist( "StartColour1G", paramIndex, LUA_TNUMBER ) )
				newResource.block->startColour1.G	= static_cast<float>( script::LuaGetNumberFromTableItem( "StartColour1G", paramIndex, 1.0 ) );
			if( script::DoesTableItemExist( "StartColour1B", paramIndex, LUA_TNUMBER ) )
				newResource.block->startColour1.B	= static_cast<float>( script::LuaGetNumberFromTableItem( "StartColour1B", paramIndex, 1.0 ) );
			if( script::DoesTableItemExist( "StartColour1A", paramIndex, LUA_TNUMBER ) )
				newResource.block->startColour1.A	= static_cast<float>( script::LuaGetNumberFromTableItem( "StartColour1A", paramIndex, 1.0 ) );

			if( script::DoesTableItemExist( "StartColour2R", paramIndex, LUA_TNUMBER ) )
				newResource.block->startColour2.R	= static_cast<float>( script::LuaGetNumberFromTableItem( "StartColour2R", paramIndex, 0.0 ) );
			if( script::DoesTableItemExist( "StartColour2G", paramIndex, LUA_TNUMBER ) )
				newResource.block->startColour2.G	= static_cast<float>( script::LuaGetNumberFromTableItem( "StartColour2G", paramIndex, 0.0 ) );
			if( script::DoesTableItemExist( "StartColour2B", paramIndex, LUA_TNUMBER ) )
				newResource.block->startColour2.B	= static_cast<float>( script::LuaGetNumberFromTableItem( "StartColour2B", paramIndex, 0.0 ) );
			if( script::DoesTableItemExist( "StartColour2A", paramIndex, LUA_TNUMBER ) )
				newResource.block->startColour2.A	= static_cast<float>( script::LuaGetNumberFromTableItem( "StartColour2A", paramIndex, 1.0 ) );
		
			
			if( script::DoesTableItemExist( "EndColour1R", paramIndex, LUA_TNUMBER ) )
				newResource.block->endColour1.R	= static_cast<float>( script::LuaGetNumberFromTableItem( "EndColour1R", paramIndex, 0.0 ) );
			if( script::DoesTableItemExist( "EndColour1G", paramIndex, LUA_TNUMBER ) )
				newResource.block->endColour1.G	= static_cast<float>( script::LuaGetNumberFromTableItem( "EndColour1G", paramIndex, 0.0 ) );
			if( script::DoesTableItemExist( "EndColour1B", paramIndex, LUA_TNUMBER ) )
				newResource.block->endColour1.B	= static_cast<float>( script::LuaGetNumberFromTableItem( "EndColour1B", paramIndex, 0.0 ) );
			if( script::DoesTableItemExist( "EndColour1A", paramIndex, LUA_TNUMBER ) )
				newResource.block->endColour1.A	= static_cast<float>( script::LuaGetNumberFromTableItem( "EndColour1A", paramIndex, 0.0 ) );

			if( script::DoesTableItemExist( "EndColour2R", paramIndex, LUA_TNUMBER ) )
				newResource.block->endColour2.R	= static_cast<float>( script::LuaGetNumberFromTableItem( "EndColour2R", paramIndex, 0.0 ) );
			if( script::DoesTableItemExist( "EndColour2G", paramIndex, LUA_TNUMBER ) )
				newResource.block->endColour2.G	= static_cast<float>( script::LuaGetNumberFromTableItem( "EndColour2G", paramIndex, 0.0 ) );
			if( script::DoesTableItemExist( "EndColour2B", paramIndex, LUA_TNUMBER ) )
				newResource.block->endColour2.B	= static_cast<float>( script::LuaGetNumberFromTableItem( "EndColour2B", paramIndex, 0.0 ) );
			if( script::DoesTableItemExist( "EndColour2A", paramIndex, LUA_TNUMBER ) )
				newResource.block->endColour2.A	= static_cast<float>( script::LuaGetNumberFromTableItem( "EndColour2A", paramIndex, 0.0 ) );
		}
		else
		{
			DBGLOG( "*ERROR* Could not load emitter script '%s'\n", filename );

			DBG_ASSERT_MSG( 0, "*ERROR* Could not load emitter script '%s'", filename );
			return(1);
		}

		lua_pop( script::LuaScripting::GetState(), 1 );	
	}
#ifdef _DEBUG
	else
	{
		DBGLOG( "*ERROR* Emitter does not have a setup function\n" );

		DBG_ASSERT_MSG( 0, "*ERROR* Emitter does not have a setup function - '%s' does not exist", funcName );
		return(1);
	}
#endif // _DEBUG

	return(0);
}

/////////////////////////////////////////////////////
/// Method: CreateEmitterResourceMap
/// Params: None
///
/////////////////////////////////////////////////////
void res::CreateEmitterResourceMap()
{
	ResourceMap.clear();

#ifdef _DEBUG
	if( script::LuaFunctionCheck( "RegisterEmitterResources" ) == 0 )
#endif // _DEBUG
	{
		int result = 0;

		int errorFuncIndex;
		errorFuncIndex = script::GetErrorFuncIndex();

		// get the function
		lua_getglobal( script::LuaScripting::GetState(), "RegisterEmitterResources" );

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
					
					if( innerCount == 3 )
					{
						// id first
						lua_rawgeti( script::LuaScripting::GetState(), -1, 1 );
						int resId = static_cast<int>( lua_tonumber( script::LuaScripting::GetState(), -1 ) );

						lua_pop( script::LuaScripting::GetState(), 1 );

						// filename
						lua_rawgeti( script::LuaScripting::GetState(), -1, 2 );
						const char* resName = lua_tostring( script::LuaScripting::GetState(), -1 );

						lua_pop( script::LuaScripting::GetState(), 1 );

						// function to call
						lua_rawgeti( script::LuaScripting::GetState(), -1, 3 );
						const char* funcName = lua_tostring( script::LuaScripting::GetState(), -1 );

						lua_pop( script::LuaScripting::GetState(), 1 );

						EmitterResourceStore resourceInfo;
						resourceInfo.block = 0;
						if( LoadAndCallEmitterFunction( resName, funcName, resourceInfo ) == 0 )
						{
							ResourceMap.insert( TIntEmitterPair( resId, resourceInfo ) );
						}
					}
				}
				lua_pop( script::LuaScripting::GetState(), 1 );	
			}
			lua_pop( script::LuaScripting::GetState(), 1 );
		}
	}
}

/////////////////////////////////////////////////////
/// Method: ClearEmitterResources
/// Params: None
///
/////////////////////////////////////////////////////
void res::ClearEmitterResources()
{
	TIntEmitterMap::iterator resIt;
	
	resIt = ResourceMap.begin();

	while( resIt != ResourceMap.end() )
	{
		delete resIt->second.block;
		resIt++;
	}

	ResourceMap.clear();
}

/////////////////////////////////////////////////////
/// Method: GetEmitterResource
/// Params: [in]index
///
/////////////////////////////////////////////////////
const res::EmitterResourceStore* res::GetEmitterResource( int index )
{
	TIntEmitterMap::iterator resIt;
	
	resIt = ResourceMap.find( index );

	if( resIt != ResourceMap.end() )
		return( &resIt->second );

	return(0);
}

/////////////////////////////////////////////////////
/// Method: SetupTexturesOnEmitter
/// Params: [in]pEmitter, [in]block
///
/////////////////////////////////////////////////////
void res::SetupTexturesOnEmitter( efx::Emitter* pEmitter )
{
/*	const res::SpriteResourceStore* sr = 0;
	const res::TextureResourceStore* tr = 0;

	if( pEmitter != 0 && 
		pEmitter->GetEmitterId() != -1 )
	{
		sr = res::GetSpriteResource(pEmitter->GetEmitterId());
		DBG_ASSERT( sr != 0 );
	
		tr = res::GetTextureResource(sr->textureResId);
		DBG_ASSERT( tr != 0 );

		pEmitter->SetTexture(tr->texId);
		pEmitter->SetTextureUVOffset( sr->uOffset, sr->vOffset, sr->w, sr->h );
	}
*/
	renderer::Texture texLoad;
	if( pEmitter->GetEmitterId() != -1 )
	{
		const res::SpriteResourceStore* sr = 0;
		sr = res::GetSpriteResource(pEmitter->GetEmitterId());

		GLuint texId = renderer::TextureLoad( "textures/fx.png", texLoad, renderer::TEXTURE_PNG, 0, false, sr->magFilter, sr->minFilter );
		pEmitter->SetTexture(texId);
		pEmitter->SetTextureUVOffset( sr->uOffset, sr->vOffset, sr->w, sr->h );
	}
}