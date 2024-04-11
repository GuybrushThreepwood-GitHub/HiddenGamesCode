
#ifndef __SPHERE_H__
#define __SPHERE_H__

// forward declare
namespace math { class Vec3; }
namespace collision { class AABB; }

namespace collision
{
	class Sphere
	{
		public:
			/// default constructor
			Sphere();
			/// constructor
			/// \param pos - center point to intialise with
			/// \param radius - radius to intialise with
			Sphere( const math::Vec3 &pos, float radius );
			/// default destructor
			~Sphere();

			/// Reset - Clears out the AABB data and sets to max/min values for each variable
			void Reset( void );
			/// ReCalculate - Check to see if a point is outside the spheres current range and increases the sphere radius if it is
			/// \param vPos - Point to check against
			void ReCalculate( const math::Vec3 &vPos );
			/// SetPosition - Sets the center point of the sphere 
			/// \param pos -  new center point position
			void SetPosition( const math::Vec3 &pos );
			/// SetRadius - Sets the radius of the sphere 
			/// \param rad -  new radius
			void SetRadius( float rad );
			/// GetPosition - Gets the center point of the sphere 
			/// \return Vec3 - ( SUCCESS: spheres position )
			math::Vec3 GetPosition( void ) {	return vCenterPoint;	}
			/// GetPosition - Sets the center point of the ellipsoid 
			/// \return float - ( SUCCESS: sphere radius )
			float GetRadius( void )	{	return fRadius;			}
			/// OPERATOR 'ASSIGN' - Assigned the data of parameters sphere to 'this' sphere
			/// \param s - sphere to assign
			/// \return Sphere - ( SUCCESS: 'this' sphere )
			Sphere &operator =  (const Sphere &s);
			/// SphereCollidesWithSphere - Check to see if a sphere is intersecting 'this' sphere
			/// \param s - sphere to check against
			/// \return boolean - ( SUCCESS: true or FAIL: false )
			bool SphereCollidesWithSphere( const Sphere &s ) const;
			/// SphereCollidesWithSphere - Check to see if a sphere is intersecting 'this' sphere and returns the distance
			/// \param s - sphere to check against
			/// \param distance - returned distance from each sphere if they are not interseting
			/// \return boolean - ( SUCCESS: true or FAIL: false )
			bool SphereCollidesWithSphere( const Sphere &s, float &distance ) const;
			/// SphereCollidesWithAABB - Check to see if an AABB is intersecting 'this' sphere
			/// \param aabb - AABB to check against
			/// \return boolean - ( SUCCESS: true or FAIL: false )
			bool SphereCollidesWithAABB( collision::AABB &aabb );

			/// center point of the sphere
			math::Vec3 vCenterPoint;
			/// radius of the sphere
			float fRadius;

		private:

	};
} // namespace collision

#endif // __SPHERE_H__



