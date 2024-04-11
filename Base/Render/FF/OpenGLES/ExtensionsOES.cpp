
/*===================================================================
	File: ExtensionsOES.cpp
	Library: Render

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"

#include "Math/Vectors.h"

#include "Render/OpenGLCommon.h"
#include "Render/FF/OpenGLES/OpenGLES.h"
#include "Render/FF/OpenGLES/ExtensionsOES.h"

#ifdef BASE_SUPPORT_OPENGLES

namespace renderer
{
	bool bExtTextureNonPowerOfTwo		= 0;
	bool bExtTextureCompression			= 0;
	bool bExtTextureCompressionPVRtc	= 0;
	bool bExtAnisotropicFiltering		= 0;
	bool bExtFrameBufferObject			= 0;
	bool bExtTextureLodBias				= 0;
	bool bExtPointSprite				= 0;
	bool bExtSpriteSizeArray			= 0;
	bool bExtDiscardFramebuffer         = 0;
    bool bExtVertexBufferObjects		= 0;
	bool bExtMappedBuffer				= 0;

} // namespace renderer

/////////////////////////////////////////////////////
/// Function: isExtensionSupported
/// Params: [in]extName
///
/////////////////////////////////////////////////////
bool renderer::isExtensionSupported(const char *extName)
{
//	char ext[4096];
//	std::sprintf( ext, "%s", glGetString(GL_EXTENSIONS) );
//	return (0 != std::strstr(ext, extName));

	if( core::IsEmptyString( extName ) )
		return 0;

	static const GLubyte *extensions = 0;
	const GLubyte *start = 0;
	GLubyte *where = 0, *terminator = 0;

	// Extension names should not have spaces
	where = (GLubyte *) strchr(extName, ' ');
	if (where || *extName == '\0')
		return(false);

	if (!extensions) 
	{
		extensions = glGetString(GL_EXTENSIONS);
	}
	// It takes a bit of care to be fool-proof about parsing the
	// OpenGL extensions string.  Don't be fooled by sub-strings,
	// etc. 
	start = extensions;
	for (;;) 
	{
		where = (GLubyte *) std::strstr((const char *) start, extName);
		if (!where)
			break;
		terminator = where + std::strlen(extName);
		if (where == start || *(where - 1) == ' ') 
		{
			if (*terminator == ' ' || *terminator == '\0') 
			{
				return(true);
			}
		}
		start = terminator;
	}

	return(false);
}

/////////////////////////////////////////////////////
/// Function: RequestExtensions
/// Params: [in]nExtRequests
///
/////////////////////////////////////////////////////
void renderer::RequestExtensions( unsigned int nExtRequests, renderer::OpenGL* openGLContext )
{
	{
		bExtTextureNonPowerOfTwo = isExtensionSupported( "GL_APPLE_texture_2D_limited_npot" );
	
		if( bExtTextureNonPowerOfTwo )
			DBGLOG( "EXTENSIONS OES:  GL_APPLE_texture_2D_limited_npot is supported\n" );
		else
			DBGLOG( "EXTENSIONS OES:  GL_APPLE_texture_2D_limited_npot requested but not supported\n" );
	}
	
	//if( nExtRequests & EXT_TEXTURE_COMPRESSION )
	{		
		// PVR
		bExtTextureCompression = bExtTextureCompressionPVRtc = isExtensionSupported( "GL_IMG_texture_compression_pvrtc" );

		if( bExtTextureCompressionPVRtc )
			DBGLOG( "EXTENSIONS OES:  GL_IMG_texture_compression_pvrtc is supported\n" );
		else
			DBGLOG( "EXTENSIONS OES:  GL_IMG_texture_compression_pvrtc requested but not supported\n" );
	}

	//if( nExtRequests & EXT_ANISOTROPIC_FILTER )
	{
		// anisotropic filtering
		bExtAnisotropicFiltering = ( isExtensionSupported( "GL_EXT_texture_filter_anisotropic" ) );

		if( bExtAnisotropicFiltering )
		{
			DBGLOG( "EXTENSIONS OES:  GL_EXT_texture_filter_anisotropic is supported\n" );
			GLint maxAnisotropyLevel = 0;
			glGetIntegerv( GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, (GLint *)&maxAnisotropyLevel );

			if( openGLContext != 0 )
				openGLContext->SetMaxAnisotropyLevel( (float)maxAnisotropyLevel );
			else
				renderer::OpenGL::GetInstance()->SetMaxAnisotropyLevel( (float)maxAnisotropyLevel );

			DBGLOG( "GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT: %d\n", maxAnisotropyLevel );
		}
		else
			DBGLOG( "EXTENSIONS OES:  GL_EXT_texture_filter_anisotropic requested but not supported\n" );

	}

	//if( nExtRequests & EXT_FRAME_BUFFER_OBJECT )
	{
		// Frame buffer objects
		bExtFrameBufferObject = isExtensionSupported( "GL_OES_framebuffer_object" );

		if( bExtFrameBufferObject )
		{
			DBGLOG( "EXTENSIONS OES:  GL_OES_framebuffer_object is supported \n" );

			GLint maxRenderBufferSize = 0;
			glGetIntegerv( GL_MAX_RENDERBUFFER_SIZE, (GLint *)&maxRenderBufferSize );
			
			if( openGLContext != 0 )
				openGLContext->SetMaxRenderBufferSize(maxRenderBufferSize);
			else
				renderer::OpenGL::GetInstance()->SetMaxRenderBufferSize(maxRenderBufferSize);

			DBGLOG( "GL_MAX_RENDERBUFFER_SIZE_EXT: %dx%d\n", maxRenderBufferSize, maxRenderBufferSize );
		}
		else
			DBGLOG( "EXTENSIONS OES:  GL_OES_framebuffer_object requested but not supported\n" );
	}
	
	//if( nExtRequests & EXT_TEXTURE_LOD_BIAS )
	{
		// Frame buffer objects
		bExtTextureLodBias = isExtensionSupported( "GL_EXT_texture_lod_bias" );
		
		if( bExtTextureLodBias )
		{
			DBGLOG( "EXTENSIONS OES:  GL_EXT_texture_lod_bias is supported \n" );
		}
		else
			DBGLOG( "EXTENSIONS OES:  GL_EXT_texture_lod_bias requested but not supported\n" );
	}
	
	//if( nExtRequests & EXT_POINT_SPRITE )
	{
		// Frame buffer objects
		bExtPointSprite = isExtensionSupported( "GL_OES_point_sprite" );
		
		if( bExtPointSprite )
		{
			DBGLOG( "EXTENSIONS OES:  GL_OES_point_sprite is supported \n" );
		}
		else
			DBGLOG( "EXTENSIONS OES:  GL_OES_point_sprite requested but not supported\n" );
	}		
	
	//if( nExtRequests & EXT_POINT_SIZE_ARRAY )
	{
		// Frame buffer objects
		bExtSpriteSizeArray = isExtensionSupported( "GL_OES_point_size_array" );
		
		if( bExtSpriteSizeArray )
		{
			DBGLOG( "EXTENSIONS OES:  GL_OES_point_size_array is supported \n" );
		}
		else
			DBGLOG( "EXTENSIONS OES:  GL_OES_point_size_array requested but not supported\n" );
	}		
    
	//if( nExtRequests & EXT_DISCARD_FRAMEBUFFER )
	{
		// Frame buffer objects
		bExtDiscardFramebuffer = isExtensionSupported( "GL_EXT_discard_framebuffer" );
		
		if( bExtDiscardFramebuffer )
		{
			DBGLOG( "EXTENSIONS OES:  GL_EXT_discard_framebuffer is supported \n" );
		}
		else
			DBGLOG( "EXTENSIONS OES:  GL_EXT_discard_framebuffer requested but not supported\n" );
	}    
	
	bExtVertexBufferObjects = true;

	{
		bExtMappedBuffer  = isExtensionSupported( "GL_OES_mapbuffer" );

		if( bExtMappedBuffer )
		{
			DBGLOG( "EXTENSIONS OES:  GL_OES_mapbuffer is supported \n" );
		}
		else
			DBGLOG( "EXTENSIONS OES:  GL_OES_mapbuffer requested but not supported\n" );
	}

	GLint nMaxTextureUnits = 1;
	glGetIntegerv( GL_MAX_TEXTURE_UNITS, &nMaxTextureUnits );
	DBGLOG( "GL_MAX_TEXTURE_UNITS: %d\n", nMaxTextureUnits );
	
	if( openGLContext != 0 )
		openGLContext->SetMaxTextureUnits( nMaxTextureUnits );
	else
		renderer::OpenGL::GetInstance()->SetMaxTextureUnits( nMaxTextureUnits );	
}

/////////////////////////////////////////////////////
/// Function: CheckFramebufferStatus
/// Params: None
///
/////////////////////////////////////////////////////
int renderer::CheckFramebufferStatus( void )
{
	GLenum eStatus;
	eStatus = (GLenum)glCheckFramebufferStatus(GL_FRAMEBUFFER);
	switch(eStatus) 
	{
		case GL_FRAMEBUFFER_COMPLETE:
			DBGLOG( "Framebuffer status: GL_FRAMEBUFFER_COMPLETE\n" );
			return(0);
		break;
		case GL_FRAMEBUFFER_UNSUPPORTED:
			DBGLOG( "Unsupported framebuffer format\n");
			return(1);
		break;
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
			DBGLOG( "Framebuffer incompleteattachment\n");
			return(1);
		break;
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
			DBGLOG( "Framebuffer incomplete, missing attachment\n");
			return(1);
		break;
		case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS:
			DBGLOG( "Framebuffer incomplete, attached images must have same dimensions\n");
			return(1);
		break;
		case GL_FRAMEBUFFER_INCOMPLETE_FORMATS:
			DBGLOG( "Framebuffer incomplete, attached images must have same format\n");
			return(1);
		break;
        //case GL_FRAMEBUFFER_UNDEFINED_OES:
        //    DBGLOG( "Framebuffer incomplete, attached images must have same format\n");
        //    break;

		default:
			DBG_ASSERT(0);
            
	}

	return(1);
}

#endif // BASE_SUPPORT_OPENGLES


