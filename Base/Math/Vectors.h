
#ifndef __VECTORS_H__
#define __VECTORS_H__

#ifndef _CMATH_
	#include <cmath>
#endif // _CMATH_
#ifndef _IOSTREAM_
	#include <iostream>
#endif // _IOSTREAM_
#ifndef _FSTREAM_
	#include <fstream>
#endif // _FSTREAM_

#include <assert.h>

#ifndef __COREBASE_H__
	#include "CoreBase.h"
#endif // __COREBASE_H__
#ifndef __DEBUGBASE_H__
	#include "DebugBase.h"
#endif // __DEBUGBASE_H__

#ifndef __MATHFUNCTIONS_H__
	#include "Math/MathFunctions.h"
#endif // __MATHFUNCTIONS_H__

namespace math
{
	class Vec2;
	class Vec3;
	class Vec4;

	const int _X_ = 0;
	const int _Y_ = 1;
	const int _Z_ = 2;

	class Vec2
	{
		public:
			/// union containing multiple forms of using the vector
			union
			{
				/// union structure for accessing as position coordinates
				struct
				{
					float X;
					float Y;			
				};
				/// union structure for accessing as texture coordinates
				struct
				{
					float U;
					float V;		
				};
				/// union structure for accessing as dimensions
				struct
				{
					float Width;
					float Height;		
				};
				/// union structure for accessing as min/max
				struct
				{
					float minVal;
					float maxVal;		
				};
				/// array access of the vector
				float v[2];
			};

		public:
			// Constructors

			/// default constructor
			Vec2() { X = Y = 0.0f; }
			/// constructor
			/// \param inX - initialising X component
			/// \param inY - initialising Y component
			Vec2(float inX, float inY );
			/// constructor
			/// \param v - intialising 2D vector
			Vec2(const Vec2  &v);

		public:
			// Operators
			/// OPERATOR 'ARRAY' - Return the data in vectors array from parameter index value
			/// \param index - array index
			/// \return float - ( SUCCESS: array index value )
			float					&operator [] (unsigned int index);
			/// OPERATOR 'ARRAY' - Return the data in vectors array from parameter index value
			/// \param index - array index
			/// \return float - ( SUCCESS: array index value )
			const float				&operator [] (unsigned int index) const;
			/// OPERATOR 'ASSIGN' - Assign the data of parameter vector to 'this' vector
			/// \param v - vector to assign
			/// \return Vec2 - ( SUCCESS: new vector )
			Vec2				&operator =  (const Vec2 &v);
			/// OPERATOR 'ADD_ASSIGN' - Add and assign the data of parameter vector to 'this' vector
			/// \param v - vector to add
			/// \return Vec2 - ( SUCCESS: new vector )
			Vec2				&operator += (const Vec2 &v);
			/// OPERATOR 'SUBTRACT_ASSIGN' - subtract and assign the data of parameter vector to 'this' vector
			/// \param v - vector to subtract
			/// \return Vec2 - ( SUCCESS: new vector )
			Vec2				&operator -= (const Vec2 &v);
			/// OPERATOR 'MULTIPLY_ASSIGN' - multiply and assign the data of parameter vector to 'this' vector
			/// \param v - vector to multiply
			/// \return Vec2 - ( SUCCESS: new vector )
			Vec2				&operator *= (const Vec2 &v);
			/// OPERATOR 'DIVIDE_ASSIGN' - divide and assign the data of parameter vector to 'this' vector
			/// \param v - vector to divide
			/// \return Vec2 - ( SUCCESS: new vector )
			Vec2				&operator /= (const Vec2 &v);
			/// OPERATOR 'MULTIPLY_ASSIGN' - multiply and assign the data of parameter float to 'this' vector
			/// \param f - float to multiply by
			/// \return Vec2 - ( SUCCESS: new vector )
			Vec2				&operator *= (float f);
			/// OPERATOR 'DIVIDE_ASSIGN' - divide and assign the data of parameter float to 'this' vector
			/// \param f - vector to divide
			/// \return Vec2 - ( SUCCESS: new vector )
			Vec2				&operator /= (float f);

			/// OPERATOR 'IS_EQUAL' - Check to see if two vector values are equal
			/// \param a - first vector
			/// \param b - second vector
			/// \return boolean - ( SUCCESS: true or FAIL: false )
			friend bool				operator == (const Vec2 &a, const Vec2 &b);
			/// OPERATOR 'IS_NOTEQUAL' - Check to see if two vector values are not equal
			/// \param a - first vector
			/// \param b - second vector
			/// \return boolean - ( SUCCESS: true or FAIL: false )
			friend bool				operator != (const Vec2 &a, const Vec2 &b);
			/// OPERATOR 'SUBTRACT' - subtracts parameters vector from 'this' vector
			/// \param a - vector to subtract
			/// \return Vec2 - ( SUCCESS: new vector )
			friend Vec2		operator - (const Vec2 &a);
			/// OPERATOR 'ADD' - Adds two vectors values together
			/// \param a - first vector
			/// \param b - second vector
			/// \return Vec2 - ( SUCCESS: new vector )
			friend Vec2		operator + (const Vec2 &a, const Vec2 &b);
			/// OPERATOR 'SUBTRACT' - subtracts two vectors values from each other
			/// \param a - first vector
			/// \param b - second vector
			/// \return Vec2 - ( SUCCESS: new vector )
			friend Vec2		operator - (const Vec2 &a, const Vec2 &b);
			/// OPERATOR 'MULTIPLY' - multiplies two vectors values together
			/// \param a - first vector
			/// \param b - second vector
			/// \return Vec2 - ( SUCCESS: new vector )
			friend Vec2		operator * (const Vec2 &a, const Vec2 &b);
			/// OPERATOR 'DIVIDE' - divides two vectors values
			/// \param a - first vector
			/// \param b - second vector
			/// \return Vec2 - ( SUCCESS: new vector )
			friend Vec2		operator / (const Vec2 &a, const Vec2 &b);

			/// OPERATOR 'ADD' - Adds a vectors values by a float
			/// \param v - vector
			/// \param f - float value to add to each element
			/// \return Vec2 - ( SUCCESS: new vector )
			friend Vec2		operator + (const Vec2 &v, float f);
			/// OPERATOR 'SUBTRACT' - subtracts a vectors values by a float
			/// \param v - vector
			/// \param f - float value to subtract from each element
			/// \return Vec2 - ( SUCCESS: new vector )
			friend Vec2		operator - (const Vec2 &v, float f);
			/// OPERATOR 'MULTIPLY' - Multiplies a vectors values by a float
			/// \param v - vector
			/// \param f - float value to multiply each element
			/// \return Vec2 - ( SUCCESS: new vector )
			friend Vec2		operator * (const Vec2 &v, float f);
			/// OPERATOR 'DIVIDE' - Divides a vectors values by a float
			/// \param v - vector
			/// \param f - float value to divide each element
			/// \return Vec2 - ( SUCCESS: new vector )
			friend Vec2		operator / (const Vec2 &v, float f);

			/// OPERATOR 'OUTPUT_STREAM' - Outputs the vector elements to the stream
			/// \param stream - stream
			/// \param v - vector
			/// \return ostream - ( SUCCESS: output stream )
			friend std::ostream&	operator << ( std::ostream& stream, Vec2& v );

		public:
			// Methods
			/// length - returns the length of the vector
			/// \return float - ( SUCCESS: vectors length )
			float				length() const;
			/// setZero - sets the values to zero
			void				setZero();
			/// normalise - normalises each element in the vector
			/// \return Vec2 - ( SUCCESS: new vector )
			Vec2				&normalise();
	};

	class Vec3
	{

		public:
			/// union containing multiple forms of using the vector
			union
			{
				/// union structure for accessing as position coordinates
				struct
				{
					float X;
					float Y;
					float Z;			
				};
				/// union structure for accessing as RGB colour
				struct
				{
					float R;
					float G;
					float B;			
				};
				/// union structure for accessing as dimensions
				struct
				{
					float Width;
					float Height;	
					float Depth;
				};
				/// array access of the vector
				float v[3];
			};

		public:
			// Constructors
			/// default constructor
			Vec3() { X = Y = Z = 0.0f; }
			/// constructor
			/// \param inX - initialising X component
			/// \param inY - initialising Y component
			/// \param inZ - initialising Z component
			Vec3(float inX, float inY, float inZ);
			/// constructor
			/// \param v - intialising 3D vector
			Vec3(const Vec3  &v);

			explicit Vec3(const Vec2  &v);
	  
