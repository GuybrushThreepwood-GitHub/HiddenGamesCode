
/*===================================================================
	File: RenderToTextureOES.cpp
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
#include "Render/Texture.h"
#include "Render/FF/OpenGLES/TextureLoadAndUploadOES.h"

#include "Render/FF/OpenGLES/RenderToTextureOES.h"

using renderer::RenderToTexture;

/////////////////////////////////////////////////////
/// Default constructor
/// 
///
/////////////////////////////////////////////////////
RenderToTexture::RenderToTexture()
{
	Initialise();
}

/////////////////////////////////////////////////////
/// Default destructor
/// 
///
/////////////////////////////////////////////////////
RenderToTexture::~RenderToTexture()
{
	Release();
}

/////////////////////////////////////////////////////
/// Method: Initialise
/// Params: None
///
/////////////////////////////////////////////////////
void RenderToTexture::Initialise( void )
{
	vUVCoords[_BL_] = math::Vec2( 0.0f, 0.0f );
	vUVCoords[_BR_] = math::Vec2( 1.0f, 0.0f );
	vUVCoords[_TR_] = math::Vec2( 1.0f, 1.0f );
	vUVCoords[_TL_] = math::Vec2( 0.0f, 1.0f );

	nID = INVALID_OBJECT;
	vClearColour = math::Vec4( 0.0f, 0.0f, 0.0f, 1.0f );
	vColour = math::Vec4( 1.0f, 1.0f, 1.0f, 1.0f );
	nWidth = 0;
	nHeight = 0;
	nBPP = 0;
	bHasAlpha = false;
	bHasMipMaps = false;
	bDepthOnly = false;
	eInternalFormat = GL_RGBA;
	eFormat = GL_RGBA;
	eWrapMode = GL_CLAMP_TO_EDGE;

	eFullscreenFilter = GL_NEAREST;

	image = 0;
	depthBuffer = 0;
	bInitialised = false;
	nUnit = 0;
	bValidTexture = false;

	nPreviousWidth = 0;
	nPreviousHeight = 0;
	nPreviousBoundFrameBuffer = INVALID_OBJECT;

	nID = INVALID_OBJECT;
	nFramebufferID = INVALID_OBJECT;
	nDepth_rb = INVALID_OBJECT;
	nStencil_rb = INVALID_OBJECT;
}

/////////////////////////////////////////////////////
/// Method: Release
/// Params: None 
///
/////////////////////////////////////////////////////
void RenderToTexture::Release( void )
{
	nWidth = 0;
	nHeight = 0;
	nBPP = 0;
	bHasAlpha = false;
	bHasMipMaps = false;
	bDepthOnly = false;
	eInternalFormat = GL_RGB;
	eFormat = GL_RGB;
	eWrapMode = GL_CLAMP_TO_EDGE;

	// remove any texture data
	if( nID != INVALID_OBJECT )
	{
		renderer::RemoveTexture( nID );
		nID = INVALID_OBJECT;
	}

	if( nDepth_rb != INVALID_OBJECT )
	{
		if( glIsRenderbufferOES(nDepth_rb) )
			glDeleteRenderbuffersOES( 1, &nDepth_rb );
		nDepth_rb = INVALID_OBJECT;
	}

	if( nStencil_rb != INVALID_OBJECT )
	{
		if( glIsRenderbufferOES(nStencil_rb) )
			glDeleteRenderbuffersOES( 1, &nStencil_rb );
		nDepth_rb = INVALID_OBJECT;
	}

	if( nFramebufferID != INVALID_OBJECT )
	{
		if( glIsFramebufferOES(nFramebufferID) )
			glDeleteFramebuffersOES( 1, &nFramebufferID );
		nFramebufferID = INVALID_OBJECT;
	}

	if( image )
	{
		delete image;
		image = 0;
	}

	if( depthBuffer )
	{
		delete depthBuffer;
		depthBuffer = 0;
	}
	
	bInitialised = false;
	nUnit = 0;
	bValidTexture = false;
}

/////////////////////////////////////////////////////
/// Method: Create
/// Params: [in]nTexWidth, [in]nTexHeight, [in]nTexInternalFormat, [in]nTexFormat, [in]nTexType, [in]eTexWrapMode, [in]bMipMap
///
/////////////////////////////////////////////////////
int RenderToTexture::Create( int nTexWidth, int nTexHeight, short flags, GLenum nTexInternalFormat, GLenum nTexFormat, GLenum nTexType, GLenum eTexWrapMode, bool bMipMap )
{
	if( bExtFrameBufferObject )
	{
		return( CreateFBO( nTexWidth, nTexHeight, flags, nTexInternalFormat, nTexFormat, nTexType, eTexWrapMode, bMipMap ) );
	}

	return(1);
}

/////////////////////////////////////////////////////
/// Method: CreateDepthOnly
/// Params: [in]nTexWidth, [in]nTexHeight
///
/////////////////////////////////////////////////////
int RenderToTexture::CreateDepthOnly( int nTexWidth, int nTexHeight )
{
	if( bExtFrameBufferObject )
	{
		return( CreateFBODepthOnly( nTexWidth, nTexHeight ) );
	}

	return(1);
}

/////////////////////////////////////////////////////
/// Method: CreateFBO
/// Params: [in]nTexWidth, [in]nTexHeight,  [in]nTexInternalFormat, [in]nTexFormat, [in]nTexType, [in]eTexWrapMode, [in]bMipMap
///
/////////////////////////////////////////////////////
int RenderToTexture::CreateFBO( int nTexWidth, int nTexHeight,  short flags, GLenum nTexInternalFormat, GLenum nTexFormat, GLenum nTexType, GLenum eTexWrapMode, bool bMipMap )
{
	GLenum activeMinFilter=GL_LINEAR, activeMagFilter=GL_LINEAR;

	eRTTMode = RTTMODE_FBO;
	bHasMipMaps = bMipMap;
	eInternalFormat = nTexInternalFormat;
	eFormat = nTexFormat;
	eType = nTexType;
	nWidth = nTexWidth;
	nHeight = nTexHeight;
	eWrapMode = eTexWrapMode;
	bHasMipMaps = bMipMap;

	eTarget = GL_TEXTURE_2D;

	// depth bits
	GLenum eDepthFormat;
	GLint depth_bits;
	glGetIntegerv( GL_DEPTH_BITS, &depth_bits );

	eDepthFormat = GL_DEPTH_COMPONENT24;

	int nIsHeightPOW2 = renderer::IsPowerOfTwo( nTexHeight );
	int nIsWidthPOW2 = renderer::IsPowerOfTwo( nTexWidth );

	// no NPOT extension, downscale the width and height
	if( !nIsHeightPOW2 )
		nHeight = renderer::GetPreviousPowerOfTwo( nTexHeight, renderer::OpenGL::GetInstance()->GetMaxRenderBufferSize() );
	if( !nIsWidthPOW2 )
		nWidth = renderer::GetPreviousPowerOfTwo( nTexWidth, renderer::OpenGL::GetInstance()->GetMaxRenderBufferSize() );

	if( !nIsHeightPOW2 || !nIsWidthPOW2 )
		DBGLOG( "RENDERTOTEXTUREOES: *WARNING*  RTT had NPOT size and GL_ARB_texture_non_power_of_two is not available\n" );

	if( bExtFrameBufferObject )
	{
		// create colour buffer, depth and vanilla texture ids
		glGenFramebuffersOES( 1, &nFramebufferID );
		glGenTextures( 1, &nID );
		glGenRenderbuffersOES(1, &nDepth_rb );
  
		renderer::OpenGL::GetInstance()->BindFrameBufferObject( nFramebufferID );

		// init texture
		renderer::OpenGL::GetInstance()->BindUnitTexture( 0, eTarget, nID );
		glTexImage2D( eTarget, 0, eInternalFormat, nWidth, nHeight, 0, eFormat, eType, 0 );
		
		// check for mipmaps, cannot mipmap rectangular textures
		if( bHasMipMaps )
		{
			// generate mipmaps for the target
			glGenerateMipmapOES( eTarget );
		}
		else
		{
			// default to bilinear
			activeMagFilter = GL_LINEAR;
			activeMinFilter = GL_LINEAR;
		}

		GL_CHECK
		
		glTexParameterf( eTarget, GL_TEXTURE_MIN_FILTER, (GLfloat)activeMinFilter );
		glTexParameterf( eTarget, GL_TEXTURE_MAG_FILTER, (GLfloat)activeMagFilter );

		glTexParameterf( eTarget, GL_TEXTURE_WRAP_S, (GLfloat)eWrapMode );
		glTexParameterf( eTarget, GL_TEXTURE_WRAP_T, (GLfloat)eWrapMode );

		glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, eTarget, nID, 0 );

		GL_CHECK

		// bind the depth render buffer and attach its ID
		glBindRenderbuffer( GL_RENDERBUFFER, nDepth_rb );
		glRenderbufferStorage( GL_RENDERBUFFER, eDepthFormat, nWidth, nHeight );
		glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, nDepth_rb );

		GL_CHECK

		// make sure the frame buffer isn't enabled while returning
		renderer::OpenGL::GetInstance()->ClearFrameBufferObject();

		DBGLOG( "RENDERTOTEXTUREOES: Creating FBO - TextureID (%d) FrameBufferID(%d) DepthBits (%d) DepthRenderBufferID (%d)\n", nID, nFramebufferID, depth_bits, nDepth_rb );

		if( CheckFramebufferStatus() == 0 )
		{
			bValidTexture = true;
			return(0);
		}

		bValidTexture = false;
		return(1);
	}

	return(1);
}

/////////////////////////////////////////////////////
/// Method: CreateFBODepthOnly
/// Params: [in]nTexWidth, [in]nTexHeight
///
/////////////////////////////////////////////////////
int RenderToTexture::CreateFBODepthOnly( int nTexWidth, int nTexHeight )
{
// not really supported

	return(1);

	eRTTMode = RTTMODE_FBO;
	bHasMipMaps = 0;

	// depth bits
	GLenum eDepthFormat;
	GLint depth_bits;
	glGetIntegerv( GL_DEPTH_BITS, &depth_bits );

	eDepthFormat = GL_DEPTH_COMPONENT24;

	eInternalFormat = eDepthFormat;
	eFormat = GL_DEPTH_COMPONENT24;
	eType = GL_UNSIGNED_BYTE;
	nWidth = nTexWidth;
	nHeight = nTexHeight;

	eWrapMode = GL_CLAMP_TO_EDGE;

	eTarget = GL_TEXTURE_2D;

	// filtering mode
	GLenum eFilterMode = GL_LINEAR;

	int nIsHeightPOW2 = renderer::IsPowerOfTwo( nTexHeight );
	int nIsWidthPOW2 = renderer::IsPowerOfTwo( nTexWidth );

	// no NPOT extension, downscale the width and height
	if( !nIsHeightPOW2 )
		nHeight = renderer::GetPreviousPowerOfTwo( nTexHeight, renderer::OpenGL::GetInstance()->GetMaxRenderBufferSize() );
	if( !nIsWidthPOW2 )
		nWidth = renderer::GetPreviousPowerOfTwo( nTexWidth, renderer::OpenGL::GetInstance()->GetMaxRenderBufferSize() );

	if( !nIsHeightPOW2 || !nIsWidthPOW2 )
		DBGLOG( "RENDERTOTEXTUREOES: *WARNING*  RTT had NPOT size and GL_ARB_texture_non_power_of_two is not available\n" );

	if( bExtFrameBufferObject )
	{
		// create colour buffer, depth and vanilla texture ids
		glGenFramebuffers( 1, &nFramebufferID );
		glGenTextures( 1, &nID );
		renderer::OpenGL::GetInstance()->BindFrameBufferObject( nFramebufferID );

		// init texture
		renderer::OpenGL::GetInstance()->BindUnitTexture( 0, eTarget, nID );
		glTexImage2D( eTarget, 0, eInternalFormat, nWidth, nHeight, 0, eFormat, eType, 0 );

		GL_CHECK
		
		// depth cannot be mipmapped, so force the correct filters
		glTexParameterf( eTarget, GL_TEXTURE_MIN_FILTER, (GLfloat)eFilterMode );
		glTexParameterf( eTarget, GL_TEXTURE_MAG_FILTER, (GLfloat)eFilterMode );

		glTexParameterf( eTarget, GL_TEXTURE_WRAP_S, (GLfloat)eWrapMode );
		glTexParameterf( eTarget, GL_TEXTURE_WRAP_T, (GLfloat)eWrapMode );

        // Set up depth_tex for render-to-texture
        glFramebufferTexture2D( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, eTarget, nID, 0 );

		GL_CHECK

		// make sure the frame buffer isn't enabled while returning
		renderer::OpenGL::GetInstance()->ClearFrameBufferObject( );
		
		DBGLOG( "RENDERTOTEXTUREOES: Creating FBO Depth - TextureID (%d) FrameBufferID(%d) DepthBits (%d)\n", nID, nFramebufferID, depth_bits );

		if( CheckFramebufferStatus() == 0 )
		{
			bValidTexture = true;
			bDepthOnly = true;
			return(0);
		}

		bValidTexture = false;
		return(1);
	}

	return(1);
}

/////////////////////////////////////////////////////
/// Method: Create
/// Params: None
///
/////////////////////////////////////////////////////
void RenderToTexture::StartRenderToTexture( GLbitfield mask )
{
	if( !bValidTexture )
		return;

	// FBO
	if( eRTTMode == RTTMODE_FBO )
	{
		if( bExtFrameBufferObject && nFramebufferID != INVALID_OBJECT )
		{
			// if this function is called during another bound framebuffer, need to know what ID to switch back to
			nPreviousBoundFrameBuffer = renderer::OpenGL::GetInstance()->GetActiveFrameBuffer();

			// clear the texture bind, and bind the frame buffer
			renderer::OpenGL::GetInstance()->BindUnitTexture( 0, eTarget, 0 );

			renderer::OpenGL::GetInstance()->BindFrameBufferObject( nFramebufferID );
		}
	}

	glClearColor( vClearColour.R, vClearColour.G, vClearColour.B, vClearColour.A );		

	renderer::OpenGL::GetInstance()->GetWidthHeight( &nPreviousWidth, &nPreviousHeight );

	renderer::OpenGL::GetInstance()->SetViewport( nWidth, nHeight );

	glPushMatrix();
		glLoadIdentity();
		glClear( mask );
}

/////////////////////////////////////////////////////
/// Method: EndRenderToTexture
/// Params: None
///
/////////////////////////////////////////////////////
void RenderToTexture::EndRenderToTexture( void )
{
	if( !bValidTexture )
		return;

	glPopMatrix();
	// reset viewport
	renderer::OpenGL::GetInstance()->SetViewport( nPreviousWidth, nPreviousHeight );

	if( eRTTMode == RTTMODE_FBO )
	{
		// FBO
		if( bExtFrameBufferObject && nFramebufferID != INVALID_OBJECT )
		{
			// unbind the framebuffer
			renderer::OpenGL::GetInstance()->ClearFrameBufferObject( );

			// bind the texture
			renderer::OpenGL::GetInstance()->BindUnitTexture( 0, eTarget, nID );

			// mip the texture?
			if( bHasMipMaps )
			{
				// generate mipmaps for the target
				glGenerateMipmap( eTarget );
			}
		}

		if( nPreviousBoundFrameBuffer > 0 && nPreviousBoundFrameBuffer != INVALID_OBJECT )
		{
			// clear the texture bind, and bind the frame buffer
			renderer::OpenGL::GetInstance()->BindUnitTexture( 0, eTarget, 0 );

			// rebind any previous framebuffer
			renderer::OpenGL::GetInstance()->BindFrameBufferObject( nPreviousBoundFrameBuffer );
		}
	}
}

/////////////////////////////////////////////////////
/// Method: Bind
/// Params: [in]nTexUnit
///
/////////////////////////////////////////////////////
void RenderToTexture::Bind( int nTexUnit )
{
	if( !bValidTexture )
		return;

	if( nID != INVALID_OBJECT )
	{
		renderer::OpenGL::GetInstance()->BindUnitTexture( nTexUnit, eTarget, nID );
		nUnit = nTexUnit;
	}
}

/////////////////////////////////////////////////////
/// Method: UnBind
/// Params: None
///
/////////////////////////////////////////////////////
void RenderToTexture::UnBind( void )
{
	if( !bValidTexture )
		return;

	// unbind the framebuffer
	if( eRTTMode == RTTMODE_FBO )
	{
		renderer::OpenGL::GetInstance()->ClearFrameBufferObject( );
	}

	renderer::OpenGL::GetInstance()->DisableUnitTexture( nUnit );
}

/////////////////////////////////////////////////////
/// Method: RenderToScreen
/// Params: [in]nTexUnit
///
/////////////////////////////////////////////////////
void RenderToTexture::RenderToScreen( int nTexUnit )
{
	if( !bValidTexture )
		return;

	int nScreenWidth = core::app::GetOrientationWidth();
	int nScreenHeight = core::app::GetOrientationHeight();

	TViewState SavedView;
	renderer::OpenGL::GetInstance()->SaveViewState( &SavedView );

	renderer::OpenGL::GetInstance()->DisableLighting();
	renderer::OpenGL::GetInstance()->SetColour4f( vColour.R, vColour.G, vColour.B, vColour.A );
	renderer::OpenGL::GetInstance()->DisableVBO();

	// bind the texture
	Bind( nTexUnit );

	if( eFullscreenFilter != GL_NEAREST )
	{
		glTexParameterf( eTarget, GL_TEXTURE_MIN_FILTER, (GLfloat)eFullscreenFilter );
		glTexParameterf( eTarget, GL_TEXTURE_MAG_FILTER, (GLfloat)eFullscreenFilter );
	}
	else
	{
		glTexParameterf( eTarget, GL_TEXTURE_MIN_FILTER, (GLfloat)GL_NEAREST );
		glTexParameterf( eTarget, GL_TEXTURE_MAG_FILTER, (GLfloat)GL_NEAREST );
	}

	// draw texture to full resolution
	renderer::OpenGL::GetInstance()->SetupOrthographicView( nScreenWidth, nScreenHeight );

	// work out correct uv coords
	if( eRTTMode == RTTMODE_FBO )
	{
		vUVCoords[_BL_] = math::Vec2( 0.0f, 0.0f );
		vUVCoords[_BR_] = math::Vec2( 1.0f, 0.0f );
		vUVCoords[_TL_] = math::Vec2( 0.0f, 1.0f );
		vUVCoords[_TR_] = math::Vec2( 1.0f, 1.0f );
	}
	
	glEnableClientState( GL_VERTEX_ARRAY );
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );
	glDisableClientState( GL_COLOR_ARRAY );
	glDisableClientState( GL_NORMAL_ARRAY );	

	float vertList[] = 
	{
		0.0f, 0.0f,
		static_cast<float>(nScreenWidth), 0.0f,
		0.0f, static_cast<float>(nScreenHeight),
		static_cast<float>(nScreenWidth), static_cast<float>(nScreenHeight)
	};
	
	glPushMatrix();
		glTranslatef( 0.0f, 0.0f, 0.0f );

		glVertexPointer( 2, GL_FLOAT, 0, &vertList[0] );
		glTexCoordPointer( 2, GL_FLOAT, 0, &vUVCoords[0] );
	
		glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
	
/*		glBegin( GL_QUADS );
			glTexCoord2f( vUVCoords[_BL_].fU, vUVCoords[_BL_].fV );
			glVertex3f( 0.0f, 0.0f, 0.0f );
			glTexCoord2f( vUVCoords[_BR_].fU, vUVCoords[_BR_].fV );
			glVertex3f( (float)nScreenWidth, 0.0f, 0.0f );
			glTexCoord2f( vUVCoords[_TR_].fU, vUVCoords[_TR_].fV);
			glVertex3f( (float)nScreenWidth, (float)nScreenHeight, 0.0f );
			glTexCoord2f( vUVCoords[_TL_].fU, vUVCoords[_TL_].fV );
			glVertex3f( 0.0f, (float)nScreenHeight, 0.0f);
		glEnd();
*/
	glPopMatrix();

	// unbind the texture, so it affects nothing after it
	UnBind();

	glDisableClientState( GL_TEXTURE_COORD_ARRAY );

	// reset view
	renderer::OpenGL::GetInstance()->SetViewState( &SavedView );
}

/////////////////////////////////////////////////////
/// Method: SetWidthHeight
/// Params: [in]nTexWidth, [in]nTexHeight
///
/////////////////////////////////////////////////////
void RenderToTexture::SetWidthHeight( int nTexWidth, int nTexHeight )
{
	nWidth = nTexWidth;
	nHeight = nTexHeight;
}


#endif // BASE_SUPPORT_OPENGLES

