
/*===================================================================
	File: OpenGLCommon.cpp
	Library: RenderLib

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"

#include <cmath>

#include "Core/CoreFunctions.h"

#include "Math/Vectors.h"
#include "Collision/AABB.h"

#include "Render/RenderConsts.h"
#include "Render/FF/OpenGL/OpenGL.h"
#include "Render/FF/OpenGLES/OpenGLES.h"
#include "Render/GLSL/OpenGLSL.h"
#include "Render/TextureShared.h"

#include "Render/OpenGLCommon.h"


/////////////////////////////////////////////////////
/// Function: CheckForGLError
/// Params: [in]szFile, [in]nLine
///
/////////////////////////////////////////////////////
GLint renderer::CheckForGLError( const char *szFile, GLint nLine )
{
	// Returns 1 if an OpenGL error occurred, 0 otherwise.
	GLint retCode = 0;
		
	if( renderer::OpenGL::IsInitialised() )
	{
		GLenum glErr;
			
		glErr = glGetError();
		while( glErr != GL_NO_ERROR )
		{
			DBGLOG( "GL Error in file %s @ line %d: %d\n", szFile, nLine, glErr );
			retCode = 1;
			// always stop in debug mode to check the state
#ifdef _DEBUG
			DBG_ASSERT(0);
#endif // _DEBUG
			
			glErr = glGetError();
			
		}
	}
	
	return retCode;
}

/////////////////////////////////////////////////////
/// Function: SupportsGLVersion
/// Params: [in]nLeastMajor, [in]nLeastMinor
///
/////////////////////////////////////////////////////
bool renderer::SupportsGLVersion( GLint nLeastMajor, GLint nLeastMinor )
{
	const char *szVersion = 0;
	GLint nMajor, nMinor, nRevision;
	
	szVersion = reinterpret_cast<const char *>(glGetString( GL_VERSION ));
	
	if( !core::IsEmptyString( szVersion ) )
	{
		/*if( std::sscanf( szVersion, "%d.%d", &nMajor, &nMinor ) == 2 )
		{
			// found a version higher than needed
			if( nMajor > nLeastMajor )
				return(true);
			// 0 need a major version and greater or equal minor
			if( nMajor == nLeastMajor && nMinor >= nLeastMinor )
				return(true);
		}
		else
		{
			DBGLOG( "OPENGL: *ERROR* GL string version is malformed\n" );
			return(false);
		}*/

		const char *ptr;

		// Parse string
		ptr = szVersion;
		for( nMajor = 0; *ptr >= '0' && *ptr <= '9'; ptr ++ )
		{
			nMajor = 10*nMajor + (*ptr - '0');
		}
		if( *ptr == '.' )
		{
			ptr ++;
			for( nMinor = 0; *ptr >= '0' && *ptr <= '9'; ptr ++ )
			{
				nMinor = 10*nMinor + (*ptr - '0');
			}
			if( *ptr == '.' )
			{
				ptr ++;
				for( nRevision = 0; *ptr >= '0' && *ptr <= '9'; ptr ++ )
				{
					nRevision = 10*nRevision + (*ptr - '0');
				}
			}
		}

		// found a version higher than needed
		if( nMajor > nLeastMajor )
			return(true);
		// 0 need a major version and greater or equal minor
		if( nMajor == nLeastMajor && nMinor >= nLeastMinor )
			return(true);
	}
	
	return(false);
}

/////////////////////////////////////////////////////
/// Function: SetTextureUVOffset
/// Params: [in]vUVCoords, [in]nX, [in]nY, [in]nWidth, [in]nHeight, [in]nTextureWidth, [in]nTextureHeight
///
/////////////////////////////////////////////////////
void renderer::SetTextureUVOffset( math::Vec2* vUVCoords, int nX, int nY, int nWidth, int nHeight, int nTextureWidth, int nTextureHeight, EPointOrigin origin )
{
	if( origin == POINTORIGIN_TOPLEFT )
	{
		nY = (nTextureHeight - nY) - nHeight;
	}

	// bottom left
	vUVCoords[0].U = renderer::GetUVCoord( nX, nTextureWidth );
	vUVCoords[0].V = renderer::GetUVCoord( nY, nTextureHeight );

	// bottom right
	vUVCoords[1].U = renderer::GetUVCoord( nX+nWidth, nTextureWidth );
	vUVCoords[1].V = renderer::GetUVCoord( nY, nTextureHeight );

	// top left
	vUVCoords[2].U = renderer::GetUVCoord( nX, nTextureWidth );
	vUVCoords[2].V = renderer::GetUVCoord( nY+nHeight, nTextureHeight );

	// top right
	vUVCoords[3].U = renderer::GetUVCoord( nX+nWidth, nTextureWidth );
	vUVCoords[3].V = renderer::GetUVCoord( nY+nHeight, nTextureHeight );
}

