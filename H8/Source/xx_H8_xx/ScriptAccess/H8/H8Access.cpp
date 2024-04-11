
/*===================================================================
	File: H8Access.cpp
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

#include "Audio/AudioSystem.h"
#include "Resources/EmitterResources.h"

#include "Player/Player.h"

#include "ScriptAccess/ScriptDataHolder.h"
#include "ScriptAccess/H8/H8Access.h"

namespace hScript
{
	ScriptDataHolder* pScriptHData = 0;
}

/////////////////////////////////////////////////////
/// Function: RegisterH8Functions
/// Params: [in]dataHolder
///
/////////////////////////////////////////////////////
void RegisterH8Functions( ScriptDataHolder& dataHolder )
{
	hScript::pScriptHData = &dataHolder;

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
		GameData gameData = hScript::pScriptHData->GetGameData();

		// ui
		gameData.ACHIEVEMENT_ICON_MAXX	= static_cast<float>(script::LuaGetNumberFromTableItem( "ACHIEVEMENT_ICON_MAXX", 1 ));
		gameData.ACHIEVEMENT_ICON_SPEED	= static_cast<float>(script::LuaGetNumberFromTableItem( "ACHIEVEMENT_ICON_SPEED", 1 ));
		gameData.ACHIEVEMENT_ICON_SHOWTIME	= static_cast<float>(script::LuaGetNumberFromTableItem( "ACHIEVEMENT_ICON_SHOWTIME", 1 ));

		gameData.ADBAR_PHONE_POSX	= static_cast<float>(script::LuaGetNumberFromTableItem( "ADBAR_PHONE_POSX", 1 ));
		gameData.ADBAR_PHONE_POSY	= static_cast<float>(script::LuaGetNumberFromTableItem( "ADBAR_PHONE_POSY", 1 ));
		gameData.ADBAR_PHONEHD_POSX	= static_cast<float>(script::LuaGetNumberFromTableItem( "ADBAR_PHONEHD_POSX", 1 ));
		gameData.ADBAR_PHONEHD_POSY	= static_cast<float>(script::LuaGetNumberFromTableItem( "ADBAR_PHONEHD_POSY", 1 ));
		gameData.ADBAR_TABLET_POSX	= static_cast<float>(script::LuaGetNumberFromTableItem( "ADBAR_TABLET_POSX", 1 ));
		gameData.ADBAR_TABLET_POSY	= static_cast<float>(script::LuaGetNumberFromTableItem( "ADBAR_TABLET_POSY", 1 ));
		gameData.ADBAR_TABLETHD_POSX	= static_cast<float>(script::LuaGetNumberFromTableItem( "ADBAR_TABLETHD_POSX", 1 ));
		gameData.ADBAR_TABLETHD_POSY	= static_cast<float>(script::LuaGetNumberFromTableItem( "ADBAR_TABLETHD_POSY", 1 ));

		// playlist
		gameData.MAX_PLAYLISTNAME_CHARACTERS	= static_cast<int>(script::LuaGetNumberFromTableItem( "MAX_PLAYLISTNAME_CHARACTERS", 1 ));
		gameData.MAX_SONGNAME_CHARACTERS		= static_cast<int>(script::LuaGetNumberFromTableItem( "MAX_SONGNAME_CHARACTERS", 1 ));
		gameData.ANALOGUE_DEAD_ZONE				= static_cast<float>(script::LuaGetNumberFromTableItem( "ANALOGUE_DEAD_ZONE", 1 ));

		gameData.BG_HLAYER1_SPEED				= static_cast<float>(script::LuaGetNumberFromTableItem( "BG_HLAYER1_SPEED", 1 ));
		gameData.BG_HLAYER1_MAX_SPEED			= static_cast<float>(script::LuaGetNumberFromTableItem( "BG_HLAYER1_MAX_SPEED", 1 ));
		gameData.BG_HLAYER1_SPEED_INC_PER_LEVEL	= static_cast<float>(script::LuaGetNumberFromTableItem( "BG_HLAYER1_SPEED_INC_PER_LEVEL", 1 ));
		gameData.BG_HLAYER1_BLEND_SRC			= static_cast<GLenum>(script::LuaGetNumberFromTableItem( "BG_HLAYER1_BLEND_SRC", 1 ));
		gameData.BG_HLAYER1_BLEND_DEST			= static_cast<GLenum>(script::LuaGetNumberFromTableItem( "BG_HLAYER1_BLEND_DEST", 1 ));
	
		gameData.BG_HLAYER2_SPEED				= static_cast<float>(script::LuaGetNumberFromTableItem( "BG_HLAYER2_SPEED", 1 ));
		gameData.BG_HLAYER2_MAX_SPEED			= static_cast<float>(script::LuaGetNumberFromTableItem( "BG_HLAYER2_MAX_SPEED", 1 ));
		gameData.BG_HLAYER2_SPEED_INC_PER_LEVEL	= static_cast<float>(script::LuaGetNumberFromTableItem( "BG_HLAYER2_SPEED_INC_PER_LEVEL", 1 ));
		gameData.BG_HLAYER2_BLEND_SRC			= static_cast<GLenum>(script::LuaGetNumberFromTableItem( "BG_HLAYER2_BLEND_SRC", 1 ));
		gameData.BG_HLAYER2_BLEND_DEST			= static_cast<GLenum>(script::LuaGetNumberFromTableItem( "BG_HLAYER2_BLEND_DEST", 1 ));

		gameData.BG_HLAYER3_SPEED				= static_cast<float>(script::LuaGetNumberFromTableItem( "BG_HLAYER3_SPEED", 1 ));
		gameData.BG_HLAYER3_MAX_SPEED			= static_cast<float>(script::LuaGetNumberFromTableItem( "BG_HLAYER3_MAX_SPEED", 1 ));
		gameData.BG_HLAYER3_SPEED_INC_PER_LEVEL	= static_cast<float>(script::LuaGetNumberFromTableItem( "BG_HLAYER3_SPEED_INC_PER_LEVEL", 1 ));
		gameData.BG_HLAYER3_BLEND_SRC			= static_cast<GLenum>(script::LuaGetNumberFromTableItem( "BG_HLAYER3_BLEND_SRC", 1 ));
		gameData.BG_HLAYER3_BLEND_DEST			= static_cast<GLenum>(script::LuaGetNumberFromTableItem( "BG_HLAYER3_BLEND_DEST", 1 ));	
	
		gameData.BG_VLAYER1_SPEED				= static_cast<float>(script::LuaGetNumberFromTableItem( "BG_VLAYER1_SPEED", 1 ));
		gameData.BG_VLAYER1_MAX_SPEED			= static_cast<float>(script::LuaGetNumberFromTableItem( "BG_VLAYER1_MAX_SPEED", 1 ));
		gameData.BG_VLAYER1_SPEED_INC_PER_LEVEL	= static_cast<float>(script::LuaGetNumberFromTableItem( "BG_VLAYER1_SPEED_INC_PER_LEVEL", 1 ));
		gameData.BG_VLAYER1_BLEND_SRC			= static_cast<GLenum>(script::LuaGetNumberFromTableItem( "BG_VLAYER1_BLEND_SRC", 1 ));
		gameData.BG_VLAYER1_BLEND_DEST			= static_cast<GLenum>(script::LuaGetNumberFromTableItem( "BG_VLAYER1_BLEND_DEST", 1 ));
	
		gameData.BG_VLAYER2_SPEED				= static_cast<float>(script::LuaGetNumberFromTableItem( "BG_VLAYER2_SPEED", 1 ));
		gameData.BG_VLAYER2_MAX_SPEED			= static_cast<float>(script::LuaGetNumberFromTableItem( "BG_VLAYER2_MAX_SPEED", 1 ));
		gameData.BG_VLAYER2_SPEED_INC_PER_LEVEL	= static_cast<float>(script::LuaGetNumberFromTableItem( "BG_VLAYER2_SPEED_INC_PER_LEVEL", 1 ));
		gameData.BG_VLAYER2_BLEND_SRC			= static_cast<GLenum>(script::LuaGetNumberFromTableItem( "BG_VLAYER2_BLEND_SRC", 1 ));
		gameData.BG_VLAYER2_BLEND_DEST			= static_cast<GLenum>(script::LuaGetNumberFromTableItem( "BG_VLAYER2_BLEND_DEST", 1 ));

		gameData.BG_VLAYER3_SPEED				= static_cast<float>(script::LuaGetNumberFromTableItem( "BG_VLAYER3_SPEED", 1 ));
		gameData.BG_VLAYER3_MAX_SPEED			= static_cast<float>(script::LuaGetNumberFromTableItem( "BG_VLAYER3_MAX_SPEED", 1 ));
		gameData.BG_VLAYER3_SPEED_INC_PER_LEVEL	= static_cast<float>(script::LuaGetNumberFromTableItem( "BG_VLAYER3_SPEED_INC_PER_LEVEL", 1 ));
		gameData.BG_VLAYER3_BLEND_SRC			= static_cast<GLenum>(script::LuaGetNumberFromTableItem( "BG_VLAYER3_BLEND_SRC", 1 ));
		gameData.BG_VLAYER3_BLEND_DEST			= static_cast<GLenum>(script::LuaGetNumberFromTableItem( "BG_VLAYER3_BLEND_DEST", 1 ));

		// game
		gameData.CAMERA_POS_TITLE_Z				= static_cast<float>(script::LuaGetNumberFromTableItem( "CAMERA_POS_TITLE_Z", 1 ));
		gameData.CAMERA_POS_GAME_Z				= static_cast<float>(script::LuaGetNumberFromTableItem( "CAMERA_POS_GAME_Z", 1 ));

		gameData.INSTRUCTIONS_FADE_TIME			= static_cast<float>(script::LuaGetNumberFromTableItem( "INSTRUCTIONS_FADE_TIME", 1 ));

		gameData.LEVELUP_COUNT					= static_cast<int>(script::LuaGetNumberFromTableItem( "LEVELUP_COUNT", 1 ));
		gameData.LEVEL_SPEED_UP_START			= static_cast<int>(script::LuaGetNumberFromTableItem( "LEVEL_SPEED_UP_START", 1 ));
		gameData.TROJAN_MIN_APPEAR_LEVEL		= static_cast<int>(script::LuaGetNumberFromTableItem( "TROJAN_MIN_APPEAR_LEVEL", 1 ));
		gameData.VIRUS_MIN_APPEAR_LEVEL			= static_cast<int>(script::LuaGetNumberFromTableItem( "VIRUS_MIN_APPEAR_LEVEL", 1 ));

		gameData.TROJAN_APPEAR_PROBABILITY		= static_cast<int>(script::LuaGetNumberFromTableItem( "TROJAN_APPEAR_PROBABILITY", 1 ));
		gameData.VIRUS_APPEAR_PROBABILITY		= static_cast<int>(script::LuaGetNumberFromTableItem( "VIRUS_APPEAR_PROBABILITY", 1 ));

		gameData.DOWNLOADBAR_TIME				= static_cast<float>(script::LuaGetNumberFromTableItem( "DOWNLOADBAR_TIME", 1 ));
		gameData.DOWNLOADBAR_MAX_WIDTH			= static_cast<float>(script::LuaGetNumberFromTableItem( "DOWNLOADBAR_MAX_WIDTH", 1 ));
		gameData.DOWNLOADBAR_MAX_HEIGHT			= static_cast<float>(script::LuaGetNumberFromTableItem( "DOWNLOADBAR_MAX_HEIGHT", 1 ));
		gameData.DOWNLOADBAR_POINT_BONUS		= static_cast<int>(script::LuaGetNumberFromTableItem( "DOWNLOADBAR_POINT_BONUS", 1 ));

		gameData.CONSECUTIVE_BONUS				= static_cast<int>(script::LuaGetNumberFromTableItem( "CONSECUTIVE_BONUS", 1 ));
		gameData.COMPLETE_ROUTINE				= static_cast<int>(script::LuaGetNumberFromTableItem( "COMPLETE_ROUTINE", 1 ));

		// core
		gameData.CORE_RADIUS				= static_cast<float>(script::LuaGetNumberFromTableItem( "CORE_RADIUS", 1 ));
		gameData.CORE_START_HEALTH			= static_cast<int>(script::LuaGetNumberFromTableItem( "CORE_START_HEALTH", 1 ));

		gameData.CORE_CHANGE_TIME_MIN		= static_cast<float>(script::LuaGetNumberFromTableItem( "CORE_CHANGE_TIME_MIN", 1 ));
		gameData.CORE_CHANGE_TIME_MAX		= static_cast<float>(script::LuaGetNumberFromTableItem( "CORE_CHANGE_TIME_MAX", 1 ));

		gameData.CORE_PETAL_COUNT				= static_cast<int>(script::LuaGetNumberFromTableItem( "CORE_PETAL_COUNT", 1 ));

		// shield
		gameData.SHIELD_MASS				= static_cast<float>(script::LuaGetNumberFromTableItem( "SHIELD_MASS", 1 ));
		gameData.SHIELD_MASS_INERTIA		= static_cast<float>(script::LuaGetNumberFromTableItem( "SHIELD_MASS_INERTIA", 1 ));
		gameData.SHIELD_ANGULAR_DAMPING		= static_cast<float>(script::LuaGetNumberFromTableItem( "SHIELD_ANGULAR_DAMPING", 1 ));

		gameData.SHIELD_ROTATIONAL_FORCE			= static_cast<float>(script::LuaGetNumberFromTableItem( "SHIELD_ROTATIONAL_FORCE", 1 ));
		gameData.SHIELD_MINIMAL_PIXEL_MOVEMENT		= static_cast<float>(script::LuaGetNumberFromTableItem( "SHIELD_MINIMAL_PIXEL_MOVEMENT", 1 ));

		gameData.SHIELD_DISABLE_TIME		= static_cast<float>(script::LuaGetNumberFromTableItem( "SHIELD_DISABLE_TIME", 1 ));
		gameData.SHIELD_PIECE_ANIM_TIME		= static_cast<float>(script::LuaGetNumberFromTableItem( "SHIELD_PIECE_ANIM_TIME", 1 ));

		// enemy
		gameData.ENEMY_RESET_BOUND_X		= static_cast<float>(script::LuaGetNumberFromTableItem( "ENEMY_RESET_BOUND_X", 1 ));
		gameData.ENEMY_RESET_BOUND_Y		= static_cast<float>(script::LuaGetNumberFromTableItem( "ENEMY_RESET_BOUND_Y", 1 ));	
	
		gameData.ENEMY_ATTACK_TIME_MIN		= static_cast<float>(script::LuaGetNumberFromTableItem( "ENEMY_ATTACK_TIME_MIN", 1 ));
		gameData.ENEMY_ATTACK_TIME_MAX		= static_cast<float>(script::LuaGetNumberFromTableItem( "ENEMY_ATTACK_TIME_MAX", 1 ));	
	
		gameData.ENEMY_MASS					= static_cast<float>(script::LuaGetNumberFromTableItem( "ENEMY_MASS", 1 ));
		gameData.ENEMY_INERTIA				= static_cast<float>(script::LuaGetNumberFromTableItem( "ENEMY_INERTIA", 1 ));
		gameData.ENEMY_ATTACK_FORCE			= static_cast<float>(script::LuaGetNumberFromTableItem( "ENEMY_ATTACK_FORCE", 1 ));

		gameData.ENEMY_ATTACK_INC			= static_cast<float>(script::LuaGetNumberFromTableItem( "ENEMY_ATTACK_INC", 1 ));
		gameData.ENEMY_ATTACK_MAX_FORCE		= static_cast<float>(script::LuaGetNumberFromTableItem( "ENEMY_ATTACK_MAX_FORCE", 1 ));
		gameData.ENEMY_RANDOM_ANGULAR_MIN	= static_cast<float>(script::LuaGetNumberFromTableItem( "ENEMY_RANDOM_ANGULAR_MIN", 1 ));
		gameData.ENEMY_RANDOM_ANGULAR_MAX	= static_cast<float>(script::LuaGetNumberFromTableItem( "ENEMY_RANDOM_ANGULAR_MAX", 1 ));

		gameData.ENEMY_RADIUS_VALID		= static_cast<float>(script::LuaGetNumberFromTableItem( "ENEMY_RADIUS_VALID", 1 ));
		gameData.ENEMY_RADIUS_CORRUPT	= static_cast<float>(script::LuaGetNumberFromTableItem( "ENEMY_RADIUS_CORRUPT", 1 ));
		gameData.ENEMY_RADIUS_TROJAN	= static_cast<float>(script::LuaGetNumberFromTableItem( "ENEMY_RADIUS_TROJAN", 1 ));
		gameData.ENEMY_RADIUS_VIRUS		= static_cast<float>(script::LuaGetNumberFromTableItem( "ENEMY_RADIUS_VIRUS", 1 ));
		gameData.ENEMY_RADIUS_BULLET	= static_cast<float>(script::LuaGetNumberFromTableItem( "ENEMY_RADIUS_BULLET", 1 ));
		gameData.ENEMY_RADIUS_HACK		= static_cast<float>(script::LuaGetNumberFromTableItem( "ENEMY_RADIUS_HACK", 1 ));

		gameData.ENEMY_TROJAN_COLOUR_SWAP_TIME			= static_cast<float>(script::LuaGetNumberFromTableItem( "ENEMY_TROJAN_COLOUR_SWAP_TIME", 1 ));
		gameData.ENEMY_TROJAN_COLOUR_SWAP_STOP_RADIUS	= static_cast<float>(script::LuaGetNumberFromTableItem( "ENEMY_TROJAN_COLOUR_SWAP_STOP_RADIUS", 1 ));

		gameData.ENEMY_VIRUS_CIRCLE_DISTANCE_MIN	= static_cast<float>(script::LuaGetNumberFromTableItem( "ENEMY_VIRUS_CIRCLE_DISTANCE_MIN", 1 ));
		gameData.ENEMY_VIRUS_CIRCLE_DISTANCE_MAX	= static_cast<float>(script::LuaGetNumberFromTableItem( "ENEMY_VIRUS_CIRCLE_DISTANCE_MAX", 1 ));
		gameData.ENEMY_VIRUS_CIRCLE_SPEED			= static_cast<float>(script::LuaGetNumberFromTableItem( "ENEMY_VIRUS_CIRCLE_SPEED", 1 ));
		gameData.ENEMY_VIRUS_BULLETS				= static_cast<int>(script::LuaGetNumberFromTableItem( "ENEMY_VIRUS_BULLETS", 1 ));

		gameData.ENEMY_VIRUS_BULLET_FIRE_TIME		= static_cast<float>(script::LuaGetNumberFromTableItem( "ENEMY_VIRUS_BULLET_FIRE_TIME", 1 ));

		gameData.ENEMY_DAMAGE_CORE_VALID	= static_cast<int>(script::LuaGetNumberFromTableItem( "ENEMY_DAMAGE_CORE_VALID", 1 ));
		gameData.ENEMY_DAMAGE_CORE_CORRUPT	= static_cast<int>(script::LuaGetNumberFromTableItem( "ENEMY_DAMAGE_CORE_CORRUPT", 1 ));
		gameData.ENEMY_DAMAGE_CORE_TROJAN	= static_cast<int>(script::LuaGetNumberFromTableItem( "ENEMY_DAMAGE_CORE_TROJAN", 1 ));
		gameData.ENEMY_DAMAGE_CORE_VIRUS	= static_cast<int>(script::LuaGetNumberFromTableItem( "ENEMY_DAMAGE_CORE_VIRUS", 1 ));
		gameData.ENEMY_DAMAGE_CORE_BULLET	= static_cast<int>(script::LuaGetNumberFromTableItem( "ENEMY_DAMAGE_CORE_BULLET", 1 ));
		gameData.ENEMY_DAMAGE_CORE_HACK		= static_cast<int>(script::LuaGetNumberFromTableItem( "ENEMY_DAMAGE_CORE_HACK", 1 ));

		gameData.POINTS_ENEMY_MATCH_VALID	= static_cast<int>(script::LuaGetNumberFromTableItem( "POINTS_ENEMY_MATCH_VALID", 1 ));
		gameData.POINTS_ENEMY_MATCH_CORRUPT	= static_cast<int>(script::LuaGetNumberFromTableItem( "POINTS_ENEMY_MATCH_CORRUPT", 1 ));
		gameData.POINTS_ENEMY_MATCH_TROJAN	= static_cast<int>(script::LuaGetNumberFromTableItem( "POINTS_ENEMY_MATCH_TROJAN", 1 ));
		gameData.POINTS_ENEMY_MATCH_VIRUS	= static_cast<int>(script::LuaGetNumberFromTableItem( "POINTS_ENEMY_MATCH_VIRUS", 1 ));
		gameData.POINTS_ENEMY_MATCH_BULLET	= static_cast<int>(script::LuaGetNumberFromTableItem( "POINTS_ENEMY_MATCH_BULLET", 1 ));
		gameData.POINTS_ENEMY_MATCH_HACK	= static_cast<int>(script::LuaGetNumberFromTableItem( "POINTS_ENEMY_MATCH_HACK", 1 ));

		gameData.ENEMY_MISMATCH_SHIELD_VALID	= static_cast<int>(script::LuaGetNumberFromTableItem( "ENEMY_MISMATCH_SHIELD_VALID", 1 ));
		gameData.ENEMY_MISMATCH_SHIELD_CORRUPT	= static_cast<int>(script::LuaGetNumberFromTableItem( "ENEMY_MISMATCH_SHIELD_CORRUPT", 1 ));
		gameData.ENEMY_MISMATCH_SHIELD_TROJAN	= static_cast<int>(script::LuaGetNumberFromTableItem( "ENEMY_MISMATCH_SHIELD_TROJAN", 1 ));
		gameData.ENEMY_MISMATCH_SHIELD_VIRUS	= static_cast<int>(script::LuaGetNumberFromTableItem( "ENEMY_MISMATCH_SHIELD_VIRUS", 1 ));
		gameData.ENEMY_MISMATCH_SHIELD_BULLET	= static_cast<int>(script::LuaGetNumberFromTableItem( "ENEMY_MISMATCH_SHIELD_BULLET", 1 ));
		gameData.ENEMY_MISMATCH_SHIELD_HACK		= static_cast<int>(script::LuaGetNumberFromTableItem( "ENEMY_MISMATCH_SHIELD_HACK", 1 ));

		hScript::pScriptHData->SetGameData( gameData );
	}

	return(0);
}

