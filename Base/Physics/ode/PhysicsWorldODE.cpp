
/*===================================================================
	File: PhysicsWorldODE.cpp
	Library: Physics (ODE)

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"
#include "MathBase.h"
#include "RenderBase.h"
#include "PhysicsBase.h"

#ifdef BASE_SUPPORT_ODE

/// statics
dWorldID physics::PhysicsWorldODE::ms_WorldID = 0;
dSpaceID physics::PhysicsWorldODE::ms_SpaceID = 0;
dSpaceID physics::PhysicsWorldODE::ms_EditorSpaceID = 0;
dSpaceID physics::PhysicsWorldODE::ms_RayCastSpaceID = 0;
dJointGroupID physics::PhysicsWorldODE::ms_JointGroupID = 0;

namespace physics
{
	//const int DEFAULT_AUTO_DISABLE				= 1;
	//const float DEFAULT_AUTO_DISABLE_LINEAR		= 0.50f;
	//const float DEFAULT_AUTO_DISABLE_ANGULAR	= 0.50f;
	//const int DEFAULT_AUTO_DISABLE_STEPS		= 5;
	//const float DEFAULT_AUTO_DISABLE_TIME		= 0.75f;
	//const float DEFAULT_WORLD_GRAVITY			= -50.0f;
	//const float DEFAULT_FALLOUTPLANE_HEIGHT		= -3000.0f;

	//const int DEFAULT_MAX_CONTACTS				= 10;
	//const int DEFAULT_SURFACE_MU				= 10;

	//const float DEFAULT_MAX_LINEAR_VEL			= 250.0f;
	//const float DEFAULT_MAX_ANGULAR_VEL			= 250.0f;

	bool physicsEnabled							= false;
}

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
physics::PhysicsWorldODE::PhysicsWorldODE()
{

}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
physics::PhysicsWorldODE::~PhysicsWorldODE()
{

}

/////////////////////////////////////////////////////
/// Static Method: Create
/// Params: None
///
/////////////////////////////////////////////////////
void physics::PhysicsWorldODE::Create()
{
	// ODE
	dInitODE2(dAllocateMaskAll);

	physics::PhysicsWorldODE::ms_WorldID = dWorldCreate();

	if( physics::PhysicsWorldODE::ms_WorldID == 0 )
	{
		DBGLOG( "PHYSICSWORLDODE: *ERROR* Could not create physics world\n" );
		DBG_ASSERT_MSG( 0, "Could not create ODE world" );
	}

	// create a temp joint group
	physics::PhysicsWorldODE::ms_JointGroupID = dJointGroupCreate( 0 );
	DBG_ASSERT( physics::PhysicsWorldODE::ms_JointGroupID != 0 );

	// create a space
	//dVector3 center = { 0.0f, 0.0f, 0.0f };
	//dVector3 Extents = { 50.0f, 50.0f, 50.0f };
	//physics::PhysicsWorldODE::ms_SpaceID = dQuadTreeSpaceCreate(0, center, Extents, 4);
	
	//physics::PhysicsWorldODE::ms_SpaceID = dHashSpaceCreate( 0 );
	physics::PhysicsWorldODE::ms_SpaceID = dSweepAndPruneSpaceCreate( 0, dSAP_AXES_XZY );
	//physics::PhysicsWorldODE::ms_SpaceID = dSimpleSpaceCreate( 0 );

	physics::PhysicsWorldODE::ms_EditorSpaceID = dSweepAndPruneSpaceCreate( 0, dSAP_AXES_XZY );
	physics::PhysicsWorldODE::ms_RayCastSpaceID = dSweepAndPruneSpaceCreate( 0, dSAP_AXES_XZY );

	if( physics::PhysicsWorldODE::ms_SpaceID == 0 )
	{
		DBGLOG( "PHYSICSWORLDODE: *ERROR* Could not create physics space\n" );
		DBG_ASSERT_MSG( 0, "Could not create ode space" );
	}
	if( physics::PhysicsWorldODE::ms_EditorSpaceID == 0 )
	{
		DBGLOG( "PHYSICSWORLDODE: *ERROR* Could not create physics space\n" );
		DBG_ASSERT_MSG( 0, "Could not create ode space" );
	}
	if( physics::PhysicsWorldODE::ms_RayCastSpaceID == 0 )
	{
		DBGLOG( "PHYSICSWORLDODE: *ERROR* Could not create physics space\n" );
		DBG_ASSERT_MSG( 0, "Could not create ode space" );
	}

#ifdef BASE_ENABLE_ODE_TEXTURE
	physics::InitialiseODETexture(); // create a debug texture to visualise physics objects
#endif // BASE_ENABLE_ODE_TEXTURE
}

/////////////////////////////////////////////////////
/// Static  Method: Destroy
/// Params: None
///
/////////////////////////////////////////////////////
void physics::PhysicsWorldODE::Destroy()
{	
#ifdef BASE_ENABLE_ODE_TEXTURE
	physics::ReleaseODETexture();
#endif // BASE_ENABLE_ODE_TEXTURE

	if( physics::PhysicsWorldODE::ms_JointGroupID )
	{
		dJointGroupDestroy( physics::PhysicsWorldODE::ms_JointGroupID );
		physics::PhysicsWorldODE::ms_JointGroupID = 0;
	}

	if( physics::PhysicsWorldODE::ms_SpaceID )
	{
		dSpaceDestroy( physics::PhysicsWorldODE::ms_SpaceID );
		physics::PhysicsWorldODE::ms_SpaceID = 0;
	}
	if( physics::PhysicsWorldODE::ms_EditorSpaceID )
	{
		dSpaceDestroy( physics::PhysicsWorldODE::ms_EditorSpaceID );
		physics::PhysicsWorldODE::ms_EditorSpaceID = 0;
	}
	if( physics::PhysicsWorldODE::ms_RayCastSpaceID )
	{
		dSpaceDestroy( physics::PhysicsWorldODE::ms_RayCastSpaceID );
		physics::PhysicsWorldODE::ms_RayCastSpaceID = 0;
	}
	if( physics::PhysicsWorldODE::ms_WorldID )
	{
		dWorldDestroy( physics::PhysicsWorldODE::ms_WorldID );
		physics::PhysicsWorldODE::ms_WorldID = 0;
	}

	dCloseODE();
}

/////////////////////////////////////////////////////
/// Static Method: GetWorld
/// Params: None
///
/////////////////////////////////////////////////////
dWorldID physics::PhysicsWorldODE::GetWorld()
{
	return physics::PhysicsWorldODE::ms_WorldID;
}

/////////////////////////////////////////////////////
/// Static Method: SetSpace
/// Params: None
///
/////////////////////////////////////////////////////
void physics::PhysicsWorldODE::SetSpace( dSpaceID spaceId )
{
	if( physics::PhysicsWorldODE::ms_SpaceID )
	{
		dSpaceDestroy( physics::PhysicsWorldODE::ms_SpaceID );
		physics::PhysicsWorldODE::ms_SpaceID = 0;
	}

	physics::PhysicsWorldODE::ms_SpaceID = spaceId;
}

/////////////////////////////////////////////////////
/// Static Method: SetEditorSpace
/// Params: None
///
/////////////////////////////////////////////////////
void physics::PhysicsWorldODE::SetEditorSpace( dSpaceID spaceId )
{
	if( physics::PhysicsWorldODE::ms_EditorSpaceID )
	{
		dSpaceDestroy( physics::PhysicsWorldODE::ms_EditorSpaceID );
		physics::PhysicsWorldODE::ms_EditorSpaceID = 0;
	}

	physics::PhysicsWorldODE::ms_EditorSpaceID = spaceId;
}

/////////////////////////////////////////////////////
/// Static Method: SetRayCastSpace
/// Params: None
///
/////////////////////////////////////////////////////
void physics::PhysicsWorldODE::SetRayCastSpace( dSpaceID spaceId )
{
	if( physics::PhysicsWorldODE::ms_RayCastSpaceID )
	{
		dSpaceDestroy( physics::PhysicsWorldODE::ms_RayCastSpaceID );
		physics::PhysicsWorldODE::ms_RayCastSpaceID = 0;
	}

	physics::PhysicsWorldODE::ms_EditorSpaceID = spaceId;
}

/////////////////////////////////////////////////////
/// Static Method: GetSpace
/// Params: None
///
/////////////////////////////////////////////////////
dSpaceID physics::PhysicsWorldODE::GetSpace()
{
	return physics::PhysicsWorldODE::ms_SpaceID;
}

/////////////////////////////////////////////////////
/// Static Method: GetEditorSpace
/// Params: None
///
/////////////////////////////////////////////////////
dSpaceID physics::PhysicsWorldODE::GetEditorSpace()
{
	return physics::PhysicsWorldODE::ms_EditorSpaceID;
}

/////////////////////////////////////////////////////
/// Static Method: GetRayCastSpace
/// Params: None
///
/////////////////////////////////////////////////////
dSpaceID physics::PhysicsWorldODE::GetRayCastSpace()
{
	return physics::PhysicsWorldODE::ms_RayCastSpaceID;
}

/////////////////////////////////////////////////////
/// Static Method: GetJointGroup
/// Params: None
///
/////////////////////////////////////////////////////
dJointGroupID physics::PhysicsWorldODE::GetJointGroup()
{
	return physics::PhysicsWorldODE::ms_JointGroupID;
}

/////////////////////////////////////////////////////
/// Static Method: SetPhysicsState
/// Params: [in]state
///
/////////////////////////////////////////////////////
void physics::PhysicsWorldODE::SetPhysicsState( bool state )
{
	physicsEnabled = state;
}

/////////////////////////////////////////////////////
/// Static Method: GetPhysicsState
/// Params: None
///
/////////////////////////////////////////////////////
bool physics::PhysicsWorldODE::GetPhysicsState()
{
	return physicsEnabled;
}

#endif // BASE_SUPPORT_ODE

