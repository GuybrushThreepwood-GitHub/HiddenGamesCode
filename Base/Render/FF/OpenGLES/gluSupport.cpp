
/*===================================================================
 File: gluSupport.cpp
 Library: Render
 
 (C)Hidden Games
 =====================================================================*/

#include "CoreBase.h"

#ifdef BASE_SUPPORT_OPENGLES

#include <cstring>
#include <cmath>

#include "Math/MathConsts.h"
#include "Math/Vectors.h"

#include "Render/RenderConsts.h"
#include "Render/OpenGLCommon.h"
#include "Render/FF/OpenGLES/gluSupport.h"

namespace gluUtil
{
	struct token_string
	{
	   GLuint Token;
	   const char *String;
	};

	static const struct token_string Errors[] = 
	{
	   { GL_NO_ERROR, "no error" },
	   { GL_INVALID_ENUM, "invalid enumerant" },
	   { GL_INVALID_VALUE, "invalid value" },
	   { GL_INVALID_OPERATION, "invalid operation" },
	   { GL_STACK_OVERFLOW, "stack overflow" },
	   { GL_STACK_UNDERFLOW, "stack underflow" },
	   { GL_OUT_OF_MEMORY, "out of memory" }
	};

	static void normalise(float v[3])
	{
		float r;

		r = std::sqrt( v[0]*v[0] + v[1]*v[1] + v[2]*v[2] );
		if (r == 0.0f) 
			return;

		v[0] /= r;
		v[1] /= r;
		v[2] /= r;
	}

	static void cross(float v1[3], float v2[3], float result[3])
	{
		result[0] = v1[1]*v2[2] - v1[2]*v2[1];
		result[1] = v1[2]*v2[0] - v1[0]*v2[2];
		result[2] = v1[0]*v2[1] - v1[1]*v2[0];
	}

	static void gluMakeIdentityf(GLfloat m[16])
	{
		m[0+4*0] = 1.0f; m[0+4*1] = 0.0f; m[0+4*2] = 0.0f; m[0+4*3] = 0.0f;
		m[1+4*0] = 0.0f; m[1+4*1] = 1.0f; m[1+4*2] = 0.0f; m[1+4*3] = 0.0f;
		m[2+4*0] = 0.0f; m[2+4*1] = 0.0f; m[2+4*2] = 1.0f; m[2+4*3] = 0.0f;
		m[3+4*0] = 0.0f; m[3+4*1] = 0.0f; m[3+4*2] = 0.0f; m[3+4*3] = 1.0f;
	}

} // namespace gluUtil

/////////////////////////////////////////////////////
/// Method: gluBuild2DMipmapLevels
/// Params: [in]target, [in]internalFormat, [in]width, [in]height, [in]format, [in]type, [in]level, [in]base, [in]max, [in]data
///
/////////////////////////////////////////////////////
GLint gluBuild2DMipmapLevels (GLenum target, GLint internalFormat, GLsizei width, GLsizei height, GLenum format, GLenum type, GLint level, GLint base, GLint max, const void *data)
{
	return(1);
}

/////////////////////////////////////////////////////
/// Method: gluBuild2DMipmaps
/// Params: [in]target, [in]internalFormat, [in]width, [in]height, [in]format, [in]type, [in]data
///
/////////////////////////////////////////////////////
GLint gluBuild2DMipmaps (GLenum target, GLint internalFormat, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *data)
{
	return(1);
}

/////////////////////////////////////////////////////
/// Method: gluErrorString
/// Params: [in]error
///
/////////////////////////////////////////////////////
const GLubyte * gluErrorString (GLenum errorCode)
{
    int i;
	for( i = 0; gluUtil::Errors[i].String; i++ ) 
	{
        if (gluUtil::Errors[i].Token == errorCode)
            return (const GLubyte *)gluUtil::Errors[i].String;
    }

	return 0;
}

/////////////////////////////////////////////////////
/// Method: gluLookAt
/// Params: [in]eyeX, [in]eyeY, [in]eyeZ, [in]centerX, [in]centerY, [in]centerZ, [in]upX, [in]upY, [in]upZ
///
/////////////////////////////////////////////////////
void gluLookAt (GLfloat eyeX, GLfloat eyeY, GLfloat eyeZ, GLfloat centerX, GLfloat centerY, GLfloat centerZ, GLfloat upX, GLfloat upY, GLfloat upZ)
{
    float forward[3], side[3], up[3];
    GLfloat m[4][4];

    forward[0] = centerX - eyeX;
    forward[1] = centerY - eyeY;
    forward[2] = centerZ - eyeZ;

    up[0] = upX;
    up[1] = upY;
    up[2] = upZ;

    gluUtil::normalise(forward);

    // Side = forward x up
    gluUtil::cross(forward, up, side);
    gluUtil::normalise(side);

    // Recompute up as: up = side x forward
    gluUtil::cross(side, forward, up);

    gluUtil::gluMakeIdentityf(&m[0][0]);
    m[0][0] = side[0];
    m[1][0] = side[1];
    m[2][0] = side[2];

    m[0][1] = up[0];
    m[1][1] = up[1];
    m[2][1] = up[2];

    m[0][2] = -forward[0];
    m[1][2] = -forward[1];
    m[2][2] = -forward[2];

    glMultMatrixf(&m[0][0]);
    glTranslatef(-eyeX, -eyeY, -eyeZ);	
}

/////////////////////////////////////////////////////
/// Method: gluOrtho2D
/// Params: [in]left, [in]right, [in]bottom, [in]top
///
/////////////////////////////////////////////////////
void gluOrtho2D (GLfloat left, GLfloat right, GLfloat bottom, GLfloat top)
{
	glOrthof(left, right, bottom, top, -1.0f, 1.0f);
}

/////////////////////////////////////////////////////
/// Method: gluPerspective
/// Params: [in]fovy, [in]aspect, [in]zNear, [in]zFar
///
/////////////////////////////////////////////////////
void gluPerspective (GLfloat fovy, GLfloat aspect, GLfloat zNear, GLfloat zFar)
{
    GLfloat m[4][4];
    float sine, cotangent, deltaZ;
	float radians = fovy / 2 * math::PI / 180.0f;

    deltaZ = zFar - zNear;
	sine = std::sin(radians);
    if ((deltaZ == 0) || (sine == 0) || (aspect == 0)) 
	{
		return;
    }
	cotangent = std::cos(radians) / sine;

    gluUtil::gluMakeIdentityf(&m[0][0]);
    m[0][0] = cotangent / aspect;
    m[1][1] = cotangent;
    m[2][2] = -(zFar + zNear) / deltaZ;
    m[2][3] = -1.0f;
    m[3][2] = -2.0f * zNear * zFar / deltaZ;
    m[3][3] = 0;
    glMultMatrixf(&m[0][0]);	
}

/////////////////////////////////////////////////////
/// Method: gluScaleImage
/// Params: [in]format, [in]wIn, [in]hIn, [in]typeIn, [in]dataIn, [in]wOut, [in]hOut, [in]typeOut, [in/out]dataOut
///
/////////////////////////////////////////////////////
GLint gluScaleImage (GLenum format, GLsizei wIn, GLsizei hIn, GLenum typeIn, const void *dataIn, GLsizei wOut, GLsizei hOut, GLenum typeOut, GLvoid* dataOut)
{
	return(1);
}

#endif // BASE_SUPPORT_OPENGLES