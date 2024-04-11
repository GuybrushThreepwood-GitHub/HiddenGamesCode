
/*===================================================================
	File: H1Access.h
	Game: AirCadets

	(C)Hidden Games
=====================================================================*/

#ifndef __H1ACCESS_H__
#define __H1ACCESS_H__

#include "ScriptBase.h"

// forward declare
class ScriptDataHolder;

struct GameData
{
	int MAX_ITEMS_PER_PACK;
	int TOTAL_LEVELS;
	int TOTAL_VEHICLES;
	int TOTAL_LEVEL_PACKS;
	int TOTAL_VEHICLE_PACKS;

	int RING_MODE_LAPS;

	int TOTAL_LAYOUTS;

	float ACHIEVEMENT_ICON_MAXX;
	float ACHIEVEMENT_ICON_SPEED;
	float ACHIEVEMENT_ICON_SHOWTIME;

	float ADBAR_PHONE_POSX;
	float ADBAR_PHONE_POSY;	
	float ADBAR_TABLET_POSX;
	float ADBAR_TABLET_POSY;

	// ui
	int MAX_PLAYLISTNAME_CHARACTERS;
	int MAX_SONGNAME_CHARACTERS;
	float ANALOGUE_DEAD_ZONE;

	// camera
	float CAMERA_X_OFFSET;
	float CAMERA_Y_OFFSET;
	float CAMERA_Z_OFFSET;

	float CAMERA_TARGET_MULTIPLY;

	// scene
	float FARCLIP_MODIFY;
	float LOD_SCALER;
	float SKYBOX_Y_OFFSET_DEFAULT;

	int MAX_SPRITES_PER_BATCH;
	float MAX_SPRITE_DRAW_DISTANCE;
	float SPHERE_DRAW_DISTANCE_ON_PLANE;
	
	bool ALLOW_DETAIL_MAP;
	float WALL_FADE_START_DISTANCE;
	int WALL_FADE_IN_INC;
	int WALL_FADE_OUT_DEC;

	// object data
	float DEFAULT_RING_COLLISION_RADIUS;
	float DEFAULT_TOKEN_COLLISION_RADIUS;

	// planes
	float FLAP_RESET_AUTO_SNAP;
	float BULLET_SIZE;

	float SHADOW_OFFSET;

	// game
	float TOKEN_COLLECT_TIME;
	float TOKEN_DRAW_DISTANCE;
	float RING_PITCH_INC;

	float MIN_BOUNDING_X;
	float MIN_BOUNDING_Z;
	
	float MAX_BOUNDING_X;
	float MAX_BOUNDING_Y;
	float MAX_BOUNDING_Z;


	float ACH_PLANE_USED_TIMER;
	float ACH_LEVEL_PLAYED_TIMER;
	float ACH_WEATHER_RAINSNOW_TIMER;
	float ACH_TIMEOFDAY_TIMER;
	float ACH_FREEFLIGHT_MODE_TIMER;

	float ACH_FLYING_LOW_TIME;
	float ACH_FLYING_LOW_HEIGHT;

	float ACH_STUDENT_PILOT;
	float ACH_SPORT_PILOT;
	float ACH_RECREATIONAL_PILOT;
	float ACH_PRIVATE_PILOT;
	float ACH_COMMERCIAL_PILOT;
	float ACH_AIRLINE_TRANSPORT_PILOT;

	// effects
	bool ALLOW_WIND_EFFECT;
	float FLYING_WIND_ENABLE_HEIGHT;
	float FLYING_WIND_DISTANCE_IN_FRONT;

	bool ALLOW_SNOW_EFFECT;
	float SNOW_HEIGHT_ABOVE_PLAYER;		
	float SNOW_DISTANCE_IN_FRONT;	

	bool ALLOW_RAIN_EFFECT;
	float RAIN_HEIGHT_ABOVE_PLAYER;		
	float RAIN_DISTANCE_IN_FRONT;

	float BOUNDING_EFFECT_DISTANCE_IN_FRONT;

};

void RegisterH1Functions( ScriptDataHolder& dataHolder );

int ScriptSetGameData( lua_State* pState );

#endif // __H1ACCESS_H__