		public:
			// Operators
			/// OPERATOR 'ARRAY' - Return the data in vectors array from parameter index value
			/// \param index - array index
			/// \return float - ( SUCCESS: array index value )
			float					&operator [] (unsigned int index);
			/// OPERATOR 'ARRAY' - Return the data in vectors array from parameter index value
			/// \param index - array index
			/// \return float - ( SUCCESS: array index value )
			const float				&operator [] (unsigned int index) const;
			/// OPERATOR 'ASSIGN' - Assign the data of parameter 3d vector to 'this' vector
			/// \param v - vector to assign
			/// \return Vec3 - ( SUCCESS: new vector )
			Vec3				&operator =  (const Vec3 &v);
			/// OPERATOR 'ASSIGN' - Assign the data of parameter 2d vector to 'this' vector
			/// \param v - vector to assign
			/// \return Vec3 - ( SUCCESS: new vector )
			Vec3				&operator =  (const Vec2 &v);
			/// OPERATOR 'ASSIGN' - Assign the data of parameter 4d vector to 'this' vector
			/// \param v - vector to assign
			/// \return Vec3 - ( SUCCESS: new vector )
			Vec3				&operator =  (const Vec4 &v);
			/// OPERATOR 'ADD_ASSIGN' - Add and assign the data of parameter vector to 'this' vector
			/// \param v - vector to add
			/// \return Vec3 - ( SUCCESS: new vector )
			Vec3				&operator += (const Vec2 &v);
			/// OPERATOR 'ADD_ASSIGN' - Add and assign the data of parameter vector to 'this' vector
			/// \param v - vector to add
			/// \return Vec3 - ( SUCCESS: new vector )
			Vec3				&operator += (const Vec3 &v);
			/// OPERATOR 'SUBTRACT_ASSIGN' - subtract and assign the data of parameter vector to 'this' vector
			/// \param v - vector to subtract
			/// \return Vec3 - ( SUCCESS: new vector )
			Vec3				&operator -= (const Vec2 &v);
			/// OPERATOR 'SUBTRACT_ASSIGN' - subtract and assign the data of parameter vector to 'this' vector
			/// \param v - vector to subtract
			/// \return Vec3 - ( SUCCESS: new vector )
			Vec3				&operator -= (const Vec3 &v);
			/// OPERATOR 'MULTIPLY_ASSIGN' - multiply and assign the data of parameter vector to 'this' vector
			/// \param v - vector to multiply
			/// \return Vec3 - ( SUCCESS: new vector )
			Vec3				&operator *= (const Vec3 &v);
			/// OPERATOR 'DIVIDE_ASSIGN' - divide and assign the data of parameter vector to 'this' vector
			/// \param v - vector to divide
			/// \return Vec3 - ( SUCCESS: new vector )
			Vec3				&operator /= (const Vec3 &v);
			/// OPERATOR 'MULTIPLY_ASSIGN' - multiply and assign the data of parameter float to 'this' vector
			/// \param f - float to multiply by
			/// \return Vec3 - ( SUCCESS: new vector )
			Vec3				&operator *= (float f);
			/// OPERATOR 'DIVIDE_ASSIGN' - divide and assign the data of parameter float to 'this' vector
			/// \param f - vector to divide
			/// \return Vec3 - ( SUCCESS: new vector )
			Vec3				&operator /= (float f);

			/// OPERATOR 'IS_EQUAL' - Check to see if two vector values are equal
			/// \param a - first vector
			/// \param b - second vector
			/// \return boolean - ( SUCCESS: true or FAIL: false )
			friend bool				operator == (const Vec3 &a, const Vec3 &b);
			/// OPERATOR 'IS_NOTEQUAL' - Check to see if two vector values are not equal
			/// \param a - first vector
			/// \param b - second vector
			/// \return boolean - ( SUCCESS: true or FAIL: false )
			friend bool				operator != (const Vec3 &a, const Vec3 &b);
			/// OPERATOR 'SUBTRACT' - subtracts parameters vector from 'this' vector
			/// \param a - vector to subtract
			/// \return Vec3 - ( SUCCESS: new vector )
			friend Vec3		operator - (const Vec3 &a);
			/// OPERATOR 'ADD' - Adds two vectors values together
			/// \param a - first vector
			/// \param b - second vector
			/// \return Vec3 - ( SUCCESS: new vector )
			friend Vec3		operator + (const Vec3 &a, const Vec2 &b);
			/// OPERATOR 'ADD' - Adds two vectors values together
			/// \param a - first vector
			/// \param b - second vector
			/// \return Vec3 - ( SUCCESS: new vector )
			friend Vec3		operator + (const Vec3 &a, const Vec3 &b);
			/// OPERATOR 'SUBTRACT' - subtracts two vectors values from each other
			/// \param a - first vector
			/// \param b - second vector
			/// \return Vec3 - ( SUCCESS: new vector )
			friend Vec3		operator - (const Vec3 &a, const Vec2 &b);
			/// OPERATOR 'SUBTRACT' - subtracts two vectors values from each other
			/// \param a - first vector
			/// \param b - second vector
			/// \return Vec3 - ( SUCCESS: new vector )
			friend Vec3		operator - (const Vec3 &a, const Vec3 &b);
			/// OPERATOR 'MULTIPLY' - multiplies two vectors values together
			/// \param a - first vector
			/// \param b - second vector
			/// \return Vec3 - ( SUCCESS: new vector )
			friend Vec3		operator * (const Vec3 &a, const Vec3 &b);
			/// OPERATOR 'DIVIDE' - divides two vectors values
			/// \param a - first vector
			/// \param b - second vector
			/// \return Vec3 - ( SUCCESS: new vector )
			friend Vec3		operator / (const Vec3 &a, const Vec3 &b);

			/// OPERATOR 'ADD' - Adds a vectors values by a float
			/// \param v - vector
			/// \param f - float value to add to each element
			/// \return Vec3 - ( SUCCESS: new vector )
			friend Vec3		operator + (const Vec3 &v, float f);
			/// OPERATOR 'SUBTRACT' - subtracts a vectors values by a float
			/// \param v - vector
			/// \param f - float value to subtract from each element
			/// \return Vec3 - ( SUCCESS: new vector )
			friend Vec3		operator - (const Vec3 &v, float f);
			/// OPERATOR 'MULTIPLY' - Multiplies a vectors values by a float
			/// \param v - vector
			/// \param f - float value to multiply each element
			/// \return Vec3 - ( SUCCESS: new vector )
			friend Vec3		operator * (const Vec3 &v, float f);
			/// OPERATOR 'DIVIDE' - Divides a vectors values by a float
			/// \param v - vector
			/// \param f - float value to divide each element
			/// \return Vec3 - ( SUCCESS: new vector )
			friend Vec3		operator / (const Vec3 &v, float f);
			
			// scalar product 
			double					operator | (const Vec3 &V) const;

			/// OPERATOR 'OUTPUT_STREAM' - Outputs the vector elements to the stream
			/// \param stream - stream
			/// \param v - vector
			/// \return ostream - ( SUCCESS: output stream )
			friend std::ostream&	operator << ( std::ostream& stream, Vec3& v );

		public:
			// Methods
			/// length - returns the length of the vector
			/// \return float - ( SUCCESS: vectors length )
			float					length() const;
			/// lengthSqr - returns the squared length of the vector
			/// \return float - ( SUCCESS: vectors squared length )
			float					lengthSqr() const;
			/// setZero - sets the values to zero
			void					setZero();
			/// isZero - checks to see if each element is zero
			/// \return boolean - ( SUCCESS: true or FAIL: false )
			bool					isZero() const;
			/// normalise - normalises each element in the vector
			/// \return Vec3 - ( SUCCESS: new vector )
			Vec3					&normalise();
			/// TransformByMatrix4x4 - Translates each element in the vector by a 4x4 matrix translation values
			/// \param m - matrix to transform by
			void					TransformByMatrix4x4( const float* m );
	};

	class Vec4
	{

		public:
			/// union containing multiple forms of using the vector
			union
			{
				/// union structure for accessing as position coordinates
				struct
				{
					float X;
					float Y;
					float Z;		
					float W;
				};
				/// union structure for accessing as a region
				struct
				{
					float OriginX;
					float OriginY;
					float Width;
					float Height;
				};
				/// union structure for accessing as RGBA colour
				struct
				{
					float R;
					float G;
					float B;
					float A;
				};
				/// array access of the vector
				float v[4];
			};

		public:
			// Constructors
			/// default constructor
			Vec4() { X = Y = Z = 0.0f; W = 1.0f; }
			/// constructor
			/// \param inX - initialising X component
			/// \param inY - initialising Y component
			/// \param inZ - initialising Z component
			/// \param inW - initialising W component
			Vec4(float inX, float inY, float inZ, float inW );
			/// constructor
			Vec4(const Vec4  &v);

			explicit Vec4(const Vec3  &v);
	  
