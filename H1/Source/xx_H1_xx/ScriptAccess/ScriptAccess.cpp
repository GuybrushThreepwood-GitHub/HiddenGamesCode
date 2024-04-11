
/*===================================================================
	File: ScriptAccess.cpp
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
#include "GameSystems.h"

#include "Level/Level.h"
#include "Player/Player.h"

#include "Resources/StringResources.h"
#include "Resources/TextureResources.h"

#include "ScriptAccess/ScriptDataHolder.h"
#include "ScriptAccess/ScriptAccess.h"
#include "ScriptAccess/H1/H1Access.h"
#include "ScriptAccess/H1/VehicleAccess.h"

namespace h1Script
{
	ScriptDataHolder* pScriptData = 0;
}

LuaGlobal scriptGlobals[] = 
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

	{ "GAMEMODE_RINGRUN",	static_cast<double>(GameSystems::GAMEMODE_RINGRUN),	LUA_TNUMBER },
	{ "GAMEMODE_TARGET",	static_cast<double>(GameSystems::GAMEMODE_TARGET),	LUA_TNUMBER },
	{ "GAMEMODE_FREEFLIGHT",	static_cast<double>(GameSystems::GAMEMODE_FREEFLIGHT),	LUA_TNUMBER }

};


/////////////////////////////////////////////////////
/// Function: RegisterScriptFunctions
/// Params: [in]dataHolder
///
/////////////////////////////////////////////////////
void RegisterScriptFunctions( ScriptDataHolder& dataHolder )
{
	int i=0;
	h1Script::pScriptData = &dataHolder;

	// setup globals
	for( i=0; i < sizeof(scriptGlobals)/sizeof(LuaGlobal); ++i )
	{
		lua_pushnumber( script::LuaScripting::GetState(), scriptGlobals[i].nConstantValue );
		lua_setglobal( script::LuaScripting::GetState(), scriptGlobals[i].szConstantName );
	}
	script::LuaScripting::GetInstance()->RegisterFunction( "SetDevData",		ScriptSetDevData );

	script::LuaScripting::GetInstance()->RegisterFunction( "AddLevelPack", ScriptAddLevelPack );
	script::LuaScripting::GetInstance()->RegisterFunction( "SetStartLevel", ScriptSetStartLevel );

	script::LuaScripting::GetInstance()->RegisterFunction( "SetLevelData", ScriptSetLevelData );
	script::LuaScripting::GetInstance()->RegisterFunction( "SetPlayerData", ScriptSetPlayerData );
	script::LuaScripting::GetInstance()->RegisterFunction( "SetPhysicsData", ScriptSetPhysicsData );

	// DEVELOPMENT ONLY
	script::LuaScripting::GetInstance()->RegisterFunction( "SetProfileState", ScriptSetProfileState );

	RegisterH1Functions( dataHolder );
	RegisterVehicleFunctions( dataHolder );
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
		ScriptDataHolder::DevScriptData devData = h1Script::pScriptData->GetDevData();

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

		devData.allowAdvertBarScaling	= static_cast<bool>(script::LuaGetBoolFromTableItem( "allowAdvertBarScaling", 1 ));
		devData.appAdFilterId			= script::LuaGetStringFromTableItem( "appAdFilterId", 1 );
		devData.localAdvertXML			= script::LuaGetStringFromTableItem( "localAdvertXML", 1 );
		devData.externalAdvertXML		= script::LuaGetStringFromTableItem( "externalAdvertXML", 1 );

		devData.allowDebugCam		= static_cast<bool>(script::LuaGetBoolFromTableItem( "allowDebugCam", 1 ));
		devData.levelTest			= static_cast<bool>(script::LuaGetBoolFromTableItem( "levelTest", 1 ));
		devData.levelTestMode		= static_cast<int>(script::LuaGetNumberFromTableItem( "levelTestMode", 1 ));
		devData.levelTestId			= static_cast<int>(script::LuaGetNumberFromTableItem( "levelTestId", 1 ));
		devData.levelTestVehicle	= static_cast<int>(script::LuaGetNumberFromTableItem( "levelTestVehicle", 1 ));
		devData.levelTestName		= script::LuaGetStringFromTableItem( "levelTestName", 1 );

		h1Script::pScriptData->SetDevData( devData );
	}

	return(0);
}

/////////////////////////////////////////////////////
/// Function: ScriptSetPhysicsData
/// Params: [in]pState
///
/////////////////////////////////////////////////////
int ScriptSetPhysicsData( lua_State* pState )
{
	if( lua_istable( pState, 1 ) )
	{
		ScriptDataHolder::PhysicsSetup physicsData = h1Script::pScriptData->GetPhysicsData();

		// grab the data
		physicsData.gravityX					= static_cast<float>(script::LuaGetNumberFromTableItem( "gravityX", 1 )); 
		physicsData.gravityY					= static_cast<float>(script::LuaGetNumberFromTableItem( "gravityY", 1 )); 
		physicsData.gravityZ					= static_cast<float>(script::LuaGetNumberFromTableItem( "gravityZ", 1 )); 
		physicsData.autoDisableLinearThreshold	= static_cast<float>(script::LuaGetNumberFromTableItem( "autoDisableLinearThreshold", 1 )); 
		physicsData.autoDisableAngularThreshold	= static_cast<float>(script::LuaGetNumberFromTableItem( "autoDisableAngularThreshold", 1 )); 
		physicsData.autoDisableSteps			= static_cast<int>(script::LuaGetNumberFromTableItem( "autoDisableSteps", 1 )); 
		physicsData.autoDisableTime				= static_cast<float>(script::LuaGetNumberFromTableItem( "autoDisableTime", 1 )); 
		physicsData.CFM							= static_cast<float>(script::LuaGetNumberFromTableItem( "CFM", 1 )); 
		physicsData.linearDamping				= static_cast<float>(script::LuaGetNumberFromTableItem( "linearDamping", 1 )); 
		physicsData.angularDamping				= static_cast<float>(script::LuaGetNumberFromTableItem( "angularDamping", 1 )); 
		physicsData.contactMaxCorrectingVel		= static_cast<float>(script::LuaGetNumberFromTableItem( "contactMaxCorrectingVel", 1 )); 
		physicsData.contactSurfaceLayer			= static_cast<float>(script::LuaGetNumberFromTableItem( "contactSurfaceLayer", 1 )); 

		h1Script::pScriptData->SetPhysicsData( physicsData );
		return(0);
	}

	DBG_ASSERT_MSG( 0, "*ERROR* ScriptSetPhysicsData invalid parameter" );
	return(0);
}

/////////////////////////////////////////////////////
/// Function: ScriptAddLevelPack
/// Params: [in]pState
///
/////////////////////////////////////////////////////
int ScriptAddLevelPack( lua_State* pState )
{
	if( lua_istable( pState, 1 ) )
	{
		int i=0;

		ScriptDataHolder::LevelPackData* pLevelPack = 0;
		pLevelPack = new ScriptDataHolder::LevelPackData;
		DBG_ASSERT( pLevelPack != 0 );

		std::memset( pLevelPack, 0, sizeof(ScriptDataHolder::LevelPackData) );

		pLevelPack->packId = static_cast<int>( script::LuaGetNumberFromTableItem( "packId", 1 ) );
		pLevelPack->packLevelCount = static_cast<unsigned int>( script::LuaGetNumberFromTableItem( "packCount", 1 ) );

		pLevelPack->pPackLevelInfo = new ScriptDataHolder::LevelInfo[pLevelPack->packLevelCount];
		DBG_ASSERT( pLevelPack->pPackLevelInfo != 0 );

		for( i=0; i < pLevelPack->packLevelCount; ++i )
		{
			char levelNameId[core::MAX_PATH];
			std::memset( levelNameId, core::MAX_PATH, sizeof(char)*core::MAX_PATH );
			snprintf( levelNameId, core::MAX_PATH, "levelSetupFilename%d", i+1 );

			pLevelPack->pPackLevelInfo[i].levelScriptName = script::LuaGetStringFromTableItem( levelNameId, 1 );
			DBG_ASSERT( pLevelPack->pPackLevelInfo[i].levelScriptName != 0 );

			// load the script for the pack
			script::LoadScript( pLevelPack->pPackLevelInfo[i].levelScriptName );

			std::memset( levelNameId, core::MAX_PATH, sizeof(char)*core::MAX_PATH );
			snprintf( levelNameId, core::MAX_PATH, "levelSetupFunction%d", i+1 );

			pLevelPack->pPackLevelInfo[i].levelLoadFuncName = script::LuaGetStringFromTableItem( levelNameId, 1 );
			DBG_ASSERT( pLevelPack->pPackLevelInfo[i].levelLoadFuncName != 0 );

			std::memset( levelNameId, core::MAX_PATH, sizeof(char)*core::MAX_PATH );
			snprintf( levelNameId, core::MAX_PATH, "levelName%d", i+1 );

			pLevelPack->pPackLevelInfo[i].levelName = res::GetScriptString( static_cast<int>(script::LuaGetNumberFromTableItem( levelNameId, 1 )) );
			DBG_ASSERT( pLevelPack->pPackLevelInfo[i].levelName != 0 );

			std::memset( levelNameId, core::MAX_PATH, sizeof(char)*core::MAX_PATH );
			snprintf( levelNameId, core::MAX_PATH, "liteVersionPurchase%d", i+1 );
			pLevelPack->pPackLevelInfo[i].liteVersionPurchase = static_cast<bool>( script::LuaGetBoolFromTableItem( levelNameId, 1, false ) != 0 );

			std::memset( levelNameId, core::MAX_PATH, sizeof(char)*core::MAX_PATH );
			snprintf( levelNameId, core::MAX_PATH, "liteVersionPurchaseId%d", i+1 );
			pLevelPack->pPackLevelInfo[i].liteVersionPurchaseId = static_cast<int>(script::LuaGetNumberFromTableItem( levelNameId, 1, -1.0 ) );

		}

		h1Script::pScriptData->GetLevelPackList().push_back(pLevelPack);
	}

	return(0);
}

/////////////////////////////////////////////////////
/// Function: ScriptSetStartLevel
/// Params: [in]pState
///
/////////////////////////////////////////////////////
int ScriptSetStartLevel( lua_State* pState )
{
	if( lua_isstring( pState, 1 ) )
	{
		for( unsigned int i=0; i < h1Script::pScriptData->GetLevelPackList().size(); ++i )
		{
			for( int j=0; j < h1Script::pScriptData->GetLevelPackList()[i]->packLevelCount; ++j )
			{
				if( std::strcmp( h1Script::pScriptData->GetLevelPackList()[i]->pPackLevelInfo[j].levelScriptName, lua_tostring( pState, 1 ) ) == 0 )
				{
					script::LuaCallFunction( h1Script::pScriptData->GetLevelPackList()[i]->pPackLevelInfo[j].levelLoadFuncName, 0, 0 );
				}
			}
		}
	}

	return(0);
}

/////////////////////////////////////////////////////
/// Function: ScriptSetLevelData
/// Params: [in]pState
///
/////////////////////////////////////////////////////
int ScriptSetLevelData( lua_State* pState )
{
	if( lua_istable( pState, 1 ) )
	{
		ScriptDataHolder::LevelScriptData levelData = h1Script::pScriptData->GetLevelData();

		// grab the data
		levelData.levelName			= res::GetScriptString( static_cast<int>( script::LuaGetNumberFromTableItem( "levelName", 1 ) ) );

		levelData.levelModel		= static_cast<int>( script::LuaGetNumberFromTableItem( "levelModel", 1 ) ); 

		levelData.levelPhysicsComplex = script::LuaGetStringFromTableItem( "levelPhysicsComplex", 1 ); 
		levelData.levelData = script::LuaGetStringFromTableItem( "levelData", 1 );

		levelData.levelNearClip = static_cast<float>(script::LuaGetNumberFromTableItem( "levelNearClip", 1 )); 
		levelData.levelFarClip = static_cast<float>(script::LuaGetNumberFromTableItem( "levelFarClip", 1 )); 

		levelData.levelDayLight			= script::LuaGetStringFromTableItem( "levelDayLight", 1 ); 
		levelData.levelAfternoonLight	= script::LuaGetStringFromTableItem( "levelAfternoonLight", 1 ); 
		levelData.levelNightLight		= script::LuaGetStringFromTableItem( "levelNightLight", 1 ); 

		levelData.levelFogDay			= script::LuaGetStringFromTableItem( "levelFogDay", 1 ); 
		levelData.levelFogAfternoon		= script::LuaGetStringFromTableItem( "levelFogAfternoon", 1 ); 
		levelData.levelFogNight			= script::LuaGetStringFromTableItem( "levelFogNight", 1 ); 

		levelData.allowSnow				= static_cast<bool>( script::LuaGetBoolFromTableItem( "allowSnow", 1, false) != 0 );
		levelData.snowProbability		= static_cast<int>(script::LuaGetNumberFromTableItem( "snowProbability", 1, 0.0 ) );

		levelData.allowRain				= static_cast<bool>(script::LuaGetBoolFromTableItem( "allowRain", 1, false) != 0 );
		levelData.rainProbability		= static_cast<int>(script::LuaGetNumberFromTableItem( "rainProbability", 1, 0.0 ) );

		levelData.detailMap				= static_cast<int>(script::LuaGetNumberFromTableItem( "detailMap", 1, -1.0 ) );
		levelData.detailMapScaleX		= static_cast<float>(script::LuaGetNumberFromTableItem( "detailMapScaleX", 1, 1.0 ));
		levelData.detailMapScaleY		= static_cast<float>(script::LuaGetNumberFromTableItem( "detailMapScaleY", 1, 1.0 ));

		// BG data
		levelData.levelSkyboxOffset.X = static_cast<float>( script::LuaGetNumberFromTableItem( "levelSkyboxOffsetX", 1, 0.0 ) );
		levelData.levelSkyboxOffset.Y = static_cast<float>( script::LuaGetNumberFromTableItem( "levelSkyboxOffsetY", 1, 0.0 ) );
		levelData.levelSkyboxOffset.Z = static_cast<float>( script::LuaGetNumberFromTableItem( "levelSkyboxOffsetZ", 1, 0.0 ) );

		levelData.levelBGDayUseFog = static_cast<bool>( script::LuaGetBoolFromTableItem( "levelBGDayUseFog", 1, false ) ); 
		levelData.levelBGTextureDay = static_cast<int>( script::LuaGetNumberFromTableItem( "levelBGDayTexture", 1, -1.0 ) ); 
		
		levelData.levelBGAfternoonUseFog = static_cast<bool>( script::LuaGetBoolFromTableItem( "levelBGAfternoonUseFog", 1, false ) ); 
		levelData.levelBGTextureAfternoon = static_cast<int>( script::LuaGetNumberFromTableItem( "levelBGAfternoonTexture", 1, -1.0 ) ); 

		levelData.levelBGNightUseFog = static_cast<bool>( script::LuaGetBoolFromTableItem( "levelBGNightUseFog", 1, false ) ); 
		levelData.levelBGTextureNight  = static_cast<int>( script::LuaGetNumberFromTableItem( "levelBGNightTexture", 1, -1.0 ) ); 

		levelData.objectTargetId1  = static_cast<int>( script::LuaGetNumberFromTableItem( "objectTargetId1", 1, -1.0 ) ); 
		levelData.objectTargetId2  = static_cast<int>( script::LuaGetNumberFromTableItem( "objectTargetId2", 1, -1.0 ) ); 
		levelData.objectTargetId3  = static_cast<int>( script::LuaGetNumberFromTableItem( "objectTargetId3", 1, -1.0 ) ); 

		levelData.coverage = script::LuaGetStringFromTableItem( "coverage", 1 );

		levelData.numUsedSpriteBatches = 0;

		// read the best star scores for the levels
		int i=0;

		lua_pushstring( script::LuaScripting::GetState(), "spriteList" );
		lua_gettable( script::LuaScripting::GetState(), -2 );

		if( lua_istable( script::LuaScripting::GetState(), -1 ) )
		{
			int n = luaL_len( script::LuaScripting::GetState(), -1 );
				
			// go through all the tables in this table
			for( i = 1; i <= n; ++i )
			{
				lua_rawgeti( script::LuaScripting::GetState(), -1, i );
				if( lua_istable( script::LuaScripting::GetState(), -1 ) )
				{
					int paramIndex = 1;
					if( i <= MAX_LEVEL_SPRITES )
					{
						lua_rawgeti( script::LuaScripting::GetState(), -1, paramIndex++ );
						levelData.spriteBatches[i-1].spriteId = static_cast<int>( lua_tonumber( script::LuaScripting::GetState(), -1 ) ) ;
						lua_pop( script::LuaScripting::GetState(), 1 );

						lua_rawgeti( script::LuaScripting::GetState(), -1, paramIndex++ );
						levelData.spriteBatches[i-1].dims.X = static_cast<float>( lua_tonumber( script::LuaScripting::GetState(), -1 ) );
						lua_pop( script::LuaScripting::GetState(), 1 );

						lua_rawgeti( script::LuaScripting::GetState(), -1, paramIndex++ );
						levelData.spriteBatches[i-1].dims.Y = static_cast<float>( lua_tonumber( script::LuaScripting::GetState(), -1 ) );
						lua_pop( script::LuaScripting::GetState(), 1 );

						lua_rawgeti( script::LuaScripting::GetState(), -1, paramIndex++ );
						levelData.spriteBatches[i-1].dims.Z = static_cast<float>( lua_tonumber( script::LuaScripting::GetState(), -1 ) );
						lua_pop( script::LuaScripting::GetState(), 1 );

						lua_rawgeti( script::LuaScripting::GetState(), -1, paramIndex++ );
						levelData.spriteBatches[i-1].range.X = static_cast<float>( lua_tonumber( script::LuaScripting::GetState(), -1 ) );
						lua_pop( script::LuaScripting::GetState(), 1 );

						lua_rawgeti( script::LuaScripting::GetState(), -1, paramIndex++ );
						levelData.spriteBatches[i-1].range.Y = static_cast<float>( lua_tonumber( script::LuaScripting::GetState(), -1 ) );
						lua_pop( script::LuaScripting::GetState(), 1 );

						lua_rawgeti( script::LuaScripting::GetState(), -1, paramIndex++ );
						levelData.spriteBatches[i-1].range.Z = static_cast<float>( lua_tonumber( script::LuaScripting::GetState(), -1 ) );
						lua_pop( script::LuaScripting::GetState(), 1 );
						

						levelData.numUsedSpriteBatches++;
					}
				}
				lua_pop( script::LuaScripting::GetState(), 1 );
			}
		}
		lua_pop( script::LuaScripting::GetState(), 1 );


		h1Script::pScriptData->SetLevelData( levelData );
	}

	return(0);
}

/////////////////////////////////////////////////////
/// Function: ScriptSetPlayerData
/// Params: [in]pState
///
/////////////////////////////////////////////////////
int ScriptSetPlayerData( lua_State* pState )
{
	if( lua_istable( pState, 1 ) )
	{
		ScriptDataHolder::PlayerScriptData playerData = h1Script::pScriptData->GetPlayerData();

		playerData.smoothRotation = static_cast<bool>( script::LuaGetBoolFromTableItem( "smoothRotation", 1 ) ); 
		playerData.conformToNormal = static_cast<bool>( script::LuaGetBoolFromTableItem( "conformToNormal", 1 ) ); 

		h1Script::pScriptData->SetPlayerData( playerData );
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
	return h1Script::pScriptData;
}
