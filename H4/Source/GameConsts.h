
/*===================================================================
	File: GameConsts.h
	Game: H4

	(C)Hidden Games
=====================================================================*/

#ifndef __GAMECONSTS_H__
#define __GAMECONSTS_H__

const float TOUCH_SIZE				= 15.0f;

const float NEAR_CLIP_ORTHO			= -6.0f;
const float FAR_CLIP_ORTHO			= 6.0f;

const float PLAYER_RADIUS			= 0.5f;
const float BULLET_DELAY			= 0.25f;

const int MAX_PLAYER_BULLETS		= 10;
const float BULLET_RADIUS			= 0.005f;
const float BULLET_DECAL_SIZE		= 0.05f;
const float BULLET_DECAL_OFFSET		= 0.03f;

const float BLOOD_DECAL_SIZE		= 0.3f;
const float BLOOD_DECAL_OFFSET		= -0.115f;

const int MAX_ENEMIES				= 1;
const int MAX_ENEMY_GIBS			= 3;
const int MAX_ENEMY_LOST_PARTS		= 10;
const int MAX_ENEMY_BLOOD_SPLATS	= 3;

const float SMALL_GIB_RADIUS		= 0.05f;
const float LARGE_GIB_RADIUS		= 0.1f;

// damages
const int CRAWLINGSPIDER_DAMAGE		= 10;
const int ENEMY_PUNCH_DAMAGE		= 10;
const int ENEMY_HEADBUTT_DAMAGE		= 15;
const int FLOATINGHEAD_DAMAGE		= 5;
const int HANGINGSPIDER_DAMAGE		= 20;

const int ENEMY_HEALTH				= 6;
const int HANGINGSPIDER_HEALTH		= 3;

const int PISTOL_BULLET_DAMAGE		= 1;
const int SHOTGUN_BULLET_DAMAGE		= 3;

#endif // __GAMECONSTS_H__