/////////////////////////////////////////////////////
/// Static Method: GetAssetScale
/// 
///
/////////////////////////////////////////////////////
math::Vec2 renderer::GetAssetScale( int srcWidth, int srcHeight )
{
	math::Vec2 finalScale( 1.0f, 1.0f );

	if( core::app::IsLandscape() )
	{
		if( renderer::OpenGL::GetInstance()->GetIsRotated() )
		{
			finalScale.X = static_cast<float>(core::app::GetOrientationHeight()) / static_cast<float>(srcHeight);  
			finalScale.Y = static_cast<float>(core::app::GetOrientationWidth()) / static_cast<float>(srcWidth);
		}
		else
		{
			finalScale.X = static_cast<float>(core::app::GetOrientationWidth()) / static_cast<float>(srcHeight);
			finalScale.Y = static_cast<float>(core::app::GetOrientationHeight()) / static_cast<float>(srcWidth);  
		}
	}
	else
	{
		finalScale.X = static_cast<float>(core::app::GetOrientationWidth()) / static_cast<float>(srcWidth);
		finalScale.Y = static_cast<float>(core::app::GetOrientationHeight()) / static_cast<float>(srcHeight);
	}

	return finalScale;
}

/// converted from Mesa
void gluUtil::gluMultMatrixVecf(const GLfloat matrix[16], const GLfloat in[4], GLfloat out[4])
{
	int i;

	for (i=0; i<4; i++) 
	{
		out[i] = 
			in[0] * matrix[0*4+i] +
			in[1] * matrix[1*4+i] +
			in[2] * matrix[2*4+i] +
			in[3] * matrix[3*4+i];
	}
}

