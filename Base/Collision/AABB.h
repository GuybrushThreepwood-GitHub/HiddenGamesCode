
#ifndef __AABB_H__
#define __AABB_H__

// forward declare
namespace math { class Vec3; }
namespace collision { class Sphere; }

namespace collision
{
	class AABB
	{
		public:
			/// default constructor
			AABB();
			/// default destructor
			~AABB();

			/// OPERATOR 'ADDITION_ASSIGN' - add and assign the data of parameter aabb to 'this' aabb
			/// \param a - vector to subtract
			/// \return AABB - ( SUCCESS: new AABB )
			AABB				&operator += (const AABB &a);

			/// Reset - Clears out the AABB data and sets to max/min values for each point
			void Reset( void );
			/// ReCalculate - Check to see if a point is outside the AABB current range and increases the AABB size if it is
			/// \param vPos - Point to check against
			void ReCalculate(const math::Vec2 &vPos);
			/// ReCalculate - Check to see if a point is outside the AABB current range and increases the AABB size if it is
			/// \param vPos - Point to check against
			void ReCalculate( const math::Vec3 &vPos );
			/// ReCalculate - Check to see if a point is outside the AABB current range and increases the AABB size if it is
			/// \param vPosMin - Minimum point to check against
			/// \param vPosMax - Maximum point to check against
			void ReCalculate( const math::Vec3 &vPosMin, const math::Vec3 &vPosMax );
			/// ClosestPointTo - Check to see if a point is outside the AABB current range and increases the AABB size if it is
			/// \param p - Point to check against
			/// \return Vec3 - Closest point
			math::Vec3 ClosestPointTo( const math::Vec3 &p ); 
			/// PointInAABB - Check to see if a point is inside 'this' AABB
			/// \param p - point to check
			/// \return boolean - ( COLLISION: true or NO COLLISION: false )
			bool PointInAABB( const math::Vec3 &p );
			/// AABBCollidesWithAABB - Check to see if an AABB is intersecting 'this' AABB
			/// \param aabb - AABB to check against
			/// \return boolean - ( COLLISION: true or NO COLLISION: false )
			bool AABBCollidesWithAABB( const AABB &aabb );
			/// AABBCollidesWithSphere - Check to see if a sphere is intersecting 'this' AABB
			/// \param s - sphere to check against
			/// \return boolean - ( COLLISION: true or NO COLLISION: false )
			bool AABBCollidesWithSphere( const collision::Sphere &s );

			/// center of the pos
			math::Vec3 vCenter;
			/// minimum bounding box values
			math::Vec3 vBoxMin;
			/// maximum bounding box values
			math::Vec3 vBoxMax;

		private:

	};

	/// PointInAABB - Check to see if a point is inside 'this' AABB
	/// \param p - point to check
	/// \return boolean - ( COLLISION: true or NO COLLISION: false )
	bool PointInAABB( const math::Vec3 &p, const collision::AABB &aabb );

/////////////////////////////////////////////////////
/// Operator: ADDITION_ASSIGN (a AABB to this one)
/// Params: [in]a
///
/////////////////////////////////////////////////////
inline AABB &AABB::operator += (const AABB &a) 
{
	if( a.vBoxMin.X < vBoxMin.X )
		vBoxMin.X = a.vBoxMin.X;

	if( a.vBoxMin.Y < vBoxMin.Y )
		vBoxMin.Y = a.vBoxMin.Y;

	if( a.vBoxMin.Z < vBoxMin.Z )
		vBoxMin.Z = a.vBoxMin.Z;

	if( a.vBoxMax.X > vBoxMax.X )
		vBoxMax.X = a.vBoxMax.X;

	if( a.vBoxMax.Y > vBoxMax.Y )
		vBoxMax.Y = a.vBoxMax.Y;

	if( a.vBoxMax.Z > vBoxMax.Z )
		vBoxMax.Z = a.vBoxMax.Z;

	vCenter = ( vBoxMin + vBoxMax )*0.5f;

	return *this;
}

} // namespace collision

#endif // __AABB_H__