		public:
			// Operators
			/// OPERATOR 'ARRAY' - Return the data in vectors array from parameter index value
			/// \param index - array index
			/// \return float - ( SUCCESS: array index value )
			float					&operator [] (unsigned int index);
			/// OPERATOR 'ARRAY' - Return the data in vectors array from parameter index value
			/// \param index - array index
			/// \return float - ( SUCCESS: array index value )
			const float				&operator [] (unsigned int index) const;
			/// OPERATOR 'ASSIGN' - Assign the data of parameter 4d vector to 'this' vector
			/// \param v - vector to assign
			/// \return Vec4 - ( SUCCESS: new vector )
			Vec4				&operator =  (const Vec4 &v);
			/// OPERATOR 'ASSIGN' - Assign the data of parameter 3d vector to 'this' vector
			/// \param v - vector to assign
			/// \return Vec4 - ( SUCCESS: new vector )
			Vec4				&operator =  (const Vec3 &v);
			/// OPERATOR 'ADD_ASSIGN' - Add and assign the data of parameter vector to 'this' vector
			/// \param v - vector to add
			/// \return Vec4 - ( SUCCESS: new vector )
			Vec4				&operator += (const Vec4 &v);
			/// OPERATOR 'SUBTRACT_ASSIGN' - subtract and assign the data of parameter vector to 'this' vector
			/// \param v - vector to subtract
			/// \return Vec4 - ( SUCCESS: new vector )
			Vec4				&operator -= (const Vec4 &v);
			/// OPERATOR 'MULTIPLY_ASSIGN' - multiply and assign the data of parameter vector to 'this' vector
			/// \param v - vector to multiply
			/// \return Vec4 - ( SUCCESS: new vector )
			Vec4				&operator *= (const Vec4 &v);
			/// OPERATOR 'MULTIPLY_ASSIGN' - multiply and assign the data of parameter vector to 'this' vector
			/// \param f - float to multiply
			/// \return Vec4 - ( SUCCESS: new vector )
			Vec4				&operator *= (float f);
			/// OPERATOR 'DIVIDE_ASSIGN' - divide and assign the data of parameter float to 'this' vector
			/// \param f - vector to divide
			/// \return Vec4 - ( SUCCESS: new vector )
			Vec4				&operator /= (float f);

			/// OPERATOR 'IS_EQUAL' - Check to see if two vector values are equal
			/// \param a - first vector
			/// \param b - second vector
			/// \return boolean - ( SUCCESS: true or FAIL: false )
			friend bool				operator == (const Vec4 &a, const Vec4 &b);
			/// OPERATOR 'IS_NOTEQUAL' - Check to see if two vector values are not equal
			/// \param a - first vector
			/// \param b - second vector
			/// \return boolean - ( SUCCESS: true or FAIL: false )
			friend bool				operator != (const Vec4 &a, const Vec4 &b);
			/// OPERATOR 'SUBTRACT' - subtracts parameters vector from 'this' vector
			/// \param a - vector to subtract
			/// \return Vec4 - ( SUCCESS: new vector )
			friend Vec4		operator - (const Vec4 &a);
			/// OPERATOR 'ADD' - Adds two vectors values together
			/// \param a - first vector
			/// \param b - second vector
			/// \return Vec4 - ( SUCCESS: new vector )
			friend Vec4		operator + (const Vec4 &a, const Vec4 &b);
			/// OPERATOR 'SUBTRACT' - subtracts two vectors values from each other
			/// \param a - first vector
			/// \param b - second vector
			/// \return Vec4 - ( SUCCESS: new vector )
			friend Vec4		operator - (const Vec4 &a, const Vec4 &b);
			/// OPERATOR 'MULTIPLY' - multiplies two vectors values together
			/// \param a - first vector
			/// \param b - second vector
			/// \return Vec3 - ( SUCCESS: new vector )
			friend Vec4		operator * (const Vec4 &a, const Vec4 &b);
			/// OPERATOR 'MULTIPLY' - Multiplies a vectors values by a float
			/// \param v - vector
			/// \param f - float value to multiply each element
			/// \return Vec4 - ( SUCCESS: new vector )
			friend Vec4		operator * (const Vec4 &v, float f);
			/// OPERATOR 'DIVIDE' - Divides a vectors values by a float
			/// \param v - vector
			/// \param f - float value to divide each element
			/// \return Vec4 - ( SUCCESS: new vector )
			friend Vec4		operator / (const Vec4 &v, float f);
			
			// scalar product 
			double					operator | (const Vec4 &V) const;

			/// OPERATOR 'OUTPUT_STREAM' - Outputs the vector elements to the stream
			/// \param stream - stream
			/// \param v - vector
			/// \return ostream - ( SUCCESS: output stream )
			friend std::ostream&	operator << ( std::ostream& stream, Vec4& v );

		public:
			// Methods
			/// length - returns the length of the vector
			/// \return float - ( SUCCESS: vectors length )
			float					length() const;
			/// lengthSqr - returns the squared length of the vector
			/// \return float - ( SUCCESS: vectors squared length )
			float					lengthSqr() const;
			/// setZero - sets the values to zero
			void					setZero();
			/// isZero - checks to see if each element is zero
			/// \return boolean - ( SUCCESS: true or FAIL: false )
			bool					isZero() const;
			/// normalise - normalises each element in the vector
			/// \return Vec4 - ( SUCCESS: new vector )
			Vec4				&normalise();
			/// TransformByMatrix4x4 - Translates each element in the vector by a 4x4 matrix translation values
			/// \param m - matrix to transform by
			void					TransformByMatrix4x4( const float* m );
	};

	/// isZeroVector - Checks to see if a 3D vector is full of zeros
	/// \param a - input vector
	/// \return integer - true or false
	int isZeroVector(Vec3& a);
	/// setVectorLength - Sets a vectors length
	/// \param v - vector to set
	/// \param l - length to set
	void setVectorLength(Vec3& v, float l);
	/// DotProduct - Finds the dot product of two vectors
	/// \param a - first input vector
	/// \param b - second input vector
	/// \return float - dot product
	float DotProduct(const Vec2 &a, const Vec2 &b);
	/// DotProduct - Finds the dot product of two vectors
	/// \param a - first input vector
	/// \param b - second input vector
	/// \return float - dot product
	float DotProduct(const Vec3 &a, const Vec3 &b);
	/// SwapVec - Swaps the values of two vectors
	/// \param a - first input vector
	/// \param b - second input vector
	void SwapVec( Vec3 &a, Vec3 &b);
	/// CrossProduct - Finds the cross product of two vectors
	/// \param a - first input vector
	/// \param b - second input vector
	/// \return Vec3 - resulting vector
	Vec3 CrossProduct(const Vec3 &a, const Vec3 &b);
	/// Normalise - Normalises a vector
	/// \param a - input vector
	/// \return Vec2 - resulting vector
	Vec2 Normalise( const Vec2 &a );
	/// Normalise - Normalises a vector
	/// \param a - input vector
	/// \return Vec3 - resulting vector
	Vec3 Normalise( const Vec3 &a );
	/// Lerp - Linear Interpolation between two vectors
	/// \param a - first input vector
	/// \param b - second input vector
	/// \param t - interpolation
	/// \return Vec3 - new lerp position
	Vec3 Lerp( const Vec3 &a, const Vec3 &b, float t ); 
	/// Lerp - Linear Interpolation between two vectors
	/// \param a - first input vector
	/// \param b - second input vector
	/// \param t - interpolation
	/// \return Vec3 - new lerp position
	Vec4 Lerp( const Vec4 &a, const Vec4 &b, float t ); 
	/// AngleBetweenVectors - Finds the angle between two vectors
	/// \param v1 - first input vector
	/// \param v2 - second input vector
	/// \return float - angle between the vectors
	float AngleBetweenVectors( const Vec3 &v1, const Vec3 &v2 );
	/// AngleBetween - Finds the angle between two vectors
	/// \param v1 - first input vector
	/// \param v2 - second input vector
	/// \return float - angle between the vectors
	float AngleBetween( const Vec3 &v1, const Vec3 &v2 );
	/// RotateVector - Rotates a vector by a matrix
	/// \param m - 4x4 matrix containing the rotation
	/// \param vecIn - original vector to rotate
	/// \param vecOut - output vector with rotation
	void RotateVector( const float *m, const Vec3 &vecIn, Vec3 &vecOut);	
	/// Distance - Finds the distance between two vectors
	/// \param vPoint1 - first input vector
	/// \param vPoint2 - second input vector
	/// \return float - distance between the vectors
	float Distance( const Vec3 &vPoint1, const Vec3 &vPoint2 );
	/// DistanceSquared - Finds the distance squared between two vectors
	/// \param a - first input vector
	/// \param b - second input vector
	/// \return float - distance squared between the vectors
	float DistanceSquared(const Vec3 &a, const Vec3 &b); 
	/// TransformByMatrix4x4 - Transforms a vector by a matrix
	/// \param a - input vector
	/// \param m - input 4x4 matrix 
	/// \return Vec3 - transformed vector
	Vec3 TransformByMatrix4x4( const Vec3 &a, const float *m );
	/// WithinTolerance - is a vector within the tolerance of another
	/// \param a - first input vector
	/// \param b - second input vector
	/// \param tolerance - tolerance/epsilon
	/// \return boolean - all values are within the tolerance
	bool WithinTolerance( const Vec3 &a, const Vec3 &b, float tolerance ); 
	/// WithinTolerance - is a vector within the tolerance of another
	/// \param a - first input vector
	/// \param b - second input vector
	/// \param tolerance - tolerance/epsilon
	/// \return boolean - all values are within the tolerance
	bool WithinTolerance( const Vec4 &a, const Vec4 &b, float tolerance );

