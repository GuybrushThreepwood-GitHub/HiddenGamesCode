/* Copyright (C) Greg Snook, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Greg Snook, 2000"
 */
#ifndef PLANE_H
#define PLANE_H

//#ifndef _MTXLIB_H
//	#include "mtxlib.h"
//#endif

/*	Plane
------------------------------------------------------------------------------------------
	
	A Plane in 3D Space represented in point-normal form (Ax + By + Cz + D = 0).

	The convention for the distance constant D is:

	D = -(A, B, C) dot (X, Y, Z)

------------------------------------------------------------------------------------------
*/
class Plane
{
	public:
		Plane();
		Plane(const Plane& Plane);
		Plane(const math::Vec3& Point0, const math::Vec3& Point1, const math::Vec3& Point2);
		~Plane();

		Plane& operator=( const Plane& Src );


		// tests if data is identical
		friend bool operator==( const Plane& PlaneA, const Plane& PlaneB )
		{
			return (PlaneA.m_Normal == PlaneB.m_Normal && PlaneA.m_Point==PlaneB.m_Point);
		}


		void Set(const math::Vec3& Point0, const math::Vec3& Point1, const math::Vec3& Point2);

		float SolveForX(float Y, float Z)const;
		float SolveForY(float X, float Z)const;
		float SolveForZ(float X, float Y)const;

		// ----- ACCESSORS --------------------

		const math::Vec3& Normal() const;
		const math::Vec3& Point() const;
		const float& Distance() const;
	                     
		float SignedDistance(const math::Vec3& Point) const;

	private:
		math::Vec3     m_Normal;
		math::Vec3     m_Point;
		float       m_Distance;
};

//- Inline Functions ---------------------------------------------------------------------

//Default Constructor
inline Plane::Plane()
{
}

/*	Plane
	Default Copy Constructor
*/
inline Plane::Plane(const Plane& Rhs)
{
    m_Normal = Rhs.m_Normal;
    m_Point = Rhs.m_Point;
    m_Distance = Rhs.m_Distance;
}


inline Plane::Plane(const math::Vec3& Point0, const math::Vec3& Point1, const math::Vec3& Point2)
{
    Set(Point0, Point1, Point2);
}

inline Plane::~Plane()
{
}


inline Plane& Plane::operator=( const Plane& Src )
{
    m_Normal = Src.m_Normal;
    m_Point = Src.m_Point;
    m_Distance = Src.m_Distance;

	return (*this);
}

//:	SolveForX
//	Given Z and Y, Solve for X on the plane 
inline float Plane::SolveForX(float Y, float Z)const
{
	//Ax + By + Cz + D = 0
	// Ax = -(By + Cz + D)
	// x = -(By + Cz + D)/A

	if (m_Normal[0])
	{
		return ( -(m_Normal[1]*Y + m_Normal[2]*Z + m_Distance) / m_Normal[0] );
	}

	return (0.0f);
}

//:	SolveForY
//	Given X and Z, Solve for Y on the plane 
inline float Plane::SolveForY(float X, float Z)const
{
	//Ax + By + Cz + D = 0
	// By = -(Ax + Cz + D)
	// y = -(Ax + Cz + D)/B

	if (m_Normal[1])
	{
		return ( -(m_Normal[0]*X + m_Normal[2]*Z + m_Distance) / m_Normal[1] );
	}

	return (0.0f);
}

//:	SolveForZ
//	Given X and Y, Solve for Z on the plane 
inline float Plane::SolveForZ(float X, float Y)const
{
	//Ax + By + Cz + D = 0
	// Cz = -(Ax + By + D)
	// z = -(Ax + By + D)/C

	if (m_Normal[2])
	{
		return ( -(m_Normal[0]*X + m_Normal[1]*Y + m_Distance) / m_Normal[2] );
	}

	return (0.0f);
}


//:	Set
//	Setup Plane object given a clockwise ordering of 3D points 
inline void Plane::Set(const math::Vec3& Point0, const math::Vec3& Point1, const math::Vec3& Point2)
{
    m_Normal = CrossProduct((Point1 - Point0),(Point2 - Point0));
    m_Normal.normalise();
    m_Point = Point0;

	m_Distance = -math::DotProduct(m_Point,m_Normal);
}

inline const math::Vec3& Plane::Normal() const
{
    return(m_Normal);
}

inline const math::Vec3& Plane::Point() const
{
    return(m_Point);
}

inline const float& Plane::Distance() const
{
    return(m_Distance);
}

#endif  // PLANE_H

