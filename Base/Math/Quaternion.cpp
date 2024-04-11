
/*===================================================================
	File: Quaternion.cpp
	Library: Math

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"

#include <cmath>
#include <assert.h>

#include "DebugBase.h"
#include "MathBase.h"

using math::Quaternion;

/////////////////////////////////////////////////////
/// Default constructor
/// 
///
/////////////////////////////////////////////////////
Quaternion::Quaternion()
{
	Identity();
}

/////////////////////////////////////////////////////
/// Constructor
/// Params: [in]infX, [in]infY, [in]infZ, [in]infW
///
/////////////////////////////////////////////////////
Quaternion::Quaternion( float infX, float infY, float infZ, float infW )
{
	X = infX;
	Y = infY;
	Z = infZ;
	W = infW;
}

/////////////////////////////////////////////////////
/// Constructor
/// Params: [in]q
///
/////////////////////////////////////////////////////
Quaternion::Quaternion( const Quaternion &q )
{
	X = q.X;
	Y = q.Y;
	Z = q.Z;
	W = q.W;
}

/////////////////////////////////////////////////////
/// Constructor
/// Params: [in]v
///
/////////////////////////////////////////////////////
Quaternion::Quaternion( const math::Vec4 &v )
{
	X = v.X;
	Y = v.Y;
	Z = v.Z;
	W = v.W;
}

/////////////////////////////////////////////////////
/// Constructor
/// Params: [in]eulerAngles
///
/////////////////////////////////////////////////////
Quaternion::Quaternion( const math::Vec3 &eulerAngles )
{
	const float fSinPitch( std::sin(eulerAngles.X*0.5f) );
	const float fCosPitch( std::cos(eulerAngles.X*0.5f) );
	const float fSinYaw( std::sin(eulerAngles.Y*0.5f) );
	const float fCosYaw( std::cos(eulerAngles.Y*0.5f) );
	const float fSinRoll( std::sin(eulerAngles.Z*0.5f) );
	const float fCosRoll( std::cos(eulerAngles.Z*0.5f) );
	const float fCosPitchCosYaw( fCosPitch*fCosYaw );
	const float fSinPitchSinYaw( fSinPitch*fSinYaw );

	X = fSinRoll * fCosPitchCosYaw     - fCosRoll * fSinPitchSinYaw;
	Y = fCosRoll * fSinPitch * fCosYaw + fSinRoll * fCosPitch * fSinYaw;
	Z = fCosRoll * fCosPitch * fSinYaw - fSinRoll * fSinPitch * fCosYaw;
	W = fCosRoll * fCosPitchCosYaw     + fSinRoll * fSinPitchSinYaw;
}

/////////////////////////////////////////////////////
/// Default destructor
/// 
///
/////////////////////////////////////////////////////
Quaternion::~Quaternion()
{

}

/////////////////////////////////////////////////////
/// Operator: ASSIGN 
/// Params: [in]q
///
/////////////////////////////////////////////////////
Quaternion	&Quaternion::operator = (const Quaternion &q)
{
	X = q.X;
	Y = q.Y;
	Z = q.Z;
	W = q.W;

	return(*this);
}

/////////////////////////////////////////////////////
/// Operator: MULTIPLY (cross product) 
/// Params: [in]q
///
/////////////////////////////////////////////////////
Quaternion Quaternion::operator *(const Quaternion &a) const 
{
	Quaternion result;

	result.W = W*a.W - X*a.X - Y*a.Y - Z*a.Z;
	result.X = W*a.X + X*a.W + Z*a.Y - Y*a.Z;
	result.Y = W*a.Y + Y*a.W + X*a.Z - Z*a.X;
	result.Z = W*a.Z + Z*a.W + Y*a.X - X*a.Y;

	return result;
}

/////////////////////////////////////////////////////
/// Operator: MULTIPLY_ASSIGN
/// Params: [in]q
///
/////////////////////////////////////////////////////
Quaternion &Quaternion::operator *=(const Quaternion &a) 
{
	// Multuiply and assign
	*this = *this * a;

	// Return reference to l-value
	return *this;
}

/////////////////////////////////////////////////////
/// Operator: ADD_ASSIGN (a Quaternion to this one)
///	Params: [in]v
///
/////////////////////////////////////////////////////
Quaternion &Quaternion::operator += (const Quaternion &q) 
{
	X += q.X;
	Y += q.Y;
	Z += q.Z;
	W += q.W;

	return *this;
}

/////////////////////////////////////////////////////
/// Operator: SUBTRACT_ASSIGN (a Quaternion from this one)
/// Params: [in]q
///
/////////////////////////////////////////////////////
Quaternion &Quaternion::operator -= (const Quaternion &q) 
{
	X -= q.X;
	Y -= q.Y;
	Z -= q.Z;
	W -= q.W;

	return *this;
}

/////////////////////////////////////////////////////
/// Operator: ADD (two Quaternion's)
/// Params: [in]a, [in]b
///
///////////////////////////////////////////////////// 
Quaternion math::operator + (const Quaternion &a, const Quaternion &b) 
{
	Quaternion ret(a);

	ret += b;

	return ret;
}

/////////////////////////////////////////////////////
/// Operator: SUBTRACT (one Quaternion from another)
/// Params: [in]a, [in]b
///
///////////////////////////////////////////////////// 
Quaternion math::operator - (const Quaternion &a, const Quaternion &b) 
{
	Quaternion ret(a);

	ret -= b;

	return ret;
}

/////////////////////////////////////////////////////
/// Method: Identity 
/// Params: None
///
/////////////////////////////////////////////////////
void Quaternion::Identity( void )
{ 
	X = Y = Z = 0.0f; 
	W = 1.0f; 
}

/////////////////////////////////////////////////////
/// Method: EulerToQuat 
/// Params: [in]x, [in]y, [in]z
///
/////////////////////////////////////////////////////
void Quaternion::EulerToQuaternion( const float x, const float y, const float z )
{
	float	ex, ey, ez;		// half euler angles
	float	fCosX, fCosY, fCosZ, fSinX, fSinY, fSinZ, cpcy, spsy;		// vars in roll,pitch yaw

	ex = x / 2.0f;
	ey = y / 2.0f;
	ez = z / 2.0f;

	sinCos( &fSinX, &fCosX, ex );
	sinCos( &fSinY, &fCosY, ey );
	sinCos( &fSinZ, &fCosZ, ez );

	cpcy = fCosY * fCosZ;
	spsy = fSinY * fSinZ;

	W = float( fCosX * cpcy + fSinX * spsy );

	X = float( fSinX * cpcy - fCosX * spsy );
	Y = float( fCosX * fSinY * fCosZ + fSinX * fCosY * fSinZ );
	Z = float( fCosX * fCosY * fSinZ - fSinX * fSinY * fCosZ );

	normalise();
}

/////////////////////////////////////////////////////
/// Method: magnitude 
/// Params: None
///
/////////////////////////////////////////////////////
float Quaternion::magnitude( void ) 
{
	// Compute magnitude of the quaternion
	float	mag = std::sqrt( W*W + X*X + Y*Y + Z*Z );

	return( mag );
}

/////////////////////////////////////////////////////
/// Method: normalise 
/// Params: None
///
/////////////////////////////////////////////////////
void Quaternion::normalise( void ) 
{
	// Compute magnitude of the quaternion
	float	mag = magnitude();

	// Check for bogus length, to protect against divide by zero
	if( mag > 0.0f ) 
	{
		// Normalize it
		float	oneOverMag = 1.0f / mag;
		W *= oneOverMag;
		X *= oneOverMag;
		Y *= oneOverMag;
		Z *= oneOverMag;
	} 
	else 
	{
		// Houston, we have a problem
		DBG_ASSERT(false);

		// In a release build, just slam it to something
		Identity();
	}
}

/////////////////////////////////////////////////////
/// Method: Print
/// Params: None
///
/////////////////////////////////////////////////////
void Quaternion::Print( void )
{
	DBGLOG( "-------------------------------\n" );
	DBGLOG( "X=%.3f\tfY=%.3f\tfZ=%.3f\tfW=%.3f\n", X, Y, Z, W );
	DBGLOG( "-------------------------------\n" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////
/// Function: Slerp
/// Params: [in]q1, [in]q2, [in]t
///
/////////////////////////////////////////////////////
Quaternion math::Slerp( Quaternion &q1, Quaternion &q2, float t )
{
	// Create a local quaternion to store the interpolated quaternion
	Quaternion qInterpolated;

	// Here we do a check to make sure the 2 quaternions aren't the same, return q1 if they are
	if(q1.X == q2.X && q1.Y == q2.Y && q1.Z == q2.Z && q1.W == q2.W) 
		return q1;

	// Following the (b.a) part of the equation, we do a dot product between q1 and q2.
	// We can do a dot product because the same math applied for a 3D vector as a 4D vector.
	float result = (q1.X * q2.X) + (q1.Y * q2.Y) + (q1.Z * q2.Z) + (q1.W * q2.W);

	// If the dot product is less than 0, the angle is greater than 90 degrees
	if(result < 0.0f)
	{
		// Negate the second quaternion and the result of the dot product
		q2.X = -q2.X;
		q2.Y = -q2.Y;
		q2.Z = -q2.Z; 
		q2.W = -q2.W;
		result = -result;
	}

	// Set the first and second scale for the interpolation
	float scale0 = 1.0f - t, scale1 = t;

	// Check if the angle between the 2 quaternions was big enough to warrant such calculations
	if(1.0f - result > 0.01f)
	{
		// Get the angle between the 2 quaternions, and then store the sin() of that angle
		float theta = static_cast<float>(acos(result));
		float sinTheta = std::sin(theta);

		// Calculate the scale for q1 and q2, according to the angle and it's sine value
		scale0 = std::sin( ( 1.0f - t ) * theta) / sinTheta;
		scale1 = std::sin( ( t * theta) ) / sinTheta;
	}	

	// Calculate the x, y, z and w values for the quaternion by using a special
	// form of linear interpolation for quaternions.
	qInterpolated.X = (scale0 * q1.X) + (scale1 * q2.X);
	qInterpolated.Y = (scale0 * q1.Y) + (scale1 * q2.Y);
	qInterpolated.Z = (scale0 * q1.Z) + (scale1 * q2.Z);
	qInterpolated.W = (scale0 * q1.W) + (scale1 * q2.W);

	// Return the interpolated quaternion
	return qInterpolated;
}

/////////////////////////////////////////////////////
/// Function: Conjugate
/// Params: [in]q1
///
/////////////////////////////////////////////////////
Quaternion math::Conjugate( const Quaternion &q1 )
{
	Quaternion result;

	// Same rotation amount

	result.W = q1.W;

	// Opposite axis of rotation

	result.X = -q1.X;
	result.Y = -q1.Y;
	result.Z = -q1.Z;

	// Return it
	return result;
}

/////////////////////////////////////////////////////
/// Function: DotProduct
/// Params: [in]q1, [in]q2
///
/////////////////////////////////////////////////////
float math::DotProduct( const Quaternion &q1, const Quaternion &q2 )
{
	return( q1.W*q2.W + q1.X*q2.X + q1.Y*q2.Y + q1.Z*q2.Z );
}