	class Vec2Lite
	{
		public:
			/// union containing multiple forms of using the vector
			union
			{
				/// union structure for accessing as position coordinates
				struct
				{
					short X;
					short Y;
				};
				/// union structure for accessing as a size
				struct
				{
					short Width;
					short Height;
				};
				/// union structure for accessing as RGBA colour
				struct
				{
					short U;
					short V;
				};
				/// array access of the vector
				short v[2];
			};

		public:
			// Constructors
			/// default constructor
			Vec2Lite() { X = Y = 0; }
	};

	class Vec3Lite
	{
		public:
			/// union containing multiple forms of using the vector
			union
			{
				/// union structure for accessing as position coordinates
				struct
				{
					short X;
					short Y;
					short Z;
				};
				/// union structure for accessing as a size
				struct
				{
					short Width;
					short Height;
					short Depth;
				};
				/// union structure for accessing as RGBA colour
				struct
				{
					short R;
					short G;
					short B;
				};
				/// array access of the vector
				short v[3];
			};

		public:
			// Constructors
			/// default constructor
			Vec3Lite() { X = Y = Z = 0; }
	};

	class Vec4Int
	{
	public:
		/// union containing multiple forms of using the vector
		union
		{
			/// union structure for accessing as position coordinates
			struct
			{
				int X;
				int Y;
				int Z;
				int W;
			};
			/// union structure for accessing as RGBA colour
			struct
			{
				int R;
				int G;
				int B;
				int A;
			};
			/// array access of the vector
			int v[4];
		};

	public:
		// Constructors
		/// default constructor
		Vec4Int() { X = Y = Z = 0; W = 0; }
		Vec4Int(int a, int b, int c, int d)
		{
			X = a;
			Y = b;
			Z = c;
			W = d;
		}
	};

	class Vec4Lite
	{
		public:
			/// union containing multiple forms of using the vector
			union
			{
				/// union structure for accessing as position coordinates
				struct
				{
					unsigned char X;
					unsigned char Y;
					unsigned char Z;		
					unsigned char W;
				};
				/// union structure for accessing as RGBA colour
				struct
				{
					unsigned char R;
					unsigned char G;
					unsigned char B;
					unsigned char A;
				};
				/// array access of the vector
				unsigned char v[4];
			};

		public:
			// Constructors
			/// default constructor
			Vec4Lite() { X = Y = Z = 0; W = 255; }
			Vec4Lite( unsigned char a, unsigned char b, unsigned char c, unsigned char d ) { X = a; Y = b; Z = c; W = d; }
			Vec4Lite( int a, int b, int c, int d )
			{
				math::Clamp( &a, 0, 255 );
				math::Clamp( &b, 0, 255 );
				math::Clamp( &c, 0, 255 );
				math::Clamp( &d, 0, 255 );
				X = a; 
				Y = b; 
				Z = c;
				W = d;
			}
	};

	class Vec2Fixed
	{
		public:
			/// union containing multiple forms of using the vector
			union
			{
				/// union structure for accessing as position coordinates
				struct
				{
					int X;
					int Y;
				};
				/// union structure for accessing as a size
				struct
				{
					int Width;
					int Height;
				};
				/// union structure for accessing as UVs
				struct
				{
					int U;
					int V;
				};
				/// array access of the vector
				int v[2];
			};
		public:
			// Constructors
			/// default constructor
			Vec2Fixed() { X = Y = 0; }
			Vec2Fixed( int a, int b ) { X = a; Y = b; }
	};

	class Vec3Fixed
	{
		public:
			/// union containing multiple forms of using the vector
			union
			{
				/// union structure for accessing as position coordinates
				struct
				{
					int X;
					int Y;
					int Z;		
				};
				/// union structure for accessing as a size
				struct
				{
					int Width;
					int Height;
					int Depth;
				};
				/// union structure for accessing as RGBA colour
				struct
				{
					int R;
					int G;
					int B;
				};
				/// array access of the vector
				int v[3];
			};
		public:
			// Constructors
			/// default constructor
			Vec3Fixed() { X = Y = Z = 0; }
			Vec3Fixed( int a, int b, int c ) { X = a; Y = b; Z = c; }
	};

