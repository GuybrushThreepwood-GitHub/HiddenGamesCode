
/*===================================================================
	File: LightAccess.cpp
	Game: Cabby

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

#include "CabbyConsts.h"

#include "Audio/AudioSystem.h"

#include "Profiles/ProfileManager.h"

#include "Physics/PhysicsWorld.h"
#include "Level/Level.h"
#include "Player/Player.h"

#include "Resources/StringResources.h"

#include "ScriptAccess/ScriptDataHolder.h"
#include "ScriptAccess/Cabby/LightAccess.h"

/////////////////////////////////////////////////////
/// Function: RegisterScriptFunctions
/// Params: [in]dataHolder
///
/////////////////////////////////////////////////////
ScriptDataHolder::LevelLightBlock GetLightData( const char* tableName )
{
	ScriptDataHolder::LevelLightBlock block;
	std::memset( &block, 0, sizeof( ScriptDataHolder::LevelLightBlock ) );
			
	// grab number
	lua_getglobal( script::LuaScripting::GetState(), tableName );
	//lua_gettable( script::LuaScripting::GetState(), -1 ); // key

	// new table is top
	if( lua_istable( script::LuaScripting::GetState(), -1 ) )
	{
		block.useLight = true;

		block.updateLight = static_cast<bool>( script::LuaGetBoolFromTableItem( "levelLightUpdate", 1 ) ); 
		block.showVehicleLight = static_cast<bool>( script::LuaGetBoolFromTableItem( "showVehicleLight", 1 ) );

		block.lightPos.X = static_cast<float>( script::LuaGetNumberFromTableItem( "lightX", 1 ) ); 
		block.lightPos.Y = static_cast<float>( script::LuaGetNumberFromTableItem( "lightY", 1 ) ); 
		block.lightPos.Z = static_cast<float>( script::LuaGetNumberFromTableItem( "lightZ", 1 ) ); 
		block.lightPos.W = static_cast<float>( script::LuaGetNumberFromTableItem( "lightW", 1 ) );

		block.lightDiffuse.R = static_cast<float>( script::LuaGetNumberFromTableItem( "lightDiffColourR", 1 ) / 255.0f ); 
		block.lightDiffuse.G = static_cast<float>( script::LuaGetNumberFromTableItem( "lightDiffColourG", 1 ) / 255.0f ); 
		block.lightDiffuse.B = static_cast<float>( script::LuaGetNumberFromTableItem( "lightDiffColourB", 1 ) / 255.0f ); 
		block.lightDiffuse.A = 1.0f;

		block.lightAmbient.R = static_cast<float>( script::LuaGetNumberFromTableItem( "lightAmbColourR", 1 ) / 255.0f ); 
		block.lightAmbient.G = static_cast<float>( script::LuaGetNumberFromTableItem( "lightAmbColourG", 1 ) / 255.0f ); 
		block.lightAmbient.B = static_cast<float>( script::LuaGetNumberFromTableItem( "lightAmbColourB", 1 ) / 255.0f ); 
		block.lightAmbient.A = 1.0f;

		block.lightSpecular.R = static_cast<float>( script::LuaGetNumberFromTableItem( "lightSpecColourR", 1 ) / 255.0f ); 
		block.lightSpecular.G = static_cast<float>( script::LuaGetNumberFromTableItem( "lightSpecColourG", 1 ) / 255.0f ); 
		block.lightSpecular.B = static_cast<float>( script::LuaGetNumberFromTableItem( "lightSpecColourB", 1 ) / 255.0f );
		block.lightSpecular.A = 1.0f;

		block.lightAttenuation.X = static_cast<float>( script::LuaGetNumberFromTableItem( "lightConstantAttenuation", 1 ) );
		block.lightAttenuation.Y = static_cast<float>( script::LuaGetNumberFromTableItem( "lightLinearAttenuation", 1 ) );
		block.lightAttenuation.Z = static_cast<float>( script::LuaGetNumberFromTableItem( "lightQuadraticAttenuation", 1 ) );
	}
	else
		block.useLight = false;

	lua_pop( script::LuaScripting::GetState(), 1 );

	return block;
}
