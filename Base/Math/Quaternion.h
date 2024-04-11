
#ifndef __QUATERNION_H__
#define __QUATERNION_H__

namespace math
{
	// forward declare
	class Vec3;
	class Vec4;
	class Quaternion;

	class Quaternion
	{
		public:
			/// X component of quaternion
			float X;
			/// Y component of quaternion
			float Y;
			/// Z component of quaternion
			float Z;
			/// W component of quaternion
			float W;

		public:
			/// Default constructor
			Quaternion();
			/// Constructor
			/// \param infX - initialising X component
			/// \param infY - initialising Y component
			/// \param infZ - initialising Z component
			/// \param infW - initialising W component
			Quaternion( float infX, float infY, float infZ, float infW );
			/// constructor
			/// \param q - quaternion to initialise 'this' one with
			Quaternion( const Quaternion &q );
			/// constructor
			/// \param v - vector to initialise 'this' one with
			Quaternion( const Vec4 &v );
			/// constructor
			/// \param eulerAngles - euler angles to initialise 'this' one with
			Quaternion( const Vec3 &eulerAngles );
			/// Default destructor
			~Quaternion();

		public:
			// Operators
			/// OPERATOR 'ASSIGN' - Assigned the data of parameters quaternion to 'this' quaternion
			/// \param q - quaternion to assign
			/// \return Quaternion - ( SUCCESS: 'this' quaternion )
			Quaternion				&operator = (const Quaternion &q);
			// Cross product
			/// OPERATOR 'MULTIPLY' - multiply the data of parameters quaternion to 'this' quaternion
			/// \param a - quaternion to multiply
			/// \return Quaternion - ( SUCCESS: 'this' quaternion )
			Quaternion operator *( const Quaternion &a ) const;

			// Multiplication with assignment, as per C++ convention
			/// OPERATOR 'MULTIPLY ASSIGN' - multiply the data of parameters quaternion to 'this' quaternion
			/// \param a - quaternion to multiply
			/// \return Quaternion - ( SUCCESS: 'this' quaternion )
			Quaternion &operator *=( const Quaternion &a );

			/// OPERATOR 'ADD_ASSIGN' - Add and assign the data of parameter quaternion to 'this' quaternion
			/// \param q - quaternion to add
			/// \return Quaternion - ( SUCCESS: new quaternion )
			Quaternion				&operator += (const Quaternion &q);
			/// OPERATOR 'SUBTRACT_ASSIGN' - subtract and assign the data of parameter quaternion to 'this' quaternion
			/// \param q - quaternion to subtract
			/// \return Quaternion - ( SUCCESS: new quaternion )
			Quaternion				&operator -= (const Quaternion &q);

			/// OPERATOR 'SUBTRACT' - subtracts parameters quaternion from 'this' quaternion
			/// \param a - quaternion to subtract
			/// \return Quaternion - ( SUCCESS: new quaternion )
			friend Quaternion		operator - (const Quaternion &a);
			/// OPERATOR 'ADD' - Adds two quaternions values together
			/// \param a - first quaternion
			/// \param b - second quaternion
			/// \return Quaternion - ( SUCCESS: new quaternion )
			friend Quaternion		operator + (const Quaternion &a, const Quaternion &b);
			/// OPERATOR 'SUBTRACT' - subtracts two quaternion values from each other
			/// \param a - first quaternion
			/// \param b - second quaternion
			/// \return Quaternion - ( SUCCESS: new quaternion )
			friend Quaternion		operator - (const Quaternion &a, const Quaternion &b);

		public:
			// Methods

			/// Identity - Sets the quaternion to default identity
			void Identity( void );

			/// EulerToQuaternion - convert euler rotation to a quternion
			/// \param x - X euler rotation (roll)
			/// \param y - Y euler rotation (pitch)
			/// \param z - Z euler rotation (yaw)
			void EulerToQuaternion( const float x, const float y, const float z );
			/// magnitude - Magnitude of the quaternion
			float magnitude( void );
			/// normalise - Normalise the quaternion.
			void normalise( void );

			/// Print - prints the elements of the matrix
			void Print( void );

	};

	/// Slerp - Spherical Linear interpolation between two quaternions
	/// \param q1 - first quaternion
	/// \param q2 - second quaternion
	/// \param t - interpolation value
	/// \return Quaternion - resulting interpolated quaternion rotation
	Quaternion Slerp( Quaternion &q1, Quaternion &q2, float t );

	/// Conjugate - 
	/// \param q1 - quaternion to conjugate
	/// \return Quaternion - resulting quaternion rotation
	Quaternion Conjugate( const Quaternion &q1 );

	/// DotProduct - Finds the dot product of two quaternions
	/// \param q1 - first quaternion
	/// \param q2 - second quaternion
	/// \return float - dot product of the two input quaternions
	float DotProduct( const Quaternion &q1, const Quaternion &q2 );
} // namespace math
#endif // __QUATERNION_H__



