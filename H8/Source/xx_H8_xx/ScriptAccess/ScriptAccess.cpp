
/*===================================================================
	File: ScriptAccess.cpp
	Game: H8

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

#include "H8Consts.h"

#include "Audio/AudioSystem.h"

#include "Profiles/ProfileManager.h"
#include "GameSystems.h"

#include "Player/Player.h"

#include "Resources/StringResources.h"
#include "Resources/TextureResources.h"

#include "ScriptAccess/ScriptDataHolder.h"
#include "ScriptAccess/ScriptAccess.h"
#include "ScriptAccess/H8/H8Access.h"

namespace hScript
{
	ScriptDataHolder* pScriptData = 0;
}

script::LuaGlobal scriptGlobals[] = 
{
	// core languages
	{ "LANGUAGE_ENGLISH",				static_cast<double>(core::LANGUAGE_ENGLISH),			LUA_TNUMBER },
	{ "LANGUAGE_FRENCH",				static_cast<double>(core::LANGUAGE_FRENCH),				LUA_TNUMBER },
	{ "LANGUAGE_ITALIAN",				static_cast<double>(core::LANGUAGE_ITALIAN),			LUA_TNUMBER },
	{ "LANGUAGE_GERMAN",				static_cast<double>(core::LANGUAGE_GERMAN),				LUA_TNUMBER },
	{ "LANGUAGE_SPANISH",				static_cast<double>(core::LANGUAGE_SPANISH),			LUA_TNUMBER },

	// used by emitters
	{ "GL_EXP",		static_cast<double>(GL_EXP),	LUA_TNUMBER },
	{ "GL_EXP2",	static_cast<double>(GL_EXP2),	LUA_TNUMBER },

	// depth/alpha
	{ "GL_NEVER",		static_cast<double>(GL_NEVER),		LUA_TNUMBER }, 
	{ "GL_LESS",		static_cast<double>(GL_LESS),		LUA_TNUMBER },
	{ "GL_LEQUAL",		static_cast<double>(GL_LEQUAL),		LUA_TNUMBER },
	{ "GL_EQUAL",		static_cast<double>(GL_EQUAL),		LUA_TNUMBER },
	{ "GL_GREATER",		static_cast<double>(GL_GREATER),	LUA_TNUMBER },
	{ "GL_NOTEQUAL",	static_cast<double>(GL_NOTEQUAL),	LUA_TNUMBER },
	{ "GL_GEQUAL",		static_cast<double>(GL_GEQUAL),		LUA_TNUMBER },
	{ "GL_ALWAYS",		static_cast<double>(GL_ALWAYS),		LUA_TNUMBER },

	// blend
	{ "GL_ZERO",				static_cast<double>(GL_ZERO),				LUA_TNUMBER },
	{ "GL_ONE",					static_cast<double>(GL_ONE),				LUA_TNUMBER },
	{ "GL_SRC_COLOR",			static_cast<double>(GL_SRC_COLOR),			LUA_TNUMBER },
	{ "GL_ONE_MINUS_SRC_COLOR",	static_cast<double>(GL_ONE_MINUS_SRC_COLOR),LUA_TNUMBER },
	{ "GL_DST_COLOR",			static_cast<double>(GL_DST_COLOR),			LUA_TNUMBER },
	{ "GL_ONE_MINUS_DST_COLOR",	static_cast<double>(GL_ONE_MINUS_DST_COLOR),LUA_TNUMBER },
	{ "GL_SRC_ALPHA",			static_cast<double>(GL_SRC_ALPHA),			LUA_TNUMBER },
	{ "GL_ONE_MINUS_SRC_ALPHA",	static_cast<double>(GL_ONE_MINUS_SRC_ALPHA),LUA_TNUMBER },
	{ "GL_DST_ALPHA",			static_cast<double>(GL_DST_ALPHA),			LUA_TNUMBER },
	{ "GL_ONE_MINUS_DST_ALPHA",	static_cast<double>(GL_ONE_MINUS_DST_ALPHA),LUA_TNUMBER },
	{ "GL_SRC_ALPHA_SATURATE",	static_cast<double>(GL_SRC_ALPHA_SATURATE),	LUA_TNUMBER },


	// texture filters
	{ "GL_NEAREST",	static_cast<double>(GL_NEAREST),	LUA_TNUMBER },
	{ "GL_LINEAR",	static_cast<double>(GL_LINEAR),		LUA_TNUMBER },

	{ "GL_NEAREST_MIPMAP_NEAREST",	static_cast<double>(GL_NEAREST_MIPMAP_NEAREST),	LUA_TNUMBER },
	{ "GL_LINEAR_MIPMAP_NEAREST",	static_cast<double>(GL_LINEAR_MIPMAP_NEAREST),	LUA_TNUMBER },
	{ "GL_NEAREST_MIPMAP_LINEAR",	static_cast<double>(GL_NEAREST_MIPMAP_LINEAR),	LUA_TNUMBER },
	{ "GL_LINEAR_MIPMAP_LINEAR",	static_cast<double>(GL_LINEAR_MIPMAP_LINEAR),	LUA_TNUMBER },

};


/////////////////////////////////////////////////////
/// Function: RegisterScriptFunctions
/// Params: [in]dataHolder
///
/////////////////////////////////////////////////////
void RegisterScriptFunctions( ScriptDataHolder& dataHolder )
{
	int i=0;
	hScript::pScriptData = &dataHolder;

	// setup globals
	for( i=0; i < sizeof(scriptGlobals)/sizeof(script::LuaGlobal); ++i )
	{
		lua_pushnumber( script::LuaScripting::GetState(), scriptGlobals[i].nConstantValue );
		lua_setglobal( script::LuaScripting::GetState(), scriptGlobals[i].szConstantName );
	}
	script::LuaScripting::GetInstance()->RegisterFunction( "SetDevData",		ScriptSetDevData );

	// DEVELOPMENT ONLY
	script::LuaScripting::GetInstance()->RegisterFunction( "SetProfileState", ScriptSetProfileState );

	RegisterH8Functions( dataHolder );
	script::RegisterPhysicsB2DFunctions();
}

/////////////////////////////////////////////////////
/// Function: ScriptSetDevData
/// Params: [in]pState
///
/////////////////////////////////////////////////////
int ScriptSetDevData( lua_State* pState )
{
	if( lua_istable( pState, 1 ) )
	{
		ScriptDataHolder::DevScriptData devData = hScript::pScriptData->GetDevData();

		// grab the data
		devData.enablePhysicsDraw	= static_cast<bool>(script::LuaGetBoolFromTableItem( "enablePhysicsDraw", 1 )); 
		devData.enableDebugDraw		= static_cast<bool>(script::LuaGetBoolFromTableItem( "enableDebugDraw", 1 ));
		devData.enableDebugUIDraw	= static_cast<bool>(script::LuaGetBoolFromTableItem( "enableDebugUIDraw", 1 ));

		devData.isTablet			= static_cast<bool>(script::LuaGetBoolFromTableItem( "isTablet", 1, false ));
		devData.isRetina			= static_cast<bool>(script::LuaGetBoolFromTableItem( "isRetina", 1, false ));
		devData.isPCOnly			= static_cast<bool>(script::LuaGetBoolFromTableItem( "isPCOnly", 1, false ));
		devData.istvOS				= static_cast<bool>(script::LuaGetBoolFromTableItem("istvOS", 1, false));

		devData.frameLock30			= static_cast<bool>(script::LuaGetBoolFromTableItem( "frameLock30", 1 )); 
		devData.frameLock60			= static_cast<bool>(script::LuaGetBoolFromTableItem( "frameLock60", 1 )); 

		devData.selectionBoxThickness = static_cast<float>(script::LuaGetNumberFromTableItem("selectionBoxThickness", 1));
		devData.selectionBoxExpand = static_cast<float>(script::LuaGetNumberFromTableItem("selectionBoxExpand", 1));
		devData.selectionBoxR = static_cast<int>(script::LuaGetNumberFromTableItem("selectionBoxR", 1));
		devData.selectionBoxG = static_cast<int>(script::LuaGetNumberFromTableItem("selectionBoxG", 1));
		devData.selectionBoxB = static_cast<int>(script::LuaGetNumberFromTableItem("selectionBoxB", 1));

		devData.assetBaseWidth		= static_cast<int>(script::LuaGetNumberFromTableItem( "assetBaseWidth", 1 ));
		devData.assetBaseHeight		= static_cast<int>(script::LuaGetNumberFromTableItem( "assetBaseHeight", 1 ));
		devData.screenWidth			= static_cast<int>(script::LuaGetNumberFromTableItem( "screenWidth", 1 ));
		devData.screenHeight		= static_cast<int>(script::LuaGetNumberFromTableItem( "screenHeight", 1 ));
		devData.bootState			= static_cast<int>(script::LuaGetNumberFromTableItem( "bootState", 1 ));

		devData.showPCAdBar				= static_cast<bool>(script::LuaGetBoolFromTableItem( "showPCAdBar", 1 ));
		devData.useVertexArrays			= static_cast<bool>(script::LuaGetBoolFromTableItem( "useVertexArrays", 1 ));
		devData.useAABBCull				= static_cast<bool>(script::LuaGetBoolFromTableItem( "useAABBCull", 1 ));
		devData.alphaBlendBatchSprites	= static_cast<bool>(script::LuaGetBoolFromTableItem( "alphaBlendBatchSprites", 1 ));
		devData.alphaBlendValue			= static_cast<float>(script::LuaGetNumberFromTableItem( "alphaBlendValue", 1 ));
		devData.batchSpriteDrawMode		= static_cast<int>(script::LuaGetNumberFromTableItem( "batchSpriteDrawMode", 1 ));
		devData.developerSaveFileRoot	= script::LuaGetStringFromTableItem( "developerSaveFileRoot", 1 );
		devData.userSaveFileRoot		= script::LuaGetStringFromTableItem( "userSaveFileRoot", 1 );
		devData.language				= static_cast<int>(script::LuaGetNumberFromTableItem( "language", 1 ));

		devData.allowDebugCam		= static_cast<bool>(script::LuaGetBoolFromTableItem( "allowDebugCam", 1 ));
		devData.showEnemyLines		= static_cast<bool>(script::LuaGetBoolFromTableItem( "showEnemyLines", 1 ));
		devData.shieldSetup			= script::LuaGetStringFromTableItem( "shieldSetup", 1 );

		hScript::pScriptData->SetDevData( devData );
	}

	return(0);
}

/////////////////////////////////////////////////////
/// Function: ScriptSetProfileState
/// Params: [in]pState
///
/////////////////////////////////////////////////////
int ScriptSetProfileState( lua_State* pState )
{
	ProfileManager::GetInstance()->LoadProfileFromScript( 0, pState );

	return(0);
}

/////////////////////////////////////////////////////
/// Function: GetScriptDataHolder
/// Params: None
///
/////////////////////////////////////////////////////
ScriptDataHolder* GetScriptDataHolder()
{
	return hScript::pScriptData;
}
