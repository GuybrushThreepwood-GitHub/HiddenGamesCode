
#ifndef __MATRIX_H__
#define __MATRIX_H__

namespace math
{
	// forward declare
	class Vec3;
	class Quaternion;

	class Matrix3x3
	{
		public:

			/// union containing multiple forms of using the matrix
			// row major
			union 
			{
				struct
				{
					/// matrix as a structure
					// m11 m12 m13
					// m21 m22 m23
					// m31 m32 m33
					float m11, m12, m13;
					float m21, m22, m23;
					float m31, m32, m33;
				};

				/// matrix as an array
				// 0 1 2
				// 3 4 5
				// 6 7 8
				float m[9];
			};
				
		public:
			/// default constructor
			Matrix3x3();
			/// constructor
			/// \param a - intialising 3x3 matrix
			Matrix3x3(const Matrix3x3  &a);
			/// constructor
			/// \param m - intialising 3x3 matrix
			Matrix3x3(const float m[9]);
			/// constructor
			/// \param q - initialising a 3x3 matrix from a quaternion
			Matrix3x3(const Quaternion &q );
			/// default destructor
			~Matrix3x3();

			/// OPERATOR 'ADD' - adds two matrices values together
			/// \param m - matrix
			/// \return Matrix3x3 - ( SUCCESS: new matrix )
			Matrix3x3 operator+( const Matrix3x3 &m ) const;
			/// OPERATOR 'SUBTRACT' - subtract two matrices values together
			/// \param m - matrix
			/// \return Matrix3x3 - ( SUCCESS: new matrix )
			Matrix3x3 operator-( const Matrix3x3 &m ) const;
			/// OPERATOR 'MULTIPLY' - multiplies two matrices values together
			/// \param m - matrix
			/// \return Matrix3x3 - ( SUCCESS: new matrix )
			Matrix3x3 operator*(const Matrix3x3 &m) const;

			/// Identity - Sets the matrix to default identity
			void Identity( void );
			/// SetXRotation - Sets a Rotation on the X axis
			/// \param fAngleRadians - angle to rotate (in radians)
			void SetXRotation( float fAngleRadians );
			/// SetYRotation - Sets a Rotation on the Y axis
			/// \param fAngleRadians - angle to rotate (in radians)
			void SetYRotation( float fAngleRadians );
			/// SetZRotation - Sets a Rotation on the Z axis
			/// \param fAngleRadians - angle to rotate (in radians)
			void SetZRotation( float fAngleRadians );
			/// SetupRotate - Sets a rotation
			/// \param axis - axis to rotate on
			/// \param theta - angle to rotate on the axis
			void SetupAxisAngle( const math::Vec3 &axis, float theta );
			/// SetRotationAngles - Sets a matrix from rotation angles
			/// \param angles - angles to rotate on each axis
			void SetRotationAngles( const math::Vec3 &angles );
			/// SetScale - Sets a scale
			/// \param fXScale - scale on x axis
			/// \param fYScale - scale on y axis
			/// \param fZScale - scale on z axis
			void SetScale( float fXScale, float fYScale, float fZScale );
			/// Determinant - determinant of the matrix
			float Determinant( void );
			/// Inverse - inverse of the matrix and return a new one
			Matrix3x3 Inverse( void );
			/// Invert - inverts 'this' matrix
			void Invert( void );
			/// ConvertToQuaternion - Convert matrix to a quaternion
			math::Quaternion ConvertToQuaternion( void );

			/// Print - prints the elements of the matrix
			void Print( void );
	};

	/////////////////////////////

	class Matrix4x3 
	{
		public:
			/// union containing multiple forms of using the matrix
			union 
			{
				struct
				{
					/// matrix as a structure
					// m11 m12 m13
					// m21 m22 m23
					// m31 m32 m33
					// tx  ty  tz
					float m11, m12, m13;
					float m21, m22, m23;
					float m31, m32, m33;
					float tx, ty, tz;
				};

				/// matrix as an array
				// 0  1  2
				// 3  4  5
				// 6  7  8
				// 9  10 11
				float m[12];
			};
				
