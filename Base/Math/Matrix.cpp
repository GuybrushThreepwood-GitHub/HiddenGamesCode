
/*===================================================================
	File: Matrix.cpp
	Library: Math

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"

#include <cmath>
#include <cstring>
#include <assert.h>

#include "DebugBase.h"
#include "MathBase.h"

using math::Matrix3x3;
using math::Matrix4x3;
using math::Matrix4x4;

namespace
{

}

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
Matrix3x3::Matrix3x3()
{
	Identity();
}

/////////////////////////////////////////////////////
/// Constructor
/// Params: [in]a
///
/////////////////////////////////////////////////////
Matrix3x3::Matrix3x3(const Matrix3x3  &a)
{
	std::memcpy( m, a.m, sizeof( float )*9 );
}

/////////////////////////////////////////////////////
/// Constructor
/// Params: [in]m
///
/////////////////////////////////////////////////////
Matrix3x3::Matrix3x3(const float m[9])
{
	std::memcpy( this->m, m, sizeof( float )*9 );
}

/////////////////////////////////////////////////////
/// Constructor
/// Params: [in]q
///
/////////////////////////////////////////////////////
Matrix3x3::Matrix3x3( const math::Quaternion &q )
{
//         ¦        2     2                                      ¦
//         ¦ 1 - (2Y  + 2Z )   2XY + 2ZW         2XZ - 2YW       ¦
//         ¦                                                     ¦
//         ¦                          2     2                    ¦
//     M = ¦ 2XY - 2ZW         1 - (2X  + 2Z )   2YZ + 2XW       ¦
//         ¦                                                     ¦
//         ¦                                            2     2  ¦
//         ¦ 2XZ + 2YW         2YZ - 2XW         1 - (2X  + 2Y ) ¦
//         ¦                                                     ¦

	float xx      = q.X * q.X;
	float xy      = q.X * q.Y;
	float xz      = q.X * q.Z;
	float xw      = q.X * q.W;
	float yy      = q.Y * q.Y;
	float yz      = q.Y * q.Z;
	float yw      = q.Y * q.W;
	float zz      = q.Z * q.Z;
	float zw      = q.Z * q.W;

	m[0]  = 1.0f - 2.0f * ( yy + zz );
	m[1]  =     2.0f * ( xy + zw );
	m[2]  =     2.0f * ( xz - yw );

	m[3]  =     2.0f * ( xy - zw );
	m[4]  = 1.0f - 2.0f * ( xx + zz );
	m[5]  =     2.0f * ( yz + xw );

	m[6]  =     2.0f * ( xz + yw );
	m[7]  =     2.0f * ( yz - xw );
	m[8] = 1.0f - 2.0f * ( xx + yy );
}

/////////////////////////////////////////////////////
/// Default destructor
/// 
///
/////////////////////////////////////////////////////
Matrix3x3::~Matrix3x3()
{

}

/////////////////////////////////////////////////////
/// Operator: ADD
/// Params: [in]m
///
/////////////////////////////////////////////////////
Matrix3x3 Matrix3x3::operator+( const Matrix3x3 &m ) const
{
//    R = M + L

//        | A B C |   | J K L |
//        |       |   |       |
//      = | D E F | + | M N O |
//        |       |   |       |
//        | G H I |   | P Q R |

//        | A+J B+K C+L |
//        |             |
//      = | D+M E+N F+O |
//        |             |
//        | G+P H+Q I+R |

	float	final[9];

	final[0] = (this->m[0] + m.m[0]); 
	final[1] = (this->m[1] + m.m[1]);
	final[2] = (this->m[2] + m.m[2]);

	final[3] = (this->m[3] + m.m[3]); 
	final[4] = (this->m[4] + m.m[4]);
	final[5] = (this->m[5] + m.m[5]);

	final[6] = (this->m[6] + m.m[6]); 
	final[7] = (this->m[7] + m.m[7]);
	final[8] = (this->m[8] + m.m[8]);

	return( Matrix3x3(final) );
}

/////////////////////////////////////////////////////
/// Operator: SUBTRACT
/// Params: [in]m
///
/////////////////////////////////////////////////////
Matrix3x3 Matrix3x3::operator-( const Matrix3x3 &m ) const
{
//    R = M - L

//        | A B C |   | J K L |
//        |       |   |       |
//      = | D E F | - | M N O |
//        |       |   |       |
//        | G H I |   | P Q R |

//        | A-J B-K C-L |
//        |             |
//      = | D-M E-N F-O |
//        |             |
//        | G-P H-Q I-R |

	float	final[9];

	final[0] = (this->m[0] - m.m[0]); 
	final[1] = (this->m[1] - m.m[1]);
	final[2] = (this->m[2] - m.m[2]);

	final[3] = (this->m[3] - m.m[3]); 
	final[4] = (this->m[4] - m.m[4]);
	final[5] = (this->m[5] - m.m[5]);

	final[6] = (this->m[6] - m.m[6]); 
	final[7] = (this->m[7] - m.m[7]);
	final[8] = (this->m[8] - m.m[8]);

	return( Matrix3x3(final) );
}

/////////////////////////////////////////////////////
/// Operator: MULTIPLY
/// Params: [in]m
///
/////////////////////////////////////////////////////
Matrix3x3 Matrix3x3::operator*( const Matrix3x3 &m ) const
{
// R = M x L

//        | A B C |   | J K L |
//      = | D E F | x | M N O |
//        | G H I |   | P Q R |

//        | AJ+BM+CP  AK+BN+CQ AL+BO+CR |
//      = | DJ+EM+FP  DK+EN+FQ DL+EO+FR |
//        | GJ+HM+IP  GK+HN+IQ GL+HO+IR |

	float	final[9];

	final[0] = (this->m[0]*m.m[0]) + (this->m[1]*m.m[3]) + (this->m[2]*m.m[6]); 
	final[1] = (this->m[0]*m.m[1]) + (this->m[1]*m.m[4]) + (this->m[2]*m.m[7]);
	final[2] = (this->m[0]*m.m[2]) + (this->m[1]*m.m[5]) + (this->m[2]*m.m[8]);

	final[3] = (this->m[3]*m.m[0]) + (this->m[4]*m.m[3]) + (this->m[5]*m.m[6]); 
	final[4] = (this->m[3]*m.m[1]) + (this->m[4]*m.m[4]) + (this->m[5]*m.m[7]);
	final[5] = (this->m[3]*m.m[2]) + (this->m[4]*m.m[5]) + (this->m[5]*m.m[8]);

	final[6] = (this->m[6]*m.m[0]) + (this->m[7]*m.m[3]) + (this->m[8]*m.m[6]); 
	final[7] = (this->m[6]*m.m[1]) + (this->m[7]*m.m[4]) + (this->m[8]*m.m[7]);
	final[8] = (this->m[6]*m.m[2]) + (this->m[7]*m.m[5]) + (this->m[8]*m.m[8]);

	return( Matrix3x3(final) );
}

/////////////////////////////////////////////////////
/// Method: Identity
/// Params: None
///
/////////////////////////////////////////////////////
void Matrix3x3::Identity( void )
{
	std::memset( m, 0, sizeof( float )*9 );
	m[0] = m[4] = m[8] = 1.0f;
}

/////////////////////////////////////////////////////
/// Method: SetXRotation
/// Params: [in]fAngleRadians
///
/////////////////////////////////////////////////////
void Matrix3x3::SetXRotation( float fAngleRadians )
{
	float fSin, fCos;
	sinCos( &fSin, &fCos, fAngleRadians );

	m[0] = 1.0f;	m[1] = 0.0f;	m[2] = 0.0f;
	m[3] = 0.0f;	m[4] = fCos;	m[5] = -fSin;
	m[6] = 0.0f;	m[7] = fSin;	m[8] = fCos;
}

/////////////////////////////////////////////////////
/// Method: SetYRotation
/// Params: [in]fAngleRadians
///
/////////////////////////////////////////////////////
void Matrix3x3::SetYRotation( float fAngleRadians )
{
	float fSin, fCos;
	sinCos( &fSin, &fCos, fAngleRadians );

	m[0] = fCos;	m[1] = 0.0f;	m[2] = fSin;
	m[3] = 0.0f;	m[4] = 1.0f;	m[5] = 0.0f;
	m[6] = -fSin;	m[7] = 0.0f;	m[8] = fCos;
}

/////////////////////////////////////////////////////
/// Method: SetZRotation
/// Params: [in]fAngleRadians
///
/////////////////////////////////////////////////////
void Matrix3x3::SetZRotation( float fAngleRadians )
{
	float fSin, fCos;
	sinCos( &fSin, &fCos, fAngleRadians );

	m[0] = fCos;	m[1] = -fSin;	m[2] = 0.0f;
	m[3] = fSin;	m[4] = fCos;	m[5] = 0.0f;
	m[6] = 0.0f;	m[7] = 0.0f;	m[8] = 1.0f;
}

/////////////////////////////////////////////////////
/// Method: SetupAxisAngle
/// Params: [in]axis, [in]theta
///
/////////////////////////////////////////////////////
void Matrix3x3::SetupAxisAngle( const math::Vec3 &axis, float theta ) 
{
	// axis/angle 
	DBG_ASSERT( std::abs( DotProduct( axis, axis ) - 1.0f ) < 0.01f );

	// Get sin and cosine of rotation angle
	float	rcos, rsin;
	sinCos( &rsin, &rcos, theta );

	m[0] =      rcos + axis.X*axis.X*(1-rcos);
	m[1] =  axis.Z * rsin + axis.Y*axis.X*(1-rcos);
	m[2] = -axis.Y * rsin + axis.Z*axis.X*(1-rcos);
	m[3] = -axis.Z * rsin + axis.X*axis.Y*(1-rcos);
	m[4] =      rcos + axis.Y*axis.Y*(1-rcos);
	m[5] =  axis.X * rsin + axis.Z*axis.Y*(1-rcos);
	m[6] =  axis.Y * rsin + axis.X*axis.Z*(1-rcos);
	m[7] = -axis.X * rsin + axis.Y*axis.Z*(1-rcos);
	m[8] =      rcos + axis.Z*axis.Z*(1-rcos);

/*	// Compute 1 - cos(theta) and some common subexpressions
	float	a = 1.0f - c;
	float	ax = a * axis.X;
	float	ay = a * axis.Y;
	float	az = a * axis.Z;

	// Set the matrix elements.  There is still a little more
	// opportunity for optimization due to the many common
	// subexpressions.  We'll let the compiler handle that...
	m[0] = ax*axis.X + c;
	m[1] = ax*axis.Y + axis.Z*s;
	m[2] = ax*axis.Z - axis.Y*s;

	m[3] = ay*axis.X - axis.Z*s;
	m[4] = ay*axis.Y + c;
	m[5] = ay*axis.Z + axis.X*s;

	m[6] = az*axis.X + axis.Y*s;
	m[7] = az*axis.Y - axis.X*s;
	m[8] = az*axis.Z + c;
*/
}

