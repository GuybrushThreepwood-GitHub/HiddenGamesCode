
/*===================================================================
	File: PhysicsIds.h
	Game: H8

	(C)Hidden Games
=====================================================================*/

#ifndef __PHYSICSIDS_H__
#define __PHYSICSIDS_H__

namespace
{
	// basic identifier
	const int PHYSICSBASICID_CORE				= 1;
	const int PHYSICSBASICID_SHIELD				= 2;
	const int PHYSICSBASICID_ENEMY				= 3;

	// cast id
	const int PHYSICSCASTID_UNKNOWN				= -999;

	const int PHYSICSCASTID_CORE				= 1;
	const int PHYSICSCASTID_SHIELD				= 2;
	const int PHYSICSCASTID_ENEMY				= 3;

	// collide bits
	const short CORE_GROUP = -1;
	const short SHIELD_GROUP = -2;
	const short ENEMY_GROUP = -3;

	const uint16 CORE_CATEGORY = 0x0001;
	const uint16 SHIELD_CATEGORY = 0x0002;
	const uint16 ENEMY_CATEGORY = 0x0004;

	const uint16 NOTHING_CATEGORY = 0x1000;
}

#endif // __PHYSICSIDS_H__
