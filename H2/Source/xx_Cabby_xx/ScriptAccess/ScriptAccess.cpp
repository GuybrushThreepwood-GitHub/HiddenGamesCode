
/*===================================================================
	File: ScriptAccess.cpp
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
#include "Resources/TextureResources.h"

#include "ScriptAccess/ScriptDataHolder.h"
#include "ScriptAccess/PhysicsAccess.h"
#include "ScriptAccess/ScriptAccess.h"
#include "ScriptAccess/Cabby/CabbyAccess.h"
#include "ScriptAccess/Cabby/VehicleAccess.h"

namespace cabbyScript
{
	ScriptDataHolder* pScriptData = 0;
}

LuaGlobal scriptGlobals[] = 
{
#ifdef CABBY_LITE
	{ "CABBY_LITE",						static_cast<double>(1.0),								LUA_TNUMBER },
#endif // CABBY_LITE

#ifdef CABBY_FREE
	{ "CABBY_FREE",						static_cast<double>(1.0),								LUA_TNUMBER },
#endif // CABBY_FREE

	// core languages
	{ "LANGUAGE_ENGLISH",				static_cast<double>(core::LANGUAGE_ENGLISH),			LUA_TNUMBER },
	{ "LANGUAGE_FRENCH",				static_cast<double>(core::LANGUAGE_FRENCH),				LUA_TNUMBER },
	{ "LANGUAGE_ITALIAN",				static_cast<double>(core::LANGUAGE_ITALIAN),			LUA_TNUMBER },
	{ "LANGUAGE_GERMAN",				static_cast<double>(core::LANGUAGE_GERMAN),				LUA_TNUMBER },
	{ "LANGUAGE_SPANISH",				static_cast<double>(core::LANGUAGE_SPANISH),			LUA_TNUMBER },

	// used by emitters

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
	{ "GL_LINEAR_MIPMAP_LINEAR",	static_cast<double>(GL_LINEAR_MIPMAP_LINEAR),	LUA_TNUMBER }
};


/////////////////////////////////////////////////////
/// Function: RegisterScriptFunctions
/// Params: [in]dataHolder
///
/////////////////////////////////////////////////////
void RegisterScriptFunctions( ScriptDataHolder& dataHolder )
{
	int i=0;
	cabbyScript::pScriptData = &dataHolder;

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

	script::LuaScripting::GetInstance()->RegisterFunction( "SetSettingsData", ScriptSetSettingsData );

	script::LuaScripting::GetInstance()->RegisterFunction( "SetPlayerMeshDrawState", ScriptSetPlayerMeshDrawState );

	// DEVELOPMENT ONLY
	script::LuaScripting::GetInstance()->RegisterFunction( "SetProfileState", ScriptSetProfileState );

	RegisterCabbyFunctions( dataHolder );
	RegisterVehicleFunctions( dataHolder );
	RegisterPhysicsFunctions( dataHolder );
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
		ScriptDataHolder::DevScriptData devData = cabbyScript::pScriptData->GetDevData();

		// grab the data
		devData.enableDeveloperMenu	= static_cast<bool>(script::LuaGetBoolFromTableItem( "enableDeveloperMenu", 1 )); 
		devData.enablePhysicsDraw	= static_cast<bool>(script::LuaGetBoolFromTableItem( "enablePhysicsDraw", 1 )); 
		devData.enableDebugDraw		= static_cast<bool>(script::LuaGetBoolFromTableItem( "enableDebugDraw", 1 ));
		devData.enableDebugUIDraw	= static_cast<bool>(script::LuaGetBoolFromTableItem( "enableDebugUIDraw", 1 ));
		devData.enableSave			= static_cast<bool>(script::LuaGetBoolFromTableItem( "enableSave", 1, true ));
		devData.isTablet			= static_cast<bool>(script::LuaGetBoolFromTableItem( "isTablet", 1, false ));
		devData.isRetina			= static_cast<bool>(script::LuaGetBoolFromTableItem( "isRetina", 1, false ));
		devData.isPCOnly			= static_cast<bool>(script::LuaGetBoolFromTableItem( "isPCOnly", 1, false ));
		devData.istvOS				= static_cast<bool>(script::LuaGetBoolFromTableItem( "istvOS", 1, false));

		devData.frameLock30			= static_cast<bool>(script::LuaGetBoolFromTableItem( "frameLock30", 1 )); 
		devData.frameLock60			= static_cast<bool>(script::LuaGetBoolFromTableItem( "frameLock60", 1 )); 

		devData.selectionBoxThickness = static_cast<float>(script::LuaGetNumberFromTableItem("selectionBoxThickness", 1));
		devData.selectionBoxExpand = static_cast<float>(script::LuaGetNumberFromTableItem("selectionBoxExpand", 1));
		devData.selectionBoxR		= static_cast<int>(script::LuaGetNumberFromTableItem("selectionBoxR", 1));
		devData.selectionBoxG		= static_cast<int>(script::LuaGetNumberFromTableItem("selectionBoxG", 1));
		devData.selectionBoxB		= static_cast<int>(script::LuaGetNumberFromTableItem("selectionBoxB", 1));

		devData.levelMipmap			= static_cast<bool>(script::LuaGetBoolFromTableItem( "levelMipmap", 1, true )); 
		devData.smoothFiltering		= static_cast<bool>(script::LuaGetBoolFromTableItem( "smoothFiltering", 1 ));
		devData.assetBaseWidth		= static_cast<int>(script::LuaGetNumberFromTableItem( "assetBaseWidth", 1 ));
		devData.assetBaseHeight		= static_cast<int>(script::LuaGetNumberFromTableItem( "assetBaseHeight", 1 ));
		devData.screenWidth			= static_cast<int>(script::LuaGetNumberFromTableItem( "screenWidth", 1 ));
		devData.screenHeight		= static_cast<int>(script::LuaGetNumberFromTableItem( "screenHeight", 1 ));
		devData.bootState			= static_cast<int>(script::LuaGetNumberFromTableItem( "bootState", 1 ));

		devData.useVertexArrays		= static_cast<bool>(script::LuaGetBoolFromTableItem( "useVertexArrays", 1 ));
		devData.developerSaveFileRoot	= script::LuaGetStringFromTableItem( "developerSaveFileRoot", 1 );
		devData.userSaveFileRoot	= script::LuaGetStringFromTableItem( "userSaveFileRoot", 1 );
		devData.language			= static_cast<int>(script::LuaGetNumberFromTableItem( "language", 1 ));
		devData.showPCAdBar			= static_cast<bool>(script::LuaGetBoolFromTableItem( "showPCAdBar", 1 ));

		devData.allowAdvertBarScaling	= static_cast<bool>(script::LuaGetBoolFromTableItem( "allowAdvertBarScaling", 1 ));
		devData.appAdFilterId			= script::LuaGetStringFromTableItem( "appAdFilterId", 1 );
		devData.localAdvertXML			= script::LuaGetStringFromTableItem( "localAdvertXML", 1 );
		devData.externalAdvertXML		= script::LuaGetStringFromTableItem( "externalAdvertXML", 1 );

		devData.disableMeshDraw		= static_cast<bool>(script::LuaGetBoolFromTableItem( "disableMeshDraw", 1 ));
		devData.allowDebugCam		= static_cast<bool>(script::LuaGetBoolFromTableItem( "allowDebugCam", 1 ));
		devData.singleHitKill		= static_cast<bool>(script::LuaGetBoolFromTableItem( "singleHitKill", 1, true ));
		devData.levelTest			= static_cast<bool>(script::LuaGetBoolFromTableItem( "levelTest", 1 ));
		devData.levelTestVehicle	= static_cast<int>(script::LuaGetNumberFromTableItem( "levelTestVehicle", 1 ));
		devData.levelTestName		= script::LuaGetStringFromTableItem( "levelTestName", 1 );
		devData.hiresCustomers		= static_cast<bool>(script::LuaGetBoolFromTableItem( "hiresCustomers", 1 ));

		cabbyScript::pScriptData->SetDevData( devData );
	}

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

		cabbyScript::pScriptData->GetLevelPackList().push_back(pLevelPack);
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
		for( unsigned int i=0; i < cabbyScript::pScriptData->GetLevelPackList().size(); ++i )
		{
			for( int j=0; j < cabbyScript::pScriptData->GetLevelPackList()[i]->packLevelCount; ++j )
			{
				if( std::strcmp( cabbyScript::pScriptData->GetLevelPackList()[i]->pPackLevelInfo[j].levelScriptName, lua_tostring( pState, 1 ) ) == 0 )
				{
					script::LuaCallFunction( cabbyScript::pScriptData->GetLevelPackList()[i]->pPackLevelInfo[j].levelLoadFuncName, 0, 0 );
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
		ScriptDataHolder::LevelScriptData levelData = cabbyScript::pScriptData->GetLevelData();

		// grab the data
		levelData.levelName			= res::GetScriptString( static_cast<int>( script::LuaGetNumberFromTableItem( "levelName", 1 ) ) );

		levelData.levelModel		= static_cast<int>( script::LuaGetNumberFromTableItem( "levelModel", 1 ) ); 
		levelData.levelBackground	= static_cast<int>( script::LuaGetNumberFromTableItem( "levelBackground", 1 ) );
		levelData.levelForeground	= static_cast<int>( script::LuaGetNumberFromTableItem( "levelForeground", 1 ) );

		levelData.levelPhysicsComplex = script::LuaGetStringFromTableItem( "levelPhysicsComplex", 1 ); 
		levelData.levelData = script::LuaGetStringFromTableItem( "levelData", 1 );

		levelData.levelNearClip = static_cast<float>(script::LuaGetNumberFromTableItem( "levelNearClip", 1 )); 
		levelData.levelFarClip = static_cast<float>(script::LuaGetNumberFromTableItem( "levelFarClip", 1 )); 

		levelData.levelCustomerTotal = static_cast<int>( script::LuaGetNumberFromTableItem( "levelCustomerTotal", 1 ) ); 
		levelData.levelCustomerMaxSpawn = static_cast<int>( script::LuaGetNumberFromTableItem( "levelCustomerMaxSpawn", 1 ) ); 

		levelData.levelArcadeTime = static_cast<float>( script::LuaGetNumberFromTableItem( "levelArcadeTime", 1 ) ); 
		levelData.levelArcadeCustomers = static_cast<int>( script::LuaGetNumberFromTableItem( "levelArcadeCustomers", 1 ) ); 
		levelData.levelArcadeFuel = static_cast<int>( script::LuaGetNumberFromTableItem( "levelArcadeFuel", 1 ) ); 
		levelData.levelArcadeMoney = static_cast<int>( script::LuaGetNumberFromTableItem( "levelArcadeMoney", 1 ) ); 

		levelData.levelCameraOffsetX = static_cast<float>( script::LuaGetNumberFromTableItem( "levelCameraOffsetX", 1 ) );
		levelData.levelCameraOffsetY = static_cast<float>( script::LuaGetNumberFromTableItem( "levelCameraOffsetY", 1 ) );
		levelData.levelCameraZ = static_cast<float>( script::LuaGetNumberFromTableItem( "levelCameraZ", 1 ) ); 

		levelData.levelCamCenterY = static_cast<float>( script::LuaGetNumberFromTableItem( "levelCamCenterY", 1 ) );
		levelData.levelCamCenterZ = static_cast<float>( script::LuaGetNumberFromTableItem( "levelCamCenterZ", 1 ) );

		levelData.levelCloseZoom = static_cast<float>( script::LuaGetNumberFromTableItem( "levelCloseZoom", 1 ) );
		levelData.levelCloseZoomSpeed = static_cast<float>( script::LuaGetNumberFromTableItem( "levelCloseZoomSpeed", 1 ) );

		levelData.levelDayLight			= script::LuaGetStringFromTableItem( "levelDayLight", 1 ); 
		levelData.levelAfternoonLight	= script::LuaGetStringFromTableItem( "levelAfternoonLight", 1 ); 
		levelData.levelNightLight		= script::LuaGetStringFromTableItem( "levelNightLight", 1 ); 

		levelData.levelFogDay			= script::LuaGetStringFromTableItem( "levelFogDay", 1 ); 
		levelData.levelFogAfternoon		= script::LuaGetStringFromTableItem( "levelFogAfternoon", 1 ); 
		levelData.levelFogNight			= script::LuaGetStringFromTableItem( "levelFogNight", 1 ); 

		// BG data
		levelData.levelBGPos.X = static_cast<float>( script::LuaGetNumberFromTableItem( "levelBGPosX", 1, 0.0 ) );
		levelData.levelBGPos.Y = static_cast<float>( script::LuaGetNumberFromTableItem( "levelBGPosY", 1, 0.0 ) );
		levelData.levelBGPos.Z = static_cast<float>( script::LuaGetNumberFromTableItem( "levelBGPosZ", 1, 0.0 ) );

		levelData.levelBGDims.Width = static_cast<float>( script::LuaGetNumberFromTableItem( "levelBGWidth", 1, 100.0 ) );
		levelData.levelBGDims.Height = static_cast<float>( script::LuaGetNumberFromTableItem( "levelBGHeight", 1, 100.0 ) );

		levelData.levelBGDayUseFog = static_cast<bool>( script::LuaGetBoolFromTableItem( "levelBGDayUseFog", 1, false ) ); 
		levelData.levelBGTextureDay = static_cast<int>( script::LuaGetNumberFromTableItem( "levelBGDayTexture", 1, -1.0 ) ); 
		
		levelData.levelBGAfternoonUseFog = static_cast<bool>( script::LuaGetBoolFromTableItem( "levelBGAfternoonUseFog", 1, false ) ); 
		levelData.levelBGTextureAfternoon = static_cast<int>( script::LuaGetNumberFromTableItem( "levelBGAfternoonTexture", 1, -1.0 ) ); 

		levelData.levelBGNightUseFog = static_cast<bool>( script::LuaGetBoolFromTableItem( "levelBGNightUseFog", 1, false ) ); 
		levelData.levelBGTextureNight  = static_cast<int>( script::LuaGetNumberFromTableItem( "levelBGNightTexture", 1, -1.0 ) ); 

		// detail
		levelData.detailMap = static_cast<int>(script::LuaGetNumberFromTableItem("detailMap", 1, -1.0));
		levelData.detailMapScaleX = static_cast<float>(script::LuaGetNumberFromTableItem("detailMapScaleX", 1, 1.0));
		levelData.detailMapScaleY = static_cast<float>(script::LuaGetNumberFromTableItem("detailMapScaleY", 1, 1.0));

		cabbyScript::pScriptData->SetLevelData( levelData );
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
		ScriptDataHolder::PlayerScriptData playerData = cabbyScript::pScriptData->GetPlayerData();

		playerData.smoothRotation = static_cast<bool>( script::LuaGetBoolFromTableItem( "smoothRotation", 1 ) ); 
		playerData.conformToNormal = static_cast<bool>( script::LuaGetBoolFromTableItem( "conformToNormal", 1 ) ); 

		cabbyScript::pScriptData->SetPlayerData( playerData );
	}

	return(0);
}

/////////////////////////////////////////////////////
/// Function: ScriptSetSettingsData
/// Params: [in]pState
///
/////////////////////////////////////////////////////
int ScriptSetSettingsData( lua_State* pState )
{
	int type = static_cast<int>( lua_tonumber( pState, 1 ) );
	
	if( lua_istable( pState, 2 ) )
	{	
		switch(type)
		{
			case 0:
			{
				ScriptDataHolder::AtmosphereScriptData atmosphereData = cabbyScript::pScriptData->GetAirSettingsData();

				atmosphereData.gravity.x = static_cast<float>( script::LuaGetNumberFromTableItem( "gravityX", 2 ) );
				atmosphereData.gravity.y = static_cast<float>( script::LuaGetNumberFromTableItem( "gravityY", 2 ) );

				cabbyScript::pScriptData->SetAirSettingsData( atmosphereData );
			}break;
			case 1:
			{
				ScriptDataHolder::AtmosphereScriptData atmosphereData = cabbyScript::pScriptData->GetVacuumSettingsData();

				atmosphereData.gravity.x = static_cast<float>( script::LuaGetNumberFromTableItem( "gravityX", 2 ) );
				atmosphereData.gravity.y = static_cast<float>( script::LuaGetNumberFromTableItem( "gravityY", 2 ) );

				cabbyScript::pScriptData->SetVacuumSettingsData( atmosphereData );
			}break;
			case 2:
			{
				ScriptDataHolder::AtmosphereScriptData atmosphereData = cabbyScript::pScriptData->GetWaterSettingsData();

				atmosphereData.gravity.x = static_cast<float>( script::LuaGetNumberFromTableItem( "gravityX", 2 ) );
				atmosphereData.gravity.y = static_cast<float>( script::LuaGetNumberFromTableItem( "gravityY", 2 ) );

				cabbyScript::pScriptData->SetWaterSettingsData( atmosphereData );
			}break;
			default:
				DBGLOG( "*ERROR* Invalid settings type index for atmosphere data in script 'SetSettingsData'\n" );
				break;
		}
	}

	return(0);
}

/////////////////////////////////////////////////////
/// Function: ScriptSetPlayerMeshDrawState
/// Params: [in]pState
///
/////////////////////////////////////////////////////
int ScriptSetPlayerMeshDrawState( lua_State* pState )
{
	if( lua_isnumber( pState, 1 ) )
	{
		int index = static_cast<int>( lua_tonumber(pState,1) );
		bool state = static_cast<bool>( lua_toboolean(pState,2)!=0 );

		PhysicsWorld::GetPlayer()->SetMeshDrawState( index, state );
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
	return cabbyScript::pScriptData;
}