/////////////////////////////////////////////////////
/// Method: SetRotationAngles
/// Params: [in]angles
///
/////////////////////////////////////////////////////
void Matrix3x3::SetRotationAngles( const math::Vec3 &angles ) 
{
//    M  = X.Y.Z

//  can be split into two matrix multiplications:

//    M' = X.Y
//    M  = M'.Z

//  Evaluating M' first:

//  M' = X.Y

//         | 1  0  0 |   | C  0  D |
//    M' = | 0  A -B | . | 0  1  0 |
//         | 0  B  A |   |-D  0  C |

//         | 1.C + 0.0 +  0.-D   1.0 + 0.1 +  0.0   1.D + 0.0 +  0.C |
//    M' = | 0.C + A.0 + -B.-D   0.0 + A.1 + -B.0   0.D + A.0 + -B.C |
//         | 0.C + B.0 +  A.-D   0.0 + B.1 +  A.0   0.D + B.0 +  A.C |

//  Simplifying M' gives:

//         |  C     0   D   |
//    M' = |  B.D   A  -B.C |
//         | -A.D   B   A.C |

//	 Evaluating M gives:

//    M  = M'.Z

//         |  C   0   D  |   | E -F  0 |
//    M  = |  BD  A  -BC | . | F  E  0 |
//         | -AD  B   AC |   | 0  0  1 |

//         |   C.E + 0.F +   D.0    C.-F + 0.E +  D.0     C.0 + 0.0 +   D.1 |
//    M  = |  BD.E + A.F + -BC.0   BD.-F + A.E + -BC.0   BD.0 + A.0 + -BC.1 |
//         | -AD.E + B.F +  AC.0  -AD.-F + B.E +  AC.0  -AD.0 + 0.0 +  AC.1 |

//  Simplifying M gives a 3x3 matrix:

//         |  CE      -CF       D  |
//    M  = |  BDE+AF  -BDF+AE  -BC |
//         | -ADE+BF   ADF+BE   AC |

	float A       = std::cos(angles.X);
	float B       = std::sin(angles.X);
	float C       = std::cos(angles.Y);
	float D       = std::sin(angles.Y);
	float E       = std::cos(angles.Z);
	float F       = std::sin(angles.Z);
	float AD      =   A * D;
	float BD      =   B * D;

	m[0]	=	(C * E);
	m[1]	=	(-C * F);
	m[2]	=	D;
	m[3]	=	(BD * E) + (A * F);
	m[4]	=	(-BD * F) + (A * E);
	m[5]	=	(-B * C);
	m[6]	=	(-AD * E) + (B * F);
	m[7]	=	(AD * F) + (B * E);
	m[8]	=	(A * C);
}

/////////////////////////////////////////////////////
/// Method: SetScale
/// Params: [in]fXScale, [in]fYScale, [in]fZScale
///
/////////////////////////////////////////////////////
void Matrix3x3::SetScale( float fXScale, float fYScale, float fZScale )
{
	m[0] = fXScale;	m[1] = 0.0f;	m[2] = 0.0f;
	m[3] = 0.0f;	m[4] = fYScale;	m[5] = 0.0f;
	m[6] = 0.0f;	m[7] = 0.0f;	m[8] = fZScale;
}

/////////////////////////////////////////////////////
/// Method: Determinant
/// Params: None
///
/////////////////////////////////////////////////////
float Matrix3x3::Determinant( void )
{
//        | A B C |
//    M = | D E F |
//        | G H I |

//    det M = A * (EI - HF) - B * (DI - GF) + C * (DH - GE)

	float fDeterminant = 0.0f;

	fDeterminant = m[0] * ( (m[4]*m[8]) - (m[7]*m[5]) ) - m[1] * ( (m[3]*m[8]) - (m[6]*m[5]) ) + m[2] * ( (m[3]*m[7]) - (m[6]*m[4]) );

	return( fDeterminant );
}

/////////////////////////////////////////////////////
/// Method: Inverse
/// Params: None
///
/////////////////////////////////////////////////////
Matrix3x3 Matrix3x3::Inverse( void )
{
//     -1     1     |   EI-FH  -(BI-HC)   BF-EC  |
//    M   = ----- . | -(DI-FG)   AI-GC  -(AF-DC) |
//          det M   |   DH-GE  -(AH-GB)   AE-BD  |

	float final[9];	// result

	float det = Determinant();

	if ( std::abs( det ) < 0.0005f )
	{
		// go to identity
		std::memset( final, 0, sizeof( float )*9 );
		final[0] = final[4] = final[8] = 1.0f;

		return( Matrix3x3(final) );
	}

	final[0] =    m[4]*m[8] - m[5]*m[7]   / det;
	final[1] = -( m[1]*m[8] - m[7]*m[2] ) / det;
	final[2] =    m[1]*m[5] - m[4]*m[2]   / det;
	final[3] = -( m[3]*m[8] - m[5]*m[6] ) / det;
	final[4] =    m[0]*m[8] - m[6]*m[2]   / det;
	final[5] = -( m[0]*m[5] - m[3]*m[2] ) / det;
	final[6] =    m[3]*m[7] - m[6]*m[4]   / det;
	final[7] = -( m[0]*m[7] - m[6]*m[1] ) / det;
	final[8] =    m[0]*m[4] - m[1]*m[3]   / det;

	return( Matrix3x3(final) );
}

/////////////////////////////////////////////////////
/// Method: Invert
/// Params: None
///
/////////////////////////////////////////////////////
void Matrix3x3::Invert( void )
{
//     -1     1     |   EI-FH  -(BI-HC)   BF-EC  |
//    M   = ----- . | -(DI-FG)   AI-GC  -(AF-DC) |
//          det M   |   DH-GE  -(AH-GB)   AE-BD  |

	float final[9];	// result

	float det = Determinant();

	if ( std::abs( det ) < 0.0005f )
	{
		// go to identity
		std::memset( final, 0, sizeof( float )*9 );
		final[0] = final[4] = final[8] = 1.0f;

		std::memcpy( m, final, sizeof( float )*9 );
	}

	final[0] =    m[4]*m[8] - m[5]*m[7]   / det;
	final[1] = -( m[1]*m[8] - m[7]*m[2] ) / det;
	final[2] =    m[1]*m[5] - m[4]*m[2]   / det;
	final[3] = -( m[3]*m[8] - m[5]*m[6] ) / det;
	final[4] =    m[0]*m[8] - m[6]*m[2]   / det;
	final[5] = -( m[0]*m[5] - m[3]*m[2] ) / det;
	final[6] =    m[3]*m[7] - m[6]*m[4]   / det;
	final[7] = -( m[0]*m[7] - m[6]*m[1] ) / det;
	final[8] =    m[0]*m[4] - m[1]*m[3]   / det;

	std::memcpy( m, final, sizeof( float )*9 );
}

