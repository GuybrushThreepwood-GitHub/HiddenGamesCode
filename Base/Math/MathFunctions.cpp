
/*===================================================================
	File: MathFunctions.cpp
	Library: Math

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"

#include <cmath>

#include "DebugBase.h"
#include "MathBase.h"

// TAKEN FROM: 3D MATH PRIMER BOOK FOR GRAPHICS AND GAME DEVELOPEMENT

/////////////////////////////////////////////////////
/// Function: Lerp (two floats)
/// Params: [in]a, [in]b, [in]t
///
///////////////////////////////////////////////////// 
float math::Lerp( const float a, const float b, float t )
{
	float invt = 1.0f-t;
	
	return (((a*invt) + (b*t)));
}

/////////////////////////////////////////////////////
/// Function: WrapPi
/// Params: [in]theta
///
/////////////////////////////////////////////////////
float math::WrapPi( float theta )
{
	theta += math::PI;
	theta -= floor(theta * math::HALF_PI) * (2.0f*math::PI);
	theta -= math::PI;
	return theta;
}

/////////////////////////////////////////////////////
/// Function: SafeAcos
/// Params: [in]x
///
/////////////////////////////////////////////////////
float math::SafeAcos( float x )
{
	// Check limit conditions
	if(x <= -1.0f) 
	{
		return math::PI;
	}
	if(x >= 1.0f) 
	{
		return 0.0f;
	}

	// Value is in the domain - use standard C function
	return acos(x);
}

/////////////////////////////////////////////////////
/// Function: sinCos
/// Params: [out]returnSin, [out]returnCos, [in]theta
///
/////////////////////////////////////////////////////
void math::sinCos(float *returnSin, float *returnCos, float theta) 
{
	*returnSin = std::sin(theta);
	*returnCos = std::cos(theta);
}

/////////////////////////////////////////////////////
/// Function: WithinTolerance
/// Params: [in]valA, [in]tolerance
///
/////////////////////////////////////////////////////
bool math::WithinTolerance( float valA, float tolerance )
{
	float x = std::abs( valA );
	if( x > tolerance )
		return(false);

	return(true);
}

/////////////////////////////////////////////////////
/// Function: WithinTolerance
/// Params: [in]valA, [in]valB, [in]tolerance
///
/////////////////////////////////////////////////////
bool math::WithinTolerance( float valA, float valB, float tolerance )
{
	float x = std::abs( valA-valB );
	if( x > tolerance )
		return(false);

	return(true);
}

/////////////////////////////////////////////////////
/// Function: Clamp
/// Params: [out]value, [in]minValue, [in]maxValue
///
/////////////////////////////////////////////////////
void math::Clamp( unsigned char *value, unsigned char minValue, unsigned char maxValue ) 
{
	if( *value < minValue )
		*value = minValue;
	if( *value > maxValue )
		*value = maxValue;
}

/////////////////////////////////////////////////////
/// Function: Clamp
/// Params: [out]value, [in]minValue, [in]maxValue
///
/////////////////////////////////////////////////////
void math::Clamp( float *value, float minValue, float maxValue ) 
{
	if( *value < minValue )
		*value = minValue;
	if( *value > maxValue )
		*value = maxValue;
}

/////////////////////////////////////////////////////
/// Function: Clamp
/// Params: [out]value, [in]minValue, [in]maxValue
///
/////////////////////////////////////////////////////
void math::Clamp( int *value, int minValue, int maxValue ) 
{
	if( *value < minValue )
		*value = minValue;
	if( *value > maxValue )
		*value = maxValue;
}

/////////////////////////////////////////////////////
/// Function: Clamp
/// Params: [out]value, [in]minValue, [in]maxValue
///
/////////////////////////////////////////////////////
void math::Clamp( short *value, short minValue, short maxValue ) 
{
	if( *value < minValue )
		*value = minValue;
	if( *value > maxValue )
		*value = maxValue;
}

/////////////////////////////////////////////////////
/// Function: Clamp
/// Params: [out]value, [in]minValue, [in]maxValue
///
/////////////////////////////////////////////////////
void math::Clamp( long *value, long minValue, long maxValue ) 
{
	if( *value < minValue )
		*value = minValue;
	if( *value > maxValue )
		*value = maxValue;
}

/////////////////////////////////////////////////////
/// Function: Clamp
/// Params: [out]v, [in]minValue, [in]maxValue
///
/////////////////////////////////////////////////////
void math::Clamp( math::Vec3& v, float minValue, float maxValue ) 
{
	math::Clamp( &v.X, minValue, maxValue );
	math::Clamp( &v.Y, minValue, maxValue );
	math::Clamp( &v.Z, minValue, maxValue );
}

/////////////////////////////////////////////////////
/// Function: Clamp
/// Params: [out]v, [in]minValue, [in]maxValue
///
/////////////////////////////////////////////////////
void math::Clamp( math::Vec4& v, float minValue, float maxValue ) 
{
	math::Clamp( &v.X, minValue, maxValue );
	math::Clamp( &v.Y, minValue, maxValue );
	math::Clamp( &v.Z, minValue, maxValue );
	math::Clamp( &v.W, minValue, maxValue );
}

/////////////////////////////////////////////////////
/// Function: Round
/// Params: [in]v
///
/////////////////////////////////////////////////////
float math::Round(float v)
{
    return v < 0.0f ? std::ceil(v - 0.5f) : std::floor(v + 0.5f);
}

/////////////////////////////////////////////////////
/// Function: Normalise
/// Params: [in/out]v
///
/////////////////////////////////////////////////////
void math::Normalise( float *v )
{
	float len = v[0]*v[0] + v[1]*v[1] + v[2]*v[2];

	if( len <= 0.0f ) 
	{
		v[0] = 1.0f;
		v[1] = 0.0f;
		v[2] = 0.0f;
	}
	else 
	{
		len = 1.0f / std::sqrt(len);

		v[0] *= len;
		v[1] *= len;
		v[2] *= len;
	}
}

/////////////////////////////////////////////////////
/// Function: RadToDeg
/// Params: [in]rad
///
/////////////////////////////////////////////////////
float math::RadToDeg( float rad )
{
	return((rad*180.0f)/math::PI);
}

/////////////////////////////////////////////////////
/// Function: DegToRad
/// Params: [in]deg
///
/////////////////////////////////////////////////////
float math::DegToRad( float deg )
{
	return((deg/180.0f)*math::PI);
}

/////////////////////////////////////////////////////
/// Function: RotateAroundPoint
/// Params: [in]initialPos, [in]center, [in]radAngle
///
/////////////////////////////////////////////////////
math::Vec3 math::RotateAroundPoint( const math::Vec3& initialPos, const math::Vec3& center, float radAngle)
{	
	math::Vec3 newPoint = initialPos;
	math::Vec3 dir = initialPos - center;

	float sinVal;
	float cosVal;

	math::sinCos( &sinVal, &cosVal, radAngle );

	if(radAngle != 0.0f) 
	{
		newPoint.X = center.X + cosVal*dir.X + sinVal*dir.Z;
		newPoint.Z = center.Z - sinVal*dir.X + cosVal*dir.Z;
	}
	
	return(newPoint);
}

/////////////////////////////////////////////////////
/// Function: RotateAroundPoint
/// Params: [in]initialPos, [in]center, [in]radAngle
///
/////////////////////////////////////////////////////
math::Vec3 math::RotateAroundPoint( int axis, const math::Vec3& initialPos, const math::Vec3& center, float radAngle)
{	
	math::Vec3 newPoint = initialPos;
	math::Vec3 dir = initialPos - center;

	float sinVal;
	float cosVal;

	math::sinCos( &sinVal, &cosVal, radAngle );
	if( axis == 0 )
	{
		if(radAngle != 0.0f) 
		{
			newPoint.Y = center.Y + cosVal*dir.Y - sinVal*dir.Z;
			newPoint.Z = center.Z + sinVal*dir.Y + cosVal*dir.Z;
		}
	}
	else
	if( axis == 1 )
	{
		if(radAngle != 0.0f) 
		{
			newPoint.X = center.X + cosVal*dir.X + sinVal*dir.Z;
			newPoint.Z = center.Z - sinVal*dir.X + cosVal*dir.Z;
		}
	}
	else
	if( axis == 2 )
	{
		if(radAngle != 0.0f) 
		{
			newPoint.X = center.X + cosVal*dir.X - sinVal*dir.Y;
			newPoint.Y = center.Y + sinVal*dir.X + cosVal*dir.Y;
		}
	}	

	return(newPoint);
}

/////////////////////////////////////////////////////
/// Function: CalculateTangentVector
/// Params: [in]pos1, [in]pos2, [in]pos3, [in]texCoord1, [in]texCoord2, [in]texCoord3, [in]normal, [in/out] tangent
///
/////////////////////////////////////////////////////
void math::CalculateTangentVector(const float pos1[3], const float pos2[3], const float pos3[3], 
								const float texCoord1[2],const float texCoord2[2], const float texCoord3[2],
							 const float normal[3], float tangent[4] )
{
	// Given the 3 vertices (position and texture coordinates) of a triangle
	// calculate and return the triangle's tangent vector.

	// Create 2 vectors in object space.
	//
	// edge1 is the vector from vertex positions pos1 to pos2.
	// edge2 is the vector from vertex positions pos1 to pos3.
	math::Vec3 edge1(pos2[0] - pos1[0], pos2[1] - pos1[1], pos2[2] - pos1[2]);
	math::Vec3 edge2(pos3[0] - pos1[0], pos3[1] - pos1[1], pos3[2] - pos1[2]);

	edge1.normalise();
	edge2.normalise();

	// Create 2 vectors in tangent (texture) space that point in the same
	// direction as edge1 and edge2 (in object space).
	//
	// texEdge1 is the vector from texture coordinates texCoord1 to texCoord2.
	// texEdge2 is the vector from texture coordinates texCoord1 to texCoord3.
	math::Vec2 texEdge1(texCoord2[0] - texCoord1[0], texCoord2[1] - texCoord1[1]);
	math::Vec2 texEdge2(texCoord3[0] - texCoord1[0], texCoord3[1] - texCoord1[1]);

	texEdge1.normalise();
	texEdge2.normalise();

	// These 2 sets of vectors form the following system of equations:
	//
	//  edge1 = (texEdge1.x * tangent) + (texEdge1.y * bitangent)
	//  edge2 = (texEdge2.x * tangent) + (texEdge2.y * bitangent)
	//
	// Using matrix notation this system looks like:
	//
	//  [ edge1 ]     [ texEdge1.x  texEdge1.y ]  [ tangent   ]
	//  [       ]  =  [                        ]  [           ]
	//  [ edge2 ]     [ texEdge2.x  texEdge2.y ]  [ bitangent ]
	//
	// The solution is:
	//
	//  [ tangent   ]        1     [ texEdge2.y  -texEdge1.y ]  [ edge1 ]
	//  [           ]  =  -------  [                         ]  [       ]
	//  [ bitangent ]      det A   [-texEdge2.x   texEdge1.x ]  [ edge2 ]
	//
	//  where:
	//        [ texEdge1.x  texEdge1.y ]
	//    A = [                        ]
	//        [ texEdge2.x  texEdge2.y ]
	//
	//    det A = (texEdge1.x * texEdge2.y) - (texEdge1.y * texEdge2.x)
	//
	// From this solution the tangent space basis vectors are:
	//
	//    tangent = (1 / det A) * ( texEdge2.y * edge1 - texEdge1.y * edge2)
	//  bitangent = (1 / det A) * (-texEdge2.x * edge1 + texEdge1.x * edge2)
	//     normal = cross(tangent, bitangent)

	math::Vec3 t;
	math::Vec3 b;
	math::Vec3 n(normal[0], normal[1], normal[2]);

	float det = (texEdge1.X * texEdge2.Y) - (texEdge1.Y * texEdge2.X);

	if (math::WithinTolerance(det, 0.0f) )
	{
		t = math::Vec3(1.0f, 0.0f, 0.0f);
		b = math::Vec3(0.0f, 1.0f, 0.0f);
	}
	else
	{
		det = 1.0f / det;

		t.X = (texEdge2.Y * edge1.X - texEdge1.Y * edge2.X) * det;
		t.Y = (texEdge2.Y * edge1.Y - texEdge1.Y * edge2.Y) * det;
		t.Z = (texEdge2.Y * edge1.Z - texEdge1.Y * edge2.Z) * det;

		b.X = (-texEdge2.X * edge1.X + texEdge1.X * edge2.X) * det;
		b.Y = (-texEdge2.X * edge1.Y + texEdge1.X * edge2.Y) * det;
		b.Z = (-texEdge2.X * edge1.Z + texEdge1.X * edge2.Z) * det;

		t.normalise();
		b.normalise();
	}

	// Calculate the handedness of the local tangent space.
	// The bitangent vector is the cross product between the triangle face
	// normal vector and the calculated tangent vector. The resulting bitangent
	// vector should be the same as the bitangent vector calculated from the
	// set of linear equations above. If they point in different directions
	// then we need to invert the cross product calculated bitangent vector. We
	// store this scalar multiplier in the tangent vector's 'w' component so
	// that the correct bitangent vector can be generated in the normal mapping
	// shader's vertex shader.

	math::Vec3 bitangent = math::CrossProduct(n,t);
	float handedness = (math::DotProduct(bitangent,b) < 0.0f) ? -1.0f : 1.0f;

	tangent[0] = t.X;
	tangent[1] = t.Y;
	tangent[2] = t.Z;
	tangent[3] = handedness;
}

/////////////////////////////////////////////////////
/// Function: CalculateTangentVector
/// Params: [in]initialPos, [in]center, [in]radAngle
///
/////////////////////////////////////////////////////
math::Vec4 math::CalculateTangentVector( const math::Vec3& pos1, const math::Vec3& pos2, const math::Vec3& pos3, 
									const math::Vec2& texCoord1, const math::Vec2& texCoord2, const math::Vec2& texCoord3, const math::Vec3& normal )
{
	math::Vec4 tangent( 0.0f, 0.0f, 0.0f, 1.0f );

	// Given the 3 vertices (position and texture coordinates) of a triangle
	// calculate and return the triangle's tangent vector.

	// Create 2 vectors in object space.
	//
	// edge1 is the vector from vertex positions pos1 to pos2.
	// edge2 is the vector from vertex positions pos1 to pos3.
	math::Vec3 edge1(pos2[0] - pos1[0], pos2[1] - pos1[1], pos2[2] - pos1[2]);
	math::Vec3 edge2(pos3[0] - pos1[0], pos3[1] - pos1[1], pos3[2] - pos1[2]);

	edge1.normalise();
	edge2.normalise();

	// Create 2 vectors in tangent (texture) space that point in the same
	// direction as edge1 and edge2 (in object space).
	//
	// texEdge1 is the vector from texture coordinates texCoord1 to texCoord2.
	// texEdge2 is the vector from texture coordinates texCoord1 to texCoord3.
	math::Vec2 texEdge1(texCoord2[0] - texCoord1[0], texCoord2[1] - texCoord1[1]);
	math::Vec2 texEdge2(texCoord3[0] - texCoord1[0], texCoord3[1] - texCoord1[1]);

	texEdge1.normalise();
	texEdge2.normalise();

	// These 2 sets of vectors form the following system of equations:
	//
	//  edge1 = (texEdge1.x * tangent) + (texEdge1.y * bitangent)
	//  edge2 = (texEdge2.x * tangent) + (texEdge2.y * bitangent)
	//
	// Using matrix notation this system looks like:
	//
	//  [ edge1 ]     [ texEdge1.x  texEdge1.y ]  [ tangent   ]
	//  [       ]  =  [                        ]  [           ]
	//  [ edge2 ]     [ texEdge2.x  texEdge2.y ]  [ bitangent ]
	//
	// The solution is:
	//
	//  [ tangent   ]        1     [ texEdge2.y  -texEdge1.y ]  [ edge1 ]
	//  [           ]  =  -------  [                         ]  [       ]
	//  [ bitangent ]      det A   [-texEdge2.x   texEdge1.x ]  [ edge2 ]
	//
	//  where:
	//        [ texEdge1.x  texEdge1.y ]
	//    A = [                        ]
	//        [ texEdge2.x  texEdge2.y ]
	//
	//    det A = (texEdge1.x * texEdge2.y) - (texEdge1.y * texEdge2.x)
	//
	// From this solution the tangent space basis vectors are:
	//
	//    tangent = (1 / det A) * ( texEdge2.y * edge1 - texEdge1.y * edge2)
	//  bitangent = (1 / det A) * (-texEdge2.x * edge1 + texEdge1.x * edge2)
	//     normal = cross(tangent, bitangent)

	math::Vec3 t;
	math::Vec3 b;
	math::Vec3 n(normal[0], normal[1], normal[2]);

	float det = (texEdge1.X * texEdge2.Y) - (texEdge1.Y * texEdge2.X);

	if (math::WithinTolerance(det, 0.0f) )
	{
		t = math::Vec3(1.0f, 0.0f, 0.0f);
		b = math::Vec3(0.0f, 1.0f, 0.0f);
	}
	else
	{
		det = 1.0f / det;

		t.X = (texEdge2.Y * edge1.X - texEdge1.Y * edge2.X) * det;
		t.Y = (texEdge2.Y * edge1.Y - texEdge1.Y * edge2.Y) * det;
		t.Z = (texEdge2.Y * edge1.Z - texEdge1.Y * edge2.Z) * det;

		b.X = (-texEdge2.X * edge1.X + texEdge1.X * edge2.X) * det;
		b.Y = (-texEdge2.X * edge1.Y + texEdge1.X * edge2.Y) * det;
		b.Z = (-texEdge2.X * edge1.Z + texEdge1.X * edge2.Z) * det;

		t.normalise();
		b.normalise();
	}

	// Calculate the handedness of the local tangent space.
	// The bitangent vector is the cross product between the triangle face
	// normal vector and the calculated tangent vector. The resulting bitangent
	// vector should be the same as the bitangent vector calculated from the
	// set of linear equations above. If they point in different directions
	// then we need to invert the cross product calculated bitangent vector. We
	// store this scalar multiplier in the tangent vector's 'w' component so
	// that the correct bitangent vector can be generated in the normal mapping
	// shader's vertex shader.

	math::Vec3 bitangent = math::CrossProduct(n,t);
	float handedness = (math::DotProduct(bitangent,b) < 0.0f) ? -1.0f : 1.0f;

	tangent[0] = t.X;
	tangent[1] = t.Y;
	tangent[2] = t.Z;
	tangent[3] = handedness;

	return tangent;
}
