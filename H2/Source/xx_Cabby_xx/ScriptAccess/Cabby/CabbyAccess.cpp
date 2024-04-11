
/*===================================================================
	File: CabbyAccess.cpp
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

#include "Audio/AudioSystem.h"
#include "Resources/EmitterResources.h"

#include "Physics/PhysicsWorld.h"
#include "Level/Level.h"
#include "Player/Player.h"

#include "ScriptAccess/ScriptDataHolder.h"
#include "ScriptAccess/Cabby/CabbyAccess.h"

namespace cabbyScript
{
	ScriptDataHolder* pScriptCabbyData = 0;
}

/////////////////////////////////////////////////////
/// Function: RegisterVehicleFunctions
/// Params: [in]dataHolder
///
/////////////////////////////////////////////////////
void RegisterCabbyFunctions( ScriptDataHolder& dataHolder )
{
	cabbyScript::pScriptCabbyData = &dataHolder;

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
		GameData gameData = cabbyScript::pScriptCabbyData->GetGameData();

		gameData.MAX_ITEMS_PER_PACK		= static_cast<int>(script::LuaGetNumberFromTableItem( "MAX_ITEMS_PER_PACK", 1 ));
		gameData.TOTAL_LEVELS			= static_cast<int>(script::LuaGetNumberFromTableItem( "TOTAL_LEVELS", 1 ));
		gameData.TOTAL_VEHICLES			= static_cast<int>(script::LuaGetNumberFromTableItem( "TOTAL_VEHICLES", 1 ));
		gameData.TOTAL_LEVEL_PACKS		= static_cast<int>(script::LuaGetNumberFromTableItem( "TOTAL_LEVEL_PACKS", 1 ));
		gameData.TOTAL_VEHICLE_PACKS	= static_cast<int>(script::LuaGetNumberFromTableItem( "TOTAL_VEHICLE_PACKS", 1 ));

		gameData.ACHIEVEMENT_ICON_MAXX	= static_cast<float>(script::LuaGetNumberFromTableItem( "ACHIEVEMENT_ICON_MAXX", 1 ));
		gameData.ACHIEVEMENT_ICON_SPEED	= static_cast<float>(script::LuaGetNumberFromTableItem( "ACHIEVEMENT_ICON_SPEED", 1 ));
		gameData.ACHIEVEMENT_ICON_SHOWTIME	= static_cast<float>(script::LuaGetNumberFromTableItem( "ACHIEVEMENT_ICON_SHOWTIME", 1 ));

#ifdef CABBY_FREE
		gameData.ADBAR_PHONE_POSX	= static_cast<float>(script::LuaGetNumberFromTableItem( "ADBAR_PHONE_POSX", 1 ));
		gameData.ADBAR_PHONE_POSY	= static_cast<float>(script::LuaGetNumberFromTableItem( "ADBAR_PHONE_POSY", 1 ));
		gameData.ADBAR_TABLET_POSX	= static_cast<float>(script::LuaGetNumberFromTableItem( "ADBAR_TABLET_POSX", 1 ));
		gameData.ADBAR_TABLET_POSY	= static_cast<float>(script::LuaGetNumberFromTableItem( "ADBAR_TABLET_POSY", 1 ));
#endif // CABBY_FREE

		// ui
		gameData.SPEECHBUBBLE_XOFFSET_PHONE_SD	= static_cast<float>(script::LuaGetNumberFromTableItem( "SPEECHBUBBLE_XOFFSET_PHONE_SD", 1 ));
		gameData.SPEECHBUBBLE_YOFFSET_PHONE_SD	= static_cast<float>(script::LuaGetNumberFromTableItem( "SPEECHBUBBLE_YOFFSET_PHONE_SD", 1 ));
		gameData.SPEECHBUBBLE_XOFFSET_PHONE_RETINA	= static_cast<float>(script::LuaGetNumberFromTableItem( "SPEECHBUBBLE_XOFFSET_PHONE_RETINA", 1 ));
		gameData.SPEECHBUBBLE_YOFFSET_PHONE_RETINA	= static_cast<float>(script::LuaGetNumberFromTableItem( "SPEECHBUBBLE_YOFFSET_PHONE_RETINA", 1 ));
		gameData.SPEECHBUBBLE_XOFFSET_TABLET_SD	= static_cast<float>(script::LuaGetNumberFromTableItem( "SPEECHBUBBLE_XOFFSET_TABLET_SD", 1 ));
		gameData.SPEECHBUBBLE_YOFFSET_TABLET_SD	= static_cast<float>(script::LuaGetNumberFromTableItem( "SPEECHBUBBLE_YOFFSET_TABLET_SD", 1 ));

		gameData.FUEL_BAR_WIDTH		= static_cast<float>(script::LuaGetNumberFromTableItem( "FUEL_BAR_WIDTH", 1 ));
		gameData.FUEL_BAR_HEIGHT	= static_cast<float>(script::LuaGetNumberFromTableItem( "FUEL_BAR_HEIGHT", 1 ));

		gameData.ICON_LOWEST_POS	= static_cast<float>(script::LuaGetNumberFromTableItem( "ICON_LOWEST_POS", 1 ));
		gameData.ICON_HIGHEST_POS	= static_cast<float>(script::LuaGetNumberFromTableItem( "ICON_HIGHEST_POS", 1 ));
		gameData.ICON_EDGE_CLOSEST	= static_cast<float>(script::LuaGetNumberFromTableItem( "ICON_EDGE_CLOSEST", 1 ));

		gameData.TOWTRUCK_FLYBY_SPEED	= static_cast<float>(script::LuaGetNumberFromTableItem( "TOWTRUCK_FLYBY_SPEED", 1 ));

		// playlist
		gameData.MAX_PLAYLISTNAME_CHARACTERS	= static_cast<int>(script::LuaGetNumberFromTableItem( "MAX_PLAYLISTNAME_CHARACTERS", 1 ));
		gameData.MAX_SONGNAME_CHARACTERS		= static_cast<int>(script::LuaGetNumberFromTableItem( "MAX_SONGNAME_CHARACTERS", 1 ));

		// night sprites
		gameData.NIGHT_SPRITES_START			= static_cast<int>(script::LuaGetNumberFromTableItem( "NIGHT_SPRITES_START", 1 ));

		// game
		gameData.ARCADE_TIME					= static_cast<float>(script::LuaGetNumberFromTableItem( "ARCADE_TIME", 1 ));
		gameData.ARCADE_CUSTOMERS				= static_cast<int>(script::LuaGetNumberFromTableItem( "ARCADE_CUSTOMERS", 1 ));
		gameData.ARCADE_FUEL					= static_cast<int>(script::LuaGetNumberFromTableItem( "ARCADE_FUEL", 1 ));
		gameData.ARCADE_MONEY					= static_cast<int>(script::LuaGetNumberFromTableItem( "ARCADE_MONEY", 1 ));
		gameData.ARCADE_STAR_ROTATION			= static_cast<float>(script::LuaGetNumberFromTableItem( "ARCADE_STAR_ROTATION", 1 ));
		gameData.ARCADE_STAR_COLLISION_RADIUS	= static_cast<float>(script::LuaGetNumberFromTableItem( "ARCADE_STAR_COLLISION_RADIUS", 1 ));

		gameData.FARE_LOW				= static_cast<int>(script::LuaGetNumberFromTableItem( "FARE_LOW", 1 ));
		gameData.FARE_MEDIUM			= static_cast<int>(script::LuaGetNumberFromTableItem( "FARE_MEDIUM", 1 ));
		gameData.FARE_HIGH				= static_cast<int>(script::LuaGetNumberFromTableItem( "FARE_HIGH", 1 ));
		gameData.FARE_COURIER			= static_cast<int>(script::LuaGetNumberFromTableItem( "FARE_COURIER", 1 ));
		gameData.FARE_PERFECT			= static_cast<int>(script::LuaGetNumberFromTableItem( "FARE_PERFECT", 1 ));
		gameData.FARE_MINIMUM			= static_cast<int>(script::LuaGetNumberFromTableItem( "FARE_MINIMUM", 1 ));

		gameData.WAIT_DECREASE_SECS		= static_cast<int>(script::LuaGetNumberFromTableItem( "WAIT_DECREASE_SECS", 1 ));
		gameData.WAIT_FARE_LOSS			= static_cast<int>(script::LuaGetNumberFromTableItem( "WAIT_FARE_LOSS", 1 ));

		gameData.TRAVEL_DECREASE_SECS	= static_cast<int>(script::LuaGetNumberFromTableItem( "TRAVEL_DECREASE_SECS", 1 ));
		gameData.TRAVEL_FARE_LOSS		= static_cast<int>(script::LuaGetNumberFromTableItem( "TRAVEL_FARE_LOSS", 1 ));

		gameData.ROUGHLANDING_FARE_LOSS		= static_cast<int>(script::LuaGetNumberFromTableItem( "ROUGHLANDING_FARE_LOSS", 1 ));

		gameData.PASSENGER_LOSS				= static_cast<int>(script::LuaGetNumberFromTableItem( "PASSENGER_LOSS", 1 ));
		gameData.PARCEL_LOSS				= static_cast<int>(script::LuaGetNumberFromTableItem( "PARCEL_LOSS", 1 ));

		gameData.FUEL_CHARGE				= static_cast<int>(script::LuaGetNumberFromTableItem( "FUEL_CHARGE", 1 ));
		gameData.TOW_TRUCK_CHARGE			= static_cast<int>(script::LuaGetNumberFromTableItem( "TOW_TRUCK_CHARGE", 1 ));

		gameData.NEW_LIFE_AWARD				= static_cast<int>(script::LuaGetNumberFromTableItem( "NEW_LIFE_AWARD", 1 ));
		gameData.CONTINUE_COST				= static_cast<int>(script::LuaGetNumberFromTableItem( "CONTINUE_COST", 1 ));

		gameData.DELIVERY_VAN_UNLOCK_TOTALPARCELS	= static_cast<int>(script::LuaGetNumberFromTableItem( "DELIVERY_VAN_UNLOCK_TOTALPARCELS", 1 ));
		gameData.HOTROD_UNLOCK_TOTALFUEL			= static_cast<int>(script::LuaGetNumberFromTableItem( "HOTROD_UNLOCK_TOTALFUEL", 1 ));

		gameData.EXTRALIFE_REWARD				= static_cast<int>(script::LuaGetNumberFromTableItem( "EXTRALIFE_REWARD", 1 ));

		gameData.CUSTOMER_SPAWN_TIME			= static_cast<float>(script::LuaGetNumberFromTableItem( "CUSTOMER_SPAWN_TIME", 1 ));

		gameData.WORLD_DAMAGE_SPEED_MASSIVE		= static_cast<float>(script::LuaGetNumberFromTableItem( "WORLD_DAMAGE_SPEED_MASSIVE", 1 ));
		gameData.WORLD_DAMAGE_SPEED_LARGE		= static_cast<float>(script::LuaGetNumberFromTableItem( "WORLD_DAMAGE_SPEED_LARGE", 1 ));

		gameData.PORT_DAMAGE_SPEED_MASSIVE		= static_cast<float>(script::LuaGetNumberFromTableItem( "PORT_DAMAGE_SPEED_MASSIVE", 1 ));
		gameData.PORT_DAMAGE_SPEED_LARGE		= static_cast<float>(script::LuaGetNumberFromTableItem( "PORT_DAMAGE_SPEED_LARGE", 1 ));

		// player
		gameData.DEFAULT_LIVES_COUNT		= static_cast<int>(script::LuaGetNumberFromTableItem( "DEFAULT_LIVES_COUNT", 1 ));
		gameData.VERTICAL_THRUSTERS			= static_cast<int>(script::LuaGetNumberFromTableItem( "VERTICAL_THRUSTERS", 1 ));
		gameData.ROTATION_INCREMENT			= static_cast<float>(script::LuaGetNumberFromTableItem( "ROTATION_INCREMENT", 1 ));
		gameData.ROTATION_INCREMENT_CONFORM	= static_cast<float>(script::LuaGetNumberFromTableItem( "ROTATION_INCREMENT_CONFORM", 1 ));
		gameData.ROTATION_TOLERANCE			= static_cast<float>(script::LuaGetNumberFromTableItem( "ROTATION_TOLERANCE", 1 ));
		gameData.MAX_TILT_ANGLE				= static_cast<float>(script::LuaGetNumberFromTableItem( "MAX_TILT_ANGLE", 1 ));
		gameData.MAX_TILT_LAND_ANGLE		= static_cast<float>(script::LuaGetNumberFromTableItem( "MAX_TILT_LAND_ANGLE", 1 ));
		gameData.TILT_INCREMENT				= static_cast<float>(script::LuaGetNumberFromTableItem( "TILT_INCREMENT", 1 ));
		gameData.TILT_INCREMENT_CONFORM		= static_cast<float>(script::LuaGetNumberFromTableItem( "TILT_INCREMENT_CONFORM", 1 ));
		gameData.TILT_TOLERANCE				= static_cast<float>(script::LuaGetNumberFromTableItem( "TILT_TOLERANCE", 1 ));
		gameData.TILT_PUSH_FORCE			= static_cast<float>(script::LuaGetNumberFromTableItem( "TILT_PUSH_FORCE", 1 ));
		gameData.MIN_VEL					= static_cast<float>(script::LuaGetNumberFromTableItem( "MIN_VEL", 1 ));
		gameData.SLOW_DOWN					= static_cast<float>(script::LuaGetNumberFromTableItem( "SLOW_DOWN", 1 ));
		gameData.MAX_FUEL					= static_cast<int>(script::LuaGetNumberFromTableItem( "MAX_FUEL", 1 ));
		gameData.FUEL_LOSS_TIME				= static_cast<float>(script::LuaGetNumberFromTableItem( "FUEL_LOSS_TIME", 1 ));
		gameData.FUEL_INCREASE_TIME			= static_cast<float>(script::LuaGetNumberFromTableItem( "FUEL_INCREASE_TIME", 1 ));
		gameData.LOW_FUEL					= static_cast<int>(script::LuaGetNumberFromTableItem( "LOW_FUEL", 1 ));
		gameData.MEDIUM_FUEL				= static_cast<int>(script::LuaGetNumberFromTableItem( "MEDIUM_FUEL", 1 ));
		gameData.NEAR_LOW_FUEL				= static_cast<int>(script::LuaGetNumberFromTableItem( "NEAR_LOW_FUEL", 1 ));
		gameData.SPLASH_TIMER				= static_cast<float>(script::LuaGetNumberFromTableItem( "SPLASH_TIMER", 1 ));
		gameData.PORT_NUMBER_TIMER			= static_cast<float>(script::LuaGetNumberFromTableItem( "PORT_NUMBER_TIMER", 1 ));
		gameData.CRASH_ROTATION				= static_cast<float>(script::LuaGetNumberFromTableItem( "CRASH_ROTATION", 1 ));

		// customers
		gameData.CUSTOMER_MALE_MESH_HI		= static_cast<int>(script::LuaGetNumberFromTableItem( "CUSTOMER_MALE_MESH_HI", 1 ));
		gameData.CUSTOMER_FEMALE_MESH_HI	= static_cast<int>(script::LuaGetNumberFromTableItem( "CUSTOMER_FEMALE_MESH_HI", 1 ));
		gameData.CUSTOMER_PARCEL_MESH_HI	= static_cast<int>(script::LuaGetNumberFromTableItem( "CUSTOMER_PARCEL_MESH_HI", 1 ));

		gameData.CUSTOMER_MALE_MESH_LOW		= static_cast<int>(script::LuaGetNumberFromTableItem( "CUSTOMER_MALE_MESH_LOW", 1 ));
		gameData.CUSTOMER_FEMALE_MESH_LOW	= static_cast<int>(script::LuaGetNumberFromTableItem( "CUSTOMER_FEMALE_MESH_LOW", 1 ));
		gameData.CUSTOMER_PARCEL_MESH_LOW	= static_cast<int>(script::LuaGetNumberFromTableItem( "CUSTOMER_PARCEL_MESH_LOW", 1 ));

		gameData.CUSTOMER_MODEL_TEXTURE_START	= static_cast<int>(script::LuaGetNumberFromTableItem( "CUSTOMER_MODEL_TEXTURE_START", 1 ));
		gameData.CUSTOMER_MODEL_TEXTURE_END		= static_cast<int>(script::LuaGetNumberFromTableItem( "CUSTOMER_MODEL_TEXTURE_END", 1 ));

		gameData.CUSTOMER_PARCEL_TEXTURE_START				= static_cast<int>(script::LuaGetNumberFromTableItem( "CUSTOMER_PARCEL_TEXTURE_START", 1 ));
		gameData.CUSTOMER_PARCEL_TEXTURE_END				= static_cast<int>(script::LuaGetNumberFromTableItem( "CUSTOMER_PARCEL_TEXTURE_END", 1 ));

		gameData.CUSTOMER_DEPTH				= static_cast<float>(script::LuaGetNumberFromTableItem( "CUSTOMER_DEPTH", 1 ));
		gameData.CUSTOMER_DROPOFF_DEPTH		= static_cast<float>(script::LuaGetNumberFromTableItem( "CUSTOMER_DROPOFF_DEPTH", 1 ));
		gameData.CUSTOMER_WALK_SPEED		= static_cast<float>(script::LuaGetNumberFromTableItem( "CUSTOMER_WALK_SPEED", 1 ));
		gameData.CUSTOMER_CLOSE_TO_TAXI		= static_cast<float>(script::LuaGetNumberFromTableItem( "CUSTOMER_CLOSE_TO_TAXI", 1 ));
		gameData.LAND_TIME_ANGRY			= static_cast<float>(script::LuaGetNumberFromTableItem( "LAND_TIME_ANGRY", 1 ));
		gameData.ANGRY_TIME					= static_cast<float>(script::LuaGetNumberFromTableItem( "ANGRY_TIME", 1 ));

		gameData.CUSTOMER_SWEAR_BUBBLE_OFFSETX_PHONE_SD	= static_cast<float>(script::LuaGetNumberFromTableItem( "CUSTOMER_SWEAR_BUBBLE_OFFSETX_PHONE_SD", 1 ));
		gameData.CUSTOMER_SWEAR_BUBBLE_OFFSETY_PHONE_SD	= static_cast<float>(script::LuaGetNumberFromTableItem( "CUSTOMER_SWEAR_BUBBLE_OFFSETY_PHONE_SD", 1 ));
		gameData.CUSTOMER_SWEAR_BUBBLE_OFFSETX_PHONE_RETINA	= static_cast<float>(script::LuaGetNumberFromTableItem( "CUSTOMER_SWEAR_BUBBLE_OFFSETX_PHONE_RETINA", 1 ));
		gameData.CUSTOMER_SWEAR_BUBBLE_OFFSETY_PHONE_RETINA	= static_cast<float>(script::LuaGetNumberFromTableItem( "CUSTOMER_SWEAR_BUBBLE_OFFSETY_PHONE_RETINA", 1 ));

		gameData.CUSTOMER_SWEAR_BUBBLE_OFFSETX_TABLET_SD	= static_cast<float>(script::LuaGetNumberFromTableItem( "CUSTOMER_SWEAR_BUBBLE_OFFSETX_TABLET_SD", 1 ));
		gameData.CUSTOMER_SWEAR_BUBBLE_OFFSETY_TABLET_SD	= static_cast<float>(script::LuaGetNumberFromTableItem( "CUSTOMER_SWEAR_BUBBLE_OFFSETY_TABLET_SD", 1 ));

		// scene
		gameData.BG_QUAD_WIDTH		= static_cast<float>(script::LuaGetNumberFromTableItem( "BG_QUAD_WIDTH", 1 ));
		gameData.BG_QUAD_HEIGHT		= static_cast<float>(script::LuaGetNumberFromTableItem( "BG_QUAD_HEIGHT", 1 ));

		gameData.BG_POS_X_DEFAULT		= static_cast<float>(script::LuaGetNumberFromTableItem( "BG_POS_X_DEFAULT", 1 ));
		gameData.BG_POS_Y_DEFAULT		= static_cast<float>(script::LuaGetNumberFromTableItem( "BG_POS_Y_DEFAULT", 1 ));
		gameData.BG_POS_Z_DEFAULT		= static_cast<float>(script::LuaGetNumberFromTableItem( "BG_POS_Z_DEFAULT", 1 ));

		gameData.ALLOW_DETAIL_MAPPING = static_cast<bool>(script::LuaGetBoolFromTableItem("ALLOW_DETAIL_MAPPING", 1, false));

		cabbyScript::pScriptCabbyData->SetGameData( gameData );
	}

	return(0);
}