/////////////////////////////////////////////////////
/// Method: ConvertToQuaternion
/// Params: None
///
/////////////////////////////////////////////////////
math::Quaternion Matrix3x3::ConvertToQuaternion( void )
{
//    Calculate the trace of the matrix T from the equation:

//                2     2     2
//      T = 4 - 4x  - 4y  - 4z

//                 2    2    2
//        = 4( 1 -x  - y  - z )

//        = 1 + mat[0] + mat[4] + mat[8]


//    If the trace of the matrix is greater than zero, then
//    perform an "instant" calculation.
//    Important note wrt. rouning errors:

//    Test if ( T > 0.00000001 ) to avoid large distortions!

//      S = sqrt(T) * 2;
//      X = ( mat[5] - mat[7] ) / S;
//      Y = ( mat[6] - mat[2] ) / S;
//      Z = ( mat[1] - mat[3] ) / S;
//      W = 0.25 * S;

//    If the trace of the matrix is equal to zero then identify
//    which major diagonal element has the greatest value.
//    Depending on this, calculate the following:

//    if ( mat[0] > mat[4] && mat[0] > mat[8] )  {	// Column 0: 
//        S  = sqrt( 1.0 + mat[0] - mat[4] - mat[8] ) * 2;
//        X = 0.25 * S;
//        Y = (mat[1] + mat[3] ) / S;
//        Z = (mat[6] + mat[2] ) / S;
//        W = (mat[5] - mat[7] ) / S;
//    } else if ( mat[4] > mat[8] ) {			// Column 1: 
//        S  = sqrt( 1.0 + mat[4] - mat[0] - mat[8] ) * 2;
//        X = (mat[1] + mat[3] ) / S;
//        Y = 0.25 * S;
//        Z = (mat[5] + mat[7] ) / S;
//        W = (mat[6] - mat[2] ) / S;
//    } else {						// Column 2:
//        S  = sqrt( 1.0 + mat[8] - mat[0] - mat[4] ) * 2;
//        X = (mat[6] + mat[2] ) / S;
//        Y = (mat[5] + mat[7] ) / S;
//        Z = 0.25 * S;
//        W = (mat[1] - mat[3] ) / S;
//    }

//     The quaternion is then defined as:
//       Q = | X Y Z W |

	math::Quaternion q;
	float fTrace = 1.0f + m[0] + m[4] + m[8];

	if( fTrace > 0.00000001f )
	{
		float S = std::sqrt(fTrace) * 2.0f;
		q.X = ( m[5] - m[7] ) / S;
		q.Y = ( m[6] - m[2] ) / S;
		q.Z = ( m[1] - m[3] ) / S;
		q.W = 0.25f * S;
	}
	else
	{
		if ( m[0] > m[4] && m[0] > m[8] )  
		{	
			// Column 0: 
			float S  = std::sqrt( 1.0f + m[0] - m[4] - m[8] ) * 2.0f;
			q.X = 0.25f * S;
			q.Y = (m[1] + m[3] ) / S;
			q.Z = (m[6] + m[2] ) / S;
			q.W = (m[5] - m[7] ) / S;
		} 
		else if ( m[4] > m[8] ) 
		{			
			// Column 1: 
			float S  = std::sqrt( 1.0f + m[4] - m[0] - m[8] ) * 2.0f;
			q.X = (m[1] + m[3] ) / S;
			q.Y = 0.25f * S;
			q.Z = (m[5] + m[7] ) / S;
			q.W = (m[6] - m[2] ) / S;
		} 
		else 
		{						
			// Column 2:
			float S  = std::sqrt( 1.0f + m[8] - m[0] - m[4] ) * 2.0f;
			q.X = (m[6] + m[2] ) / S;
			q.Y = (m[5] + m[7] ) / S;
			q.Z = 0.25f * S;
			q.W = (m[1] - m[3] ) / S;
		}
	}

	q.normalise();

	return( q );
}

