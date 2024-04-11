
/*===================================================================
	File: RenderToTexture.cpp
	Library: RenderLib

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"

#ifdef BASE_SUPPORT_OPENGL

#include "MathBase.h"

#include "Render/RenderConsts.h"
#include "Render/OpenGLCommon.h"
#include "Render/FF/OpenGL/OpenGL.h"
#include "Render/FF/OpenGL/Extensions.h"
#include "Render/TextureShared.h"
#include "Render/Texture.h"
#include "Render/FF/OpenGL/TextureLoadAndUpload.h"

#include "Render/FF/OpenGL/RenderToTexture.h"

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
	eInternalFormat = GL_RGBA8;
	eFormat = GL_RGBA;
	eWrapMode = GL_CLAMP;

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
	eInternalFormat = GL_RGB8;
	eFormat = GL_RGB;
	eWrapMode = GL_CLAMP;

	// remove any texture data
	if( nID != INVALID_OBJECT )
	{
		renderer::RemoveTexture( nID );
		nID = INVALID_OBJECT;
	}

	if( nDepth_rb != INVALID_OBJECT )
	{
		if( glIsRenderbufferEXT(nDepth_rb) )
			glDeleteRenderbuffersEXT( 1, &nDepth_rb );
		nDepth_rb = INVALID_OBJECT;
	}

	if( nStencil_rb != INVALID_OBJECT )
	{
		if( glIsRenderbufferEXT(nStencil_rb) )
			glDeleteRenderbuffersEXT( 1, &nStencil_rb );
		nDepth_rb = INVALID_OBJECT;
	}

	if( nFramebufferID != INVALID_OBJECT )
	{
		if( glIsFramebufferEXT(nFramebufferID) )
			glDeleteFramebuffersEXT( 1, &nFramebufferID );
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
	else
	{
		return( CreateCopyTex( nTexWidth, nTexHeight, nTexInternalFormat, nTexFormat, nTexType, eTexWrapMode, bMipMap ) );
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
	else
	{
		return( CreateCopyTexDepthOnly( nTexWidth, nTexHeight ) );
	}

	return(1);
}

/////////////////////////////////////////////////////
/// Method: CreateFloat
/// Params: [in]nTexWidth, [in]nTexHeight
///
/////////////////////////////////////////////////////
int RenderToTexture::CreateFloat( int nTexWidth, int nTexHeight )
{
	if( bExtFrameBufferObject )
	{
		return( CreateFBOFloat( nTexWidth, nTexHeight ) );
	}
	else
	{
		return( CreateCopyTexFloat( nTexWidth, nTexHeight ) );
	}

	return(1);
}

/////////////////////////////////////////////////////
/// Method: CreateCopyTex
/// Params: [in]nTexWidth, [in]nTexHeight, [in]nTexInternalFormat, [in]nTexFormat, [in]nTexType, [in]eTexWrapMode, [in]bMipMap
///
/////////////////////////////////////////////////////
int RenderToTexture::CreateCopyTex( int nTexWidth, int nTexHeight, GLenum nTexInternalFormat, GLenum nTexFormat, GLenum nTexType, GLenum eTexWrapMode, bool bMipMap )
{
	GLenum activeMinFilter, activeMagFilter;
	float anisotropyLevel = 1.0f;
	bool bTextureCompress = false;

	if( renderer::OpenGL::IsInitialised() )
	{
		renderer::OpenGL::GetInstance()->GetActiveTextureFilter( &activeMinFilter, &activeMagFilter );
		anisotropyLevel = renderer::OpenGL::GetInstance()->GetAnisotropyLevel();
		bTextureCompress = renderer::OpenGL::GetInstance()->GetTextureCompressFlag();
	}

	eRTTMode = RTTMODE_SIMPLECOPY;
	eInternalFormat = nTexInternalFormat;
	eFormat = nTexFormat;
	eType = nTexType;
	nWidth = nTexWidth;
	nHeight = nTexHeight;
	eWrapMode = eTexWrapMode;
	bHasMipMaps = bMipMap;

	eTarget = GL_TEXTURE_2D;

	int nIsHeightPOW2 = renderer::IsPowerOfTwo( nTexHeight );
	int nIsWidthPOW2 = renderer::IsPowerOfTwo( nTexWidth );

	// no NPOT extension, downscale the width and height
	if( !bExtTextureNonPowerOfTwo )
	{
		if( !nIsHeightPOW2 )
			nHeight = renderer::GetPreviousPowerOfTwo( nTexHeight, renderer::OpenGL::GetInstance()->GetMaxTextureSize() );
		if( !nIsWidthPOW2 )
			nWidth = renderer::GetPreviousPowerOfTwo( nTexWidth, renderer::OpenGL::GetInstance()->GetMaxTextureSize() );

		if( !nIsHeightPOW2 || !nIsWidthPOW2 )
			DBGLOG( "RENDERTOTEXTURE: *WARNING*  RTT had NPOT size and GL_ARB_texture_non_power_of_two is not available\n" );
	}

	// no texture already created ?
	if( nID == INVALID_OBJECT )
	{
		glGenTextures( 1, &nID );
		renderer::OpenGL::GetInstance()->BindUnitTexture( 0, eTarget, nID );

		// rectangular textures have to be nearest
		if( bHasMipMaps && eTarget == GL_TEXTURE_RECTANGLE_ARB)
		{
			activeMagFilter = GL_NEAREST;
			activeMinFilter = GL_NEAREST;
		}
		else if( bHasMipMaps && bExtTextureAutomipmap )
		{
			// FIXME: keep an eye on this, can the automipmap work for textures that will be rendered to?
			glTexParameteri( eTarget, GL_GENERATE_MIPMAP_SGIS, GL_TRUE ); 
		}
		else
		{
			// fall right back to bilinear
			activeMagFilter = GL_LINEAR;
			activeMinFilter = GL_LINEAR;
		}

		// wrap mode
		glTexParameterf( eTarget, GL_TEXTURE_WRAP_S, (GLfloat)eWrapMode );
		glTexParameterf( eTarget, GL_TEXTURE_WRAP_T, (GLfloat)eWrapMode );

		// filtering
		glTexParameterf( eTarget, GL_TEXTURE_MIN_FILTER, (GLfloat)activeMinFilter );
		glTexParameterf( eTarget, GL_TEXTURE_MAG_FILTER, (GLfloat)activeMagFilter );

		// create the holding texture
		glTexImage2D( eTarget, 0, eInternalFormat, nWidth, nHeight, 0, eFormat, eType, 0 );

		GL_CHECK

		DBGLOG( "RENDERTOTEXTURE: Creating CopyTex Texture - TextureID (%d)\n", nID );

		bValidTexture = true;

		return(0);
	}

	return(1);

}

/////////////////////////////////////////////////////
/// Method: CreateCopyTexDepthOnly
/// Params: [in]nTexWidth, [in]nTexHeight
///
/////////////////////////////////////////////////////
int RenderToTexture::CreateCopyTexDepthOnly( int nTexWidth, int nTexHeight )
{
	GLenum eDepthFormat;
	GLint depth_bits;
	float anisotropyLevel = 1.0f;

	eRTTMode = RTTMODE_SIMPLECOPY;
	bHasMipMaps = 0;

	if( bExtDepthTexture )
	{
		glGetIntegerv( GL_DEPTH_BITS, &depth_bits );
		                   
		eDepthFormat = GL_DEPTH_COMPONENT;

		eInternalFormat = eDepthFormat;
	}
	else
	{
		eInternalFormat = GL_LUMINANCE8_ALPHA8;
	}

	eFormat = GL_DEPTH_COMPONENT;
	eType = GL_UNSIGNED_BYTE;
	nWidth = nTexWidth;
	nHeight = nTexHeight;

	if( bExtTextureBorderClamp ) 
	{
		eWrapMode = GL_CLAMP_TO_BORDER;
	}
	else 
	{
		// really want "clamp to border", but this may be good enough
		if( bExtTextureEdgeClamp ) 
		{
			eWrapMode = GL_CLAMP_TO_EDGE;
		}
		else 
		{
			// a bad option, but still better than "repeat". 
			eWrapMode = GL_CLAMP;
		}
	}

	eTarget = GL_TEXTURE_2D;

	int nIsHeightPOW2 = renderer::IsPowerOfTwo( nTexHeight );
	int nIsWidthPOW2 = renderer::IsPowerOfTwo( nTexWidth );

	// no NPOT extension, downscale the width and height
	if( !bExtTextureNonPowerOfTwo )
	{
		if( !nIsHeightPOW2 )
			nHeight = renderer::GetPreviousPowerOfTwo( nTexHeight, renderer::OpenGL::GetInstance()->GetMaxTextureSize() );
		if( !nIsWidthPOW2 )
			nWidth = renderer::GetPreviousPowerOfTwo( nTexWidth, renderer::OpenGL::GetInstance()->GetMaxTextureSize() );

		if( !nIsHeightPOW2 || !nIsWidthPOW2 )
			DBGLOG( "RENDERTOTEXTURE: *WARNING*  RTT had NPOT size and GL_ARB_texture_non_power_of_two is not available\n" );
	}

	GLenum eFilterMode = GL_LINEAR;

	// no texture already created ?
	if( nID == INVALID_OBJECT )
	{
		glGenTextures( 1, &nID );
		renderer::OpenGL::GetInstance()->BindUnitTexture( 0, eTarget, nID );

		// make sure it's not generating hardware mipmaps
		if( bExtTextureAutomipmap )
			glTexParameteri( eTarget, GL_GENERATE_MIPMAP_SGIS, GL_FALSE ); 

		// depth usually should clamp
		glTexParameterf( eTarget, GL_TEXTURE_WRAP_S, (GLfloat)eWrapMode );
		glTexParameterf( eTarget, GL_TEXTURE_WRAP_T, (GLfloat)eWrapMode );

		// no depth mipmapping
		glTexParameterf( eTarget, GL_TEXTURE_MIN_FILTER, (GLfloat)eFilterMode );
		glTexParameterf( eTarget, GL_TEXTURE_MAG_FILTER, (GLfloat)eFilterMode );

		if( bExtAnisotropicFiltering )
			glTexParameterf( eTarget, GL_TEXTURE_MAX_ANISOTROPY_EXT, anisotropyLevel );

		if( bExtDepthTexture )
		{
			glTexImage2D( eTarget, 0, eInternalFormat, nWidth, nHeight, 0, eFormat, eType, 0 );

			DBGLOG( "RENDERTOTEXTURE: Creating CopyTex Depth Texture - TextureID (%d)\n", nID );
		}
		else
		{
			// allocate a buffer to read the depth back
			DBG_MEMTRY
				depthBuffer = new unsigned char[nWidth*nHeight*1];
			DBG_MEMCATCH

			DBGLOG( "RENDERTOTEXTURE: Creating ReadPixels Depth Texture - TextureID (%d)\n", nID );
		}

		GL_CHECK

		bValidTexture = true;
		bDepthOnly = true;

		return(0);
	}

	return(1);

}

/////////////////////////////////////////////////////
/// Method: CreateCopyTexFloat
/// Params: [in]nTexWidth, [in]nTexHeight
///
/////////////////////////////////////////////////////
int RenderToTexture::CreateCopyTexFloat( int nTexWidth, int nTexHeight )
{
	if( !bExtTextureFloat )
	{
		DBGLOG( "RENDERTOTEXTURE: *ERROR* Could not create FBO float texture, extension not available\n" );
		return(1);
	}

	eRTTMode = RTTMODE_SIMPLECOPY;
	eInternalFormat = GL_RGBA16F_ARB;
	eFormat = GL_RGBA;
	eType = GL_FLOAT;
	nWidth = nTexWidth;
	nHeight = nTexHeight;
	eWrapMode = GL_CLAMP;
	bHasMipMaps = false;

	eTarget = GL_TEXTURE_2D;

	int nIsHeightPOW2 = renderer::IsPowerOfTwo( nTexHeight );
	int nIsWidthPOW2 = renderer::IsPowerOfTwo( nTexWidth );

	// no NPOT extension, downscale the width and height
	if( !bExtTextureNonPowerOfTwo )
	{
		if( !nIsHeightPOW2 )
			nHeight = renderer::GetPreviousPowerOfTwo( nTexHeight, renderer::OpenGL::GetInstance()->GetMaxTextureSize() );
		if( !nIsWidthPOW2 )
			nWidth = renderer::GetPreviousPowerOfTwo( nTexWidth, renderer::OpenGL::GetInstance()->GetMaxTextureSize() );

		if( !nIsHeightPOW2 || !nIsWidthPOW2 )
			DBGLOG( "RENDERTOTEXTURE: *WARNING*  RTT had NPOT size and GL_ARB_texture_non_power_of_two is not available\n" );
	}

//	GLenum eFilterMode = (eTarget == GL_TEXTURE_RECTANGLE_ARB) ? GL_NEAREST : GL_LINEAR;

	// no texture already created ?
	if( nID == INVALID_OBJECT )
	{
		glGenTextures( 1, &nID );
		renderer::OpenGL::GetInstance()->BindUnitTexture( 0, eTarget, nID );

		// create the holding texture
		glTexImage2D( eTarget, 0, eInternalFormat, nWidth, nHeight, 0, eFormat, eType, 0 );

		// wrap mode
		glTexParameterf( eTarget, GL_TEXTURE_WRAP_S, (GLfloat)eWrapMode );
		glTexParameterf( eTarget, GL_TEXTURE_WRAP_T, (GLfloat)eWrapMode );

		// filtering
		glTexParameterf( eTarget, GL_TEXTURE_MIN_FILTER, (GLfloat)GL_NEAREST );
		glTexParameterf( eTarget, GL_TEXTURE_MAG_FILTER, (GLfloat)GL_NEAREST );

		GL_CHECK

		DBGLOG( "RENDERTOTEXTURE: Creating CopyTex Float Texture - TextureID (%d)\n", nID );

		bValidTexture = true;

		return(0);
	}

	return(1);
}

/////////////////////////////////////////////////////
/// Method: CreateFBO
/// Params: [in]nTexWidth, [in]nTexHeight, [in]nTexInternalFormat, [in]nTexFormat, [in]nTexType, [in]eTexWrapMode, [in]bMipMap
///
/////////////////////////////////////////////////////
int RenderToTexture::CreateFBO( int nTexWidth, int nTexHeight, short flags, GLenum nTexInternalFormat, GLenum nTexFormat, GLenum nTexType, GLenum eTexWrapMode, bool bMipMap )
{
	GLenum activeMinFilter, activeMagFilter;
	float anisotropyLevel = 1.0f;
	bool bTextureCompress = false;

	if( renderer::OpenGL::IsInitialised() )
	{
		renderer::OpenGL::GetInstance()->GetActiveTextureFilter( &activeMinFilter, &activeMagFilter );
		anisotropyLevel = renderer::OpenGL::GetInstance()->GetAnisotropyLevel();
		bTextureCompress = renderer::OpenGL::GetInstance()->GetTextureCompressFlag();
	}

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

	eDepthFormat = GL_DEPTH_COMPONENT;

	int nIsHeightPOW2 = renderer::IsPowerOfTwo( nTexHeight );
	int nIsWidthPOW2 = renderer::IsPowerOfTwo( nTexWidth );

	// no NPOT extension, downscale the width and height
	if( !bExtTextureNonPowerOfTwo )
	{
		if( !nIsHeightPOW2 )
			nHeight = renderer::GetPreviousPowerOfTwo( nTexHeight, renderer::OpenGL::GetInstance()->GetMaxRenderBufferSize() );
		if( !nIsWidthPOW2 )
			nWidth = renderer::GetPreviousPowerOfTwo( nTexWidth, renderer::OpenGL::GetInstance()->GetMaxRenderBufferSize() );

		if( !nIsHeightPOW2 || !nIsWidthPOW2 )
			DBGLOG( "RENDERTOTEXTURE: *WARNING*  RTT had NPOT size and GL_ARB_texture_non_power_of_two is not available\n" );
	}

	if( bExtFrameBufferObject )
	{
		// create colour buffer, depth and vanilla texture ids
		glGenFramebuffersEXT( 1, &nFramebufferID );
		glGenTextures( 1, &nID );
		glGenRenderbuffersEXT(1, &nDepth_rb );
  
		renderer::OpenGL::GetInstance()->BindFrameBufferObject( nFramebufferID );

		// init texture
		renderer::OpenGL::GetInstance()->BindUnitTexture( 0, eTarget, nID );
		glTexImage2D( eTarget, 0, eInternalFormat, nWidth, nHeight, 0, eFormat, eType, 0 );
		
		// check for mipmaps, cannot mipmap rectangular textures
		if( bHasMipMaps && (eTarget != GL_TEXTURE_RECTANGLE_ARB) )
		{
			// generate mipmaps for the target
			glGenerateMipmapEXT( eTarget );
		}
		else
		{
			// rectangular textures have to be nearest
			if(eTarget == GL_TEXTURE_RECTANGLE_ARB)
			{
				activeMagFilter = GL_NEAREST;
				activeMinFilter = GL_NEAREST;
			}
			else
			{
				// default to bilinear
				activeMagFilter = GL_LINEAR;
				activeMinFilter = GL_LINEAR;
			}
			
		}

		GL_CHECK
		
		glTexParameterf( eTarget, GL_TEXTURE_MIN_FILTER, (GLfloat)activeMinFilter );
		glTexParameterf( eTarget, GL_TEXTURE_MAG_FILTER, (GLfloat)activeMagFilter );

		glTexParameterf( eTarget, GL_TEXTURE_WRAP_S, (GLfloat)eWrapMode );
		glTexParameterf( eTarget, GL_TEXTURE_WRAP_T, (GLfloat)eWrapMode );

		//if( bExtAnisotropicFiltering )
			//glTexParameterf( eTarget, GL_TEXTURE_MAX_ANISOTROPY_EXT, anisotropyLevel );

		if( flags & renderer::RTTFLAG_CREATE_COLOURBUFFER )
			glFramebufferTexture2DEXT( GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, eTarget, nID, 0 );

		GL_CHECK

		// bind the depth render buffer and attach its ID
		if( flags & renderer::RTTFLAG_CREATE_DEPTHBUFFER )
		{
			glBindRenderbufferEXT( GL_RENDERBUFFER_EXT, nDepth_rb );
			glRenderbufferStorageEXT( GL_RENDERBUFFER_EXT, eDepthFormat, nWidth, nHeight );
			glFramebufferRenderbufferEXT( GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, nDepth_rb );
		}

		GL_CHECK

		// make sure the frame buffer isn't enabled while returning
		renderer::OpenGL::GetInstance()->ClearFrameBufferObject( );

		DBGLOG( "RENDERTOTEXTURE: Creating FBO - TextureID (%d) FrameBufferID(%d) DepthBits (%d) DepthRenderBufferID (%d)\n", nID, nFramebufferID, depth_bits, nDepth_rb );

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
	eRTTMode = RTTMODE_FBO;
	bHasMipMaps = 0;

	// depth bits
	GLenum eDepthFormat;
	GLint depth_bits;
	glGetIntegerv( GL_DEPTH_BITS, &depth_bits );

	eDepthFormat = GL_DEPTH_COMPONENT;

	eInternalFormat = eDepthFormat;
	eFormat = GL_DEPTH_COMPONENT;
	eType = GL_UNSIGNED_BYTE;
	nWidth = nTexWidth;
	nHeight = nTexHeight;

	if( bExtTextureBorderClamp ) 
	{
		eWrapMode = GL_CLAMP_TO_BORDER;
	}
	else 
	{
		// really want "clamp to border", but this may be good enough
		if( bExtTextureEdgeClamp ) 
		{
			eWrapMode = GL_CLAMP_TO_EDGE;
		}
		else 
		{
			// A bad option, but still better than "repeat". 
			eWrapMode = GL_CLAMP;
		}
	}

	eTarget = GL_TEXTURE_2D;

	// filtering mode
	GLenum eFilterMode = GL_LINEAR;

	int nIsHeightPOW2 = renderer::IsPowerOfTwo( nTexHeight );
	int nIsWidthPOW2 = renderer::IsPowerOfTwo( nTexWidth );

	// no NPOT extension, downscale the width and height
	if( !bExtTextureNonPowerOfTwo )
	{
		if( !nIsHeightPOW2 )
			nHeight = renderer::GetPreviousPowerOfTwo( nTexHeight, renderer::OpenGL::GetInstance()->GetMaxRenderBufferSize() );
		if( !nIsWidthPOW2 )
			nWidth = renderer::GetPreviousPowerOfTwo( nTexWidth, renderer::OpenGL::GetInstance()->GetMaxRenderBufferSize() );

		if( !nIsHeightPOW2 || !nIsWidthPOW2 )
			DBGLOG( "RENDERTOTEXTURE: *WARNING*  RTT had NPOT size and GL_ARB_texture_non_power_of_two is not available\n" );
	}

	if( bExtFrameBufferObject )
	{
		// create colour buffer, depth and vanilla texture ids
		glGenFramebuffersEXT( 1, &nFramebufferID );
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
        glFramebufferTexture2DEXT( GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, eTarget, nID, 0 );
		glDrawBuffer( GL_NONE );
		glReadBuffer( GL_NONE );

		GL_CHECK

		// make sure the frame buffer isn't enabled while returning
		renderer::OpenGL::GetInstance()->ClearFrameBufferObject( );
		
		DBGLOG( "RENDERTOTEXTURE: Creating FBO Depth - TextureID (%d) FrameBufferID(%d) DepthBits (%d)\n", nID, nFramebufferID, depth_bits );

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
/// Method: CreateFBO
/// Params: [in]nTexWidth, [in]nTexHeight
///
/////////////////////////////////////////////////////
int RenderToTexture::CreateFBOFloat( int nTexWidth, int nTexHeight )
{
	if( !bExtTextureFloat )
	{
		DBGLOG( "RENDERTOTEXTURE: *ERROR* Could not create copytex float texture, extension not available\n" );
		return(1);
	}

	eRTTMode = RTTMODE_FBO;
	bHasMipMaps = false;
	eInternalFormat = GL_RGBA16F_ARB;
	eFormat = GL_RGBA;
	eType = GL_FLOAT;
	nWidth = nTexWidth;
	nHeight = nTexHeight;
	eWrapMode = GL_CLAMP;
	bHasMipMaps = false;

	eTarget = GL_TEXTURE_2D;

	// depth bits
	GLenum eDepthFormat;
	GLint depth_bits;
	glGetIntegerv( GL_DEPTH_BITS, (GLint *)&depth_bits );
	                         
	eDepthFormat = GL_DEPTH_COMPONENT;

	int nIsHeightPOW2 = renderer::IsPowerOfTwo( nTexHeight );
	int nIsWidthPOW2 = renderer::IsPowerOfTwo( nTexWidth );

	// no NPOT extension, downscale the width and height
	if( !bExtTextureNonPowerOfTwo )
	{
		if( !nIsHeightPOW2 )
			nHeight = renderer::GetPreviousPowerOfTwo( nTexHeight, renderer::OpenGL::GetInstance()->GetMaxRenderBufferSize() );
		if( !nIsWidthPOW2 )
			nWidth = renderer::GetPreviousPowerOfTwo( nTexWidth, renderer::OpenGL::GetInstance()->GetMaxRenderBufferSize() );

		if( !nIsHeightPOW2 || !nIsWidthPOW2 )
			DBGLOG( "RENDERTOTEXTURE: *WARNING*  RTT had NPOT size and GL_ARB_texture_non_power_of_two is not available\n" );
	}

	if( bExtFrameBufferObject )
	{
		// create colour buffer, depth and vanilla texture ids
		glGenFramebuffersEXT( 1, &nFramebufferID );
		glGenTextures( 1, &nID );
		glGenRenderbuffersEXT(1, &nDepth_rb );
  
		renderer::OpenGL::GetInstance()->BindFrameBufferObject( nFramebufferID );

		// init texture
		renderer::OpenGL::GetInstance()->BindUnitTexture( 0, eTarget, nID );
		glTexImage2D( eTarget, 0, eInternalFormat, nWidth, nHeight, 0, eFormat, eType, 0 );

		GL_CHECK
		
		glTexParameterf( eTarget, GL_TEXTURE_MIN_FILTER, (GLfloat)GL_NEAREST );
		glTexParameterf( eTarget, GL_TEXTURE_MAG_FILTER, (GLfloat)GL_NEAREST );

		glTexParameterf( eTarget, GL_TEXTURE_WRAP_S, (GLfloat)eWrapMode );
		glTexParameterf( eTarget, GL_TEXTURE_WRAP_T, (GLfloat)eWrapMode );

		//if( bExtAnisotropicFiltering )
			//glTexParameterf( eTarget, GL_TEXTURE_MAX_ANISOTROPY_EXT, anisotropyLevel );

		glFramebufferTexture2DEXT( GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, eTarget, nID, 0 );

		GL_CHECK

		// bind the depth render buffer and attach its ID
		glBindRenderbufferEXT( GL_RENDERBUFFER_EXT, nDepth_rb );
		glRenderbufferStorageEXT( GL_RENDERBUFFER_EXT, eDepthFormat, nWidth, nHeight );
		glFramebufferRenderbufferEXT( GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, nDepth_rb );

		GL_CHECK

		// make sure the frame buffer isn't enabled while returning
		renderer::OpenGL::GetInstance()->ClearFrameBufferObject( );

		DBGLOG( "RENDERTOTEXTURE: Creating FBO Float - TextureID (%d) FrameBufferID(%d) DepthBits (%d) DepthRenderBufferID (%d)\n", nID, nFramebufferID, depth_bits, nDepth_rb );

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

	if( eRTTMode == RTTMODE_SIMPLECOPY )
	{
		// valid texture ?
		if( nID != INVALID_OBJECT )
		{
			// render it to the texture
			renderer::OpenGL::GetInstance()->BindUnitTexture( 0, eTarget, nID );

			// don't have hardware depth textures, then do a read of the depth into the allocated buffer
			if( bDepthOnly && !bExtDepthTexture )
			{
				if( depthBuffer )
				{
					// read the depth into a buffer
					glReadPixels( 0, 0, nWidth, nHeight, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, depthBuffer );

					// copy the texture
					if( !bInitialised )
					{
						glTexImage2D( eTarget, 0, eInternalFormat, nWidth, nHeight, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, depthBuffer );
						bInitialised = true;
					}
					else
						glTexSubImage2D( eTarget, 0, 0, 0, nWidth, nHeight, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, depthBuffer );
				}
			}
			else
			{
				// do a normal copy
				if( !bInitialised )
				{
					glCopyTexImage2D( eTarget, 0, eInternalFormat, 0, 0, nWidth, nHeight, 0 );
					bInitialised = true;
				}
				else
					glCopyTexSubImage2D( eTarget, 0, 0, 0, 0, 0, nWidth, nHeight );
			}
			
			glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		}
	}

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
			if( bHasMipMaps && (eTarget != GL_TEXTURE_RECTANGLE_ARB) )
			{
				// generate mipmaps for the target
				glGenerateMipmapEXT( eTarget );
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

void RenderToTexture::DoCopy()
{
	if( eRTTMode == RTTMODE_SIMPLECOPY )
	{
		// valid texture ?
		if( nID != INVALID_OBJECT )
		{
			// render it to the texture
			renderer::OpenGL::GetInstance()->BindUnitTexture( 0, eTarget, nID );

			// don't have hardware depth textures, then do a read of the depth into the allocated buffer
			if( bDepthOnly && !bExtDepthTexture )
			{
				if( depthBuffer )
				{
					// read the depth into a buffer
					glReadPixels( 0, 0, nWidth, nHeight, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, depthBuffer );

					// copy the texture
					if( !bInitialised )
					{
						glTexImage2D( eTarget, 0, eInternalFormat, nWidth, nHeight, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, depthBuffer );
						bInitialised = true;
					}
					else
						glTexSubImage2D( eTarget, 0, 0, 0, nWidth, nHeight, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, depthBuffer );
				}
			}
			else
			{
				// do a normal copy
				if( !bInitialised )
				{
					glCopyTexImage2D( eTarget, 0, eInternalFormat, 0, 0, nWidth, nHeight, 0 );
					bInitialised = true;
				}
				else
					glCopyTexSubImage2D( eTarget, 0, 0, 0, 0, 0, nWidth, nHeight );
			}
			
			glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
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
	renderer::OpenGL::GetInstance()->SetupOrthographicView( core::app::GetOrientationWidth(), core::app::GetOrientationHeight(), true );

	// work out correct uv coords
	if( eRTTMode == RTTMODE_FBO )
	{
		if( eTarget == GL_TEXTURE_RECTANGLE_ARB )
		{
			vUVCoords[_BL_] = math::Vec2( 0.0f, 0.0f );
			vUVCoords[_BR_] = math::Vec2( (float)nScreenWidth, 0.0f );
			vUVCoords[_TR_] = math::Vec2( (float)nScreenWidth, (float)nScreenHeight );
			vUVCoords[_TL_] = math::Vec2( 0.0f, (float)nScreenHeight );
		}
		else
		{
			vUVCoords[_BL_] = math::Vec2( 0.0f, 0.0f );
			vUVCoords[_BR_] = math::Vec2( 1.0f, 0.0f );
			vUVCoords[_TR_] = math::Vec2( 1.0f, 1.0f );
			vUVCoords[_TL_] = math::Vec2( 0.0f, 1.0f );
		}
	}
	else
	{
		// in copytex mode, the texture may of been badly clipped
		if( eTarget == GL_TEXTURE_RECTANGLE_ARB )
		{
			vUVCoords[_BL_] = math::Vec2( 0.0f, 0.0f );
			vUVCoords[_BR_] = math::Vec2( (float)nScreenWidth, 0.0f );
			vUVCoords[_TR_] = math::Vec2( (float)nScreenWidth, (float)nScreenHeight );
			vUVCoords[_TL_] = math::Vec2( 0.0f, (float)nScreenHeight );
		}
		else
		{
			vUVCoords[_BL_] = math::Vec2( 0.0f, 0.0f );
			vUVCoords[_BR_] = math::Vec2( 1.0f, 0.0f );
			vUVCoords[_TR_] = math::Vec2( 1.0f, 1.0f );
			vUVCoords[_TL_] = math::Vec2( 0.0f, 1.0f );
		}
	}

	glPushMatrix();
		glTranslatef( 0.0f, 0.0f, 0.0f );

		glBegin( GL_QUADS );
			glTexCoord2f( vUVCoords[_BL_].U, vUVCoords[_BL_].V );
			glVertex3f( 0.0f, 0.0f, 0.0f );
			glTexCoord2f( vUVCoords[_BR_].U, vUVCoords[_BR_].V );
			glVertex3f( (float)nScreenWidth, 0.0f, 0.0f );
			glTexCoord2f( vUVCoords[_TR_].U, vUVCoords[_TR_].V);
			glVertex3f( (float)nScreenWidth, (float)nScreenHeight, 0.0f );
			glTexCoord2f( vUVCoords[_TL_].U, vUVCoords[_TL_].V );
			glVertex3f( 0.0f, (float)nScreenHeight, 0.0f);
		glEnd();

	glPopMatrix();

	// unbind the texture, so it affects nothing after it
	UnBind();

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

#endif // BASE_SUPPORT_OPENGL