		public:
			/// default constructor
			Matrix4x3();
			/// \param m - intialising 4x3 matrix
			Matrix4x3(const float m[12]);
			/// \param a - intialising 4x3 matrix
			Matrix4x3(const Matrix4x3  &a);
			/// default destructor
			~Matrix4x3();

			/// Identity - Sets the matrix to default identity
			void Identity( void );

			// Access the translation portion of the matrix directly
			void ZeroTranslation( void );
			void SetTranslation(const math::Vec3 &d);
			void SetupTranslation(const math::Vec3 &d);

			// Setup the matrix to perform a rotation about a cardinal axis
			void SetupRotate(int axis, float theta);

			// Setup the matrix to perform a rotation about an arbitrary axis
			void SetupRotate(const Vec3 &axis, float theta);

			// Setup the matrix to perform a rotation, given
			// the angular displacement in quaternion form
			void FromQuaternion( const math::Quaternion &q );

			// Setup the matrix to perform scale on each axis
			void SetupScale(const math::Vec3 &s);

			// Setup the matrix to perform scale along an arbitrary axis
			void SetupScaleAlongAxis(const math::Vec3 &axis, float k);

			// Setup the matrix to perform a shear
			void SetupShear(int axis, float s, float t);

			// Setup the matrix to perform a projection onto a plane passing
			// through the origin
			void SetupProject(const Vec3 &n);

			// Setup the matrix to perform a reflection about a plane parallel
			// to a cardinal plane
			void SetupReflect(int axis, float k = 0.0f);

			// Setup the matrix to perform a reflection about an arbitrary plane
			// through the origin
			void SetupReflect(const Vec3 &n);
	};

//	Vec3	operator*( const Vec3 &p, const Matrix4x3 &m );
//	Matrix4x3	operator*( const Matrix4x3 &a, const Matrix4x3 &b );

	// Operator *= for conformance to C++ standards
	Vec3	&operator*=( Vec3 &p, const Matrix4x3 &m ); 
	Matrix4x3	&operator*=( const Matrix4x3 &a, const Matrix4x3 &m );

	// Compute the determinant of the 3x3 portion of the matrix
	float Determinant( const Matrix4x3 &m );

	// Compute the inverse of a matrix
	Matrix4x3 Inverse( const Matrix4x3 &m );

	// Extract the translation portion of the matrix
	Vec3	GetTranslation( const Matrix4x3 &m );

	// Extract the position/orientation from a local->parent matrix,
	// or a parent->local matrix
	Vec3	GetPositionFromParentToLocalMatrix( const Matrix4x3 &m );
	Vec3	GetPositionFromLocalToParentMatrix( const Matrix4x3 &m );

	/////////////////////////////

	class Matrix4x4
	{
		public:
			/// union containing multiple forms of using the matrix
			union
			{
				struct
				{
					/// matrix as a structure
					// m11 m12 m13 m14
					// m21 m22 m23 m24
					// m31 m32 m33 m34
					// m41 m42 m43 m44
					float m11, m12, m13, m14;
					float m21, m22, m23, m24;
					float m31, m32, m33, m34;
					float m41, m42, m43, m44;			
				};

				/// matrix as an array
				// 0  1  2  3
				// 4  5  6  7
				// 8  9  10 11
				// 12 13 14 15
				float m[16];
			};
				
		public:
			/// default constructor
			Matrix4x4();
			/// constructor
			/// \param a - intialising 4x4 matrix
			Matrix4x4(const Matrix4x4  &a);
			/// constructor
			/// \param m - intialising 3x3 matrix
			Matrix4x4(const float m[16]);
			/// constructor
			/// \param q - initialising a 4x4 matrix from a quaternion
			Matrix4x4(const math::Quaternion &q );
			/// default destructor
			~Matrix4x4();