/////////////////////////////////////////////////////
/// Method: Print
/// Params: None
///
/////////////////////////////////////////////////////
void Matrix3x3::Print( void )
{
	DBGLOG( "-------------------------------\n" );
	DBGLOG( "m[0]=%.3f\tm[1]=%.3f\tm[2]=%.3f\n", m[0], m[1], m[2] );
	DBGLOG( "m[3]=%.3f\tm[4]=%.3f\tm[5]=%.3f\n", m[3], m[4], m[5] );
	DBGLOG( "m[6]=%.3f\tm[7]=%.3f\tm[8]=%.3f\n", m[6], m[7], m[8] );
	DBGLOG( "-------------------------------\n" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////
/// Default constructor
/// 
///
/////////////////////////////////////////////////////
Matrix4x3::Matrix4x3()
{
	Identity();
}

/////////////////////////////////////////////////////
/// Constructor
/// Params: [in]m
///
/////////////////////////////////////////////////////
Matrix4x3::Matrix4x3(const float m[12])
{
	std::memcpy( this->m, m, sizeof( float )*12 );
}

/////////////////////////////////////////////////////
/// Constructor
/// Params: [in]a
///
/////////////////////////////////////////////////////
Matrix4x3::Matrix4x3(const Matrix4x3  &a)
{
	std::memcpy( m, a.m, sizeof( float )*12 );
}

/////////////////////////////////////////////////////
/// Default destructor
/// 
///
/////////////////////////////////////////////////////
Matrix4x3::~Matrix4x3()
{

}

/////////////////////////////////////////////////////
/// Method: Identity
/// 
///
/////////////////////////////////////////////////////
void Matrix4x3::Identity( void ) 
{
	m11 = 1.0f; m12 = 0.0f; m13 = 0.0f;
	m21 = 0.0f; m22 = 1.0f; m23 = 0.0f;
	m31 = 0.0f; m32 = 0.0f; m33 = 1.0f;
	tx  = 0.0f; ty  = 0.0f; tz  = 0.0f;
}

//---------------------------------------------------------------------------
// ZeroTranslation
//
// Zero the 4th row of the matrix, which contains the translation portion.
void Matrix4x3::ZeroTranslation( void ) 
{
	tx = ty = tz = 0.0f;
}

//---------------------------------------------------------------------------
// SetTranslation
//
// Sets the translation portion of the matrix in vector form
void Matrix4x3::SetTranslation( const math::Vec3 &d ) 
{
	tx = d.X; ty = d.Y; tz = d.Z;
}

//---------------------------------------------------------------------------
// SetupTranslation
//
// Sets the translation portion of the matrix in vector form
void Matrix4x3::SetupTranslation( const math::Vec3 &d ) 
{
	// Set the linear transformation portion to identity
	m11 = 1.0f; m12 = 0.0f; m13 = 0.0f;
	m21 = 0.0f; m22 = 1.0f; m23 = 0.0f;
	m31 = 0.0f; m32 = 0.0f; m33 = 1.0f;

	// Set the translation portion

	tx = d.X; ty = d.Y; tz = d.Z;
}

//---------------------------------------------------------------------------
// SetupRotate
//
// Setup the matrix to perform a rotation about a cardinal axis
//
// The axis of rotation is specified using a 1-based index:
//
//	1 => rotate about the x-axis
//	2 => rotate about the y-axis
//	3 => rotate about the z-axis
//
// theta is the amount of rotation, in radians.  The left-hand rule is
// used to define "positive" rotation.
//
// The translation portion is reset.
//
// See 8.2.2 for more info.
void Matrix4x3::SetupRotate( int axis, float theta ) 
{
	// Get sin and cosine of rotation angle
	float	s, c;
	sinCos( &s, &c, theta );

	// Check which axis they are rotating about
	switch( axis ) 
	{
		case 1: // Rotate about the x-axis
			m11 = 1.0f; m12 = 0.0f; m13 = 0.0f;
			m21 = 0.0f; m22 = c;    m23 = s;
			m31 = 0.0f; m32 = -s;   m33 = c;
		break;

		case 2: // Rotate about the y-axis
			m11 = c;    m12 = 0.0f; m13 = -s;
			m21 = 0.0f; m22 = 1.0f; m23 = 0.0f;
			m31 = s;    m32 = 0.0f; m33 = c;
		break;

		case 3: // Rotate about the z-axis
			m11 = c;    m12 = s;    m13 = 0.0f;
			m21 = -s;   m22 = c;    m23 = 0.0f;
			m31 = 0.0f; m32 = 0.0f; m33 = 1.0f;
		break;

		default:
			// bogus axis index
			DBG_ASSERT(false);
	}

	// Reset the translation portion
	tx = ty = tz = 0.0f;
}

//---------------------------------------------------------------------------
// SetupRotate
//
// Setup the matrix to perform a rotation about an arbitrary axis.
// The axis of rotation must pass through the origin.
//
// axis defines the axis of rotation, and must be a unit vector.
//
// theta is the amount of rotation, in radians.  The left-hand rule is
// used to define "positive" rotation.
//
// The translation portion is reset.
//
// See 8.2.3 for more info.
void Matrix4x3::SetupRotate( const math::Vec3 &axis, float theta ) 
{
	// Quick sanity check to make sure they passed in a unit vector
	// to specify the axis
//	DBG_ASSERT( std::abs( axis*axis - 1.0f ) < .01f );

	// Get sin and cosine of rotation angle
	float	s, c;
	sinCos( &s, &c, theta );

	// Compute 1 - cos(theta) and some common subexpressions
	float	a = 1.0f - c;
	float	ax = a * axis.X;
	float	ay = a * axis.Y;
	float	az = a * axis.Z;

	// Set the matrix elements.  There is still a little more
	// opportunity for optimization due to the many common
	// subexpressions.  We'll let the compiler handle that...
	m11 = ax*axis.X + c;
	m12 = ax*axis.Y + axis.Z*s;
	m13 = ax*axis.Z - axis.Y*s;

	m21 = ay*axis.X - axis.Z*s;
	m22 = ay*axis.Y + c;
	m23 = ay*axis.Z + axis.X*s;

	m31 = az*axis.X + axis.Y*s;
	m32 = az*axis.Y - axis.X*s;
	m33 = az*axis.Z + c;

	// Reset the translation portion
	tx = ty = tz = 0.0f;
}

//---------------------------------------------------------------------------
// FromQuaternion
//
// Setup the matrix to perform a rotation, given the angular displacement
// in quaternion form.
//
// The translation portion is reset.
//
// See 10.6.3 for more info.

void Matrix4x3::FromQuaternion( const math::Quaternion &q ) 
{
	// Compute a few values to optimize common subexpressions
	float	ww = 2.0f * q.W;
	float	xx = 2.0f * q.X;
	float	yy = 2.0f * q.Y;
	float	zz = 2.0f * q.Z;

	// Set the matrix elements.  There is still a little more
	// opportunity for optimization due to the many common
	// subexpressions.  We'll let the compiler handle that...
	m11 = 1.0f - yy*q.Y - zz*q.Z;
	m12 = xx*q.Y + ww*q.Z;
	m13 = xx*q.Z - ww*q.X;

	m21 = xx*q.Y - ww*q.Z;
	m22 = 1.0f - xx*q.X - zz*q.Z;
	m23 = yy*q.Z + ww*q.X;

	m31 = xx*q.Z + ww*q.Y;
	m32 = yy*q.Z - ww*q.X;
	m33 = 1.0f - xx*q.X - yy*q.Y;

	// Reset the translation portion
	tx = ty = tz = 0.0f;
}

//---------------------------------------------------------------------------
// Matrix4x3::setupScale
//
// Setup the matrix to perform scale on each axis.  For uniform scale by k,
// use a vector of the form Vector3(k,k,k)
//
// The translation portion is reset.
//
// See 8.3.1 for more info.
void Matrix4x3::SetupScale(const math::Vec3 &s) 
{
	// Set the matrix elements.  Pretty straightforward
	m11 = s.X;  m12 = 0.0f; m13 = 0.0f;
	m21 = 0.0f; m22 = s.Y;  m23 = 0.0f;
	m31 = 0.0f; m32 = 0.0f; m33 = s.Z;

	// Reset the translation portion
	tx = ty = tz = 0.0f;
}

//---------------------------------------------------------------------------
// SetupScaleAlongAxis
//
// Setup the matrix to perform scale along an arbitrary axis.
//
// The axis is specified using a unit vector.
//
// The translation portion is reset.
//
// See 8.3.2 for more info.
void Matrix4x3::SetupScaleAlongAxis( const math::Vec3 &axis, float k ) 
{
	// Quick sanity check to make sure they passed in a unit vector
	// to specify the axis
//	DBG_ASSERT( std::abs( axis*axis - 1.0f ) < .01f );

	// Compute k-1 and some common subexpressions
	float	a = k - 1.0f;
	float	ax = a * axis.X;
	float	ay = a * axis.Y;
	float	az = a * axis.Z;

	// Fill in the matrix elements.  We'll do the common
	// subexpression optimization ourselves here, since diagonally
	// opposite matrix elements are equal
	m11 = ax*axis.X + 1.0f;
	m22 = ay*axis.Y + 1.0f;
	m32 = az*axis.Z + 1.0f;

	m12 = m21 = ax*axis.Y;
	m13 = m31 = ax*axis.Z;
	m23 = m32 = ay*axis.Z;

	// Reset the translation portion
	tx = ty = tz = 0.0f;
}

//---------------------------------------------------------------------------
// SetupShear
//
// Setup the matrix to perform a shear
//
// The type of shear is specified by the 1-based "axis" index.  The effect
// of transforming a point by the matrix is described by the pseudocode
// below:
//
//	axis == 1  =>  y += s*x, z += t*x
//	axis == 2  =>  x += s*y, z += t*y
//	axis == 3  =>  x += s*z, y += t*z
//
// The translation portion is reset.
//
// See 8.6 for more info.
void Matrix4x3::SetupShear( int axis, float s, float t ) 
{

	// Check which type of shear they want
	switch( axis ) 
	{
		case 1: // Shear y and z using x
			m11 = 1.0f; m12 = s;    m13 = t;
			m21 = 0.0f; m22 = 1.0f; m23 = 0.0f;
			m31 = 0.0f; m32 = 0.0f; m33 = 1.0f;
		break;

		case 2: // Shear x and z using y
			m11 = 1.0f; m12 = 0.0f; m13 = 0.0f;
			m21 = s;    m22 = 1.0f; m23 = t;
			m31 = 0.0f; m32 = 0.0f; m33 = 1.0f;
		break;

		case 3: // Shear x and y using z
			m11 = 1.0f; m12 = 0.0f; m13 = 0.0f;
			m21 = 0.0f; m22 = 1.0f; m23 = 0.0f;
			m31 = s;    m32 = t;    m33 = 1.0f;
		break;

		default:
			// bogus axis index
			DBG_ASSERT(false);
	}

	// Reset the translation portion
	tx = ty = tz = 0.0f;
}

//---------------------------------------------------------------------------
// SetupProject
//
// Setup the matrix to perform a projection onto a plane passing
// through the origin.  The plane is perpendicular to the
// unit vector n.
//
// See 8.4.2 for more info.

void Matrix4x3::SetupProject( const math::Vec3 &n ) 
{
	// Quick sanity check to make sure they passed in a unit vector
	// to specify the axis
//	DBG_ASSERT( std::abs( n*n - 1.0f ) < .01f );

	// Fill in the matrix elements.  We'll do the common
	// subexpression optimization ourselves here, since diagonally
	// opposite matrix elements are equal
	m11 = 1.0f - n.X*n.X;
	m22 = 1.0f - n.Y*n.Y;
	m33 = 1.0f - n.Z*n.Z;

	m12 = m21 = -n.X*n.Y;
	m13 = m31 = -n.X*n.Z;
	m23 = m32 = -n.Y*n.Z;

	// Reset the translation portion
	tx = ty = tz = 0.0f;
}

//---------------------------------------------------------------------------
// SetupReflect
//
// Setup the matrix to perform a reflection about a plane parallel
// to a cardinal plane.
//
// axis is a 1-based index which specifies the plane to project about:
//
//	1 => reflect about the plane x=k
//	2 => reflect about the plane y=k
//	3 => reflect about the plane z=k
//
// The translation is set appropriately, since translation must occur if
// k != 0
//
// See 8.5 for more info.
void Matrix4x3::SetupReflect( int axis, float k ) 
{
	// Check which plane they want to reflect about
	switch( axis ) 
	{
		case 1: // Reflect about the plane x=k

			m11 = -1.0f; m12 =  0.0f; m13 =  0.0f;
			m21 =  0.0f; m22 =  1.0f; m23 =  0.0f;
			m31 =  0.0f; m32 =  0.0f; m33 =  1.0f;

			tx = 2.0f * k;
			ty = 0.0f;
			tz = 0.0f;

		break;

		case 2: // Reflect about the plane y=k

			m11 =  1.0f; m12 =  0.0f; m13 =  0.0f;
			m21 =  0.0f; m22 = -1.0f; m23 =  0.0f;
			m31 =  0.0f; m32 =  0.0f; m33 =  1.0f;

			tx = 0.0f;
			ty = 2.0f * k;
			tz = 0.0f;

		break;

		case 3: // Reflect about the plane z=k

			m11 =  1.0f; m12 =  0.0f; m13 =  0.0f;
			m21 =  0.0f; m22 =  1.0f; m23 =  0.0f;
			m31 =  0.0f; m32 =  0.0f; m33 = -1.0f;

			tx = 0.0f;
			ty = 0.0f;
			tz = 2.0f * k;

		break;

		default:
			// bogus axis index
			DBG_ASSERT(false);
	}

}

//---------------------------------------------------------------------------
// SetupReflect
//
// Setup the matrix to perform a reflection about an arbitrary plane
// through the origin.  The unit vector n is perpendicular to the plane.
//
// The translation portion is reset.
//
// See 8.5 for more info.
void Matrix4x3::SetupReflect( const math::Vec3 &n ) 
{
	// Quick sanity check to make sure they passed in a unit vector
	// to specify the axis
//	DBG_ASSERT( std::abs( n*n - 1.0f ) < .01f );

	// Compute common subexpressions
	float	ax = -2.0f * n.X;
	float	ay = -2.0f * n.Y;
	float	az = -2.0f * n.Z;

	// Fill in the matrix elements.  We'll do the common
	// subexpression optimization ourselves here, since diagonally
	// opposite matrix elements are equal
	m11 = 1.0f + ax*n.X;
	m22 = 1.0f + ay*n.Y;
	m32 = 1.0f + az*n.Z;

	m12 = m21 = ax*n.X;
	m13 = m31 = ax*n.Z;
	m23 = m32 = ay*n.Z;

	// Reset the translation portion
	tx = ty = tz = 0.0f;
}

//---------------------------------------------------------------------------
// Vector * Matrix4x3
//
// Transform the point.  This makes using the vector class look like it
// does with linear algebra notation on paper.
//
// We also provide a *= operator, as per C convention.
//
// See 7.1.7
math::Vec3 operator*( const math::Vec3 &p, const math::Matrix4x3 &m );
math::Vec3 operator*( const math::Vec3 &p, const math::Matrix4x3 &m )
{

	// Grind through the linear algebra.
	return math::Vec3(
		p.X*m.m11 + p.Y*m.m21 + p.Z*m.m31 + m.tx,
		p.X*m.m12 + p.Y*m.m22 + p.Z*m.m32 + m.ty,
		p.X*m.m13 + p.Y*m.m23 + p.Z*m.m33 + m.tz
	);
}

math::Vec3& operator*=( math::Vec3 &p, const math::Matrix4x3 &m );
math::Vec3& operator*=( math::Vec3 &p, const math::Matrix4x3 &m )
{
	p = p * m;
	return p;
}

//---------------------------------------------------------------------------
// Matrix4x3 * Matrix4x3
//
// Matrix concatenation.  This makes using the vector class look like it
// does with linear algebra notation on paper.
//
// We also provide a *= operator, as per C convention.
//
// See 7.1.6
Matrix4x3 operator*( const Matrix4x3 &a, const Matrix4x3 &b );
Matrix4x3 operator*( const Matrix4x3 &a, const Matrix4x3 &b )
{

	Matrix4x3 r;

	// Compute the upper 3x3 (linear transformation) portion
	r.m11 = a.m11*b.m11 + a.m12*b.m21 + a.m13*b.m31;
	r.m12 = a.m11*b.m12 + a.m12*b.m22 + a.m13*b.m32;
	r.m13 = a.m11*b.m13 + a.m12*b.m23 + a.m13*b.m33;

	r.m21 = a.m21*b.m11 + a.m22*b.m21 + a.m23*b.m31;
	r.m22 = a.m21*b.m12 + a.m22*b.m22 + a.m23*b.m32;
	r.m23 = a.m21*b.m13 + a.m22*b.m23 + a.m23*b.m33;

	r.m31 = a.m31*b.m11 + a.m32*b.m21 + a.m33*b.m31;
	r.m32 = a.m31*b.m12 + a.m32*b.m22 + a.m33*b.m32;
	r.m33 = a.m31*b.m13 + a.m32*b.m23 + a.m33*b.m33;

	// Compute the translation portion
	r.tx = a.tx*b.m11 + a.ty*b.m21 + a.tz*b.m31 + b.tx;
	r.ty = a.tx*b.m12 + a.ty*b.m22 + a.tz*b.m32 + b.ty;
	r.tz = a.tx*b.m13 + a.ty*b.m23 + a.tz*b.m33 + b.tz;

	// Return it.  Ouch - involves a copy constructor call.  If speed
	// is critical, we may need a seperate function which places the
	// result where we want it...
	return r;
}

Matrix4x3& operator*=( Matrix4x3 &a, const Matrix4x3 &b );
Matrix4x3& operator*=( Matrix4x3 &a, const Matrix4x3 &b )
{
	a = a * b;
	return a;
}

//---------------------------------------------------------------------------
// determinant
//
// Compute the determinant of the 3x3 portion of the matrix.
//
// See 9.1.1 for more info.
float math::Determinant(const Matrix4x3 &m) 
{
	return
		  m.m11 * (m.m22*m.m33 - m.m23*m.m32)
		+ m.m12 * (m.m23*m.m31 - m.m21*m.m33)
		+ m.m13 * (m.m21*m.m32 - m.m22*m.m31);
}

//---------------------------------------------------------------------------
// inverse
//
// Compute the inverse of a matrix.  We use the classical adjoint divided
// by the determinant method.
//
// See 9.2.1 for more info.
Matrix4x3 math::Inverse(const Matrix4x3 &m) 
{
	// Compute the determinant
	float	det = Determinant(m);

	// If we're singular, then the determinant is zero and there's
	// no inverse
	DBG_ASSERT( std::abs(det) > 0.000001f );

	// Compute one over the determinant, so we divide once and
	// can *multiply* per element
	float	oneOverDet = 1.0f / det;

	// Compute the 3x3 portion of the inverse, by
	// dividing the adjoint by the determinant
	Matrix4x3	r;

	r.m11 = (m.m22*m.m33 - m.m23*m.m32) * oneOverDet;
	r.m12 = (m.m13*m.m32 - m.m12*m.m33) * oneOverDet;
	r.m13 = (m.m12*m.m23 - m.m13*m.m22) * oneOverDet;

	r.m21 = (m.m23*m.m31 - m.m21*m.m33) * oneOverDet;
	r.m22 = (m.m11*m.m33 - m.m13*m.m31) * oneOverDet;
	r.m23 = (m.m13*m.m21 - m.m11*m.m23) * oneOverDet;

	r.m31 = (m.m21*m.m32 - m.m22*m.m31) * oneOverDet;
	r.m32 = (m.m12*m.m31 - m.m11*m.m32) * oneOverDet;
	r.m33 = (m.m11*m.m22 - m.m12*m.m21) * oneOverDet;

	// Compute the translation portion of the inverse
	r.tx = -(m.tx*r.m11 + m.ty*r.m21 + m.tz*r.m31);
	r.ty = -(m.tx*r.m12 + m.ty*r.m22 + m.tz*r.m32);
	r.tz = -(m.tx*r.m13 + m.ty*r.m23 + m.tz*r.m33);

	// Return it.  Ouch - involves a copy constructor call.  If speed
	// is critical, we may need a seperate function which places the
	// result where we want it...
	return r;
}

//---------------------------------------------------------------------------
// getTranslation
//
// Return the translation row of the matrix in vector form
math::Vec3 GetTranslation(const Matrix4x3 &m);
math::Vec3 GetTranslation(const Matrix4x3 &m) 
{
	return math::Vec3( m.tx, m.ty, m.tz );
}

//---------------------------------------------------------------------------
// getPositionFromParentToLocalMatrix
//
// Extract the position of an object given a parent -> local transformation
// matrix (such as a world -> object matrix)
//
// We assume that the matrix represents a rigid transformation.  (No scale,
// skew, or mirroring)
math::Vec3 GetPositionFromParentToLocalMatrix( const Matrix4x3 &m );
math::Vec3 GetPositionFromParentToLocalMatrix( const Matrix4x3 &m ) 
{
	// Multiply negative translation value by the
	// transpose of the 3x3 portion.  By using the transpose,
	// we assume that the matrix is orthogonal.  (This function
	// doesn't really make sense for non-rigid transformations...)
	return math::Vec3(
		-(m.tx*m.m11 + m.ty*m.m12 + m.tz*m.m13),
		-(m.tx*m.m21 + m.ty*m.m22 + m.tz*m.m23),
		-(m.tx*m.m31 + m.ty*m.m32 + m.tz*m.m33)
	);
}

//---------------------------------------------------------------------------
// getPositionFromLocalToParentMatrix
//
// Extract the position of an object given a local -> parent transformation
// matrix (such as an object -> world matrix)
math::Vec3 GetPositionFromLocalToParentMatrix(const Matrix4x3 &m);
math::Vec3 GetPositionFromLocalToParentMatrix(const Matrix4x3 &m) 
{
	// Position is simply the translation portion
	return math::Vec3( m.tx, m.ty, m.tz );
}


///////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////
/// Default constructor
/// 
///
/////////////////////////////////////////////////////
Matrix4x4::Matrix4x4()
{
	Identity();
}

/////////////////////////////////////////////////////
/// Constructor
/// Params: [in]a
///
/////////////////////////////////////////////////////
Matrix4x4::Matrix4x4(const Matrix4x4  &a)
{
	std::memcpy( m, a.m, sizeof( float )*16 );
}

/////////////////////////////////////////////////////
/// Constructor
/// Params: [in]m
///
/////////////////////////////////////////////////////
Matrix4x4::Matrix4x4(const float m[16])
{
	std::memcpy( this->m, m, sizeof( float )*16 );
}

/////////////////////////////////////////////////////
/// Constructor
/// Params: [in]q
///
/////////////////////////////////////////////////////
Matrix4x4::Matrix4x4( const math::Quaternion &q )
{
//         ¦        2     2                                            ¦
//         ¦ 1 - (2Y  + 2Z )   2XY + 2ZW         2XZ - 2YW           0 ¦
//         ¦                                                           ¦
//         ¦                          2     2                          ¦
//     M = ¦ 2XY - 2ZW         1 - (2X  + 2Z )   2YZ + 2XW           0 ¦
//         ¦                                                           ¦
//         ¦                                            2     2        ¦
//         ¦ 2XZ + 2YW         2YZ - 2XW         1 - (2X  + 2Y )     0 ¦
//         ¦                                                           ¦
//         ¦     0                 0                 0               1 ¦

	float xx      = q.X * q.X;
	float xy      = q.X * q.Y;
	float xz      = q.X * q.Z;
	float xw      = q.X * q.W;
	float yy      = q.Y * q.Y;
	float yz      = q.Y * q.Z;
	float yw      = q.Y * q.W;
	float zz      = q.Z * q.Z;
	float zw      = q.Z * q.W;

	m[0]  = 1.0f - 2.0f * ( yy + zz );
	m[1]  =     2.0f * ( xy + zw );
	m[2]  =     2.0f * ( xz - yw );
	m[3]  = 0.0f;

	m[4]  =     2.0f * ( xy - zw );
	m[5]  = 1.0f - 2.0f * ( xx + zz );
	m[6]  =     2.0f * ( yz + xw );
	m[7]  = 0.0f;

	m[8]  =     2.0f * ( xz + yw );
	m[9]  =     2.0f * ( yz - xw );
	m[10]  = 1.0f - 2.0f * ( xx + yy );
	m[11]  = 0.0f;

	m[12]  = 0.0f;
	m[13]  = 0.0f;
	m[14]  = 0.0f;
	m[15]  = 1.0f;
}

/////////////////////////////////////////////////////
/// Default destructor
/// 
///
/////////////////////////////////////////////////////
Matrix4x4::~Matrix4x4()
{

}

/////////////////////////////////////////////////////
/// Operator: ADD
/// Params: [in]m
///
/////////////////////////////////////////////////////
Matrix4x4 Matrix4x4::operator+( const Matrix4x4 &m ) const
{
//    R = M + L

//        | A B C D |   | Q R S T |
//        |         |   |         |
//      = | E F G H | + | U V W X |
//        |         |   |         |
//        | I J K L |   | Y Z 0 1 |
//        |         |   |         |
//        | M N O P |   | 2 3 4 5 |

//        | A+Q B+R C+S D+T |
//        |                 |
//      = | E+U F+V G+W H+X |
//        |                 |
//        | I+Y J+Z K+0 L+1 |
//        |                 |
//        | M+2 N+3 O+4 P+5 |


	float	final[16];

	final[0] = (this->m[0] + m.m[0]); 
	final[1] = (this->m[1] + m.m[1]);
	final[2] = (this->m[2] + m.m[2]);
	final[3] = (this->m[3] + m.m[3]);

	final[4] = (this->m[4] + m.m[4]); 
	final[5] = (this->m[5] + m.m[5]);
	final[6] = (this->m[6] + m.m[6]);
	final[7] = (this->m[7] + m.m[7]);

	final[8] = (this->m[8] + m.m[8]); 
	final[9] = (this->m[9] + m.m[9]);
	final[10] = (this->m[10] + m.m[10]);
	final[11] = (this->m[11] + m.m[11]);

	final[12] = (this->m[12] + m.m[12]);
	final[13] = (this->m[13] + m.m[13]);
	final[14] = (this->m[14] + m.m[14]);
	final[15] = (this->m[15] + m.m[15]);

	return( Matrix4x4(final) );
}

/////////////////////////////////////////////////////
/// Operator: SUBTRACT
/// Params: [in]m
///
/////////////////////////////////////////////////////
Matrix4x4 Matrix4x4::operator-( const Matrix4x4 &m ) const
{
//    R = M - L

//        | A B C D |   | Q R S T |
//        |         |   |         |
//      = | E F G H | - | U V W X |
//        |         |   |         |
//        | I J K L |   | Y Z 0 1 |
//        |         |   |         |
//        | M N O P |   | 2 3 4 5 |

//        | A-Q B-R C-S D-T |
//        |                 |
//      = | E-U F-V G-W H-X |
//        |                 |
//        | I-Y J-Z K-0 L-1 |
//        |                 |
//        | M-2 N-3 O-4 P-5 |

	float	final[16];

	final[0] = (this->m[0] - m.m[0]); 
	final[1] = (this->m[1] - m.m[1]);
	final[2] = (this->m[2] - m.m[2]);
	final[3] = (this->m[3] - m.m[3]);

	final[4] = (this->m[4] - m.m[4]); 
	final[5] = (this->m[5] - m.m[5]);
	final[6] = (this->m[6] - m.m[6]);
	final[7] = (this->m[7] - m.m[7]);

	final[8] = (this->m[8] - m.m[8]); 
	final[9] = (this->m[9] - m.m[9]);
	final[10] = (this->m[10] - m.m[10]);
	final[11] = (this->m[11] - m.m[11]);

	final[12] = (this->m[12] - m.m[12]);
	final[13] = (this->m[13] - m.m[13]);
	final[14] = (this->m[14] - m.m[14]);
	final[15] = (this->m[15] - m.m[15]);

	return( Matrix4x4(final) );
}

/////////////////////////////////////////////////////
/// Operator: MULTIPLY
/// Params: [in]m
///
/////////////////////////////////////////////////////
Matrix4x4 Matrix4x4::operator*( const Matrix4x4 &m ) const
{
// R = M x L

//        | A B C D |   | Q R S T |
//      = | E F G H | x | U V W X |
//        | I J K L |   | Y Z 0 1 |
//        | M N O P |   | 2 3 4 5 |

//        | AQ+BU+CY+D2  AR+BB+CZ+D3 AS+BW+C0+D4 AT+BX+C1+D5 |
//      = | EQ+FU+GY+H2  ER+FB+GZ+H3 ES+FW+G0+H4 ET+FX+G1+H5 |
//        | IQ+JU+KY+L2  IR+JB+KZ+L3 IS+JW+K0+L4 IT+JX+K1+L5 |
//        | MQ+NU+OY+P2  MR+NB+OZ+P3 MS+NW+O0+P4 MT+NX+O1+P5 |

	float	final[16];

	final[0] = (this->m[0]*m.m[0]) + (this->m[1]*m.m[4]) + (this->m[2]*m.m[8]) + (this->m[3]*m.m[12]); 
	final[1] = (this->m[0]*m.m[1]) + (this->m[1]*m.m[5]) + (this->m[2]*m.m[9]) + (this->m[3]*m.m[13]);
	final[2] = (this->m[0]*m.m[2]) + (this->m[1]*m.m[6]) + (this->m[2]*m.m[10]) + (this->m[3]*m.m[14]);
	final[3] = (this->m[0]*m.m[3]) + (this->m[1]*m.m[7]) + (this->m[2]*m.m[11]) + (this->m[3]*m.m[15]);

	final[4] = (this->m[4]*m.m[0]) + (this->m[5]*m.m[4]) + (this->m[6]*m.m[8]) + (this->m[7]*m.m[12]); 
	final[5] = (this->m[4]*m.m[1]) + (this->m[5]*m.m[5]) + (this->m[6]*m.m[9]) + (this->m[7]*m.m[13]);
	final[6] = (this->m[4]*m.m[2]) + (this->m[5]*m.m[6]) + (this->m[6]*m.m[10]) + (this->m[7]*m.m[14]);
	final[7] = (this->m[4]*m.m[3]) + (this->m[5]*m.m[7]) + (this->m[6]*m.m[11]) + (this->m[7]*m.m[15]);

	final[8] = (this->m[8]*m.m[0]) + (this->m[9]*m.m[4]) + (this->m[10]*m.m[8]) + (this->m[11]*m.m[12]); 
	final[9] = (this->m[8]*m.m[1]) + (this->m[9]*m.m[5]) + (this->m[10]*m.m[9]) + (this->m[11]*m.m[13]);
	final[10] = (this->m[8]*m.m[2]) + (this->m[9]*m.m[6]) + (this->m[10]*m.m[10]) + (this->m[11]*m.m[14]);
	final[11] = (this->m[8]*m.m[3]) + (this->m[9]*m.m[7]) + (this->m[10]*m.m[11]) + (this->m[11]*m.m[15]);

	final[12] = (this->m[12]*m.m[0]) + (this->m[13]*m.m[4]) + (this->m[14]*m.m[8]) + (this->m[15]*m.m[12]); 
	final[13] = (this->m[12]*m.m[1]) + (this->m[13]*m.m[5]) + (this->m[14]*m.m[9]) + (this->m[15]*m.m[13]);
	final[14] = (this->m[12]*m.m[2]) + (this->m[13]*m.m[6]) + (this->m[14]*m.m[10]) + (this->m[15]*m.m[14]);
	final[15] = (this->m[12]*m.m[3]) + (this->m[13]*m.m[7]) + (this->m[14]*m.m[11]) + (this->m[15]*m.m[15]);

	return( Matrix4x4(final) );
}

/////////////////////////////////////////////////////
/// Operator: MULTIPLY
/// Params: [in]f
///
/////////////////////////////////////////////////////
Matrix4x4 Matrix4x4::operator*( const float &f ) const
{
// R = M x f

//        | A B C D |   | f |
//      = | E F G H | x 
//        | I J K L |   
//        | M N O P |   

	float	final[16];
	int i = 0;

	for( i = 0; i < 16; i++ )
		final[i] = (this->m[i]*f); 

	return( Matrix4x4(final) );
}

/////////////////////////////////////////////////////
/// Operator: MULTIPLY
/// Params: [in]v
///
/////////////////////////////////////////////////////
math::Vec3 Matrix4x4::operator*( const math::Vec3 &/*v*/ ) const
{
// R = M x V

//        | A B C D |   | V |
//      = | E F G H | x 
//        | I J K L |   
//        | M N O P |   

	Vec3 temp( 0.0f, 0.0f, 0.0f );

	return( temp );
}

/*
/////////////////////////////////////////////////////
/// Operator: MULTIPLY
/// Params: [in]a, [in]b
///
/////////////////////////////////////////////////////
Matrix4x4	operator *( const Matrix4x4 &a, const Matrix4x4 &b )
{
// R = M x L

//        | A B C D |   | Q R S T |
//      = | E F G H | x | U V W X |
//        | I J K L |   | Y Z 0 1 |
//        | M N O P |   | 2 3 4 5 |

//        | AQ+BU+CY+D2  AR+BB+CZ+D3 AS+BW+C0+D4 AT+BX+C1+D5 |
//      = | EQ+FU+GY+H2  ER+FB+GZ+H3 ES+FW+G0+H4 ET+FX+G1+H5 |
//        | IQ+JU+KY+L2  IR+JB+KZ+L3 IS+JW+K0+L4 IT+JX+K1+L5 |
//        | MQ+NU+OY+P2  MR+NB+OZ+P3 MS+NW+O0+P4 MT+NX+O1+P5 |

	Matrix4x4 r;

	// Compute the upper 3x3 (linear transformation) portion

	r.m11 = a.m11*b.m11 + a.m12*b.m21 + a.m13*b.m31;
	r.m12 = a.m11*b.m12 + a.m12*b.m22 + a.m13*b.m32;
	r.m13 = a.m11*b.m13 + a.m12*b.m23 + a.m13*b.m33;
	r.m14 = 0.0f;

	r.m21 = a.m21*b.m11 + a.m22*b.m21 + a.m23*b.m31;
	r.m22 = a.m21*b.m12 + a.m22*b.m22 + a.m23*b.m32;
	r.m23 = a.m21*b.m13 + a.m22*b.m23 + a.m23*b.m33;
	r.m24 = 0.0f;

	r.m31 = a.m31*b.m11 + a.m32*b.m21 + a.m33*b.m31;
	r.m32 = a.m31*b.m12 + a.m32*b.m22 + a.m33*b.m32;
	r.m33 = a.m31*b.m13 + a.m32*b.m23 + a.m33*b.m33;
	r.m34 = 0.0f;

	// Compute the translation portion

	r.m41 = a.m41*b.m11 + a.m42*b.m21 + a.m43*b.m31 + b.m41;
	r.m42 = a.m41*b.m12 + a.m42*b.m22 + a.m43*b.m32 + b.m42;
	r.m43 = a.m41*b.m13 + a.m42*b.m23 + a.m43*b.m33 + b.m43;
	r.m44 = 1.0f;

	// Return it.  Ouch - involves a copy constructor call.  If speed
	// is critical, we may need a seperate function which places the
	// result where we want it...

	return r;
}
*/

/////////////////////////////////////////////////////
/// Method: Identity
/// Params: None
///
/////////////////////////////////////////////////////
void Matrix4x4::Identity( void )
{
	std::memset( m, 0, sizeof( float )*16 );
	m[0] = m[5] = m[10] = m[15] = 1.0f;
}

/////////////////////////////////////////////////////
/// Method: SetXRotation
/// Params: [in]fAngleRadians
///
/////////////////////////////////////////////////////
void Matrix4x4::SetXRotation( float fAngleRadians )
{
	float fSin, fCos;
	sinCos( &fSin, &fCos, fAngleRadians );

	m[0] = 1.0f;	m[1] = 0.0f;	m[2] = 0.0f;	m[3] = 0.0f;
	m[4] = 0.0f;	m[5] = fCos;	m[6] = -fSin;	m[7] = 0.0f;
	m[8] = 0.0f;	m[9] = fSin;	m[10] = fCos;	m[11] = 0.0f;
	m[12] = 0.0f;	m[13] = 0.0f;	m[14] = 0.0f;	m[15] = 1.0f;
}

/////////////////////////////////////////////////////
/// Method: SetYRotation
/// Params: [in]fAngleRadians
///
/////////////////////////////////////////////////////
void Matrix4x4::SetYRotation( float fAngleRadians )
{
	float fSin, fCos;
	sinCos( &fSin, &fCos, fAngleRadians );

	m[0] = fCos;	m[1] = 0.0f;	m[2] = fSin;	m[3] = 0.0f;
	m[4] = 0.0f;	m[5] = 1.0f;	m[6] = 0.0f;	m[7] = 0.0f;
	m[8] = -fSin;	m[9] = 0.0f;	m[10] = fCos;	m[11] = 0.0f;
	m[12] = 0.0f;	m[13] = 0.0f;	m[14] = 0.0f;	m[15] = 1.0f;
}

/////////////////////////////////////////////////////
/// Method: SetZRotation
/// Params: [in]fAngleRadians
///
/////////////////////////////////////////////////////
void Matrix4x4::SetZRotation( float fAngleRadians )
{
	float fSin, fCos;
	sinCos( &fSin, &fCos, fAngleRadians );

	m[0] = fCos;	m[1] = -fSin;	m[2] = 0.0f;	m[3] = 0.0f;
	m[4] = fSin;	m[5] = fCos;	m[6] = 0.0f;	m[7] = 0.0f;
	m[8] = 0.0f;	m[9] = 0.0f;	m[10] = 1.0f;	m[11] = 0.0f;
	m[12] = 0.0f;	m[13] = 0.0f;	m[14] = 0.0f;	m[15] = 1.0f;
}

/////////////////////////////////////////////////////
/// Method: SetupAxisAngle
/// Params: [in]axis, [in]theta
///
/////////////////////////////////////////////////////
void Matrix4x4::SetupAxisAngle( const math::Vec3 &axis, float theta ) 
{
	// axis/angle 
	DBG_ASSERT( std::abs( DotProduct( axis, axis ) - 1.0f ) < 0.01f );

	// Get sin and cosine of rotation angle
	float	rcos, rsin;
	sinCos( &rsin, &rcos, theta );

	m[0] =      rcos + axis.X*axis.X*(1-rcos);
	m[1] =  axis.Z * rsin + axis.Y*axis.X*(1-rcos);
	m[2] = -axis.Y * rsin + axis.Z*axis.X*(1-rcos);
	m[3] = 0.0f;

	m[4] = -axis.Z * rsin + axis.X*axis.Y*(1-rcos);
	m[5] =      rcos + axis.Y*axis.Y*(1-rcos);
	m[6] =  axis.X * rsin + axis.Z*axis.Y*(1-rcos);
	m[7] = 0.0f;

	m[8] =  axis.Y * rsin + axis.X*axis.Z*(1-rcos);
	m[9] = -axis.X * rsin + axis.Y*axis.Z*(1-rcos);
	m[10] =      rcos + axis.Z*axis.Z*(1-rcos);
	m[11] = 0.0f;

	m[12] = 0.0f;	
	m[13] = 0.0f;	
	m[14] = 0.0f;	
	m[15] = 1.0f;

/*	// Compute 1 - cos(theta) and some common subexpressions
	float	a = 1.0f - c;
	float	ax = a * axis.X;
	float	ay = a * axis.Y;
	float	az = a * axis.Z;

	// Set the matrix elements.  There is still a little more
	// opportunity for optimization due to the many common
	// subexpressions.  We'll let the compiler handle that...
	m[0] = ax*axis.X + c;
	m[1] = ax*axis.Y + axis.Z*s;
	m[2] = ax*axis.Z - axis.Y*s;

	m[3] = ay*axis.X - axis.Z*s;
	m[4] = ay*axis.Y + c;
	m[5] = ay*axis.Z + axis.X*s;

	m[6] = az*axis.X + axis.Y*s;
	m[7] = az*axis.Y - axis.X*s;
	m[8] = az*axis.Z + c;
*/
}

/////////////////////////////////////////////////////
/// Method: SetRotationAngles
/// Params: [in]angles
///
/////////////////////////////////////////////////////
void Matrix4x4::SetRotationAngles( const math::Vec3 &angles ) 
{
//    M  = X.Y.Z

//  can be split into two matrix multiplications:

//    M' = X.Y
//    M  = M'.Z

//  Evaluating M' first:

//  M' = X.Y

//         | 1  0  0 |   | C  0  D |
//    M' = | 0  A -B | . | 0  1  0 |
//         | 0  B  A |   |-D  0  C |

//         | 1.C + 0.0 +  0.-D   1.0 + 0.1 +  0.0   1.D + 0.0 +  0.C |
//    M' = | 0.C + A.0 + -B.-D   0.0 + A.1 + -B.0   0.D + A.0 + -B.C |
//         | 0.C + B.0 +  A.-D   0.0 + B.1 +  A.0   0.D + B.0 +  A.C |

//  Simplifying M' gives:

//         |  C     0   D   |
//    M' = |  B.D   A  -B.C |
//         | -A.D   B   A.C |

//	 Evaluating M gives:

//    M  = M'.Z

//         |  C   0   D  |   | E -F  0 |
//    M  = |  BD  A  -BC | . | F  E  0 |
//         | -AD  B   AC |   | 0  0  1 |

//         |   C.E + 0.F +   D.0    C.-F + 0.E +  D.0     C.0 + 0.0 +   D.1 |
//    M  = |  BD.E + A.F + -BC.0   BD.-F + A.E + -BC.0   BD.0 + A.0 + -BC.1 |
//         | -AD.E + B.F +  AC.0  -AD.-F + B.E +  AC.0  -AD.0 + 0.0 +  AC.1 |

//  Simplifying M gives a 4x4 matrix:

//         |  CE      -CF       D   0 |
//    M  = |  BDE+AF  -BDF+AE  -BC  0 |
//         | -ADE+BF   ADF+BE   AC  0 |
//         |    0        0      0   1 |

	float A       = std::cos(angles.X);
	float B       = std::sin(angles.X);
	float C       = std::cos(angles.Y);
	float D       = std::sin(angles.Y);
	float E       = std::cos(angles.Z);
	float F       = std::sin(angles.Z);
	float AD      =   A * D;
	float BD      =   B * D;

	m[0]	=	(C * E);
	m[1]	=	(-C * F);
	m[2]	=	D;
	m[3]	=	0.0f;

	m[4]	=	(BD * E) + (A * F);
	m[5]	=	(-BD * F) + (A * E);
	m[6]	=	(-B * C);
	m[7]	=	0.0f;

	m[8]	=	(-AD * E) + (B * F);
	m[9]	=	(AD * F) + (B * E);
	m[10]	=	(A * C);
	m[11]	=	0.0f;

	m[12]	=	0.0f;
	m[13]	=	0.0f;
	m[14]	=	0.0f;
	m[15]	=	1.0f;
}

/////////////////////////////////////////////////////
/// Method: SetScale
/// Params: [in]fXScale, [in]fYScale, [in]fZScale
///
/////////////////////////////////////////////////////
void Matrix4x4::SetScale( float fXScale, float fYScale, float fZScale )
{
	m[0] = fXScale;	m[1] = 0.0f;	m[2] = 0.0f;	m[3] = 0.0f;
	m[4] = 0.0f;	m[5] = fYScale;	m[6] = 0.0f;	m[7] = 0.0f;
	m[8] = 0.0f;	m[9] = 0.0f;	m[10] = fZScale;m[11] = 0.0f;
	m[12] = 0.0f;	m[13] = 0.0f;	m[14] = 0.0f;	m[15] = 1.0f;
}

/////////////////////////////////////////////////////
/// Method: Determinant
/// Params: None
///
/////////////////////////////////////////////////////
float Matrix4x4::Determinant( void )
{

//        | A B C D |
//    M = | E F G H |
//        | I J K L |
//        | M N O P |

//    det M = A * (FK - JG) - B * (EK - IG) + C * (EJ - IF)

	float fDeterminant = 0.0f;

	fDeterminant = m[0] * ( (m[5]*m[10]) - (m[9]*m[6]) ) - m[1] * ( (m[4]*m[10]) - (m[8]*m[6]) ) + m[2] * ( (m[4]*m[9]) - (m[8]*m[5]) );

	return( fDeterminant );
}

/////////////////////////////////////////////////////
/// Method: Multiply
/// Params: [in]inMatrix
///
/////////////////////////////////////////////////////
void Matrix4x4::Multiply( Matrix4x4 &inMatrix )
{
	float newMatrix[16];
	const float *m1 = m, *m2 = inMatrix.m;

	newMatrix[0] = (m1[0]*m2[0]) + (m1[4]*m2[1]) + (m1[8]*m2[2]);
	newMatrix[1] = (m1[1]*m2[0]) + (m1[5]*m2[1]) + (m1[9]*m2[2]);
	newMatrix[2] = (m1[2]*m2[0]) + (m1[6]*m2[1]) + (m1[10]*m2[2]);
	newMatrix[3] = 0.0f;

	newMatrix[4] = (m1[0]*m2[4]) + (m1[4]*m2[5]) + (m1[8]*m2[6]);
	newMatrix[5] = (m1[1]*m2[4]) + (m1[5]*m2[5]) + (m1[9]*m2[6]);
	newMatrix[6] = (m1[2]*m2[4]) + (m1[6]*m2[5]) + (m1[10]*m2[6]);
	newMatrix[7] = 0.0f;

	newMatrix[8] = (m1[0]*m2[8]) + (m1[4]*m2[9]) + (m1[8]*m2[10]);
	newMatrix[9] = (m1[1]*m2[8]) + (m1[5]*m2[9]) + (m1[9]*m2[10]);
	newMatrix[10] = (m1[2]*m2[8]) + (m1[6]*m2[9]) + (m1[10]*m2[10]);
	newMatrix[11] = 0.0f;

	newMatrix[12] = (m1[0]*m2[12]) + (m1[4]*m2[13]) + (m1[8]*m2[14]) + m1[12];
	newMatrix[13] = (m1[1]*m2[12]) + (m1[5]*m2[13]) + (m1[9]*m2[14]) + m1[13];
	newMatrix[14] = (m1[2]*m2[12]) + (m1[6]*m2[13]) + (m1[10]*m2[14]) + m1[14];
	newMatrix[15] = 1.0f;

	// assign the new matrix to 'this' matrix
	m[0] = newMatrix[0];
	m[1] = newMatrix[1];
	m[2] = newMatrix[2];
	m[3] = newMatrix[3];

	m[4] = newMatrix[4];
	m[5] = newMatrix[5];
	m[6] = newMatrix[6];
	m[7] = newMatrix[7];

	m[8] = newMatrix[8];
	m[9] = newMatrix[9];
	m[10] = newMatrix[10];
	m[11] = newMatrix[11];

	m[12] = newMatrix[12];
	m[13] = newMatrix[13];
	m[14] = newMatrix[14];
	m[15] = newMatrix[15];

}

/////////////////////////////////////////////////////
/// Method: SetTranslation
/// Params: [in]v
///
/////////////////////////////////////////////////////
void Matrix4x4::SetTranslation( const math::Vec3 &v )
{
	m[12] = v.X;
	m[13] = v.Y;
	m[14] = v.Z;
}

/////////////////////////////////////////////////////
/// Method: SetTranslation
/// Params: [in]fTranslation
///
/////////////////////////////////////////////////////
void Matrix4x4::SetTranslation( float *fTranslation )
{
	m[12] = fTranslation[_X_];
	m[13] = fTranslation[_Y_];
	m[14] = fTranslation[_Z_];
}

/////////////////////////////////////////////////////
/// Method: InverseTranslation
/// Params: [in/out]v
///
/////////////////////////////////////////////////////
void Matrix4x4::InverseTranslation( math::Vec3 &v )
{
	v.X = v.X - m[12];
	v.Y = v.Y - m[13];
	v.Z = v.Z - m[14];
}

/////////////////////////////////////////////////////
/// Method: InverseTranslation
/// Params: [in/out]fTranslation
///
/////////////////////////////////////////////////////
void Matrix4x4::InverseTranslation( float *fTranslation )
{
	fTranslation[_X_] = fTranslation[_X_] - m[12];
	fTranslation[_Y_] = fTranslation[_Y_] - m[13];
	fTranslation[_Z_] = fTranslation[_Z_] - m[14];
}

/////////////////////////////////////////////////////
/// Method: InverseRotate
/// Params: [in]v
///
/////////////////////////////////////////////////////
void Matrix4x4::InverseRotate( math::Vec3 &v )
{
	float vec[3];

	vec[_X_] = v.X*m[0] + v.Y*m[1] + v.Z*m[2];
	vec[_Y_] = v.X*m[4] + v.Y*m[5] + v.Z*m[6];
	vec[_Z_] = v.X*m[8] + v.Y*m[9] + v.Z*m[10];

	v.X = vec[_X_];
	v.Y = vec[_Y_];
	v.Z = vec[_Z_];
}

/////////////////////////////////////////////////////
/// Method: InverseRotate
/// Params: [in]fRotation
///
/////////////////////////////////////////////////////
void Matrix4x4::InverseRotate( float *fRotation )
{
	float vec[3];

	vec[_X_] = fRotation[_X_]*m[0] + fRotation[_Y_]*m[1] + fRotation[_Z_]*m[2];
	vec[_Y_] = fRotation[_X_]*m[4] + fRotation[_Y_]*m[5] + fRotation[_Z_]*m[6];
	vec[_Z_] = fRotation[_X_]*m[8] + fRotation[_Y_]*m[9] + fRotation[_Z_]*m[10];

	fRotation[_X_] = vec[_X_];
	fRotation[_Y_] = vec[_Y_];
	fRotation[_Z_] = vec[_Z_];
}

/////////////////////////////////////////////////////
/// Method: SetMatrix
/// Params: [in]inMatrix
///
/////////////////////////////////////////////////////
void Matrix4x4::SetMatrix( const float *inMatrix )
{
	std::memcpy( m, inMatrix, sizeof( float )*16 );
}

/////////////////////////////////////////////////////
/// Method: SetMatrix
/// Params: [in]m
///
/////////////////////////////////////////////////////
void Matrix4x4::SetMatrix( const Matrix4x4 *inMatrix )
{
	std::memcpy( m, inMatrix, sizeof( Matrix4x4 ) );
}

/////////////////////////////////////////////////////
/// Method: ConvertToQuaternion
/// Params: None
///
/////////////////////////////////////////////////////
math::Quaternion Matrix4x4::ConvertToQuaternion( void )
{
//   Calculate the trace of the matrix T from the equation:

//                2     2     2
//      T = 4 - 4x  - 4y  - 4z

//                 2    2    2
//        = 4( 1 -x  - y  - z )

//        = 1 + mat[0] + mat[5] + mat[10]


//    If the trace of the matrix is greater than zero, then
//    perform an "instant" calculation.
//    Important note wrt. rouning errors:

//    Test if ( T > 0.00000001 ) to avoid large distortions!

//      S = sqrt(T) * 2;
//      X = ( mat[6] - mat[9] ) / S;
//      Y = ( mat[8] - mat[2] ) / S;
//      Z = ( mat[1] - mat[4] ) / S;
//      W = 0.25 * S;

//    If the trace of the matrix is equal to zero then identify
//    which major diagonal element has the greatest value.
//    Depending on this, calculate the following:

//    if ( mat[0] > mat[5] && mat[0] > mat[10] )  {	// Column 0: 
//        S  = sqrt( 1.0 + mat[0] - mat[5] - mat[10] ) * 2;
//        X = 0.25 * S;
//        Y = (mat[1] + mat[4] ) / S;
//        Z = (mat[8] + mat[2] ) / S;
//        W = (mat[6] - mat[9] ) / S;
//    } else if ( mat[5] > mat[10] ) {			// Column 1: 
//        S  = sqrt( 1.0 + mat[5] - mat[0] - mat[10] ) * 2;
//        X = (mat[1] + mat[4] ) / S;
//        Y = 0.25 * S;
//        Z = (mat[6] + mat[9] ) / S;
//        W = (mat[8] - mat[2] ) / S;
//    } else {						// Column 2:
//        S  = sqrt( 1.0 + mat[10] - mat[0] - mat[5] ) * 2;
//        X = (mat[8] + mat[2] ) / S;
//        Y = (mat[6] + mat[9] ) / S;
//        Z = 0.25 * S;
//        W = (mat[1] - mat[4] ) / S;
//    }

//     The quaternion is then defined as:
//       Q = | X Y Z W |


	math::Quaternion q;
	float fTrace = 1.0f + m[0] + m[5] + m[10];

	if( fTrace > 0.00000001f )
	{
		float S = std::sqrt(fTrace) * 2.0f;
		q.X = ( m[6] - m[9] ) / S;
		q.Y = ( m[8] - m[2] ) / S;
		q.Z = ( m[1] - m[4] ) / S;
		q.W = 0.25f * S;
	}
	else
	{
		if ( m[0] > m[5] && m[0] > m[10] )  
		{	
			// Column 0: 
			float S  = std::sqrt( 1.0f + m[0] - m[5] - m[10] ) * 2.0f;
			q.X = 0.25f * S;
			q.Y = (m[1] + m[4] ) / S;
			q.Z = (m[8] + m[2] ) / S;
			q.W = (m[6] - m[9] ) / S;
		} 
		else if ( m[5] > m[10] ) 
		{			
			// Column 1: 
			float S  = std::sqrt( 1.0f + m[5] - m[0] - m[10] ) * 2.0f;
			q.X = (m[1] + m[4] ) / S;
			q.Y = 0.25f * S;
			q.Z = (m[6] + m[9] ) / S;
			q.W = (m[8] - m[2] ) / S;
		} 
		else 
		{						
			// Column 2:
			float S  = std::sqrt( 1.0f + m[10] - m[0] - m[5] ) * 2.0f;
			q.X = (m[8] + m[2] ) / S;
			q.Y = (m[6] + m[9] ) / S;
			q.Z = 0.25f * S;
			q.W = (m[1] - m[4] ) / S;
		}
	}

	q.normalise();

	return( q );
}

/////////////////////////////////////////////////////
/// Method: Print
/// Params: None
///
/////////////////////////////////////////////////////
void Matrix4x4::Print( void )
{
	DBGLOG( "-------------------------------\n" );
	DBGLOG( "m[0 ]=%.3f\tm[1 ]=%.3f\tm[2 ]=%.3f\tm[3 ]=%.3f\n", m[0], m[1], m[2], m[3] );
	DBGLOG( "m[4 ]=%.3f\tm[5 ]=%.3f\tm[6 ]=%.3f\tm[7 ]=%.3f\n", m[4], m[5], m[6], m[7] );
	DBGLOG( "m[8 ]=%.3f\tm[9 ]=%.3f\tm[10]=%.3f\tm[11]=%.3f\n", m[8], m[9], m[10], m[11] );
	DBGLOG( "m[12]=%.3f\tm[13]=%.3f\tm[14]=%.3f\tm[15]=%.3f\n", m[12], m[13], m[14], m[15] );
	DBGLOG( "-------------------------------\n" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////