	template<typename T> class TVec3
	{
		public:
			/// union containing multiple forms of using the vector
			union
			{
				/// union structure for accessing as position coordinates
				struct
				{
					T X;
					T Y;
					T Z;		
					T W;
				};
				/// union structure for accessing as RGBA colour
				struct
				{
					T R;
					T G;
					T B;
					T A;
				};
				/// array access of the vector
				T v[4];
			};
		public:
			// Constructors
			/// default constructor
			TVec3();
			TVec3( T a, T b, T c, T d ) { X = a; Y = b; Z = c; W = d; }
	};


// ##################################################################################################################################################
// ##################################################################################################################################################

/////////////////////////////////////////////////////
/// Constructor
/// Params: [in]inX, [in]inY
///
/////////////////////////////////////////////////////
inline Vec2::Vec2(float inX, float inY) 
{
	X = inX;
	Y = inY;
}

/////////////////////////////////////////////////////
/// Constructor with initialising Vec2
/// Params: [in]v
///
///////////////////////////////////////////////////// 
inline Vec2::Vec2(const Vec2 &v) 
{
	X = v.X;
	Y = v.Y;
}

/////////////////////////////////////////////////////
/// Operator: ARRAY_INDEX
/// Params: [in]i
///
/////////////////////////////////////////////////////
inline float &Vec2::operator [] (unsigned int i) 
{
//	ASSERT(i<3);
        
	return *(&X+i);
}

/////////////////////////////////////////////////////
/// Operator: ARRAY_INDEX
/// Params: [in]i
///
/////////////////////////////////////////////////////
inline const float &Vec2::operator [] (unsigned int i) const 
{
//	ASSERT (i<3);

	return *(&X+i);
}

/////////////////////////////////////////////////////
/// Operator: ASSIGN (from a Vec2)
/// Params: [in]v
///
/////////////////////////////////////////////////////
inline Vec2 &Vec2::operator = (const Vec2 &v) 
{
	X = v.X;
	Y = v.Y;

	return *this;
}

/////////////////////////////////////////////////////
/// Operator: ADD_ASSIGN (a Vec2 to this one)
///	Params: [in]v
///
/////////////////////////////////////////////////////
inline Vec2 &Vec2::operator += (const Vec2 &v) 
{
	X += v.X;
	Y += v.Y;

	return *this;
}

/////////////////////////////////////////////////////
/// Operator: SUBTRACT_ASSIGN (a Vec2 from this one)
/// Params: [in]v
///
/////////////////////////////////////////////////////
inline Vec2 &Vec2::operator -= (const Vec2 &v) 
{
	X -= v.X;
	Y -= v.Y;

	return *this;
}

/////////////////////////////////////////////////////
/// Operator: MULTIPLY_ASSIGN (a Vec2 by this one)
///	Params: [in]v
///
/////////////////////////////////////////////////////
inline Vec2 &Vec2::operator *= (const Vec2 &v) 
{
	X *= v.X;
	Y *= v.Y;

	return *this;
}

/////////////////////////////////////////////////////
/// Operator: DIVIDE_ASSIGN (a Vec2 by this one)
/// Params: [in]v
///
/////////////////////////////////////////////////////
inline Vec2 &Vec2::operator /= (const Vec2 &v) 
{
	X /= v.X;
	Y /= v.Y;

	return *this;
}

/////////////////////////////////////////////////////
/// Operator: MULTIPLY_ASSIGN (the Vec2 by a float)
/// Params: [in]f
///
///////////////////////////////////////////////////// 
inline Vec2 &Vec2::operator *= (float f) 
{
	X *= f;
	Y *= f;

	return *this;
}

/////////////////////////////////////////////////////
/// Operator: DIVIDE_ASSIGN (the Vec2 by a float)
/// Params: [in]f
///
///////////////////////////////////////////////////// 
inline Vec2 &Vec2::operator /= (float f) 
{
	X /= f;
	Y /= f;

	return *this;
}

/////////////////////////////////////////////////////
/// Operator: IS_EQUAL (Are these two Vec2's equal)
/// Params: [in]a, [in]b
///
///////////////////////////////////////////////////// 
inline bool operator == (const math::Vec2 &a, const math::Vec2 &b) 
{
	return ((a.X == b.X) && (a.Y == b.Y));
}

/////////////////////////////////////////////////////
/// Operator: IS_NOT_EQUAL (Are these two Vec2's not equal)
/// Params: [in]a, [in]b
///
/////////////////////////////////////////////////////
inline bool operator != (const Vec2 &a, const Vec2 &b) 
{
	return ((a.X != b.X) || (a.Y != b.Y));
}

/////////////////////////////////////////////////////
/// Operator: ADD (two Vec2's)
/// Params: [in]a, [in]b
///
///////////////////////////////////////////////////// 
inline Vec2 operator + (const Vec2 &a, const Vec2 &b) 
{
	Vec2 ret(a);

	ret += b;

	return ret;
}

/////////////////////////////////////////////////////
/// Operator: SUBTRACT (one Vec2 from another)
/// Params: [in]a, [in]b
///
///////////////////////////////////////////////////// 
inline Vec2 operator - (const Vec2 &a, const Vec2 &b) 
{
	Vec2 ret(a);

	ret -= b;

	return ret;
}

/////////////////////////////////////////////////////
/// Operator: MULTIPLY (one Vec2 by another)
/// Params: [in]a, [in]b
///
///////////////////////////////////////////////////// 
inline Vec2 operator * (const Vec2 &a, const Vec2 &b) 
{
	Vec2 ret(a);

	ret *= b;

	return ret;
}

/////////////////////////////////////////////////////
/// Operator: DIVIDE (one Vec2 by another)
/// Params: [in]a, [in]b
///
///////////////////////////////////////////////////// 
inline Vec2 operator / (const Vec2 &a, const Vec2 &b) 
{
	Vec2 ret(a);

	ret /= b;

	return ret;
}

/////////////////////////////////////////////////////
/// Operator: ADD (Vec2 by a float)
/// Params: [in]v, [in]f
///
///////////////////////////////////////////////////// 
inline Vec2 operator + (const Vec2 &v, float f) 
{
	return Vec2(f + v.X, f + v.Y);
}

/////////////////////////////////////////////////////
/// Operator: SUBTRACT (Vec2 by a float)
/// Params: [in]v, [in]f
///
///////////////////////////////////////////////////// 
inline Vec2 operator - (const Vec2 &v, float f) 
{
	return Vec2(f - v.X, f - v.Y);
}

/////////////////////////////////////////////////////
/// Operator: MULTIPLY (Vec2 by a float)
/// Params: [in]v, [in]f
///
///////////////////////////////////////////////////// 
inline Vec2 operator * (const Vec2 &v, float f) 
{
	return Vec2(f * v.X, f * v.Y);
}

/////////////////////////////////////////////////////
/// Operator: DIVIDE (Vec2 by a float)
/// Params: [in]v, [in]f
///
///////////////////////////////////////////////////// 
inline Vec2 operator / (const Vec2 &v, float f) 
{
	return Vec2(v.X / f, v.Y / f);
}

/////////////////////////////////////////////////////
/// Operator: SUBTRACT (a Vec2)
/// Params: [in]a
///
/////////////////////////////////////////////////////
inline Vec2 operator - (const Vec2 &a) 
{
	return Vec2(-a.X, -a.Y);
}

/////////////////////////////////////////////////////
/// Operator: OUTPUT_STREAM (a Vec2)
/// Params: [in]stream, [in]v
///
/////////////////////////////////////////////////////
inline std::ostream& operator << ( std::ostream& stream, Vec2& v )
{
	stream << "[ ";
	stream << v.X << ", ";
	stream << v.Y;
	stream << " ]";

	return(stream);
}

/////////////////////////////////////////////////////
/// Method: length (Get length of a Vec2)
/// Params: None
///
///////////////////////////////////////////////////// 
inline float Vec2::length() const 
{
	return std::sqrt(X*X + Y*Y);
}

/////////////////////////////////////////////////////
/// Method: setZero (Set the values to zero)
/// Params: None
///
///////////////////////////////////////////////////// 
inline void Vec2::setZero() 
{
	X = Y = 0.0f;
}

/////////////////////////////////////////////////////
/// Method: normalise (a Vec2)
/// Params: None
///
///////////////////////////////////////////////////// 
inline Vec2 &Vec2::normalise() 
{
	float m = length();

	if (m > 0.0f) 
		m = 1.0f / m;
	else 
		m = 0.0f;

	X *= m;
	Y *= m;

	return *this;
}

// ##################################################################################################################################################
// ##################################################################################################################################################

/////////////////////////////////////////////////////
/// Constructor
/// Params: [in]inX, [in]inY, [in]inZ
///
/////////////////////////////////////////////////////
inline Vec3::Vec3(float inX, float inY, float inZ) 
{
	X = inX;
	Y = inY;
	Z = inZ;
}

/////////////////////////////////////////////////////
/// Constructor with initialising Vec3
/// Params: [in]v
///
///////////////////////////////////////////////////// 
inline Vec3::Vec3(const Vec3 &v) 
{
	X = v.X;
	Y = v.Y;
	Z = v.Z;
}

/////////////////////////////////////////////////////
/// Constructor with initialising Vec2
/// Params: [in]v
///
/////////////////////////////////////////////////////
inline Vec3::Vec3(const Vec2 &v) 
{
	X = v.X;
	Y = v.Y;
	Z = 0.0f;
}

/////////////////////////////////////////////////////
/// Operator: ARRAY_INDEX
/// Params: [in]i
///
/////////////////////////////////////////////////////
inline float &Vec3::operator [] (unsigned int i) 
{
//	ASSERT(i<3);
        
	return *(&X+i);
}

/////////////////////////////////////////////////////
/// Operator: ARRAY_INDEX
/// Params: [in]i
///
/////////////////////////////////////////////////////
inline const float &Vec3::operator [] (unsigned int i) const 
{
//	ASSERT (i<3);

	return *(&X+i);
}

/////////////////////////////////////////////////////
/// Operator: ASSIGN (from a Vec3)
/// Params: [in]v
///
/////////////////////////////////////////////////////
inline Vec3 &Vec3::operator = (const Vec3 &v) 
{
	X = v.X;
	Y = v.Y;
	Z = v.Z;

	return *this;
}

/////////////////////////////////////////////////////
/// Operator: ASSIGN (from a Vec2)
/// Params: [in]v
///
/////////////////////////////////////////////////////
inline Vec3 &Vec3::operator = (const Vec2 &v) 
{
	X = v.X;
	Y = v.Y;
	Z = 0.0f;

	return *this;
}

/////////////////////////////////////////////////////
/// Operator: ASSIGN (from a Vec4)
/// Params: [in]v
///
/////////////////////////////////////////////////////
inline Vec3 &Vec3::operator = (const Vec4 &v) 
{
	X = v.X;
	Y = v.Y;
	Z = v.Z;

	return *this;
}

/////////////////////////////////////////////////////
/// Operator: ADD_ASSIGN (a Vec3 to this one)
///	Params: [in]v
///
/////////////////////////////////////////////////////
inline Vec3 &Vec3::operator += (const Vec2 &v) 
{
	X += v.X;
	Y += v.Y;

	return *this;
}

/////////////////////////////////////////////////////
/// Operator: ADD_ASSIGN (a Vec3 to this one)
///	Params: [in]v
///
/////////////////////////////////////////////////////
inline Vec3 &Vec3::operator += (const Vec3 &v) 
{
	X += v.X;
	Y += v.Y;
	Z += v.Z;

	return *this;
}

/////////////////////////////////////////////////////
/// Operator: SUBTRACT_ASSIGN (a Vec3 from this one)
/// Params: [in]v
///
/////////////////////////////////////////////////////
inline Vec3 &Vec3::operator -= (const Vec2 &v) 
{
	X -= v.X;
	Y -= v.Y;

	return *this;
}

/////////////////////////////////////////////////////
/// Operator: SUBTRACT_ASSIGN (a Vec3 from this one)
/// Params: [in]v
///
/////////////////////////////////////////////////////
inline Vec3 &Vec3::operator -= (const Vec3 &v) 
{
	X -= v.X;
	Y -= v.Y;
	Z -= v.Z;

	return *this;
}

/////////////////////////////////////////////////////
/// Operator: MULTIPLY_ASSIGN (a Vec3 by this one)
///	Params: [in]v
///
/////////////////////////////////////////////////////
inline Vec3 &Vec3::operator *= (const Vec3 &v) 
{
	X *= v.X;
	Y *= v.Y;
	Z *= v.Z;

	return *this;
}

/////////////////////////////////////////////////////
/// Operator: DIVIDE_ASSIGN (a Vec3 by this one)
/// Params: [in]v
///
/////////////////////////////////////////////////////
inline Vec3 &Vec3::operator /= (const Vec3 &v) 
{
	X /= v.X;
	Y /= v.Y;
	Z /= v.Z;

	return *this;
}

/////////////////////////////////////////////////////
/// Operator: MULTIPLY_ASSIGN (the Vec3 by a float)
/// Params: [in]f
///
///////////////////////////////////////////////////// 
inline Vec3 &Vec3::operator *= (float f) 
{
	X *= f;
	Y *= f;
	Z *= f;

	return *this;
}

/////////////////////////////////////////////////////
/// Operator: DIVIDE_ASSIGN (the Vec3 by a float)
/// Params: [in]f
///
///////////////////////////////////////////////////// 
inline Vec3 &Vec3::operator /= (float f) 
{
	X /= f;
	Y /= f;
	Z /= f;

	return *this;
}

/////////////////////////////////////////////////////
/// Operator: IS_EQUAL (Are these two Vec3's equal)
/// Params: [in]a, [in]b
///
///////////////////////////////////////////////////// 
inline bool operator == (const Vec3 &a, const Vec3 &b) 
{
	return ((a.X == b.X) && (a.Y == b.Y) && (a.Z == b.Z));
}

/////////////////////////////////////////////////////
/// Operator: IS_NOT_EQUAL (Are these two Vec3's not equal)
/// Params: [in]a, [in]b
///
/////////////////////////////////////////////////////
inline bool operator != (const Vec3 &a, const Vec3 &b) 
{
	return ((a.X != b.X) || (a.Y != b.Y) || (a.Z != b.Z));
}

/////////////////////////////////////////////////////
/// Operator: ADD
/// Params: [in]a, [in]b
///
///////////////////////////////////////////////////// 
inline Vec3 operator + (const Vec3 &a, const Vec2 &b) 
{
	Vec3 ret(a);

	ret += b;

	return ret;
}

/////////////////////////////////////////////////////
/// Operator: ADD (two Vec3's)
/// Params: [in]a, [in]b
///
///////////////////////////////////////////////////// 
inline Vec3 operator + (const Vec3 &a, const Vec3 &b) 
{
	Vec3 ret(a);

	ret += b;

	return ret;
}

/////////////////////////////////////////////////////
/// Operator: SUBTRACT 
/// Params: [in]a, [in]b
///
///////////////////////////////////////////////////// 
inline Vec3 operator - (const Vec3 &a, const Vec2 &b) 
{
	Vec3 ret(a);

	ret -= b;

	return ret;
}

/////////////////////////////////////////////////////
/// Operator: SUBTRACT (one Vec3 from another)
/// Params: [in]a, [in]b
///
///////////////////////////////////////////////////// 
inline Vec3 operator - (const Vec3 &a, const Vec3 &b) 
{
	Vec3 ret(a);

	ret -= b;

	return ret;
}

/////////////////////////////////////////////////////
/// Operator: MULTIPLY (one Vec3 by another)
/// Params: [in]a, [in]b
///
///////////////////////////////////////////////////// 
inline Vec3 operator * (const Vec3 &a, const Vec3 &b) 
{
	Vec3 ret(a);

	ret *= b;

	return ret;
}

/////////////////////////////////////////////////////
/// Operator: DIVIDE (one Vec3 by another)
/// Params: [in]a, [in]b
///
///////////////////////////////////////////////////// 
inline Vec3 operator / (const Vec3 &a, const Vec3 &b) 
{
	Vec3 ret(a);

	ret /= b;

	return ret;
}

/////////////////////////////////////////////////////
/// Operator: ADD (Vec3 by a float)
/// Params: [in]v, [in]f
///
///////////////////////////////////////////////////// 
inline Vec3 operator + (const Vec3 &v, float f) 
{
	return Vec3(f + v.X, f + v.Y, f + v.Z);
}

/////////////////////////////////////////////////////
/// Operator: SUBTRACT (Vec3 by a float)
/// Params: [in]v, [in]f
///
///////////////////////////////////////////////////// 
inline Vec3 operator - (const Vec3 &v, float f) 
{
	return Vec3(f - v.X, f - v.Y, f - v.Z);
}

/////////////////////////////////////////////////////
/// Operator: MULTIPLY (Vec3 by a float)
/// Params: [in]v, [in]f
///
///////////////////////////////////////////////////// 
inline Vec3 operator * (const Vec3 &v, float f) 
{
	return Vec3(f * v.X, f * v.Y, f * v.Z);
}

/////////////////////////////////////////////////////
/// Operator: DIVIDE (Vec3 by a float)
/// Params: [in]v, [in]f
///
///////////////////////////////////////////////////// 
inline Vec3 operator / (const Vec3 &v, float f) 
{
	return Vec3(v.X / f, v.Y / f, v.Z / f);
}

/////////////////////////////////////////////////////
/// Operator: SUBTRACT (a Vec3)
/// Params: [in]a
///
/////////////////////////////////////////////////////
inline Vec3 operator - (const Vec3 &a) 
{
	return Vec3(-a.X, -a.Y, -a.Z);
}

/////////////////////////////////////////////////////
/// Operator: MAGNITUDE (of a Vec3)
/// Params: [in]V
///
/////////////////////////////////////////////////////
inline double Vec3::operator | (const Vec3 &V) const
{
	// A | B = A.Magnitude() * A.Magnitude() * Cos(AB);
	return X*V.X + Y*V.Y + Z*V.Z;
	// NormOfVector | Vector = LengthOfVector
}

/////////////////////////////////////////////////////
/// Method: length (Get length of a Vec3)
/// Params: None
///
///////////////////////////////////////////////////// 
inline float Vec3::length() const 
{
	return std::sqrt(X*X + Y*Y + Z*Z);
}

/////////////////////////////////////////////////////
/// Method: lengthSqr (Get squared length of a Vec3)
/// Params: None
///
///////////////////////////////////////////////////// 
inline float Vec3::lengthSqr() const 
{
	return (X*X + Y*Y + Z*Z);
}

/////////////////////////////////////////////////////
/// Method: setZero (Set the values to zero)
/// Params: None
///
///////////////////////////////////////////////////// 
inline void Vec3::setZero() 
{
	X = Y = Z = 0.0f;
}

/////////////////////////////////////////////////////
/// Method: isZero (Does Vec3 equal (0, 0, 0))
/// Params: None
///
/////////////////////////////////////////////////////
inline bool Vec3::isZero() const 
{
	return ((X == 0.0f) && (Y == 0.0f) && (Z == 0.0f));
}

/////////////////////////////////////////////////////
/// Method: normalise (a Vec3)
/// Params: None
///
///////////////////////////////////////////////////// 
inline Vec3 &Vec3::normalise() 
{
	float m = length();

	if (m > 0.0f) 
		m = 1.0f / m;
	else 
		m = 0.0f;

	X *= m;
	Y *= m;
	Z *= m;

	return *this;
}

/////////////////////////////////////////////////////
/// Method: TransformByMatrix4x4 (a Vec3 by a 4x4)
/// Params: [in]m 
///
/////////////////////////////////////////////////////
inline void Vec3::TransformByMatrix4x4( const float* m )
{
	float vector[3];

	vector[_X_] = X*m[0] + Y*m[4] + Z*m[8] + m[12];
	vector[_Y_] = X*m[1] + Y*m[5] + Z*m[9] + m[13];
	vector[_Z_] = X*m[2] + Y*m[6] + Z*m[10] + m[14];

	X = vector[_X_];
	Y = vector[_Y_];
	Z = vector[_Z_];
}

/////////////////////////////////////////////////////
/// Operator: OUTPUT_STREAM (a Vec3)
/// Params: [in]stream, [in]v
///
/////////////////////////////////////////////////////
inline std::ostream& operator << ( std::ostream& stream, Vec3& v )
{
	stream << "[ ";
	stream << v.X << ", ";
	stream << v.Y << ", ";
	stream << v.Z;
	stream << " ]";

	return(stream);
}

// ##################################################################################################################################################
// ##################################################################################################################################################

/////////////////////////////////////////////////////
/// Constructor
/// Params: [in]inX, [in]inY, [in]inZ , [in]inW
///
/////////////////////////////////////////////////////
inline Vec4::Vec4(float inX, float inY, float inZ, float inW) 
{
	X = inX;
	Y = inY;
	Z = inZ;
	W = inW;
}

/////////////////////////////////////////////////////
/// Constructor with initialising Vec4
/// Params: [in]v
///
///////////////////////////////////////////////////// 
inline Vec4::Vec4(const Vec4 &v) 
{
	X = v.X;
	Y = v.Y;
	Z = v.Z;
	W = v.W;
}

/////////////////////////////////////////////////////
/// Constructor with initialising Vec3
/// Params: [in]v
///
/////////////////////////////////////////////////////
inline Vec4::Vec4(const Vec3 &v) 
{
	X = v.X;
	Y = v.Y;
	Z = v.Z;
	W = 1.0f;
}

/////////////////////////////////////////////////////
/// Operator: ARRAY_INDEX
/// Params: [in]i
///
/////////////////////////////////////////////////////
inline float &Vec4::operator [] (unsigned int i) 
{
//	ASSERT(i<3);
        
	return *(&X+i);
}

/////////////////////////////////////////////////////
/// Operator: ARRAY_INDEX
/// Params: [in]i
///
/////////////////////////////////////////////////////
inline const float &Vec4::operator [] (unsigned int i) const 
{
//	ASSERT (i<3);

	return *(&X+i);
}

/////////////////////////////////////////////////////
/// Operator: ASSIGN (from a Vec4)
/// Params: [in]v
///
/////////////////////////////////////////////////////
inline Vec4 &Vec4::operator = (const Vec4 &v) 
{
	X = v.X;
	Y = v.Y;
	Z = v.Z;
	W = v.W;

	return *this;
}

/////////////////////////////////////////////////////
/// Operator: ASSIGN (from a Vec3)
/// Params: [in]v
///
/////////////////////////////////////////////////////
inline Vec4 &Vec4::operator = (const Vec3 &v) 
{
	X = v.X;
	Y = v.Y;
	Z = v.Z;
	W = 1.0f;

	return *this;
}

/////////////////////////////////////////////////////
/// Operator: ADD_ASSIGN (a Vec4 to this one)
///	Params: [in]v
///
/////////////////////////////////////////////////////
inline Vec4 &Vec4::operator += (const Vec4 &v) 
{
	X += v.X;
	Y += v.Y;
	Z += v.Z;
	W += v.W;

	return *this;
}

/////////////////////////////////////////////////////
/// Operator: SUBTRACT_ASSIGN (a Vec4 from this one)
/// Params: [in]v
///
/////////////////////////////////////////////////////
inline Vec4 &Vec4::operator -= (const Vec4 &v) 
{
	X -= v.X;
	Y -= v.Y;
	Z -= v.Z;
	W -= v.W;

	return *this;
}

/////////////////////////////////////////////////////
/// Operator: MULTIPLY_ASSIGN (a Vec3 by this one)
///	Params: [in]v
///
/////////////////////////////////////////////////////
inline Vec4 &Vec4::operator *= (const Vec4 &v) 
{
	X *= v.X;
	Y *= v.Y;
	Z *= v.Z;
	W *= v.W;

	return *this;
}

/////////////////////////////////////////////////////
/// Operator: MULTIPLY_ASSIGN (the Vec4 by a float)
/// Params: [in]f
///
///////////////////////////////////////////////////// 
inline Vec4 &Vec4::operator *= (float f) 
{
	X *= f;
	Y *= f;
	Z *= f;
	W *= f;

	return *this;
}

/////////////////////////////////////////////////////
/// Operator: DIVIDE_ASSIGN (the Vec4 by a float)
/// Params: [in]f
///
///////////////////////////////////////////////////// 
inline Vec4 &Vec4::operator /= (float f) 
{
	X /= f;
	Y /= f;
	Z /= f;
	W /= f;

	return *this;
}

/////////////////////////////////////////////////////
/// Operator: IS_EQUAL (Are these two Vec4's equal)
/// Params: [in]a, [in]b
///
///////////////////////////////////////////////////// 
inline bool operator == (const Vec4 &a, const Vec4 &b) 
{
	return ((a.X == b.X) && (a.Y == b.Y) && (a.Z == b.Z) && (a.W == b.W) );
}

/////////////////////////////////////////////////////
/// Operator: IS_NOT_EQUAL (Are these two Vec3's not equal)
/// Params: [in]a, [in]b
///
/////////////////////////////////////////////////////
inline bool operator != (const Vec4 &a, const Vec4 &b) 
{
	return ((a.X != b.X) || (a.Y != b.Y) || (a.Z != b.Z) || (a.W != b.W) );
}

/////////////////////////////////////////////////////
/// Operator: ADD (two Vec4's)
/// Params: [in]a, [in]b
///
///////////////////////////////////////////////////// 
inline Vec4 operator + (const Vec4 &a, const Vec4 &b) 
{
	Vec4 ret(a);

	ret += b;

	return ret;
}

/////////////////////////////////////////////////////
/// Operator: SUBTRACT (one Vec3 from another)
/// Params: [in]a, [in]b
///
///////////////////////////////////////////////////// 
inline Vec4 operator - (const Vec4 &a, const Vec4 &b) 
{
	Vec4 ret(a);

	ret -= b;

	return ret;
}

/////////////////////////////////////////////////////
/// Operator: MULTIPLY (one Vec4 by another)
/// Params: [in]a, [in]b
///
///////////////////////////////////////////////////// 
inline Vec4 operator * (const Vec4 &a, const Vec4 &b) 
{
	Vec4 ret(a);

	ret *= b;

	return ret;
}

/////////////////////////////////////////////////////
/// Operator: MULTIPLY (Vec4 by a float)
/// Params: [in]v, [in]f
///
///////////////////////////////////////////////////// 
inline Vec4 operator * (const Vec4 &v, float f) 
{
	return Vec4(f * v.X, f * v.Y, f * v.Z, f * v.W);
}

/////////////////////////////////////////////////////
/// Operator: DIVIDE (Vec4 by a float)
/// Params: [in]v, [in]f
///
///////////////////////////////////////////////////// 
inline Vec4 operator / (const Vec4 &v, float f) 
{
	return Vec4(v.X / f, v.Y / f, v.Z / f, v.W / f);
}

/////////////////////////////////////////////////////
/// Operator: SUBTRACT (a Vec4)
/// Params: [in]a
///
/////////////////////////////////////////////////////
inline Vec4 operator - (const Vec4 &a) 
{
	return Vec4(-a.X, -a.Y, -a.Z, -a.W);
}

/////////////////////////////////////////////////////
/// Operator: MAGNITUDE (of a Vec3)
/// Params: [in]V
///
/////////////////////////////////////////////////////
inline double Vec4::operator | (const Vec4 &V) const
{
	// A | B = A.Magnitude() * A.Magnitude() * Cos(AB);
	return X*V.X + Y*V.Y + Z*V.Z + W*V.W;
	// NormOfVector | Vector = LengthOfVector
}

/////////////////////////////////////////////////////
/// Method: length (Get length of a Vec4)
/// Params: None
///
///////////////////////////////////////////////////// 
inline float Vec4::length() const 
{
	return std::sqrt(X*X + Y*Y + Z*Z + W*W);
}

/////////////////////////////////////////////////////
/// Method: lengthSqr (Get squared length of a Vec4)
/// Params: None
///
///////////////////////////////////////////////////// 
inline float Vec4::lengthSqr() const 
{
	return (X*X + Y*Y + Z*Z + W*W);
}

/////////////////////////////////////////////////////
/// Method: setZero (Set the values to zero)
/// Params: None
///
///////////////////////////////////////////////////// 
inline void Vec4::setZero() 
{
	X = Y = Z = W = 0.0f;
}

/////////////////////////////////////////////////////
/// Method: isZero (Does Vec4 equal (0, 0, 0))
/// Params: None
///
/////////////////////////////////////////////////////
inline bool Vec4::isZero() const 
{
	return ((X == 0.0f) && (Y == 0.0f) && (Z == 0.0f) && (W == 0.0f));
}

/////////////////////////////////////////////////////
/// Method: normalise (a Vec4)
/// Params: None
///
///////////////////////////////////////////////////// 
inline Vec4 &Vec4::normalise() 
{
	float m = length();

	if (m > 0.0f) 
		m = 1.0f / m;
	else 
		m = 0.0f;

	X *= m;
	Y *= m;
	Z *= m;
	W *= m;

	return *this;
}

/////////////////////////////////////////////////////
/// Method: transform (a Vec3 by a 4x4)
/// Params: [in]m 
///
/////////////////////////////////////////////////////
inline void Vec4::TransformByMatrix4x4( const float *m )
{
	float vector[4];

	vector[_X_] = X*m[0] + Y*m[4] + Z*m[8] + m[12];
	vector[_Y_] = X*m[1] + Y*m[5] + Z*m[9] + m[13];
	vector[_Z_] = X*m[2] + Y*m[6] + Z*m[10] + m[14];

	X = vector[_X_];
	Y = vector[_Y_];
	Z = vector[_Z_];
	W = 1.0f;
}

/////////////////////////////////////////////////////
/// Operator: OUTPUT_STREAM (a Vec4)
/// Params: [in]stream, [in]v
///
/////////////////////////////////////////////////////
inline std::ostream& operator << ( std::ostream& stream, Vec4& v )
{
	stream << "[ ";
	stream << v.X << ", ";
	stream << v.Y << ", ";
	stream << v.Z << ", ";
	stream << v.W;
	stream << " ]";

	return(stream);
}

// ##################################################################################################################################################
// ##################################################################################################################################################

/////////////////////////////////////////////////////
/// Function: isZeroVector (is a Vec3 equal to zero (non-member))
/// Params: [in]a
///
/////////////////////////////////////////////////////
inline int isZeroVector(Vec3& a) 
{
	if ((a.X == 0.0f) && (a.Y == 0.0f) && (a.Z == 0.0f))
		return(true);
   
	return(false);	
}

/////////////////////////////////////////////////////
/// Function: setVectorLength (of a Vec3)
/// Params: [in]v, [in]l
///
/////////////////////////////////////////////////////
inline void setVectorLength(Vec3& v, float l) 
{
	float len = std::sqrt(v.X*v.X + v.Y*v.Y + v.Z*v.Z);	
		
	DBG_ASSERT( (len == 0.0f) );

	v.X = v.X * (l / len);
	v.Y = v.Y * (l / len);
	v.Z = v.Z * (l / len);
} 

/////////////////////////////////////////////////////
/// Function: DotProduct (of two Vec3's)
/// Params: [in]a, [in]b
///
/////////////////////////////////////////////////////
inline float DotProduct(const Vec2 &a, const Vec2 &b) 
{
	return( (a.X*b.X) + (a.Y*b.Y) );
}

/////////////////////////////////////////////////////
/// Function: DotProduct (of two Vec3's)
/// Params: [in]a, [in]b
///
/////////////////////////////////////////////////////
inline float DotProduct(const Vec3 &a, const Vec3 &b) 
{
	return( (a.X*b.X) + (a.Y*b.Y) + (a.Z*b.Z) );
}

/////////////////////////////////////////////////////
/// Function: SwapVec (Swap two Vec3's)
/// Params: [in]a, [in]b
///
///////////////////////////////////////////////////// 
inline void SwapVec(Vec3 &a, Vec3 &b) 
{
	Vec3 tmp(a);

	a = b;
	b = tmp;
}

/////////////////////////////////////////////////////
/// Function: CrossProduct (of two Vec3's)
/// Params: [in]a, [in]b
///
///////////////////////////////////////////////////// 
inline Vec3 CrossProduct(const Vec3 &a, const Vec3 &b) 
{
	return Vec3(a.Y*b.Z - a.Z*b.Y,
					a.Z*b.X - a.X*b.Z,
					a.X*b.Y - a.Y*b.X);
}

/////////////////////////////////////////////////////
/// Function: Normalise (Vec3)
/// Params: [in]a
///
///////////////////////////////////////////////////// 
inline Vec2 Normalise( const Vec2 &a )
{
	float m = a.length();

	if (m > 0.0f) 
		m = 1.0f / m;
	else 
		m = 0.0f;

	return Vec2( a.X * m, a.Y * m );
}

/////////////////////////////////////////////////////
/// Function: Normalise (Vec3)
/// Params: [in]a
///
///////////////////////////////////////////////////// 
inline Vec3 Normalise( const Vec3 &a )
{
	float m = a.length();

	if (m > 0.0f) 
		m = 1.0f / m;
	else 
		m = 0.0f;

	return Vec3( a.X * m, a.Y * m, a.Z * m );
}

/////////////////////////////////////////////////////
/// Function: Lerp (two Vec3)
/// Params: [in]a, [in]b, [in]t
///
///////////////////////////////////////////////////// 
inline Vec3 Lerp( const Vec3 &a, const Vec3 &b, float t )
{
	float invt = 1.0f-t;
	
	return Vec3(((a.X*invt) + (b.X*t)),
					 ((a.Y*invt) + (b.Y*t)),
					 ((a.Z*invt) + (b.Z*t)) );
}

/////////////////////////////////////////////////////
/// Function: Lerp (two Vec4)
/// Params: [in]a, [in]b, [in]t
///
///////////////////////////////////////////////////// 
inline Vec4 Lerp( const Vec4 &a, const Vec4 &b, float t )
{
	float invt = 1.0f-t;
	
	return Vec4(((a.X*invt) + (b.X*t)),
					 ((a.Y*invt) + (b.Y*t)),
					 ((a.Z*invt) + (b.Z*t)),
					 ((a.W*invt) + (b.W*t)) );
}

/////////////////////////////////////////////////////
/// Function: AngleBetweenVectors
/// Params: [in]v1, [in]v2
///
/////////////////////////////////////////////////////
inline float AngleBetweenVectors( const Vec3 &v1, const Vec3 &v2 )
{							
	// Get the dot product of the vectors
	float dotProduct = DotProduct( v1, v2 );				

	// Get the product of both of the vectors magnitudes
	float vectorsMagnitude = v1.length() * v2.length();

	//DBG_ASSERT( ( vectorsMagnitude != 0.0f ) );
	if( vectorsMagnitude == 0.0f )
		return(0.0f);

	// Return the arc cosine of the (dotProduct / vectorsMagnitude) which is the angle in RADIANS.
//	if( dotProduct < 0 )
//		return( (float)-acos( dotProduct / vectorsMagnitude ) );
//	else
	return( static_cast<float>(std::acos( dotProduct / vectorsMagnitude ) ));

}

/////////////////////////////////////////////////////
/// Function: AngleBetweenXZ
/// Params: [in]v1, [in]v2
///
/////////////////////////////////////////////////////
inline float AngleBetweenXZ( const Vec3 &v1, const Vec3 &v2 )
{							
	Vec3 vVector = v2 - v1;

	return( static_cast<float>(atan2( vVector.X, vVector.Z ) ));
}

/////////////////////////////////////////////////////
/// Function: AngleBetweenXY
/// Params: [in]v1, [in]v2
///
/////////////////////////////////////////////////////
inline float AngleBetweenXY( const Vec3 &v1, const Vec3 &v2 )
{							
	Vec3 vVector = v2 - v1;

	return( std::atan2( vVector.X, vVector.Y ) );
}

/////////////////////////////////////////////////////
/// Function: RotateVector (Rotate a Vec3 by a 4x4)
/// Params: [in]matrix, [in]vecIn, [out]vecOut
///
/////////////////////////////////////////////////////
inline void RotateVector( const float *m, const Vec3 &vecIn, Vec3 &vecOut)		
{
	vecOut.X = (m[0] * vecIn.X) + (m[4] * vecIn.Y) + (m[8]  * vecIn.Z);	
	vecOut.Y = (m[1] * vecIn.X) + (m[5] * vecIn.Y) + (m[9]  * vecIn.Z);	
	vecOut.Z = (m[2] * vecIn.X) + (m[6] * vecIn.Y) + (m[10] * vecIn.Z);
}

/////////////////////////////////////////////////////
/// Function: DistanceSquared
/// Params: [in]a, [in]b
///
/////////////////////////////////////////////////////
inline float Distance( const Vec3 &vPoint1, const Vec3 &vPoint2 )
{
	double distance = std::sqrt( (vPoint2.X - vPoint1.X) * (vPoint2.X - vPoint1.X) +
						    (vPoint2.Y - vPoint1.Y) * (vPoint2.Y - vPoint1.Y) +
						    (vPoint2.Z - vPoint1.Z) * (vPoint2.Z - vPoint1.Z) );

	// Return the distance between the 2 points
	return static_cast<float>(distance);
}

/////////////////////////////////////////////////////
/// Function: DistanceSquared
/// Params: [in]a, [in]b
///
/////////////////////////////////////////////////////
inline float DistanceSquared(const Vec3 &a, const Vec3 &b)
{
	float dx = a.X - b.X;
	float dy = a.Y - b.Y;
	float dz = a.Z - b.Z;

	return dx*dx + dy*dy + dz*dz;
}

/////////////////////////////////////////////////////
/// Function: TransformByMatrix4x4
/// Params: [in]a, [in]m
///
/////////////////////////////////////////////////////
inline Vec3 TransformByMatrix4x4( const Vec3 &a, const float *m )
{
	Vec3 v;

	v.X = a.X*m[0] + a.Y*m[4] + a.Z*m[8] + (m[12]);
	v.Y = a.X*m[1] + a.Y*m[5] + a.Z*m[9] + (m[13]);
	v.Z = a.X*m[2] + a.Y*m[6] + a.Z*m[10] + (m[14]);

	return( v );
}

/////////////////////////////////////////////////////
/// Function: WithinTolerance
/// Params: [in]a, [in]b, [in]tolerance
///
/////////////////////////////////////////////////////
inline bool WithinTolerance( const Vec3 &a, const Vec3 &b, float tolerance )
{
	float x = std::abs( a.X - b.X );
	if( x > tolerance )
		return(false);
	float y = std::abs( a.Y - b.Y );
	if( y > tolerance )
		return(false);
	float z = std::abs( a.Z - b.Z );
	if( z > tolerance )
		return(false);

	return(true);
}

/////////////////////////////////////////////////////
/// Function: WithinTolerance
/// Params: [in]a, [in]b, [in]tolerance
///
/////////////////////////////////////////////////////
inline bool WithinTolerance( const Vec4 &a, const Vec4 &b, float tolerance )
{
	float x = std::abs( a.X - b.X );
	if( x > tolerance )
		return(false);
	float y = std::abs( a.Y - b.Y );
	if( y > tolerance )
		return(false);
	float z = std::abs( a.Z - b.Z );
	if( z > tolerance )
		return(false);
	float w = std::abs( a.W - b.W );
	if( w > tolerance )
		return(false);

	return(true);
}

} // namespace math

#endif // __VECTORS_H__





