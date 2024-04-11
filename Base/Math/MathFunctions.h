
#ifndef __MATHFUNCTIONS_H__
#define __MATHFUNCTIONS_H__

namespace math
{
	class Vec2;
	class Vec3;
	class Vec4;

	/// Lerp - lerp a float over time
	/// \param a - source value
	/// \param b - target value
	/// \param t - time
	/// \return float - lerped value
	float Lerp( const float a, const float b, float t );
	/// WrapPi - wrap an angle in range -pi...pi by adding the correct multiple of 2 pi
	/// \param theta - angle to wrap
	/// \return float - wrapped angle
	float WrapPi(float theta);
	/// SafeAcos - safe inverse trig functions
	/// \param x - value to acos
	/// \return float - returned acos value
	float SafeAcos(float x);
	/// sinCos - Gets both the sin() and cos() of an angle
	/// \param returnSin - returned sin of value
	/// \param returnCos - returned cos of value
	/// \param theta - angle input
	void sinCos(float *returnSin, float *returnCos, float theta);
	/// WithinTolerance - Is a value within range of the tolerance
	/// \param valA - current val
	/// \param tolerance - how close to be
	/// \return boolean - true if within the tolerance
	bool WithinTolerance( float valA, float tolerance );
	/// WithinTolerance - Is a value within range of the tolerance
	/// \param valA - current val
	/// \param valB - target Val
	/// \param tolerance - how close to be
	/// \return boolean - true if within the tolerance
	bool WithinTolerance( float valA, float valB, float tolerance );
	/// Clamp - Clamps a value between min and max
	/// \param value - passed and returned clamped value
	/// \param minValue - min value of unsigned char
	/// \param maxValue - max value of unsigned char	
	void Clamp( unsigned char *value, unsigned char minValue, unsigned char maxValue );
	/// Clamp - Clamps a value between min and max
	/// \param value - passed and returned clamped value
	/// \param minValue - min value of float
	/// \param maxValue - max value of float
	void Clamp( float *value, float minValue, float maxValue );
	/// Clamp - Clamps a value between min and max
	/// \param value - passed and returned clamped value
	/// \param minValue - min value of float
	/// \param maxValue - max value of float
	void Clamp( int *value, int minValue, int maxValue );
	/// Clamp - Clamps a value between min and max
	/// \param value - passed and returned clamped value
	/// \param minValue - min value of float
	/// \param maxValue - max value of float
	void Clamp( short *value, short minValue, short maxValue );
	/// Clamp - Clamps a value between min and max
	/// \param value - passed and returned clamped value
	/// \param minValue - min value of float
	/// \param maxValue - max value of float
	void Clamp( long *value, long minValue, long maxValue );

	/// Clamp - Clamps a value between min and max
	/// \param v - passed and returned clamped value
	/// \param minValue - min value of float
	/// \param maxValue - max value of float
	void Clamp( math::Vec3& v, float minValue, float maxValue );
	/// Clamp - Clamps a value between min and max
	/// \param v - passed and returned clamped value
	/// \param minValue - min value of float
	/// \param maxValue - max value of float
	void Clamp( math::Vec4& v, float minValue, float maxValue );

	/// Round - Rounds a value up or down
	/// \param v - value to round
	float Round(float v);

	/// Normalise - Normalises a vector
	/// \param v - float vector to normalise
	void Normalise( float *v );
	/// RadToDeg - Covert from radians to degrees
	/// \param rad - radian
	/// \return float - angle in degrees
	float RadToDeg( float rad );
	/// RadToDeg - Covert from degrees to radians
	/// \param deg - degrees
	/// \return float - angle in radians
	float DegToRad( float deg );
	/// RotateAroundPoint - Rotate a vector around a point on the Y axis
	/// \param initialPos - initial vector position
	/// \param center - center to rotate around
	/// \param radAngle - angle to rotate (in radians)
	/// \return Vec3 - new rotated vector
	math::Vec3 RotateAroundPoint( const math::Vec3& initialPos, const math::Vec3& center, float radAngle);
	/// RotateAroundPoint - Rotate a vector around a point on the Y axis
	/// \param axis - which axis
	/// \param initialPos - initial vector position
	/// \param center - center to rotate around
	/// \param radAngle - angle to rotate (in radians)
	/// \return Vec3 - new rotated vector
	math::Vec3 RotateAroundPoint( int axis, const math::Vec3& initialPos, const math::Vec3& center, float radAngle);

	/// CalculateTangentVector - calculates a tanget vector
	void CalculateTangentVector(const float pos1[3], const float pos2[3], const float pos3[3], 
								const float texCoord1[2],const float texCoord2[2], const float texCoord3[2],
							 const float normal[3], float tangent[4] );
	/// CalculateTangentVector - calculates a tanget vector
	math::Vec4 CalculateTangentVector( const math::Vec3& pos1, const math::Vec3& pos2, const math::Vec3& pos3, 
									const math::Vec2& texCoord1, const math::Vec2& texCoord2, const math::Vec2& texCoord3, const math::Vec3& normal );

} // namespace math

#endif // __MATHFUNCTIONS_H__



