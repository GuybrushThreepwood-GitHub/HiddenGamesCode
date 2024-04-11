
/*===================================================================
	File: LightAccess.cpp
	Game: AirCadets

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"
#include "MathBase.h"
#include "RenderBase.h"
#include "SoundBase.h"
#include "CollisionBase.h"
#include "Model/ModelCommon.h"
#include "ModelBase.h"
#include "InputBase.h"
#include "ScriptBase.h"

#include "H1Consts.h"

#include "Audio/AudioSystem.h"

#include "Profiles/ProfileManager.h"

#include "Level/Level.h"
#include "Player/Player.h"

#include "Resources/StringResources.h"

#include "ScriptAccess/ScriptDataHolder.h"
#include "ScriptAccess/H1/FogAccess.h"

/////////////////////////////////////////////////////
/// Function: RegisterScriptFunctions
/// Params: [in]dataHolder
///
/////////////////////////////////////////////////////
renderer::TGLFogState GetFogData( const char* tableName )
{
	renderer::TGLFogState block;
	std::memset( &block, 0, sizeof( renderer::TGLFogState ) );
			
	// grab number
	lua_getglobal( script::LuaScripting::GetState(), tableName );

	// new table is top
	if( lua_istable( script::LuaScripting::GetState(), -1 ) )
	{
		block.bFogState = true;
		block.eFogMode = static_cast<GLenum>( script::LuaGetNumberFromTableItem( "fogMode", 1 ) );
		block.fFogDensity = static_cast<float>( script::LuaGetNumberFromTableItem( "fogDensity", 1 ) );
		block.fFogNearClip = static_cast<float>( script::LuaGetNumberFromTableItem( "fogNear", 1 ) );
		block.fFogFarClip = static_cast<float>( script::LuaGetNumberFromTableItem( "fogFar", 1 ) );
		block.vFogColour.R = static_cast<float>( script::LuaGetNumberFromTableItem( "fogR", 1 ) / 255.0f );
		block.vFogColour.G = static_cast<float>( script::LuaGetNumberFromTableItem( "fogG", 1 ) / 255.0f );
		block.vFogColour.B = static_cast<float>( script::LuaGetNumberFromTableItem( "fogB", 1 ) / 255.0f );
	}

	lua_pop( script::LuaScripting::GetState(), 1 );

	return block;
}
