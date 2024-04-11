
/*===================================================================
	File: PhysicsIds.h
	Game: AirCadets

	(C)Hidden Games
=====================================================================*/

#ifndef __PHYSICSIDS_H__
#define __PHYSICSIDS_H__

namespace
{
	// basic identifier
	const int PHYSICSBASICID_WORLD				= 1;
	const int PHYSICSBASICID_WORLDOBJECT		= 2;
	const int PHYSICSBASICID_PLAYER				= 3;
	const int PHYSICSBASICID_BULLET				= 4;

	const int PHYSICSBASICID_RAYTESTER			= 1000;

	// cast id
	const int PHYSICSCASTID_UNKNOWN				= -999;

	const int PHYSICSCASTID_WORLD				= 1;
	const int PHYSICSCASTID_TERRAIN				= 2;
	const int PHYSICSCASTID_WORLDOBJECT			= 3;
	const int PHYSICSCASTID_PLAYER				= 4;
	const int PHYSICSCASTID_BULLET				= 5;

	const int PHYSICSCASTID_RAY					= 1000;

	// collide bits
	const unsigned long	CATEGORY_ALL			= -1;

	const unsigned long	CATEGORY_WORLD			= 0x00000001;
	const unsigned long	CATEGORY_WORLDOBJECT	= 0x00000002;
	const unsigned long	CATEGORY_PLAYER			= 0x00000004;
	const unsigned long	CATEGORY_BULLET			= 0x00000008;
	const unsigned long	CATEGORY_RAYCAST		= 0x01000000;

	const unsigned long CATEGORY_TYPICAL		= CATEGORY_WORLD;
	const unsigned long	CATEGORY_NOTHING		= 0x10000000;
	const unsigned long	CATEGORY_NOTHING2		= 0x20000000;

}

#endif // __PHYSICSIDS_H__
