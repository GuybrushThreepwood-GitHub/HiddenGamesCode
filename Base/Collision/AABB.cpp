
/*===================================================================
	File: AABB.cpp
	Library: Collision

	(C)Hidden Games
=====================================================================*/

#include "Core/CoreDefines.h"
#include "Core/CoreFunctions.h"

#include "Math/Vectors.h"

#include "CollisionBase.h"

using collision::AABB;

/////////////////////////////////////////////////////
/// Default constructor
/// 
///
/////////////////////////////////////////////////////
AABB::AABB()
{
	Reset();
}

/////////////////////////////////////////////////////
/// Default destructor
/// 
///
/////////////////////////////////////////////////////
AABB::~AABB()
{

}

/////////////////////////////////////////////////////
/// Method: Reset
/// Params: None
///
/////////////////////////////////////////////////////
void AABB::Reset( void )
{
	float fBigNumber = 1e37f;
	vCenter = math::Vec3( 0.0f, 0.0f, 0.0f );
	vBoxMin.X = vBoxMin.Y = vBoxMin.Z = fBigNumber;
	vBoxMax.X = vBoxMax.Y = vBoxMax.Z = -fBigNumber;
}

/////////////////////////////////////////////////////
/// Method: ReCalculate
/// Params: [in]vPos
///
/////////////////////////////////////////////////////
void AABB::ReCalculate(const math::Vec2 &vPos)
{
	if (vPos.X < vBoxMin.X)
		vBoxMin.X = vPos.X;

	if (vPos.Y < vBoxMin.Y)
		vBoxMin.Y = vPos.Y;

	if (vPos.X > vBoxMax.X)
		vBoxMax.X = vPos.X;

	if (vPos.Y > vBoxMax.Y)
		vBoxMax.Y = vPos.Y;
}

/////////////////////////////////////////////////////
/// Method: ReCalculate
/// Params: [in]vPos
///
/////////////////////////////////////////////////////
void AABB::ReCalculate( const math::Vec3 &vPos )
{
	if( vPos.X < vBoxMin.X )
		vBoxMin.X = vPos.X;

	if( vPos.Y < vBoxMin.Y )
		vBoxMin.Y = vPos.Y;

	if( vPos.Z < vBoxMin.Z )
		vBoxMin.Z = vPos.Z;


	if( vPos.X > vBoxMax.X )
		vBoxMax.X  = vPos.X ;

	if( vPos.Y > vBoxMax.Y  )
		vBoxMax.Y = vPos.Y;

	if( vPos.Z > vBoxMax.Z )
		vBoxMax.Z = vPos.Z;
}

/////////////////////////////////////////////////////
/// Method: ReCalculate
/// Params: [in]vPosMin, [in]vPosMax
///
/////////////////////////////////////////////////////
void AABB::ReCalculate( const math::Vec3 &vPosMin, const math::Vec3 &vPosMax )
{
	if( vPosMin.X < vBoxMin.X )
		vBoxMin.X = vPosMin.X;

	if( vPosMin.Y < vBoxMin.Y )
		vBoxMin.Y = vPosMin.Y;

	if( vPosMin.Z < vBoxMin.Z )
		vBoxMin.Z = vPosMin.Z;


	if( vPosMax.X > vBoxMax.X )
		vBoxMax.X  = vPosMax.X ;

	if( vPosMax.Y > vBoxMax.Y  )
		vBoxMax.Y = vPosMax.Y;

	if( vPosMax.Z > vBoxMax.Z )
		vBoxMax.Z = vPosMax.Z;
}

/////////////////////////////////////////////////////
/// Method: ClosestPointTo
/// Params: [in]p
///
/////////////////////////////////////////////////////
math::Vec3 AABB::ClosestPointTo( const math::Vec3 &p ) 
{
	// "Push" p into the box, on each dimension
	math::Vec3 r;

	if( p.X < vBoxMin.X ) 
	{
		r.X = vBoxMin.X;
	} 
	else if(p.X > vBoxMax.X) 
	{
		r.X = vBoxMax.X;
	} 
	else 
	{
		r.X = p.X;
	}

	if (p.Y < vBoxMin.Y) 
	{
		r.Y = vBoxMin.Y;
	} 
	else if (p.Y > vBoxMax.Y) 
	{
		r.Y = vBoxMax.Y;
	} 
	else 
	{
		r.Y = p.Y;
	}

	if (p.Z < vBoxMin.Z) 
	{
		r.Z = vBoxMin.Z;
	} 
	else if (p.Z > vBoxMax.Z) 
	{
		r.Z = vBoxMax.Z;
	} 
	else 
	{
		r.Z = p.Z;
	}

	return(r);
}

/////////////////////////////////////////////////////
/// Method: PointInAABB
/// Params: [in]p
///
/////////////////////////////////////////////////////
bool AABB::PointInAABB( const math::Vec3 &p )
{
	if( p.X < vBoxMin.X || p.X > vBoxMax.X )
		return(false);
	if( p.Y < vBoxMin.Y || p.Y > vBoxMax.Y )
		return(false);
	if( p.Z < vBoxMin.Z || p.Z > vBoxMax.Z )
		return(false);

	return(true);
}

/////////////////////////////////////////////////////
/// Method: AABBCollidesWithAABB
/// Params: [in]aabb
///
/////////////////////////////////////////////////////
bool AABB::AABBCollidesWithAABB( const AABB &aabb )
{
	if(vBoxMin.X > aabb.vBoxMax.X) 
		return(false);
	if(vBoxMax.X < aabb.vBoxMin.X) 
		return(false);
	if(vBoxMin.Y > aabb.vBoxMax.Y) 
		return(false);
	if(vBoxMax.Y < aabb.vBoxMin.Y) 
		return(false);
	if(vBoxMin.Z > aabb.vBoxMax.Z) 
		return(false);
	if(vBoxMax.Z < aabb.vBoxMin.Z) 
		return(false);

	return(true);
}

/////////////////////////////////////////////////////
/// Method: AABBCollidesWithSphere
/// Params: [in]s
///
/////////////////////////////////////////////////////
bool AABB::AABBCollidesWithSphere( const collision::Sphere &s )
{
	// find the closest point on box to the point
	math::Vec3 vPoint = ClosestPointTo(s.vCenterPoint);

	// check if it's within range
	if( DistanceSquared(s.vCenterPoint, vPoint) < s.fRadius*s.fRadius )
		return(true);

	return(false);
}

/////////////////////////////////////////////////////
/// Method: PointInAABB
/// Params: [in]p
///
/////////////////////////////////////////////////////
bool collision::PointInAABB( const math::Vec3 &p, const collision::AABB &aabb )
{
	if( p.X < aabb.vBoxMin.X || p.X > aabb.vBoxMax.X )
		return(false);
	if( p.Y < aabb.vBoxMin.Y || p.Y > aabb.vBoxMax.Y )
		return(false);
	if( p.Z < aabb.vBoxMin.Z || p.Z > aabb.vBoxMax.Z )
		return(false);

	return(true);
}
