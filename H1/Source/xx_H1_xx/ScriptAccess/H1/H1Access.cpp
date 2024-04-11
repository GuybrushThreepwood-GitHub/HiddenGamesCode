
/*===================================================================
	File: H1Access.cpp
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

#include "Audio/AudioSystem.h"
#include "Resources/EmitterResources.h"

#include "Level/Level.h"
#include "Player/Player.h"

#include "ScriptAccess/ScriptDataHolder.h"
#include "ScriptAccess/H1/H1Access.h"

namespace h1Script
{
	ScriptDataHolder* pScriptH1Data = 0;
}

/////////////////////////////////////////////////////
/// Function: RegisterH1Functions
/// Params: [in]dataHolder
///
/////////////////////////////////////////////////////
void RegisterH1Functions( ScriptDataHolder& dataHolder )
{
	h1Script::pScriptH1Data = &dataHolder;

	script::LuaScripting::GetInstance()->RegisterFunction( "SetGameData", ScriptSetGameData );
}

/////////////////////////////////////////////////////
/// Function: ScriptSetGameData
/// Params: [in]pState
///
/////////////////////////////////////////////////////
int ScriptSetGameData( lua_State* pState )
{
	if( lua_istable( pState, 1 ) )
	{
		GameData gameData = h1Script::pScriptH1Data->GetGameData();

		gameData.MAX_ITEMS_PER_PACK		= static_cast<int>(script::LuaGetNumberFromTableItem( "MAX_ITEMS_PER_PACK", 1 ));
		gameData.TOTAL_LEVELS			= static_cast<int>(script::LuaGetNumberFromTableItem( "TOTAL_LEVELS", 1 ));
		gameData.TOTAL_VEHICLES			= static_cast<int>(script::LuaGetNumberFromTableItem( "TOTAL_VEHICLES", 1 ));
		gameData.TOTAL_LEVEL_PACKS		= static_cast<int>(script::LuaGetNumberFromTableItem( "TOTAL_LEVEL_PACKS", 1 ));
		gameData.TOTAL_VEHICLE_PACKS	= static_cast<int>(script::LuaGetNumberFromTableItem( "TOTAL_VEHICLE_PACKS", 1 ));

		gameData.RING_MODE_LAPS			= static_cast<int>(script::LuaGetNumberFromTableItem( "RING_MODE_LAPS", 1 ));

		gameData.TOTAL_LAYOUTS			= static_cast<int>(script::LuaGetNumberFromTableItem( "TOTAL_LAYOUTS", 1 ));

		// ui
		gameData.ACHIEVEMENT_ICON_MAXX	= static_cast<float>(script::LuaGetNumberFromTableItem( "ACHIEVEMENT_ICON_MAXX", 1 ));
		gameData.ACHIEVEMENT_ICON_SPEED	= static_cast<float>(script::LuaGetNumberFromTableItem( "ACHIEVEMENT_ICON_SPEED", 1 ));
		gameData.ACHIEVEMENT_ICON_SHOWTIME	= static_cast<float>(script::LuaGetNumberFromTableItem( "ACHIEVEMENT_ICON_SHOWTIME", 1 ));

		gameData.ADBAR_PHONE_POSX	= static_cast<float>(script::LuaGetNumberFromTableItem( "ADBAR_PHONE_POSX", 1 ));
		gameData.ADBAR_PHONE_POSY	= static_cast<float>(script::LuaGetNumberFromTableItem( "ADBAR_PHONE_POSY", 1 ));
		gameData.ADBAR_TABLET_POSX	= static_cast<float>(script::LuaGetNumberFromTableItem( "ADBAR_TABLET_POSX", 1 ));
		gameData.ADBAR_TABLET_POSY	= static_cast<float>(script::LuaGetNumberFromTableItem( "ADBAR_TABLET_POSY", 1 ));

		// playlist
		gameData.MAX_PLAYLISTNAME_CHARACTERS	= static_cast<int>(script::LuaGetNumberFromTableItem( "MAX_PLAYLISTNAME_CHARACTERS", 1 ));
		gameData.MAX_SONGNAME_CHARACTERS		= static_cast<int>(script::LuaGetNumberFromTableItem( "MAX_SONGNAME_CHARACTERS", 1 ));
		gameData.ANALOGUE_DEAD_ZONE				= static_cast<float>(script::LuaGetNumberFromTableItem( "ANALOGUE_DEAD_ZONE", 1 ));
		
		// camera
		gameData.CAMERA_X_OFFSET				= static_cast<float>(script::LuaGetNumberFromTableItem( "CAMERA_X_OFFSET", 1 ));
		gameData.CAMERA_Y_OFFSET				= static_cast<float>(script::LuaGetNumberFromTableItem( "CAMERA_Y_OFFSET", 1 ));
		gameData.CAMERA_Z_OFFSET				= static_cast<float>(script::LuaGetNumberFromTableItem( "CAMERA_Z_OFFSET", 1 ));
		gameData.CAMERA_TARGET_MULTIPLY			= static_cast<float>(script::LuaGetNumberFromTableItem( "CAMERA_TARGET_MULTIPLY", 1 ));

		// scene
		gameData.FARCLIP_MODIFY					= static_cast<float>(script::LuaGetNumberFromTableItem( "FARCLIP_MODIFY", 1 ));
		gameData.LOD_SCALER						= static_cast<float>(script::LuaGetNumberFromTableItem( "LOD_SCALER", 1 ));
		gameData.SKYBOX_Y_OFFSET_DEFAULT		= static_cast<float>(script::LuaGetNumberFromTableItem( "SKYBOX_Y_OFFSET_DEFAULT", 1 ));

		gameData.MAX_SPRITES_PER_BATCH			= static_cast<int>(script::LuaGetNumberFromTableItem( "MAX_SPRITES_PER_BATCH", 1 ));
		gameData.MAX_SPRITE_DRAW_DISTANCE		= static_cast<float>(script::LuaGetNumberFromTableItem( "MAX_SPRITE_DRAW_DISTANCE", 1 ));
		gameData.SPHERE_DRAW_DISTANCE_ON_PLANE	= static_cast<float>(script::LuaGetNumberFromTableItem( "SPHERE_DRAW_DISTANCE_ON_PLANE", 1 ));

		gameData.ALLOW_DETAIL_MAP				= static_cast<bool>(script::LuaGetBoolFromTableItem( "ALLOW_DETAIL_MAP", 1, true ) != 0 );
		
		gameData.WALL_FADE_START_DISTANCE		= static_cast<float>(script::LuaGetNumberFromTableItem( "WALL_FADE_START_DISTANCE", 1 ));
		gameData.WALL_FADE_IN_INC				= static_cast<int>(script::LuaGetNumberFromTableItem( "WALL_FADE_IN_INC", 1 ));
		gameData.WALL_FADE_OUT_DEC				= static_cast<int>(script::LuaGetNumberFromTableItem( "WALL_FADE_OUT_DEC", 1 ));

		// objects
		gameData.DEFAULT_RING_COLLISION_RADIUS	= static_cast<float>(script::LuaGetNumberFromTableItem( "DEFAULT_RING_COLLISION_RADIUS", 1 ));
		gameData.DEFAULT_TOKEN_COLLISION_RADIUS	= static_cast<float>(script::LuaGetNumberFromTableItem( "DEFAULT_TOKEN_COLLISION_RADIUS", 1 ));
		gameData.RING_PITCH_INC					= static_cast<float>(script::LuaGetNumberFromTableItem( "RING_PITCH_INC", 1 ));

		// planes
		gameData.FLAP_RESET_AUTO_SNAP			= static_cast<float>(script::LuaGetNumberFromTableItem( "FLAP_RESET_AUTO_SNAP", 1 ));
		gameData.BULLET_SIZE					= static_cast<float>(script::LuaGetNumberFromTableItem( "BULLET_SIZE", 1 ));
	
		gameData.SHADOW_OFFSET					= -static_cast<float>(script::LuaGetNumberFromTableItem( "SHADOW_OFFSET", 1 ));

		// game
		gameData.TOKEN_COLLECT_TIME				= static_cast<float>(script::LuaGetNumberFromTableItem( "TOKEN_COLLECT_TIME", 1 ));
		gameData.TOKEN_DRAW_DISTANCE			= static_cast<float>(script::LuaGetNumberFromTableItem( "TOKEN_DRAW_DISTANCE", 1 ));

		gameData.MIN_BOUNDING_X					= static_cast<float>(script::LuaGetNumberFromTableItem( "MIN_BOUNDING_X", 1 ));
		gameData.MIN_BOUNDING_Z					= static_cast<float>(script::LuaGetNumberFromTableItem( "MIN_BOUNDING_Z", 1 ));
	
		gameData.MAX_BOUNDING_X					= static_cast<float>(script::LuaGetNumberFromTableItem( "MAX_BOUNDING_X", 1 ));
		gameData.MAX_BOUNDING_Y					= static_cast<float>(script::LuaGetNumberFromTableItem( "MAX_BOUNDING_Y", 1 ));
		gameData.MAX_BOUNDING_Z					= static_cast<float>(script::LuaGetNumberFromTableItem( "MAX_BOUNDING_Z", 1 ));
		
		gameData.ACH_PLANE_USED_TIMER			= static_cast<float>(script::LuaGetNumberFromTableItem( "ACH_PLANE_USED_TIMER", 1 ));
		gameData.ACH_LEVEL_PLAYED_TIMER			= static_cast<float>(script::LuaGetNumberFromTableItem( "ACH_LEVEL_PLAYED_TIMER", 1 ));
		gameData.ACH_WEATHER_RAINSNOW_TIMER		= static_cast<float>(script::LuaGetNumberFromTableItem( "ACH_WEATHER_RAINSNOW_TIMER", 1 ));
		gameData.ACH_TIMEOFDAY_TIMER			= static_cast<float>(script::LuaGetNumberFromTableItem( "ACH_TIMEOFDAY_TIMER", 1 ));
		gameData.ACH_FREEFLIGHT_MODE_TIMER		= static_cast<float>(script::LuaGetNumberFromTableItem( "ACH_FREEFLIGHT_MODE_TIMER", 1 ));

		gameData.ACH_FLYING_LOW_TIME			= static_cast<float>(script::LuaGetNumberFromTableItem( "ACH_FLYING_LOW_TIME", 1 ));
		gameData.ACH_FLYING_LOW_HEIGHT			= static_cast<float>(script::LuaGetNumberFromTableItem( "ACH_FLYING_LOW_HEIGHT", 1 ));


		gameData.ACH_STUDENT_PILOT				= static_cast<float>(script::LuaGetNumberFromTableItem( "ACH_STUDENT_PILOT", 1 ));
		gameData.ACH_SPORT_PILOT				= static_cast<float>(script::LuaGetNumberFromTableItem( "ACH_SPORT_PILOT", 1 ));
		gameData.ACH_RECREATIONAL_PILOT			= static_cast<float>(script::LuaGetNumberFromTableItem( "ACH_RECREATIONAL_PILOT", 1 ));
		gameData.ACH_PRIVATE_PILOT				= static_cast<float>(script::LuaGetNumberFromTableItem( "ACH_PRIVATE_PILOT", 1 ));
		gameData.ACH_COMMERCIAL_PILOT			= static_cast<float>(script::LuaGetNumberFromTableItem( "ACH_COMMERCIAL_PILOT", 1 ));
		gameData.ACH_AIRLINE_TRANSPORT_PILOT	= static_cast<float>(script::LuaGetNumberFromTableItem( "ACH_AIRLINE_TRANSPORT_PILOT", 1 ));

		// weather
		gameData.ALLOW_WIND_EFFECT				= static_cast<bool>(script::LuaGetBoolFromTableItem( "ALLOW_WIND_EFFECT", 1, true ) != 0 );
		gameData.FLYING_WIND_ENABLE_HEIGHT		= static_cast<float>(script::LuaGetNumberFromTableItem( "FLYING_WIND_ENABLE_HEIGHT", 1 ));
		gameData.FLYING_WIND_DISTANCE_IN_FRONT	= static_cast<float>(script::LuaGetNumberFromTableItem( "FLYING_WIND_DISTANCE_IN_FRONT", 1 ));

		gameData.ALLOW_SNOW_EFFECT				= static_cast<bool>(script::LuaGetBoolFromTableItem( "ALLOW_SNOW_EFFECT", 1, true ) != 0 );
		gameData.SNOW_HEIGHT_ABOVE_PLAYER		= static_cast<float>(script::LuaGetNumberFromTableItem( "SNOW_HEIGHT_ABOVE_PLAYER", 1 ));
		gameData.SNOW_DISTANCE_IN_FRONT			= static_cast<float>(script::LuaGetNumberFromTableItem( "SNOW_DISTANCE_IN_FRONT", 1 ));

		gameData.ALLOW_RAIN_EFFECT				= static_cast<bool>(script::LuaGetBoolFromTableItem( "ALLOW_RAIN_EFFECT", 1, true ) != 0 );
		gameData.RAIN_HEIGHT_ABOVE_PLAYER		= static_cast<float>(script::LuaGetNumberFromTableItem( "RAIN_HEIGHT_ABOVE_PLAYER", 1 ));
		gameData.RAIN_DISTANCE_IN_FRONT			= static_cast<float>(script::LuaGetNumberFromTableItem( "RAIN_DISTANCE_IN_FRONT", 1 ));

		gameData.BOUNDING_EFFECT_DISTANCE_IN_FRONT		= static_cast<float>(script::LuaGetNumberFromTableItem( "BOUNDING_EFFECT_DISTANCE_IN_FRONT", 1 ));

		h1Script::pScriptH1Data->SetGameData( gameData );
	}

	return(0);
}

