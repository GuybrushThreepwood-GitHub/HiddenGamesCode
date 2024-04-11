
/*===================================================================
	File: OBB.cpp
	Library: Collision

	(C)Hidden Games
=====================================================================*/

#include "Core/CoreDefines.h"
#include "Core/CoreFunctions.h"

#include "Math/Vectors.h"

#include "CollisionBase.h"

using collision::OBB;

/////////////////////////////////////////////////////
/// Default constructor
/// 
///
/////////////////////////////////////////////////////
OBB::OBB()
{
	Reset();
}

/////////////////////////////////////////////////////
/// Default destructor
/// 
///
/////////////////////////////////////////////////////
OBB::~OBB()
{

}

/////////////////////////////////////////////////////
/// Method: Reset
/// Params: None
///
/////////////////////////////////////////////////////
void OBB::Reset( void )
{
	unsigned int i=0;
	vCenter = math::Vec3( 0.0f, 0.0f, 0.0f );

	for( i=0; i<3; ++i )
		vAxis[i] = math::Vec3( 0.0f, 0.0f, 0.0f );

	vHalfWidths = math::Vec3( 1.0f, 1.0f, 1.0f );
}

/////////////////////////////////////////////////////
/// Method: ClosestPointTo
/// Params: [in]p
///
/////////////////////////////////////////////////////
math::Vec3 OBB::ClosestPointTo( const math::Vec3 &p ) 
{
	unsigned int i=0;
	math::Vec3 d = p - vCenter;

	// start at the center of the box
	math::Vec3 closestP = vCenter;

	for( i=0; i<3; ++i )
	{
		// project d onto that axis to get the distance along the axis of d from the box center
		float dist = math::DotProduct( d, vAxis[i] );

		// if distance is further than the box extends, clamp to the box
		if( dist > vHalfWidths[i] )
			dist = vHalfWidths[i];
		if( dist < -vHalfWidths[i] )
			dist = -vHalfWidths[i];

		// step that distance along the axis to get world coodinate
		closestP += vAxis[i] * dist;
	}

	return(closestP);
}

/////////////////////////////////////////////////////
/// Method: OBBCollidesWithSphere
/// Params: [in]s
///
/////////////////////////////////////////////////////
bool OBB::OBBCollidesWithSphere( const collision::Sphere &s )
{
	math::Vec3 p = ClosestPointTo( s.vCenterPoint );

	// sphere and OBB intersect if the squared distance from the sphere center to point p is less than
	// the squared sphere radius
	math::Vec3 v = p - s.vCenterPoint;

	return( (math::DotProduct( v, v ) <= (s.fRadius*s.fRadius)) );
}

