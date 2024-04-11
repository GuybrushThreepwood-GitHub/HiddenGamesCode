

#ifndef __OBB_H__
#define __OBB_H__

// forward declare
namespace math { class Vec3; }
namespace collision { class Sphere; }

namespace collision
{
	class OBB
	{
		public:
			/// default constructor
			OBB();
			/// default destructor
			~OBB();

			/// Reset - Clears out the OBB data and sets to max/min values for each point
			void Reset( void );
			/// ClosestPointTo - Gets the closest point on the OBB from another point
			/// \param p - Point to check against
			/// \return Vec3 - Closest point
			math::Vec3 ClosestPointTo( const math::Vec3 &p ); 
			/// OBBCollidesWithSphere - Check to see if a sphere is intersecting 'this' OBB
			/// \param s - sphere to check against
			/// \return boolean - ( COLLISION: true or NO COLLISION: false )
			bool OBBCollidesWithSphere( const collision::Sphere &s );

			/// OBB center point
			math::Vec3 vCenter;
			/// local x, y, z axis (orientation)
			math::Vec3 vAxis[3];
			/// positive halfwidth extends of OBB along ach axis
			math::Vec3 vHalfWidths;

		private:

	};

} // namespace collision

#endif // __OBB_H__


