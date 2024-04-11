
/*===================================================================
	File: EmitterAccess.cpp
	Game: H4

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"
#include "MathBase.h"
#include "RenderBase.h"
#include "SoundBase.h"
#include "CollisionBase.h"
#include "ModelBase.h"
#include "InputBase.h"
#include "ScriptBase.h"

#include "Effects/Emitter.h"
#include "ScriptAccess/ScriptDataHolder.h"
#include "ScriptAccess/Emitter/EmitterAccess.h"

namespace h4Script
{
	ScriptDataHolder* pScriptEmitterData = 0;
}

/////////////////////////////////////////////////////
/// Function: RegisterEmitterFunctions
/// Params: [in]dataHolder
///
/////////////////////////////////////////////////////
void RegisterEmitterFunctions( ScriptDataHolder& dataHolder )
{
	h4Script::pScriptEmitterData = &dataHolder;

	script::LuaScripting::GetInstance()->RegisterFunction( "AddEmitter",			ScriptAddEmitter );
}

/////////////////////////////////////////////////////
/// Function: ScriptAddEmitter
/// Params: [in]pState
///
/////////////////////////////////////////////////////
int ScriptAddEmitter( lua_State* pState )
{
	efx::Emitter::EmitterBlock newEmitter;

	if( lua_istable( pState, 1 ) )
	{
		if( script::DoesTableItemExist( "Id", 1, LUA_TNUMBER ) )
			newEmitter.emitterId		= static_cast<int>( script::LuaGetNumberFromTableItem( "Id", 1, 1 ) ); 

		//if( script::DoesTableItemExist( "TextureFile", 1, LUA_TSTRING ) )
		//	newEmitter.emitterTexture	= static_cast<const char*>( script::LuaGetStringFromTableItem( "TextureFile", 1 ) ); 

		if( script::DoesTableItemExist( "UsePoints", 1, LUA_TBOOLEAN ) )
			newEmitter.usePoints			= static_cast<bool>( script::LuaGetBoolFromTableItem( "UsePoints", 1, false ) ); 
		if( script::DoesTableItemExist( "SmoothPoints", 1, LUA_TBOOLEAN ) )
			newEmitter.smoothPoints		= static_cast<bool>( script::LuaGetBoolFromTableItem( "SmoothPoints", 1, false ) ); 
		if( script::DoesTableItemExist( "UseHWPointSprites", 1, LUA_TBOOLEAN ) )
			newEmitter.useHWPointSprites	= static_cast<bool>( script::LuaGetBoolFromTableItem( "UseHWPointSprites", 1, false ) ); 

		if( script::DoesTableItemExist( "MaxParticles", 1, LUA_TNUMBER ) )
			newEmitter.maxParticles		= static_cast<int>( script::LuaGetNumberFromTableItem( "MaxParticles", 1, 1 ) ); 
	
		// depth
		if( script::DoesTableItemExist( "EnableDepth", 1, LUA_TBOOLEAN ) )
			newEmitter.enableDepth		= static_cast<bool>( script::LuaGetBoolFromTableItem( "EnableDepth", 1, false ) ); 
		if( script::DoesTableItemExist( "DepthFunc", 1, LUA_TNUMBER ) )
			newEmitter.depthFunc		= static_cast<GLenum>( script::LuaGetNumberFromTableItem( "DepthFunc", 1, GL_LESS ) ); 

		// blend
		if( script::DoesTableItemExist( "EnableBlending", 1, LUA_TBOOLEAN ) )
			newEmitter.enableBlending	= static_cast<bool>( script::LuaGetBoolFromTableItem( "EnableBlending", 1, false ) ); 
		if( script::DoesTableItemExist( "BlendSrc", 1, LUA_TNUMBER ) )
			newEmitter.blendSrc			= static_cast<GLenum>( script::LuaGetNumberFromTableItem( "BlendSrc", 1, GL_SRC_ALPHA ) );
		if( script::DoesTableItemExist( "BlendDest", 1, LUA_TNUMBER ) )
			newEmitter.blendDest		= static_cast<GLenum>( script::LuaGetNumberFromTableItem( "BlendDest", 1, GL_ONE_MINUS_SRC_ALPHA ) );

		// alpha
		if( script::DoesTableItemExist( "EnableAlpha", 1, LUA_TBOOLEAN ) )
			newEmitter.enableAlpha	= static_cast<bool>( script::LuaGetBoolFromTableItem( "EnableAlpha", 1, false ) );
		if( script::DoesTableItemExist( "AlphaCompare", 1, LUA_TNUMBER ) )
			newEmitter.alphaCompare	= static_cast<GLenum>( script::LuaGetNumberFromTableItem( "AlphaCompare", 1, GL_ALWAYS ) );
		if( script::DoesTableItemExist( "AlphaClamp", 1, LUA_TNUMBER ) )
			newEmitter.alphaClamp	= static_cast<float>( script::LuaGetNumberFromTableItem( "AlphaClamp", 1, 0.0 ) );

		// gravity
		if( script::DoesTableItemExist( "GravityX", 1, LUA_TNUMBER ) )
			newEmitter.gravity.X	= static_cast<float>( script::LuaGetNumberFromTableItem( "GravityX", 1, 0.0 ) );
		if( script::DoesTableItemExist( "GravityY", 1, LUA_TNUMBER ) )
			newEmitter.gravity.Y	= static_cast<float>( script::LuaGetNumberFromTableItem( "GravityY", 1, -1.0 ) );
		if( script::DoesTableItemExist( "GravityZ", 1, LUA_TNUMBER ) )
			newEmitter.gravity.Z	= static_cast<float>( script::LuaGetNumberFromTableItem( "GravityZ", 1, 0.0 ) );

		
		if( script::DoesTableItemExist( "EmitRateMin", 1, LUA_TNUMBER ) )
			newEmitter.emitRate.minVal	= static_cast<float>( script::LuaGetNumberFromTableItem( "EmitRateMin", 1, 1.0 ) );
		if( script::DoesTableItemExist( "EmitRateMax", 1, LUA_TNUMBER ) )
			newEmitter.emitRate.maxVal	= static_cast<float>( script::LuaGetNumberFromTableItem( "EmitRateMax", 1, 1.0 ) );

		if( script::DoesTableItemExist( "LifetimeMin", 1, LUA_TNUMBER ) )
			newEmitter.lifetime.minVal	= static_cast<float>( script::LuaGetNumberFromTableItem( "LifetimeMin", 1, 1.0 ) );
		if( script::DoesTableItemExist( "LifetimeMax", 1, LUA_TNUMBER ) )
			newEmitter.lifetime.maxVal	= static_cast<float>( script::LuaGetNumberFromTableItem( "LifetimeMax", 1, 1.0 ) );

		if( script::DoesTableItemExist( "SizeMin", 1, LUA_TNUMBER ) )
			newEmitter.size.minVal	= static_cast<float>( script::LuaGetNumberFromTableItem( "SizeMin", 1, 1.0 ) );
		if( script::DoesTableItemExist( "SizeMax", 1, LUA_TNUMBER ) )
			newEmitter.size.maxVal	= static_cast<float>( script::LuaGetNumberFromTableItem( "SizeMax", 1, 1.0 ) );
		
		if( script::DoesTableItemExist( "UseWidthHeightDepth", 1, LUA_TBOOLEAN ) )
			newEmitter.useWidthHeightDepth	= static_cast<bool>( script::LuaGetBoolFromTableItem( "UseWidthHeightDepth", 1, false ) );
		if( script::DoesTableItemExist( "Width", 1, LUA_TNUMBER ) )
			newEmitter.dims.Width	= static_cast<float>( script::LuaGetNumberFromTableItem( "Width", 1, 1.0 ) );
		if( script::DoesTableItemExist( "Height", 1, LUA_TNUMBER ) )
			newEmitter.dims.Height	= static_cast<float>( script::LuaGetNumberFromTableItem( "Height", 1, 1.0 ) );
		if( script::DoesTableItemExist( "Depth", 1, LUA_TNUMBER ) )
			newEmitter.dims.Depth	= static_cast<float>( script::LuaGetNumberFromTableItem( "Depth", 1, 0.0 ) );

		if( script::DoesTableItemExist( "SpawnDir1X", 1, LUA_TNUMBER ) )
			newEmitter.spawnDir1.X	= static_cast<float>( script::LuaGetNumberFromTableItem( "SpawnDir1X", 1, -1.0 ) );
		if( script::DoesTableItemExist( "SpawnDir1Y", 1, LUA_TNUMBER ) )
			newEmitter.spawnDir1.Y	= static_cast<float>( script::LuaGetNumberFromTableItem( "SpawnDir1Y", 1, -1.0 ) );
		if( script::DoesTableItemExist( "SpawnDir1Z", 1, LUA_TNUMBER ) )
			newEmitter.spawnDir1.Z	= static_cast<float>( script::LuaGetNumberFromTableItem( "SpawnDir1Z", 1, -1.0 ) );

		if( script::DoesTableItemExist( "SpawnDir2X", 1, LUA_TNUMBER ) )
			newEmitter.spawnDir2.X	= static_cast<float>( script::LuaGetNumberFromTableItem( "SpawnDir2X", 1, 1.0 ) );
		if( script::DoesTableItemExist( "SpawnDir2Y", 1, LUA_TNUMBER ) )
			newEmitter.spawnDir2.Y	= static_cast<float>( script::LuaGetNumberFromTableItem( "SpawnDir2Y", 1, 1.0 ) );
		if( script::DoesTableItemExist( "SpawnDir2Z", 1, LUA_TNUMBER ) )
			newEmitter.spawnDir2.Z	= static_cast<float>( script::LuaGetNumberFromTableItem( "SpawnDir2Z", 1, 1.0 ) );
		
		if( script::DoesTableItemExist( "EmitRadiusX", 1, LUA_TNUMBER ) )
			newEmitter.emitRadius.X	= static_cast<float>( script::LuaGetNumberFromTableItem( "EmitRadiusX", 1, 1.0 ) );
		if( script::DoesTableItemExist( "EmitRadiusY", 1, LUA_TNUMBER ) )
			newEmitter.emitRadius.Y	= static_cast<float>( script::LuaGetNumberFromTableItem( "EmitRadiusY", 1, 1.0 ) );
		if( script::DoesTableItemExist( "EmitRadiusZ", 1, LUA_TNUMBER ) )
			newEmitter.emitRadius.Z	= static_cast<float>( script::LuaGetNumberFromTableItem( "EmitRadiusZ", 1, 1.0 ) );

		if( script::DoesTableItemExist( "StartColour1R", 1, LUA_TNUMBER ) )
			newEmitter.startColour1.R	= static_cast<float>( script::LuaGetNumberFromTableItem( "StartColour1R", 1, 1.0 ) );
		if( script::DoesTableItemExist( "StartColour1G", 1, LUA_TNUMBER ) )
			newEmitter.startColour1.G	= static_cast<float>( script::LuaGetNumberFromTableItem( "StartColour1G", 1, 1.0 ) );
		if( script::DoesTableItemExist( "StartColour1B", 1, LUA_TNUMBER ) )
			newEmitter.startColour1.B	= static_cast<float>( script::LuaGetNumberFromTableItem( "StartColour1B", 1, 1.0 ) );
		if( script::DoesTableItemExist( "StartColour1A", 1, LUA_TNUMBER ) )
			newEmitter.startColour1.A	= static_cast<float>( script::LuaGetNumberFromTableItem( "StartColour1A", 1, 1.0 ) );

		if( script::DoesTableItemExist( "StartColour2R", 1, LUA_TNUMBER ) )
			newEmitter.startColour2.R	= static_cast<float>( script::LuaGetNumberFromTableItem( "StartColour2R", 1, 0.0 ) );
		if( script::DoesTableItemExist( "StartColour2G", 1, LUA_TNUMBER ) )
			newEmitter.startColour2.G	= static_cast<float>( script::LuaGetNumberFromTableItem( "StartColour2G", 1, 0.0 ) );
		if( script::DoesTableItemExist( "StartColour2B", 1, LUA_TNUMBER ) )
			newEmitter.startColour2.B	= static_cast<float>( script::LuaGetNumberFromTableItem( "StartColour2B", 1, 0.0 ) );
		if( script::DoesTableItemExist( "StartColour2A", 1, LUA_TNUMBER ) )
			newEmitter.startColour2.A	= static_cast<float>( script::LuaGetNumberFromTableItem( "StartColour2A", 1, 1.0 ) );
	
		
		if( script::DoesTableItemExist( "EndColour1R", 1, LUA_TNUMBER ) )
			newEmitter.endColour1.R	= static_cast<float>( script::LuaGetNumberFromTableItem( "EndColour1R", 1, 0.0 ) );
		if( script::DoesTableItemExist( "EndColour1G", 1, LUA_TNUMBER ) )
			newEmitter.endColour1.G	= static_cast<float>( script::LuaGetNumberFromTableItem( "EndColour1G", 1, 0.0 ) );
		if( script::DoesTableItemExist( "EndColour1B", 1, LUA_TNUMBER ) )
			newEmitter.endColour1.B	= static_cast<float>( script::LuaGetNumberFromTableItem( "EndColour1B", 1, 0.0 ) );
		if( script::DoesTableItemExist( "EndColour1A", 1, LUA_TNUMBER ) )
			newEmitter.endColour1.A	= static_cast<float>( script::LuaGetNumberFromTableItem( "EndColour1A", 1, 0.0 ) );

		if( script::DoesTableItemExist( "EndColour2R", 1, LUA_TNUMBER ) )
			newEmitter.endColour2.R	= static_cast<float>( script::LuaGetNumberFromTableItem( "EndColour2R", 1, 0.0 ) );
		if( script::DoesTableItemExist( "EndColour2G", 1, LUA_TNUMBER ) )
			newEmitter.endColour2.G	= static_cast<float>( script::LuaGetNumberFromTableItem( "EndColour2G", 1, 0.0 ) );
		if( script::DoesTableItemExist( "EndColour2B", 1, LUA_TNUMBER ) )
			newEmitter.endColour2.B	= static_cast<float>( script::LuaGetNumberFromTableItem( "EndColour2B", 1, 0.0 ) );
		if( script::DoesTableItemExist( "EndColour2A", 1, LUA_TNUMBER ) )
			newEmitter.endColour2.A	= static_cast<float>( script::LuaGetNumberFromTableItem( "EndColour2A", 1, 0.0 ) );

		//if( Resources::AddEmitterResource( newEmitter ) )
		{
			// there's an id clash in the emitters
			DBG_ASSERT(0);
		}
	}

	return(0);
}