			/// OPERATOR 'ADD' - adds two matrices values together
			/// \param m - matrix
			/// \return Matrix4x4 - ( SUCCESS: new matrix )
			Matrix4x4 operator+( const Matrix4x4 &m ) const;
			/// OPERATOR 'SUBTRACT' - subtract two matrices values together
			/// \param m - matrix
			/// \return Matrix4x4 - ( SUCCESS: new matrix )
			Matrix4x4 operator-( const Matrix4x4 &m ) const;
			/// OPERATOR 'MULTIPLY' - multiplies two matrices values together
			/// \param m - matrix
			/// \return Matrix4x4 - ( SUCCESS: new matrix )
			Matrix4x4 operator*(const Matrix4x4 &m) const;
			/// OPERATOR 'MULTIPLY' - multiplies a vector  by 'this' matrix
			/// \param v - vector
			/// \return Vec3 - ( SUCCESS: new vector )
			math::Vec3 operator*(const math::Vec3 &v) const;
			/// OPERATOR 'MULTIPLY' - multiplies a matrix by a float
			/// \param f - float
			/// \return Matrix4x4 - ( SUCCESS: new matrix )
			Matrix4x4 operator*(const float &f) const;

			/// Identity - Sets the matrix to default identity
			void Identity( void );
			/// SetXRotation - Sets a Rotation on the X axis
			/// \param fAngleRadians - angle to rotate (in radians)
			void SetXRotation( float fAngleRadians );
			/// SetYRotation - Sets a Rotation on the Y axis
			/// \param fAngleRadians - angle to rotate (in radians)
			void SetYRotation( float fAngleRadians );
			/// SetZRotation - Sets a Rotation on the Z axis
			/// \param fAngleRadians - angle to rotate (in radians)
			void SetZRotation( float fAngleRadians );
			/// SetupRotate - Sets a rotation
			/// \param axis - axis to rotate on
			/// \param theta - angle to rotate on the axis
			void SetupAxisAngle( const math::Vec3 &axis, float theta );
			/// SetRotationAngles - Sets a matrix from rotation angles
			/// \param angles - angles to rotate on each axis
			void SetRotationAngles( const math::Vec3 &angles );
			/// SetScale - Sets a scale
			/// \param fXScale - scale on x axis
			/// \param fYScale - scale on y axis
			/// \param fZScale - scale on z axis
			void SetScale( float fXScale, float fYScale, float fZScale );
			/// Determinant - determinant of the matrix
			float Determinant( void );
			/// Multiply - multiplies 'this' matrix by parameter matrix
			/// \param m = matrix to multiply 
			void Multiply( Matrix4x4 &m );
			/// SetTranslation - Translate a vector by the matrix translation amount
			/// \param v - vector to translate 
			void SetTranslation( const Vec3 &v );
			/// SetTranslation - Translate a vector by the matrix translation amount
			/// \param fTranslation - float pointer to translate 
			void SetTranslation( float *fTranslation );
			/// InverseTranslation - Inverse translate a vector by the matrix translation amount
			/// \param v - vector to inverse translate 
			void InverseTranslation( math::Vec3 &v );
			/// InverseTranslation - Inverse Translate a vector by the matrix translation amount
			/// \param fTranslation - float pointer to inverse translate 
			void InverseTranslation( float *fTranslation );
			/// InverseTranslation - Inverse Rotate a vector by the matrix translation amount
			/// \param v - float pointer to inverse rotate 
			void InverseRotate( math::Vec3 &v );
			/// InverseTranslation - Inverse Rotate a float array by the matrix translation amount
			/// \param fRotation - float array to inverse rotate 
			void InverseRotate( float *fRotation );
			/// SetMatrix - Assign the data of 'this' matrix by parameters matrix
			/// \param m - matrix to assign data from
			void SetMatrix( const float *m );
			/// SetMatrix - Assign the data of 'this' matrix by parameters matrix
			/// \param m - matrix to assign data from
			void SetMatrix( const Matrix4x4 *m );
			/// ConvertToQuaternion - Convert matrix to a quaternion
			math::Quaternion ConvertToQuaternion( void );
			/// Print - prints the elements of the matrix
			void Print( void );
	};
} // namespace math
#endif // __MATRIX_H__


