
#ifndef __EXTENSIONSOES_H__
#define __EXTENSIONSOES_H__

#ifndef __PLATFORMRENDERBASE_H__
	#include "PlatformRenderBase.h"
#endif // __PLATFORMRENDERBASE_H__

#ifdef BASE_SUPPORT_OPENGLES

namespace renderer
{
	// forward declare
	class OpenGL;

	extern bool bExtTextureNonPowerOfTwo;
	extern bool bExtTextureCompression;
	extern bool bExtTextureCompressionPVRtc;
	extern bool bExtAnisotropicFiltering;
	extern bool bExtFrameBufferObject;
	extern bool bExtTextureLodBias;
	extern bool bExtPointSprite;
	extern bool bExtSpriteSizeArray;
    extern bool bExtDiscardFramebuffer;
    extern bool bExtVertexBufferObjects;
	extern bool bExtMappedBuffer;

	/// CheckFramebufferStatus - Checks the current state of the active framebuffer, and returns if it is valid
	/// \return integer - (SUCCESS: OK, FAIL: FAIL )
	int CheckFramebufferStatus( void );

	// extension flags
    
	const unsigned long ALL_EXTENSIONS			= -1;

	/// isExtensionSupported - Checks to see if an extension name is found in the current GL driver
	/// \param extName - full extension name
	/// \return boolean - true or false
	bool isExtensionSupported(const char *extName);

	/// RequestExtensions - requests certain extensions via flag
	/// \param nExtRequests - extension flags
	void RequestExtensions( unsigned int nExtRequests, renderer::OpenGL* openGLContext=0 );

} // namespace renderer

// techincally not the same but the feature set is near the same
// drop the OES so OpenGL and OpenGL ES can function in the same code base
#define GL_POINT_SPRITE								GL_POINT_SPRITE_OES
#define GL_COORD_REPLACE							GL_COORD_REPLACE_OES

// GL_OES_mapbuffer
#define GL_WRITE_ONLY								GL_WRITE_ONLY_OES                                       
#define GL_BUFFER_ACCESS							GL_BUFFER_ACCESS_OES                                    
#define GL_BUFFER_MAPPED							GL_BUFFER_MAPPED_OES                                    
#define GL_BUFFER_MAP_POINTER						GL_BUFFER_MAP_POINTER_OES                               

#define glGetBufferPointerv							glGetBufferPointervOES
#define glMapBuffer									glMapBufferOES
#define glUnmapBuffer								glUnmapBufferOES

// GL_OES_framebuffer_object
#define GL_FRAMEBUFFER_INCOMPLETE_FORMATS			GL_FRAMEBUFFER_INCOMPLETE_FORMATS_OES

#define glIsRenderbuffer							glIsRenderbufferOES
#define glBindRenderbuffer							glBindRenderbufferOES
#define glDeleteRenderbuffers						glDeleteRenderbuffersOES
#define glGenRenderbuffers							glGenRenderbuffersOES
#define glRenderbufferStorage						glRenderbufferStorageOES
#define glGetRenderbufferParameteriv				glGetRenderbufferParameterivOES
#define glIsFramebuffer								glIsFramebufferOES
#define glBindFramebuffer							glBindFramebufferOES
#define glDeleteFramebuffers						glDeleteFramebuffersOES
#define glGenFramebuffers							glGenFramebuffersOES
#define glCheckFramebufferStatus					glCheckFramebufferStatusOES
#define glFramebufferRenderbuffer					glFramebufferRenderbufferOES
#define glFramebufferTexture2D						glFramebufferTexture2DOES
#define glGetFramebufferAttachmentParameteriv		glGetFramebufferAttachmentParameterivOES
#define glGenerateMipmap							glGenerateMipmapOES

// GL_OES_depth24
#define GL_DEPTH_COMPONENT24						GL_DEPTH_COMPONENT24_OES

// GL_OES_depth32
#define GL_DEPTH_COMPONENT32						GL_DEPTH_COMPONENT32_OES

