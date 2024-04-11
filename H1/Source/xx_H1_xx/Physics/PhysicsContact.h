
/*===================================================================
	File: PhysicsContact.h
	Game: AirCadets

	(C)Hidden Games
=====================================================================*/

#ifndef __PHYSICSCONTACT_H__
#define __PHYSICSCONTACT_H__

#include "PhysicsBase.h"

// forward declare
void HandleContacts( void *data, dGeomID o1, dGeomID o2 );

int HandlePlayer( void *data, physics::PhysicsIdentifier* pGeomData1, physics::PhysicsIdentifier* pGeomData2, dGeomID o1, dGeomID o2, int numCollisions );

int HandleDefault( void *data, physics::PhysicsIdentifier* pGeomData1, physics::PhysicsIdentifier* pGeomData2, dGeomID o1, dGeomID o2, int numCollisions );

#endif // __PHYSICSCONTACT_H__
