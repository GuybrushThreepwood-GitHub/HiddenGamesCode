
/*===================================================================
	File: WorldAccess.cpp
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
#include "Resources/StringResources.h"
#include "ScriptAccess/ScriptDataHolder.h"

#include "GameSystems.h"

#include "ScriptAccess/WorldAccess.h"

namespace h4Script
{
	ScriptDataHolder* pScriptStageData = 0;
}

/////////////////////////////////////////////////////
/// Function: RegisterStageFunctions
/// Params: [in]dataHolder
///
/////////////////////////////////////////////////////
void RegisterStageFunctions( ScriptDataHolder& dataHolder )
{
	h4Script::pScriptStageData = &dataHolder;

	script::LuaScripting::GetInstance()->RegisterFunction( "SetCameraData",		ScriptSetCameraData );
	script::LuaScripting::GetInstance()->RegisterFunction( "SetStartStage",		ScriptSetStartStage );
	script::LuaScripting::GetInstance()->RegisterFunction( "AddToStages",			ScriptAddToStages );
	script::LuaScripting::GetInstance()->RegisterFunction( "SetActiveStage",		ScriptSetActiveStage );
	script::LuaScripting::GetInstance()->RegisterFunction( "ChangeToStage",		ScriptChangeToStage );

	script::LuaScripting::GetInstance()->RegisterFunction( "LaunchMicroGame",		ScriptLaunchMicroGame );
	script::LuaScripting::GetInstance()->RegisterFunction( "EndTheGame",			ScriptEndTheGame );
}

/////////////////////////////////////////////////////
/// Function: ScriptSetCameraData
/// Params: [in]pState
///
/////////////////////////////////////////////////////
int ScriptSetCameraData( lua_State* pState )
{
	if( lua_istable( pState, 1 ) )
	{
		ScriptDataHolder::CameraSetup cameraData = h4Script::pScriptStageData->GetCameraData();

		// grab the data
		cameraData.cam_posx_offset	= static_cast<float>(script::LuaGetNumberFromTableItem( "cam_posx_offset", 1 )); 
		cameraData.cam_posy_offset	= static_cast<float>(script::LuaGetNumberFromTableItem( "cam_posy_offset", 1 )); 
		cameraData.cam_posz_offset	= static_cast<float>(script::LuaGetNumberFromTableItem( "cam_posz_offset", 1 )); 
		cameraData.cam_targetx_offset	= static_cast<float>(script::LuaGetNumberFromTableItem( "cam_targetx_offset", 1 )); 
		cameraData.cam_targety_offset	= static_cast<float>(script::LuaGetNumberFromTableItem( "cam_targety_offset", 1 )); 
		cameraData.cam_targetz_offset	= static_cast<float>(script::LuaGetNumberFromTableItem( "cam_targetz_offset", 1 )); 

		h4Script::pScriptStageData->SetCameraData( cameraData );
		return(0);
	}

	DBG_ASSERT_MSG( 0, "*ERROR* ScriptSetCameraData invalid parameter" );
	return(0);
}

/////////////////////////////////////////////////////
/// Function: ScriptSetStartStage
/// Params: [in]pState
///
/////////////////////////////////////////////////////
int ScriptSetStartStage( lua_State* pState )
{
	if( lua_isstring( pState, 1 ) )
	{
		for( unsigned int i=0; i < h4Script::pScriptStageData->GetStageSetupsList().size(); ++i )
		{
			if( std::strcmp( h4Script::pScriptStageData->GetStageSetupsList()[i]->stageName, lua_tostring( pState, 1 ) ) == 0 )
			{
				script::LoadScript( h4Script::pScriptStageData->GetStageSetupsList()[i]->stageFile );
				script::LuaCallFunction( h4Script::pScriptStageData->GetStageSetupsList()[i]->stageSetupFunc, 0, 0 );

				// store the data
				h4Script::pScriptStageData->GetActiveStageData().stageName = h4Script::pScriptStageData->GetStageSetupsList()[i]->stageFile;
				h4Script::pScriptStageData->GetActiveStageData().stageSetupFunc = h4Script::pScriptStageData->GetStageSetupsList()[i]->stageSetupFunc;

				// not used normally so set the game data up
				GameSystems::GameSaveData& saveData = GameSystems::GetInstance()->GetSaveData();	

				std::memset( &saveData.lastStageEntry, 0, sizeof(char)*MAX_VARIABLE_NAME );
				snprintf( saveData.lastStageEntry, MAX_VARIABLE_NAME, "%s", h4Script::pScriptStageData->GetStageSetupsList()[i]->stageFile );
				saveData.lastStageEntryDescriptionId = 0;
				saveData.lastStagePosRotId = -1;

				GameSystems::GetInstance()->SetSaveData( saveData );

				return(0);
			}
		}
	}

	DBG_ASSERT_MSG( 0, "*ERROR* ScriptSetStartStage invalid parameter" );
	return(0);
}

/////////////////////////////////////////////////////
/// Function: ScriptAddToStages
/// Params: [in]pState
///
/////////////////////////////////////////////////////
int ScriptAddToStages( lua_State* pState )
{
	if( lua_istable( pState, 1 ) )
	{
		ScriptDataHolder::StageSetup* pSetup = 0;
		pSetup = new ScriptDataHolder::StageSetup;

		DBG_ASSERT_MSG( (pSetup != 0), "Could not allocate stage setup struct memory" );	

		pSetup->stageName = script::LuaGetStringFromTableItem( "stage_name", 1 );
		pSetup->stageFile = script::LuaGetStringFromTableItem( "stage_file", 1 );
		pSetup->stageSetupFunc = script::LuaGetStringFromTableItem( "stage_setup_func", 1 );

		h4Script::pScriptStageData->GetStageSetupsList().push_back(pSetup);

		return(0);
	}

	DBG_ASSERT_MSG( 0, "*ERROR* ScriptAddToStages invalid parameter" );
	return(0);
}

/////////////////////////////////////////////////////
/// Function: ScriptSetActiveStage
/// Params: [in]pState
///
/////////////////////////////////////////////////////
int ScriptSetActiveStage( lua_State* pState )
{
	if( lua_istable( pState, 1 ) )
	{
		ScriptDataHolder::StageDefinition stageData = h4Script::pScriptStageData->GetActiveStageData();

		// grab the data
		stageData.gameLayer = script::LuaGetStringFromTableItem( "gameLayer", 1 );
		stageData.physicsFileB2D = script::LuaGetStringFromTableItem( "physicsFileB2D", 1 );
		stageData.navMeshFile = script::LuaGetStringFromTableItem( "navMeshFile", 1 );
		stageData.typesFile = script::LuaGetStringFromTableItem( "typesFile", 1 );

		stageData.skyboxIndex = static_cast<int>(script::LuaGetNumberFromTableItem( "skyboxIndex", 1, -1 ));

		stageData.magFilter		= static_cast<GLenum>(script::LuaGetNumberFromTableItem( "magFilter", 1 )); 
		stageData.minFilter		= static_cast<GLenum>(script::LuaGetNumberFromTableItem( "minFilter", 1 )); 

		if( stageData.magFilter == 0 )
			stageData.magFilter = GL_NEAREST;
		if( stageData.minFilter == 0 )
			stageData.minFilter = GL_NEAREST_MIPMAP_NEAREST;

		stageData.roomSetupFunc = script::LuaGetStringFromTableItem( "roomSetupFunc", 1 );

		stageData.clearColour.R	= static_cast<float>(script::LuaGetNumberFromTableItem( "clearColourR", 1 )); 
		stageData.clearColour.G	= static_cast<float>(script::LuaGetNumberFromTableItem( "clearColourG", 1 )); 
		stageData.clearColour.B	= static_cast<float>(script::LuaGetNumberFromTableItem( "clearColourB", 1 )); 

		stageData.fogState		= static_cast<bool>(script::LuaGetBoolFromTableItem( "fogState", 1, false ));
		stageData.fogMode		= static_cast<GLenum>(script::LuaGetNumberFromTableItem( "fogMode", 1 ));
		stageData.fogNearClip	= static_cast<float>(script::LuaGetNumberFromTableItem( "fogNearClip", 1 )); 
		stageData.fogFarClip	= static_cast<float>(script::LuaGetNumberFromTableItem( "fogFarClip", 1 )); 
		stageData.fogDensity	= static_cast<float>(script::LuaGetNumberFromTableItem( "fogDensity", 1 )); 
		stageData.fogColour.R	= static_cast<float>(script::LuaGetNumberFromTableItem( "fogColourR", 1 )); 
		stageData.fogColour.G	= static_cast<float>(script::LuaGetNumberFromTableItem( "fogColourG", 1 )); 
		stageData.fogColour.B	= static_cast<float>(script::LuaGetNumberFromTableItem( "fogColourB", 1 )); 

		stageData.fogSetClearToMatch = static_cast<bool>(script::LuaGetBoolFromTableItem( "fogSetClearToMatch", 1, false )); 

		stageData.nearClip		= static_cast<float>(script::LuaGetNumberFromTableItem( "nearClip", 1 )); 
		stageData.farClip		= static_cast<float>(script::LuaGetNumberFromTableItem( "farClip", 1 )); 

		stageData.minEnemies		= static_cast<int>(script::LuaGetNumberFromTableItem( "minEnemies", 1 ));
		stageData.maxEnemies		= static_cast<int>(script::LuaGetNumberFromTableItem( "maxEnemies", 1 )); 

		stageData.minFloatingHeads = static_cast<int>(script::LuaGetNumberFromTableItem( "minFloatingHeads", 1 ));
		stageData.maxFloatingHeads = static_cast<int>(script::LuaGetNumberFromTableItem( "maxFloatingHeads", 1 ));

		stageData.minHangingSpiders = static_cast<int>(script::LuaGetNumberFromTableItem( "minHangingSpiders", 1 ));
		stageData.maxHangingSpiders = static_cast<int>(script::LuaGetNumberFromTableItem( "maxHangingSpiders", 1 ));

		stageData.minAntSpawn = static_cast<int>(script::LuaGetNumberFromTableItem( "minAntSpawn", 1 ));
		stageData.maxAntSpawn = static_cast<int>(script::LuaGetNumberFromTableItem( "maxAntSpawn", 1 ));

		stageData.maxSpawnEnemies	= static_cast<int>(script::LuaGetNumberFromTableItem( "maxSpawnEnemies", 1 )); 

		stageData.roomMaxEmpty	= static_cast<unsigned int>(script::LuaGetNumberFromTableItem( "roomMaxEmpty", 1 )); 
		stageData.roomMaxAmmo	= static_cast<unsigned int>(script::LuaGetNumberFromTableItem( "roomMaxAmmo", 1 )); 
		stageData.roomMaxHealth = static_cast<unsigned int>(script::LuaGetNumberFromTableItem( "roomMaxHealth", 1 )); 
		stageData.roomMaxZombie = static_cast<unsigned int>(script::LuaGetNumberFromTableItem( "roomMaxZombie", 1 )); 

		stageData.dreamState = static_cast<bool>( script::LuaGetBoolFromTableItem( "dreamState", 1, false ) ); 
		stageData.allowGibs = static_cast<bool>( script::LuaGetBoolFromTableItem( "allowGibs", 1, true ) ); 
		stageData.allowNoise = static_cast<bool>( script::LuaGetBoolFromTableItem( "allowNoise", 1, false ) ); 
		stageData.enableFlashlight = static_cast<bool>( script::LuaGetBoolFromTableItem( "enableFlashlight", 1, false ) );

		h4Script::pScriptStageData->SetActiveStageData( stageData );

		return(0);
	}

	DBG_ASSERT_MSG( 0, "*ERROR* ScriptSetActiveStage invalid parameter" );
	return(0);
}

/////////////////////////////////////////////////////
/// Function: ScriptChangeToStage
/// Params: [in]pState
///
/////////////////////////////////////////////////////
int ScriptChangeToStage( lua_State* pState )
{
	if( lua_isstring( pState, 1 ) )
	{
		for( unsigned int i=0; i < h4Script::pScriptStageData->GetStageSetupsList().size(); ++i )
		{
			if( std::strcmp( h4Script::pScriptStageData->GetStageSetupsList()[i]->stageName, lua_tostring( pState, 1 ) ) == 0 )
			{
				// clear the game systems
				const char* levelName = 0;
				int playerSpawnId = -1;
				int descriptionIndex = 0;

				if( lua_isnumber( pState, 2 ) )
				{
					descriptionIndex = static_cast<int>( lua_tonumber( pState, 2 ) );
					levelName = res::GetScriptString( descriptionIndex );
				}
				if( lua_isnumber( pState, 3 ) )
				{
					playerSpawnId = static_cast<int>( lua_tonumber( pState, 3 ) );
				}

				GameSystems::GetInstance()->ChangeStage( h4Script::pScriptStageData->GetStageSetupsList()[i]->stageFile, h4Script::pScriptStageData->GetStageSetupsList()[i]->stageSetupFunc, levelName, descriptionIndex, playerSpawnId );
				return(0);
			}
		}
	}

	DBG_ASSERT_MSG( 0, "*ERROR* ScriptChangeToStage invalid parameter" );
	return(0);
}

/////////////////////////////////////////////////////
/// Function: ScriptLaunchMicroGame
/// Params: [in]pState
///
/////////////////////////////////////////////////////
int ScriptLaunchMicroGame( lua_State* pState )
{
	if( lua_isnumber( pState, 1 ) )
	{
		// tell the game systems to launch micro game
		int microGame = static_cast<int>( lua_tonumber( pState, 1 ) );

		GameSystems::GetInstance()->LaunchMicroGame( microGame, pState );

		return(0);
	}
	
	DBG_ASSERT_MSG( 0, "*ERROR* ScriptLaunchMicroGame invalid parameter" );
	return(0);
}

/////////////////////////////////////////////////////
/// Function: ScriptEndTheGame
/// Params: [in]pState
///
/////////////////////////////////////////////////////
int ScriptEndTheGame( lua_State* pState )
{
	GameSystems::GetInstance()->SetChangeStageFlag( true );
	GameSystems::GetInstance()->SetEndGameState( true );
	return(0);
}

