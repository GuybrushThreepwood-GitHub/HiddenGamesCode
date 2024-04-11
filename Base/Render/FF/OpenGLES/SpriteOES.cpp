
/*===================================================================
	File: SpriteOES.cpp
	Library: RenderLib

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"

#ifdef BASE_SUPPORT_OPENGLES

#include "MathBase.h"

#include "Render/RenderConsts.h"
#include "Render/OpenGLCommon.h"
#include "Render/FF/OpenGLES/OpenGLES.h"
#include "Render/FF/OpenGLES/ExtensionsOES.h"
#include "Render/TextureShared.h"

#include "Render/FF/OpenGLES/SpriteOES.h"

using renderer::Sprite;

/////////////////////////////////////////////////////
/// Method: SetTextureParameters
/// Params: [in]pSprite, [in]nResetData
///
/////////////////////////////////////////////////////
void renderer::SetTextureParameters( const Sprite* pSprite, int nResetData )
{
	if( pSprite )
	{
		//if( pSprite->fMagFilter != (float)GL_LINEAR )
		//	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLfloat)pSprite->fMagFilter );
		//if( pSprite->fMinFilter != (float)GL_LINEAR_MIPMAP_LINEAR )
		//	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLfloat)pSprite->fMinFilter );

		if( renderer::bExtTextureLodBias )
		{
			if( pSprite->fMipmapLodBias != 0.0f )
				glTexEnvf( GL_TEXTURE_FILTER_CONTROL_EXT, GL_TEXTURE_LOD_BIAS_EXT, (GLfloat)pSprite->fMipmapLodBias );
		}
	}
	
	if( nResetData && pSprite )
	{
		//if( pSprite->fMagFilter != (float)GL_LINEAR )
		//	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		//if( pSprite->fMinFilter != (float)GL_LINEAR_MIPMAP_LINEAR )
		//	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );


		if( renderer::bExtTextureLodBias )
		{
			if( pSprite->fMipmapLodBias != 0.0f )
				glTexEnvf( GL_TEXTURE_FILTER_CONTROL_EXT, GL_TEXTURE_LOD_BIAS_EXT, 0.0f );
		}
	}
	
	GL_CHECK
}

/////////////////////////////////////////////////////
/// Default constructor
/// 
///
/////////////////////////////////////////////////////
Sprite::Sprite()
{
	nWidth	= 1;
	nHeight = 1;
	nTexID	= INVALID_OBJECT;
	vPos	= math::Vec3( 0.0f, 0.0f, 0.0f );
	vRot	= math::Vec3( 0.0f, 0.0f, 0.0f );

	bColour = true;
	vColour = math::Vec4Lite( 255, 255, 255, 255 );

	// bottom left
	vUVCoords[0] = math::Vec2( 0.0f, 0.0f );
	// bottom right
	vUVCoords[1] = math::Vec2( 1.0f, 0.0f );
	// top left
	vUVCoords[2] = math::Vec2( 0.0f, 1.0f );
	// top right
	vUVCoords[3] = math::Vec2( 1.0f, 1.0f );

	fSWrapMode = GL_REPEAT;
	fTWrapMode = GL_REPEAT;

	fScaleS = 1.0f;
	fScaleT = 1.0f;

	fMinFilter = GL_LINEAR_MIPMAP_NEAREST;
	fMagFilter = GL_LINEAR;

	bUseDefaultMipmapping = true;
	fMipmapLodBias = 0.0f;
	fMipmapLodBiasMax = 0.0f;

	bPointSprite = false;
	bViewAlign	= true;
	bYRotationOnly = false;
	bDoubleSided = true;

	bEnableDepth = true;
	eDepthFunc = GL_LESS;

	bEnableBlending = false;
	eBlendSrc = GL_SRC_ALPHA;
	eBlendDest = GL_ONE_MINUS_SRC_ALPHA;
		
	bEnableAlpha = false;
	eAlphaCompare = GL_ALWAYS;
	fAlphaClamp = 0.0f;
}

/////////////////////////////////////////////////////
/// constructor
/// Params: [in]nSizeX, [in]nSizeY, [in]nTextureID, [in]vPosition, [in]bAlign, [in]bYRotOnly, [in]bUsePointSprite
///
/////////////////////////////////////////////////////
Sprite::Sprite( int nSizeX, int nSizeY, GLuint nTextureID, const math::Vec3 &vPosition, bool bAlign, bool bYRotOnly, bool bUsePointSprite )
{
	nWidth	= nSizeX;
	nHeight = nSizeY;
	nTexID	= nTextureID;
	vPos	= vPosition;
	bViewAlign	= bAlign;
	bYRotationOnly = bYRotOnly;
	bPointSprite = bUsePointSprite;
	
	bColour = true;
	vColour = math::Vec4Lite( 255, 255, 255, 255 );
	
	// bottom left
	vUVCoords[0] = math::Vec2( 0.0f, 0.0f );
	// bottom right
	vUVCoords[1] = math::Vec2( 1.0f, 0.0f );
	// top left
	vUVCoords[2] = math::Vec2( 0.0f, 1.0f );
	// top right
	vUVCoords[3] = math::Vec2( 1.0f, 1.0f );

	fSWrapMode = GL_REPEAT;
	fTWrapMode = GL_REPEAT;
	
	fScaleS = 1.0f;
	fScaleT = 1.0f;
	
	fMinFilter = GL_LINEAR_MIPMAP_NEAREST;
	fMagFilter = GL_LINEAR;
	
	bUseDefaultMipmapping = true;
	fMipmapLodBias = 0.0f;
	fMipmapLodBiasMax = 0.0f;
	
	bDoubleSided = true;
	
	bEnableDepth = true;
	eDepthFunc = GL_LESS;
	
	bEnableBlending = false;
	eBlendSrc = GL_SRC_ALPHA;
	eBlendDest = GL_ONE_MINUS_SRC_ALPHA;
	
	bEnableAlpha = false;
	eAlphaCompare = GL_ALWAYS;
	fAlphaClamp = 0.0f;	
}

/////////////////////////////////////////////////////
/// Default destructor
/// 
///
/////////////////////////////////////////////////////
Sprite::~Sprite()
{

}

/////////////////////////////////////////////////////
/// Operator: ASSIGN 
/// Params: [in]s
///
/////////////////////////////////////////////////////
Sprite &Sprite::operator =( const Sprite &s )
{
	vPos = s.vPos;	
	vRot = s.vRot;
	fDepth = s.fDepth;
	nWidth = s.nWidth;
	nHeight = s.nHeight;
	nTexID = s.nTexID;
	bPointSprite = s.bPointSprite;
	bViewAlign = s.bViewAlign;
	bYRotationOnly = s.bYRotationOnly;
	bDoubleSided = s.bDoubleSided;
	bColour = s.bColour;

	vColour = s.vColour;

	// bottom left
	vUVCoords[0] = s.vUVCoords[0];
	// bottom right
	vUVCoords[1] = s.vUVCoords[1];
	// top right
	vUVCoords[2] = s.vUVCoords[2];
	// top left
	vUVCoords[3] = s.vUVCoords[3];

	fSWrapMode = s.fSWrapMode;
	fTWrapMode = s.fTWrapMode;

	fScaleS = s.fScaleS;
	fScaleT = s.fScaleT;

	fMinFilter = s.fMinFilter;
	fMagFilter = s.fMagFilter;

	bUseDefaultMipmapping = s.bUseDefaultMipmapping;
	fMipmapLodBias = s.fMipmapLodBias;
	fMipmapLodBiasMax = s.fMipmapLodBiasMax;

	bEnableDepth = s.bEnableDepth;
	eDepthFunc = s.eDepthFunc;
	bEnableBlending = s.bEnableBlending;
	eBlendSrc = s.eBlendSrc;
	eBlendDest = s.eBlendDest;
	bEnableAlpha = s.bEnableAlpha;
	eAlphaCompare = s.eAlphaCompare;
	fAlphaClamp = s.fAlphaClamp;

	return *this;
}

/////////////////////////////////////////////////////
/// Method: Draw
/// Params: None
///
/////////////////////////////////////////////////////
void Sprite::Draw( void )
{
	TGLCullFaceState SavedCullState;

	if( nTexID != INVALID_OBJECT )
	{
		// bind the texture
		renderer::OpenGL::GetInstance()->BindTexture( nTexID );
	}
	else
		renderer::OpenGL::GetInstance()->DisableUnitTexture( 0 );

	renderer::OpenGL::GetInstance()->DisableVBO();

	// change texture tiling (only if it's needed)
	if( fScaleS != 1.0f ||
		fScaleT != 1.0f )
	{
		glMatrixMode(GL_TEXTURE);
			glLoadIdentity();
			glScalef( fScaleS, fScaleT, 1.0f );
		glMatrixMode(GL_MODELVIEW);
	}

	if( fSWrapMode != GL_REPEAT )
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, fSWrapMode );
	if( fTWrapMode != GL_REPEAT )
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, fTWrapMode );

	SetTextureParameters( this, false );

	// setup states
	renderer::OpenGL::GetInstance()->DepthMode( bEnableDepth, eDepthFunc );
	renderer::OpenGL::GetInstance()->BlendMode( bEnableBlending, eBlendSrc, eBlendDest );
	renderer::OpenGL::GetInstance()->AlphaMode( bEnableAlpha, eAlphaCompare, fAlphaClamp );
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);	

	if( bColour )
	{
		renderer::OpenGL::GetInstance()->SetColour4ub( vColour.R, vColour.G, vColour.B, vColour.A );
	}
	else
	{
		renderer::OpenGL::GetInstance()->SetColour4ub( 255, 255, 255, 255 );
	}

	if( bDoubleSided )
	{
		renderer::OpenGL::GetInstance()->SaveCullState( &SavedCullState );
		renderer::OpenGL::GetInstance()->SetCullState( false, GL_BACK );
	}

	if( bViewAlign )
	{
		// point sprites are SLOWER for some reason, use them only if necessary
		if( bPointSprite )
		{
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);	
			
			// point sprites
			glEnable( GL_POINT_SPRITE );
			glTexEnvf( GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE );
			glPointSize( static_cast<float>(nWidth) );
			
			GLfloat spritePoint[] = { vPos.X, vPos.Y, vPos.Z };
			glVertexPointer(3, GL_FLOAT, 0, spritePoint);
			glDrawArrays(GL_POINTS, 0, 1 );

			glPointSize( 1.0f );
			glDisable( GL_POINT_SPRITE );
		}
		else
		{
			// billboard with Y rotation only and locked to face camera
			if( bYRotationOnly )
			{
				math::Vec3 vEye, vCenter;

				// get the current view position and target
				renderer::OpenGL::GetInstance()->GetLookAt( vEye, vCenter );
				
				GLfloat spritePoints[] = 
				{ 
					-static_cast<float>(nWidth), -static_cast<float>(nHeight), 0.0f,
					static_cast<float>(nWidth), -static_cast<float>(nHeight), 0.0f,
					-static_cast<float>(nWidth), static_cast<float>(nHeight), 0.0f,
					static_cast<float>(nWidth), static_cast<float>(nHeight), 0.0f
				};			
				
				//float angleDiff = AngleBetween( vPos, vEye );
				//float fAngle = math::RadToDeg( angleDiff );

				glVertexPointer(3, GL_FLOAT, 0, spritePoints);
				glTexCoordPointer(2, GL_FLOAT, sizeof( math::Vec2 ), vUVCoords );
				glDrawArrays(GL_TRIANGLE_STRIP, 0, 4 );
			}
			else
			{
				// full camera alignment on all axis
				GLfloat *mat = renderer::OpenGL::GetInstance()->GetModelViewMatrix();

				// get the right and up vectors
				math::Vec3 right;
				right.X = mat[0];
				right.Y = mat[4];
				right.Z = mat[8];

				math::Vec3 up;
				up.X = mat[1];
				up.Y = mat[5];
				up.Z = mat[9];

				GLfloat spritePoints[] = 
				{ 
					(vPos.X) + (-right.X - up.X)*static_cast<float>(nWidth), (vPos.Y) + (-right.Y - up.Y)*static_cast<float>(nHeight), vPos.Z + (-right.Z - up.Z)*static_cast<float>(nWidth),
					(vPos.X) + (right.X - up.X)*static_cast<float>(nWidth), (vPos.Y) + (right.Y - up.Y)*static_cast<float>(nHeight), vPos.Z + (right.Z - up.Z)*static_cast<float>(nWidth),
					(vPos.X) + (up.X - right.X)*static_cast<float>(nWidth), (vPos.Y) + (up.Y - right.Y)*static_cast<float>(nHeight), vPos.Z + (up.Z - right.Z)*static_cast<float>(nWidth),
					(vPos.X) + (right.X + up.X)*static_cast<float>(nWidth), (vPos.Y) + (right.Y + up.Y)*static_cast<float>(nHeight), vPos.Z + (right.Z + up.Z)*static_cast<float>(nWidth)
				};


				glVertexPointer(3, GL_FLOAT, 0, spritePoints);
				glTexCoordPointer(2, GL_FLOAT, sizeof( math::Vec2 ), vUVCoords );
				glDrawArrays(GL_TRIANGLE_STRIP, 0, 4 );
			}
		}
	}
	else
	{
		// normal quad sprite rotated accordingly
		glPushMatrix();
			glTranslatef( vPos.X, vPos.Y, vPos.Z );

			glRotatef( vRot.X, 1.0f, 0.0f, 0.0f );
			glRotatef( vRot.Y, 0.0f, 1.0f, 0.0f );
			glRotatef( vRot.Z, 0.0f, 0.0f, 1.0f );

			GLfloat spritePoints[] = 
			{ 
				-static_cast<float>(nWidth), -static_cast<float>(nHeight), 0.0f,
				static_cast<float>(nWidth), -static_cast<float>(nHeight), 0.0f,
				-static_cast<float>(nWidth), static_cast<float>(nHeight), 0.0f,
				static_cast<float>(nWidth), static_cast<float>(nHeight), 0.0f
			};			
		
			glVertexPointer(3, GL_FLOAT, 0, spritePoints);
			glTexCoordPointer(2, GL_FLOAT, sizeof( math::Vec2 ), vUVCoords );
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4 );
		
		glPopMatrix();
	}

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);	
	
	// reset culling
	if( bDoubleSided )
		renderer::OpenGL::GetInstance()->SetCullState( &SavedCullState );

	// change texture tiling (only if it's needed)
	if( fScaleS != 1.0f ||
		fScaleT != 1.0f )
	{
		glMatrixMode(GL_TEXTURE);
			glLoadIdentity();
			glScalef( 1.0f, 1.0f, 1.0f );
		glMatrixMode(GL_MODELVIEW);
	}

	SetTextureParameters( this, true );

	// reset wrap
	if( fSWrapMode != GL_REPEAT )
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	if( fTWrapMode != GL_REPEAT )
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

}

/////////////////////////////////////////////////////
/// Method: SetPosition
/// Params: [in]vNewPos
///
/////////////////////////////////////////////////////
void Sprite::SetPosition( const math::Vec3 &vNewPos )
{
	vPos.X = vNewPos.X;
	vPos.Y = vNewPos.Y;
	vPos.Z = vNewPos.Z;
}

/////////////////////////////////////////////////////
/// Method: SetPosition
/// Params: [in]fX, [in]fY, [in]fZ
///
/////////////////////////////////////////////////////
void Sprite::SetPosition( float fX, float fY, float fZ )
{
	vPos.X = fX;
	vPos.Y = fY;
	vPos.Z = fZ;
}

/////////////////////////////////////////////////////
/// Method: SetRotation
/// Params: [in]vNewRot
///
/////////////////////////////////////////////////////
void Sprite::SetRotation( const math::Vec3 &vNewRot )
{
	vRot.X = vNewRot.X;
	vRot.Y = vNewRot.Y;
	vRot.Z = vNewRot.Z;
}

/////////////////////////////////////////////////////
/// Method: SetRotation
/// Params: [in]fX, [in]fY, [in]fZ
///
/////////////////////////////////////////////////////
void Sprite::SetRotation( float fX, float fY, float fZ )
{
	vRot.X = fX;
	vRot.Y = fY;
	vRot.Z = fZ;
}

/////////////////////////////////////////////////////
/// Method: SetWidthHeight
/// Params: [in]nNewWidth, [in]nNewHeight
///
/////////////////////////////////////////////////////
void Sprite::SetWidthHeight( int nNewWidth, int nNewHeight )
{
	nWidth = nNewWidth;
	nHeight = nNewHeight;
}

/////////////////////////////////////////////////////
/// Method: SetTexture
/// Params: [in]nTextureID
///
/////////////////////////////////////////////////////
void Sprite::SetTexture( GLuint nTextureID )
{
	nTexID = nTextureID;
}

/////////////////////////////////////////////////////
/// Method: SetTextureUVOffset
/// Params: [in]nX, [in]nY, [in]nWidth, [in]nHeight, [in]nTextureWidth, [in]nTextureHeight
///
/////////////////////////////////////////////////////
void Sprite::SetTextureUVOffset( int nX, int nY, int nWidth, int nHeight, int nTextureWidth, int nTextureHeight )
{
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
/// Method: ViewAlign
/// Params: [in]bAlign
///
/////////////////////////////////////////////////////
void Sprite::ViewAlign( bool bAlign )
{
	bViewAlign = bAlign;
}


#endif // BASE_SUPPORT_OPENGLES
