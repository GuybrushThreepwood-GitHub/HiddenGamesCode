
/*===================================================================
	File: RayObject.cpp
	Game: AirCadets

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"
#include "MathBase.h"
#include "CollisionBase.h"
#include "RenderBase.h"
#include "PhysicsBase.h"
#include "ModelBase.h"
#include "InputBase.h"
#include "ScriptBase.h"

#include "Physics/RayObject.h"

/////////////////////////////////////////////////////
/// Function: RayObject
/// Params: 
///
/////////////////////////////////////////////////////
RayObject::RayObject( dSpaceID spaceId, float length )
	: physics::PhysicsIdentifier( PHYSICSBASICID_RAYTESTER, PHYSICSCASTID_RAY )
{
	hasHits = false;
	totalHits = 0;
	nextHitIndex = 0;

	rayGeom = 0;
	rayPos.setZero();
	rayTarget.setZero();
	rayDir.setZero();

	rayLength = length;

	rayGeom = dCreateRay( spaceId, rayLength );

	dGeomSetData( rayGeom, this );
}

/////////////////////////////////////////////////////
/// Function: RayObject
/// Params: 
///
/////////////////////////////////////////////////////
RayObject::~RayObject()
{
	if( rayGeom != 0 )
		dGeomDestroy( rayGeom );
}

/////////////////////////////////////////////////////
/// Function: FacingCamera
/// Params: [in]cameraDir, [in]n
///
/////////////////////////////////////////////////////
bool FacingCamera( const math::Vec3& cameraDir, const math::Vec3& n ) 
{
	// We now know the vector from the camera to the face, 
	// and the vector that describes which direction the face 
	// is pointing, so we just need to do a dot-product and 
	// based on that we can tell if it's facing the camera or not
	float result = math::DotProduct(cameraDir, n);
 
	// if the result is positive, then it is facing the camera.
	return result < 0.0f;
}