// extension functions
#ifdef BASE_OES_EXTENSIONS_CHANGE //defined(BASE_PLATFORM_ANDROID) || defined(BASE_PLATFORM_RASPBERRYPI) // stuff for android/rpi
	// lod bias
	#define GL_MAX_TEXTURE_LOD_BIAS_EXT					0x84FD
	#define GL_TEXTURE_FILTER_CONTROL_EXT				0x8500
	#define GL_TEXTURE_LOD_BIAS_EXT						0x8501

	// pvr support
	#define GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG                      0x8C00
	#define GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG                      0x8C01
	#define GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG                     0x8C02
	#define GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG                     0x8C03

	#define GL_FRAMEBUFFER										GL_FRAMEBUFFER_OES
	#define GL_RENDERBUFFER										GL_RENDERBUFFER_OES
	#define GL_RGBA4											GL_RGBA4_OES
	#define GL_RGB5_A1											GL_RGB5_A1_OES
	#define GL_RGB565											GL_RGB565_OES
	#define GL_DEPTH_COMPONENT16								GL_DEPTH_COMPONENT16_OES
	#define GL_RENDERBUFFER_WIDTH								GL_RENDERBUFFER_WIDTH_OES
	#define GL_RENDERBUFFER_HEIGHT								GL_RENDERBUFFER_HEIGHT_OES
	#define GL_RENDERBUFFER_INTERNAL_FORMAT						GL_RENDERBUFFER_INTERNAL_FORMAT_OES
	#define GL_RENDERBUFFER_RED_SIZE							GL_RENDERBUFFER_RED_SIZE_OES
	#define GL_RENDERBUFFER_GREEN_SIZE							GL_RENDERBUFFER_GREEN_SIZE_OES
	#define GL_RENDERBUFFER_BLUE								GL_RENDERBUFFER_BLUE_SIZE_OES
	#define GL_RENDERBUFFER_ALPHA_SIZE							GL_RENDERBUFFER_ALPHA_SIZE_OES
	#define GL_RENDERBUFFER_DEPTH_SIZE							GL_RENDERBUFFER_DEPTH_SIZE_OES
	#define GL_RENDERBUFFER_STENCIL_SIZE						GL_RENDERBUFFER_STENCIL_SIZE_OES
	#define GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE				GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE_OES
	#define GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME				GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME_OES
	#define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL				GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL_OES
	#define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE		GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE_OES
	#define GL_COLOR_ATTACHMENT0								GL_COLOR_ATTACHMENT0_OES
	#define GL_DEPTH_ATTACHMENT									GL_DEPTH_ATTACHMENT_OES
	#define GL_STENCIL_ATTACHMENT								GL_STENCIL_ATTACHMENT_OES
	#define GL_FRAMEBUFFER_COMPLETE								GL_FRAMEBUFFER_COMPLETE_OES
	#define GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT				GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_OES
	#define GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT		GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_OES
	#define GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS				GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_OES
    #define GL_FRAMEBUFFER_INCOMPLETE_FORMATS                   GL_FRAMEBUFFER_INCOMPLETE_FORMATS_OES
	#define GL_FRAMEBUFFER_UNSUPPORTED							GL_FRAMEBUFFER_UNSUPPORTED_OES
	#define GL_FRAMEBUFFER_BINDING								GL_FRAMEBUFFER_BINDING_OES
	#define GL_RENDERBUFFER_BINDING								GL_RENDERBUFFER_BINDING_OES
	#define GL_MAX_RENDERBUFFER_SIZE							GL_MAX_RENDERBUFFER_SIZE_OES
	#define GL_INVALID_FRAMEBUFFER_OPERATION					GL_INVALID_FRAMEBUFFER_OPERATION_OES
#endif // BASE_OES_EXTENSIONS_CHANGE defined(BASE_PLATFORM_ANDROID) || defined(BASE_PLATFORM_RASPBERRYPI)

#endif // BASE_SUPPORT_OPENGLES

#endif // __EXTENSIONSOES_H__