/*
** Invert 4x4 matrix.
** Contributed by David Moore (See Mesa bug #6748)
*/
int gluUtil::gluInvertMatrixf(const GLfloat m[16], GLfloat invOut[16])
{
	float inv[16], det;
	int i;

	inv[0] =   m[5]*m[10]*m[15] - m[5]*m[11]*m[14] - m[9]*m[6]*m[15]
			 + m[9]*m[7]*m[14] + m[13]*m[6]*m[11] - m[13]*m[7]*m[10];
	inv[4] =  -m[4]*m[10]*m[15] + m[4]*m[11]*m[14] + m[8]*m[6]*m[15]
			 - m[8]*m[7]*m[14] - m[12]*m[6]*m[11] + m[12]*m[7]*m[10];
	inv[8] =   m[4]*m[9]*m[15] - m[4]*m[11]*m[13] - m[8]*m[5]*m[15]
			 + m[8]*m[7]*m[13] + m[12]*m[5]*m[11] - m[12]*m[7]*m[9];
	inv[12] = -m[4]*m[9]*m[14] + m[4]*m[10]*m[13] + m[8]*m[5]*m[14]
			 - m[8]*m[6]*m[13] - m[12]*m[5]*m[10] + m[12]*m[6]*m[9];
	inv[1] =  -m[1]*m[10]*m[15] + m[1]*m[11]*m[14] + m[9]*m[2]*m[15]
			 - m[9]*m[3]*m[14] - m[13]*m[2]*m[11] + m[13]*m[3]*m[10];
	inv[5] =   m[0]*m[10]*m[15] - m[0]*m[11]*m[14] - m[8]*m[2]*m[15]
			 + m[8]*m[3]*m[14] + m[12]*m[2]*m[11] - m[12]*m[3]*m[10];
	inv[9] =  -m[0]*m[9]*m[15] + m[0]*m[11]*m[13] + m[8]*m[1]*m[15]
			 - m[8]*m[3]*m[13] - m[12]*m[1]*m[11] + m[12]*m[3]*m[9];
	inv[13] =  m[0]*m[9]*m[14] - m[0]*m[10]*m[13] - m[8]*m[1]*m[14]
			 + m[8]*m[2]*m[13] + m[12]*m[1]*m[10] - m[12]*m[2]*m[9];
	inv[2] =   m[1]*m[6]*m[15] - m[1]*m[7]*m[14] - m[5]*m[2]*m[15]
			 + m[5]*m[3]*m[14] + m[13]*m[2]*m[7] - m[13]*m[3]*m[6];
	inv[6] =  -m[0]*m[6]*m[15] + m[0]*m[7]*m[14] + m[4]*m[2]*m[15]
			 - m[4]*m[3]*m[14] - m[12]*m[2]*m[7] + m[12]*m[3]*m[6];
	inv[10] =  m[0]*m[5]*m[15] - m[0]*m[7]*m[13] - m[4]*m[1]*m[15]
			 + m[4]*m[3]*m[13] + m[12]*m[1]*m[7] - m[12]*m[3]*m[5];
	inv[14] = -m[0]*m[5]*m[14] + m[0]*m[6]*m[13] + m[4]*m[1]*m[14]
			 - m[4]*m[2]*m[13] - m[12]*m[1]*m[6] + m[12]*m[2]*m[5];
	inv[3] =  -m[1]*m[6]*m[11] + m[1]*m[7]*m[10] + m[5]*m[2]*m[11]
			 - m[5]*m[3]*m[10] - m[9]*m[2]*m[7] + m[9]*m[3]*m[6];
	inv[7] =   m[0]*m[6]*m[11] - m[0]*m[7]*m[10] - m[4]*m[2]*m[11]
			 + m[4]*m[3]*m[10] + m[8]*m[2]*m[7] - m[8]*m[3]*m[6];
	inv[11] = -m[0]*m[5]*m[11] + m[0]*m[7]*m[9] + m[4]*m[1]*m[11]
			 - m[4]*m[3]*m[9] - m[8]*m[1]*m[7] + m[8]*m[3]*m[5];
	inv[15] =  m[0]*m[5]*m[10] - m[0]*m[6]*m[9] - m[4]*m[1]*m[10]
			 + m[4]*m[2]*m[9] + m[8]*m[1]*m[6] - m[8]*m[2]*m[5];

	det = m[0]*inv[0] + m[1]*inv[4] + m[2]*inv[8] + m[3]*inv[12];
	if (det == 0)
		return GL_FALSE;

	det = 1.0f / det;

	for (i = 0; i < 16; i++)
		invOut[i] = inv[i] * det;

	return GL_TRUE;
}

/////////////////////////////////////////////////////
/// Function: gluMultMatricesf
/// Params: [in]a, [in]b, [in/out]r
///
/////////////////////////////////////////////////////
void gluUtil::gluMultMatricesf(const GLfloat a[16], const GLfloat b[16], GLfloat r[16])
{
	int i, j;

	for (i = 0; i < 4; i++) 
	{
		for (j = 0; j < 4; j++) 
		{
			r[i*4+j] = 
			a[i*4+0]*b[0*4+j] +
			a[i*4+1]*b[1*4+j] +
			a[i*4+2]*b[2*4+j] +
			a[i*4+3]*b[3*4+j];
		}
	}
}

