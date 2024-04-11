
/*===================================================================
	File: PhysicsIds.h
	Game: H4

	(C)Hidden Games
=====================================================================*/

#ifndef __PHYSICSIDS_H__
#define __PHYSICSIDS_H__

namespace
{
	const int PHYSICSBASICID_UNKNOWN			= -999;

	const int PHYSICSBASICID_PLAYER				= 0;
	const int PHYSICSBASICID_WORLD				= 1;
	const int PHYSICSBASICID_ENEMY				= 2;
	const int PHYSICSBASICID_FLOATINGHEAD		= 3;
	const int PHYSICSBASICID_HANGINGSPIDER		= 4;
	const int PHYSICSBASICID_CRAWLINGSPIDER		= 5;
	const int PHYSICSBASICID_BULLET				= 6;
	const int PHYSICSBASICID_GIB				= 7;

	// cast id
	const int PHYSICSCASTID_UNKNOWN				= -999;

	const int PHYSICSCASTID_PLAYER				= 100;
	const int PHYSICSCASTID_WORLD				= 101;
	const int PHYSICSCASTID_ENEMY				= 102;
	const int PHYSICSCASTID_FLOATINGHEAD		= 103;
	const int PHYSICSCASTID_HANGINGSPIDER		= 104;
	const int PHYSICSCASTID_CRAWLINGSPIDER		= 105;
	const int PHYSICSCASTID_BULLET				= 106;
	const int PHYSICSCASTID_GIB					= 107;
}

#endif // __PHYSICSIDS_H__
