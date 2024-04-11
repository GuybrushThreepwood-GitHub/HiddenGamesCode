
/*===================================================================
	File: LevelTypes.h
	Game: AirCadets

	(C)Hidden Games
=====================================================================*/

#ifndef __LEVELTYPES_H__
#define __LEVELTYPES_H__

// match the maya exported types
const int POSROT					= 0;
const int SPAWN_POINT				= 1;
const int GATE						= 2;
const int TARGET					= 3;
const int SPRITE					= 4;
const int EMITTER					= 5;
const int SOUND						= 6;

struct SpriteType
{
	int enabled;
	int resId;
	int baseId;
	math::Vec3 pos;
	float angle;
	int type;
	float w,h,d;
	math::Vec4Lite col;
	float sceneDepth;
};

const int MAX_LEVEL_SPRITES = 10;

// sprite batch list
struct spriteBatchData
{
	int spriteId;
	math::Vec3 dims;
	math::Vec3 range;
};

#endif // __LEVELTYPES_H__