/////////////////////////////////////////////////////
/// Function: gluProjectf
/// Params: [in]objx, [in]objy, [in]objz, [in]modelMatrix, [in]projMatrix, [in]viewport, [in/out]winx, [in/out]winy, [in/out]winz
///
/////////////////////////////////////////////////////
GLint gluUtil::gluProjectf( GLfloat objx, GLfloat objy, GLfloat objz, 
		  const GLfloat modelMatrix[16], const GLfloat projMatrix[16], const GLint viewport[4],
		  GLfloat *winx, GLfloat *winy, GLfloat *winz)
{
	float in[4];
	float out[4];

	in[0]=objx;
	in[1]=objy;
	in[2]=objz;
	in[3]=1.0f;
	gluUtil::gluMultMatrixVecf(modelMatrix, in, out);
	gluUtil::gluMultMatrixVecf(projMatrix, out, in);

	if (in[3] == 0.0f) 
		return(GL_FALSE);

	in[0] /= in[3];
	in[1] /= in[3];
	in[2] /= in[3];
	// Map x, y and z to range 0-1
	in[0] = in[0] * 0.5f + 0.5f;
	in[1] = in[1] * 0.5f + 0.5f;
	in[2] = in[2] * 0.5f + 0.5f;

	// Map x,y to viewport
	in[0] = in[0] * viewport[2] + viewport[0];
	in[1] = in[1] * viewport[3] + viewport[1];

    if( renderer::OpenGL::GetInstance()->GetIsRotated() )
    {
        switch( renderer::OpenGL::GetInstance()->GetRotationStyle() )
        {                
            case renderer::VIEWROTATION_PORTRAIT_BUTTON_BOTTOM:
            {
                if( winx )
                    *winx=in[0];
                if( winy )
                    *winy=in[1];
                if( winz )
                    *winz=in[2];                 
            }break;
            case renderer::VIEWROTATION_PORTRAIT_BUTTON_TOP:
            {
                if( winx )
                    *winx=in[0];
                if( winy )
                    *winy=core::app::GetAppHeight() - in[1];
                if( winz )
                    *winz=in[2];                 
            }break;
            case renderer::VIEWROTATION_LANDSCAPE_BUTTON_LEFT:
            {
                if( winx )
                    *winx=in[1];
                if( winy )
                    *winy=core::app::GetAppWidth() - in[0];
                if( winz )
                    *winz=in[2];                 
            }break;
            case renderer::VIEWROTATION_LANDSCAPE_BUTTON_RIGHT:
            {
                if( winx )
                    *winx=core::app::GetAppHeight() - in[1];
                if( winy )
                    *winy=in[0];
                if( winz )
                    *winz=in[2]; 
            }break;
            default:
                break;
        }        
         
    }
    else
    {
        if( winx )
            *winx=in[0];
        if( winy )
            *winy=in[1];
        if( winz )
            *winz=in[2];      
    }
	return(GL_TRUE);
}

/////////////////////////////////////////////////////
/// Function: gluUnProjectf
/// Params: [in]winx, [in]winy, [in]winz, [in]modelMatrix, [in]projMatrix, [in]viewport, [in/out]objx, [in/out]objy, [in/out]objz
///
/////////////////////////////////////////////////////
GLint gluUtil::gluUnProjectf( GLfloat winx, GLfloat winy, GLfloat winz,
							 const GLfloat modelMatrix[16], const GLfloat projMatrix[16], const GLint viewport[4],
								GLfloat *objx, GLfloat *objy, GLfloat *objz )
{
	float finalMatrix[16];
	float in[4];
	float out[4];

	gluUtil::gluMultMatricesf(modelMatrix, projMatrix, finalMatrix);
	if (!gluUtil::gluInvertMatrixf(finalMatrix, finalMatrix)) 
		return(GL_FALSE); 

    if( renderer::OpenGL::GetInstance()->GetIsRotated() )
    {
        switch( renderer::OpenGL::GetInstance()->GetRotationStyle() )
        {                
            case renderer::VIEWROTATION_PORTRAIT_BUTTON_BOTTOM:
            {
                in[0] = winx;
                in[1] = winy;
                in[2] = winz;                  
            }break;
            case renderer::VIEWROTATION_PORTRAIT_BUTTON_TOP:
            {
                in[0] = core::app::GetAppWidth() - winx;
                in[1] = core::app::GetAppHeight() - winy;
                in[2] = winz;                 
            }break;
            case renderer::VIEWROTATION_LANDSCAPE_BUTTON_LEFT:
            {
                in[0] = core::app::GetAppWidth() - winy;
                in[1] =  winx;
                in[2] = winz;              
            }break;
            case renderer::VIEWROTATION_LANDSCAPE_BUTTON_RIGHT:
            {
                in[0] = winy;
                in[1] = core::app::GetAppHeight() - winx;
                in[2] = winz;  
            }break;
            default:
                break;
        }
    }
    else
    {
        in[0] = winx;
        in[1] = winy;
        in[2] = winz;        
    }
    
	in[3]=1.0f;

	/* Map x and y from window coordinates */
	in[0] = (in[0] - viewport[0]) / viewport[2];
	in[1] = (in[1] - viewport[1]) / viewport[3];

	/* Map to range -1 to 1 */
	in[0] = in[0] * 2 - 1;
	in[1] = in[1] * 2 - 1;
	in[2] = in[2] * 2 - 1;

	gluUtil::gluMultMatrixVecf(finalMatrix, in, out);
	if (out[3] == 0.0f) 
		return(GL_FALSE);

    out[0] /= out[3];
    out[1] /= out[3];
    out[2] /= out[3];


    *objx = out[0];
    *objy = out[1];
    *objz = out[2];
    
	return(GL_TRUE);
}
