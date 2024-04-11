
/*===================================================================
	File: PhysicsIds.h
	Game: Cabby

	(C)Hidden Games
=====================================================================*/

#ifndef __PHYSICSIDS_H__
#define __PHYSICSIDS_H__

const short WORLD_GROUP = -1;
const short PLAYER_GROUP = -2;

const uint16 WORLD_CATEGORY = 0x0001;
const uint16 PLAYER_CATEGORY = 0x0002;

namespace
{
	// basic identifier
	const int PHYSICSBASICID_PLAYER				= 0;
	const int PHYSICSBASICID_WORLD				= 1;

	const int PHYSICSBASICID_ENEMY				= 2;
	const int PHYSICSBASICID_PORT				= 3;
	const int PHYSICSBASICID_SHAPE				= 4;
	const int PHYSICSBASICID_DOOR				= 5;

	// cast id
	const int PHYSICSCASTID_UNKNOWN				= -999;

	const int PHYSICSCASTID_PLAYER				= 100;
	const int PHYSICSCASTID_WORLD				= 101;

	const int PHYSICSCASTID_ENEMYBOMB			= 102;
	const int PHYSICSCASTID_ENEMYBOUNCINGBOMB	= 103;
	const int PHYSICSCASTID_ENEMYDUSTDEVIL		= 104;
	const int PHYSICSCASTID_ENEMYFISH			= 105;
	const int PHYSICSCASTID_ENEMYMINE			= 106;
	const int PHYSICSCASTID_ENEMYPIRANHA		= 107;
	const int PHYSICSCASTID_ENEMYPUFFERFISH		= 108;
	const int PHYSICSCASTID_ENEMYRAT			= 109;
	const int PHYSICSCASTID_ENEMYSENTRY			= 110;

	const int PHYSICSCASTID_DOORSLIDEDOWN		= 111;
	const int PHYSICSCASTID_DOORSLIDEUP			= 112;
	const int PHYSICSCASTID_DOORSLIDELEFT		= 113;
	const int PHYSICSCASTID_DOORSLIDERIGHT		= 114;

	const int PHYSICSCASTID_SHAPEH				= 115;
	const int PHYSICSCASTID_SHAPEI				= 116;
	const int PHYSICSCASTID_SHAPEU				= 117;
	const int PHYSICSCASTID_SHAPEX				= 118;
	const int PHYSICSCASTID_SHAPEY				= 119;

	const int PHYSICSCASTID_PORT				= 120;

	// numeric definitions

	const int NUMERIC_WORLDEDGE					= 100;
}

#endif // __PHYSICSIDS_H__
