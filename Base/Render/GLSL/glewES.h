
#ifndef __GLEWES_H__
#define __GLEWES_H__

#ifdef BASE_SUPPORT_OPENGL_GLSL

extern GLboolean GLEW_OES_texture_npot;
extern GLboolean GLEW_IMG_texture_compression_pvrtc;
extern GLboolean GLEW_OES_mapbuffer;

extern GLboolean GLEW_OES_rgb8_rgba8;

extern GLboolean GLEW_OES_depth24;
extern GLboolean GLEW_OES_depth32;

extern GLboolean GLEW_OES_depth_texture;

#undef GL_CONSTANT_ATTENUATION
	#define GL_CONSTANT_ATTENUATION									0
#undef GL_LINEAR_ATTENUATION
	#define GL_LINEAR_ATTENUATION									1
#undef GL_QUADRATIC_ATTENUATION
	#define GL_QUADRATIC_ATTENUATION								2

#if defined(BASE_PLATFORM_WINDOWS) || defined(BASE_PLATFORM_MAC)
	// GL_IMG_texture_compression_pvrtc
	#define GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG                      0x8C00
	#define GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG                      0x8C01
	#define GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG                     0x8C02
	#define GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG                     0x8C03

	#define GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS					0x8CD9
#endif // defined(BASE_PLATFORM_WINDOWS) || defined(BASE_PLATFORM_MAC)

#if defined(BASE_PLATFORM_iOS) || defined(BASE_PLATFORM_ANDROID) || defined(BASE_PLATFORM_RASPBERRYPI)
	extern GLboolean GLEW_EXT_texture_filter_anisotropic;
	extern GLboolean GLEW_ARB_texture_non_power_of_two;

	// GL_OES_framebuffer_object
	#define GL_RGB8						GL_RGB8_OES
	//#define GL_RGBA4					GL_RGBA4_OES
	//#define GL_RGB5_A1				GL_RGB5_A1_OES
	#define GL_RGBA8					GL_RGBA8_OES
	//#define GL_DEPTH_COMPONENT16		GL_DEPTH_COMPONENT16_OES
	#define GL_DEPTH_COMPONENT24		GL_DEPTH_COMPONENT24_OES
	#define GL_DEPTH_COMPONENT32		GL_DEPTH_COMPONENT32_OES
	//#define GL_DEPTH24_STENCIL8		GL_DEPTH24_STENCIL8_OES
	#define GL_DRAW_FRAMEBUFFER			GL_FRAMEBUFFER

	// GL_OES_mapbuffer
	#define GL_WRITE_ONLY			GL_WRITE_ONLY_OES
	#define GL_BUFFER_ACCESS		GL_BUFFER_ACCESS_OES
	#define GL_BUFFER_MAPPED		GL_BUFFER_MAPPED_OES
	#define GL_BUFFER_MAP_POINTER	GL_BUFFER_MAP_POINTER_OES
	
	#define glMapBuffer				glMapBufferOES
	#define glUnmapBuffer			glUnmapBufferOES
	#define glGetBufferPointerv		glGetBufferPointervOES

	// stuff to remove in GLSL
	#define glPushMatrix()			
	#define glPopMatrix()	
	#define glLoadIdentity()
	#define glVertexPointer
	#define glTexCoordPointer
	#define glColorPointer
	#define glMatrixMode
	#define glMultMatrixf
	#define GL_MODELVIEW	1

	// stuff to switch
	#define glTranslatef//( x, y, z )	glm::translate	
	#define glRotatef//( x, y, z )		glm::rotate	
	#define glScalef//( x, y, z )		glm::scale	

	/// glewInit - the glew library is not used on ES devices, this is here to give a dummy call
	void glewInit();	
	
#endif // defined(BASE_PLATFORM_iOS) || defined(BASE_PLATFORM_ANDROID) || defined(BASE_PLATFORM_RASPBERRYPI)

#if defined(BASE_PLATFORM_MAC)
	extern GLboolean GLEW_EXT_texture_filter_anisotropic;
	extern GLboolean GLEW_ARB_texture_non_power_of_two;

	/// glewInit - the glew library is not used on Mac, this is here to give a dummy call
	void glewInit();
#endif // BASE_PLATFORM_MAC

/// isExtensionSupported - check if an extension is supported
/// /param extName - which extension
bool isExtensionSupported(const char *extName);

/// glewESInit - this will try and match up various extension functions and defines across normal GL and GL-ES devices
void glewESInit();

#endif // BASE_SUPPORT_OPENGL_GLSL

#endif // __GLEWES_H__

