
/*===================================================================
	File: PhysicsContact.cpp
	Game: AirCadets

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"
#include "MathBase.h"
#include "RenderBase.h"
#include "CollisionBase.h"
#include "ModelBase.h"
#include "PhysicsBase.h"

#include "H1Consts.h"

#include "Physics/PhysicsIds.h"
//#include "GameObjects/ObjectTypes.h"

#include "GameSystems.h"
#include "PhysicsContact.h"

static dContact contact[MAX_CONTACTS];

/////////////////////////////////////////////////////
/// Function: HandleContacts
/// 
///
/////////////////////////////////////////////////////
void HandleContacts( void *data, dGeomID o1, dGeomID o2 )
{

	//if( !dGeomIsEnabled(o1) || 
	//	!dGeomIsEnabled(o2) )
	//	return;

	dBodyID b1 = dGeomGetBody(o1);
	dBodyID b2 = dGeomGetBody(o2);

	// exit without doing anything if the two bodies are connected by a joint
	if (b1 && b2 && dAreConnected (b1,b2)) 
		return;

	// run the collision
	int numCollisions = dCollide( o1, o2, MAX_CONTACTS, &contact[0].geom, sizeof(dContact) );

	if( numCollisions <= 0 ) 
		return;

	physics::PhysicsIdentifier* pGeomData1 = reinterpret_cast<physics::PhysicsIdentifier*>(dGeomGetData(o1));
	physics::PhysicsIdentifier* pGeomData2 = reinterpret_cast<physics::PhysicsIdentifier*>(dGeomGetData(o2));

	DBG_ASSERT( (pGeomData1 != 0) && (pGeomData2 != 0) );

	HandlePlayer( data, pGeomData1, pGeomData2, o1, o2, numCollisions );

	HandleDefault( data, 0, 0, o1, o2, numCollisions );
}

/////////////////////////////////////////////////////
/// Function: HandlePlayer
/// 
///
/////////////////////////////////////////////////////
int HandlePlayer( void *data, physics::PhysicsIdentifier* pGeomData1, physics::PhysicsIdentifier* pGeomData2, dGeomID o1, dGeomID o2, int numCollisions )
{
	return 0;
}

/////////////////////////////////////////////////////
/// Function: HandleDefault
/// 
///
/////////////////////////////////////////////////////
int HandleDefault( void *data, physics::PhysicsIdentifier* pGeomData1, physics::PhysicsIdentifier* pGeomData2, dGeomID o1, dGeomID o2, int numCollisions )
{
	int i=0;

	dBodyID b1 = dGeomGetBody(o1);
	dBodyID b2 = dGeomGetBody(o2);

	// normal collision handle
	if( numCollisions > 0 ) 
	{
		for( i=0; i < numCollisions; i++ ) 
		{

			if( i < MAX_CONTACTS )
			{
				contact[i].surface.mode = dContactApprox1 | dContactSlip1 | dContactBounce | dContactSoftCFM;
				contact[i].surface.mu = 10.0f;
				contact[i].surface.bounce = 0.2f;
				contact[i].surface.bounce_vel = 0.01f;
				contact[i].surface.slip1 = 0.1f;
				//contact[i].surface.slip2 = 0.0;
				contact[i].surface.soft_cfm = 0.00005f;
		
				dJointID c = dJointCreateContact (physics::PhysicsWorldODE::GetWorld(),physics::PhysicsWorldODE::GetJointGroup(),contact+i);
				dJointAttach (c,b1,b2);
			}
		}

		return 1;
	}

	return 0;
}
