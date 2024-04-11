
/*===================================================================
	File: Sphere.cpp
	Library: Collision

	(C)Hidden Games
=====================================================================*/

#include "Core/CoreDefines.h"
#include "Core/CoreFunctions.h"

#include "Math/Vectors.h"

#include "CollisionBase.h"

using collision::Sphere;

/////////////////////////////////////////////////////
/// Default constructor
/// 
///
/////////////////////////////////////////////////////
Sphere::Sphere()
{

}

/////////////////////////////////////////////////////
/// Constructor
/// Params: [in]pos, [in]radius
///
/////////////////////////////////////////////////////
Sphere::Sphere( const math::Vec3 &pos, float radius )
{
	vCenterPoint = pos;
	fRadius = radius;
}

/////////////////////////////////////////////////////
/// Default destructor
/// 
///
/////////////////////////////////////////////////////
Sphere::~Sphere()
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////
/// Method: Reset
/// Params: None
///
/////////////////////////////////////////////////////
void Sphere::Reset( void )
{	
	fRadius = 0.0f;
}

/////////////////////////////////////////////////////
/// Method: ReCalculate
/// Params: [in]vPos
///
/////////////////////////////////////////////////////
void Sphere::ReCalculate( const math::Vec3 &vPos )
{	
	float fNewRadius = std::sqrt( vPos.X*vPos.X 
							+ vPos.Y*vPos.Y
							+ vPos.Z*vPos.Z );
		
	if( fNewRadius > fRadius )
		fRadius = fNewRadius;
}

/////////////////////////////////////////////////////
/// Method: SetPosition
/// Params: [in]pos
///
/////////////////////////////////////////////////////
void Sphere::SetPosition( const math::Vec3 &pos )
{
	vCenterPoint = pos;
}

/////////////////////////////////////////////////////
/// Method: SetRadius
/// Params: [in]rad
///
/////////////////////////////////////////////////////
void Sphere::SetRadius( float rad )
{
	fRadius = rad;
}

/////////////////////////////////////////////////////
/// Operator: ASSIGN
/// Params: [in]s
///
/////////////////////////////////////////////////////
Sphere &Sphere::operator =( const Sphere &s )
{
	vCenterPoint = s.vCenterPoint;
	fRadius	= s.fRadius;

	return *this;
}

/////////////////////////////////////////////////////
/// Method: SphereCollidesWithSphere
/// Params: [in]s
///
/////////////////////////////////////////////////////
bool Sphere::SphereCollidesWithSphere( const Sphere &s ) const
{
	float fDiffX, fDiffY, fDiffZ;

	fDiffX = std::abs( vCenterPoint.X - s.vCenterPoint.X );
	fDiffY = std::abs( vCenterPoint.Y - s.vCenterPoint.Y );
	fDiffZ = std::abs( vCenterPoint.Z - s.vCenterPoint.Z );

	float distance = std::sqrt( (fDiffX*fDiffX) + (fDiffY*fDiffY) + (fDiffZ*fDiffZ) );

	if( distance <= ( fRadius + s.fRadius ) )
		return(true);

	return(false);
}

/////////////////////////////////////////////////////
/// Method: SphereCollidesWithSphere
/// Params: [in]s, [out]distance
///
/////////////////////////////////////////////////////
bool Sphere::SphereCollidesWithSphere( const Sphere &s, float &distance ) const
{
	float fDiffX, fDiffY, fDiffZ;

	fDiffX = std::abs( vCenterPoint.X - s.vCenterPoint.X );
	fDiffY = std::abs( vCenterPoint.Y - s.vCenterPoint.Y );
	fDiffZ = std::abs( vCenterPoint.Z - s.vCenterPoint.Z );

	distance = std::sqrt( (fDiffX*fDiffX) + (fDiffY*fDiffY) + (fDiffZ*fDiffZ) );

	if( distance <= ( fRadius + s.fRadius ) )
		return(true);

	return(false);
}

/////////////////////////////////////////////////////
/// Method: SphereCollidesWithAABB
/// Params: [in]aabb
///
/////////////////////////////////////////////////////
bool Sphere::SphereCollidesWithAABB( collision::AABB &aabb )
{
	// find the closest point on box to the point
	math::Vec3 vCenter = vCenterPoint;
	math::Vec3 vPoint = aabb.ClosestPointTo( vCenter );

	// check if it's within range
	if( DistanceSquared(vCenterPoint, vPoint) < fRadius*fRadius )
		return(true);

	return(false);
}

