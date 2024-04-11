
/*===================================================================
 File: glewES.cpp
 Library: Render
 
 (C)Hidden Games
 =====================================================================*/

#ifdef BASE_SUPPORT_OPENGL_GLSL

#include "CoreBase.h"

#include "Math/Vectors.h"

#include "Render/RenderConsts.h"
#include "Render/OpenGLCommon.h"
#include "Render/GLSL/OpenGLSL.h"

#include "Render/GLSL/glewES.h"

namespace  {
    static const GLubyte *extensions = 0;
}

// all platforms
GLboolean GLEW_OES_texture_npot					= GL_FALSE;
GLboolean GLEW_IMG_texture_compression_pvrtc	= GL_FALSE;
GLboolean GLEW_OES_mapbuffer					= GL_FALSE;
GLboolean GLEW_OES_rgb8_rgba8					= GL_FALSE;
GLboolean GLEW_OES_depth24						= GL_FALSE;
GLboolean GLEW_OES_depth32						= GL_FALSE;
GLboolean GLEW_OES_depth_texture				= GL_FALSE;

// windows only
#ifdef BASE_PLATFORM_WINDOWS

#endif // BASE_PLATFORM_WINDOWS

// iOS and Android only
#if defined(BASE_PLATFORM_iOS) || defined(BASE_PLATFORM_ANDROID) || defined(BASE_PLATFORM_RASPBERRYPI) || defined(BASE_PLATFORM_MAC)

GLboolean GLEW_EXT_texture_filter_anisotropic = GL_FALSE;
GLboolean GLEW_ARB_texture_non_power_of_two = GL_FALSE;

/////////////////////////////////////////////////////
/// Method: glewInit
/// Params: None
///
/////////////////////////////////////////////////////
void glewInit()
{
	// does nothing on iOS/Android
    extensions = 0;
}

#endif // defined(BASE_PLATFORM_iOS) || defined(BASE_PLATFORM_ANDROID) || defined(BASE_PLATFORM_RASPBERRYPI) ||defined(BASE_PLATFORM_MAC)

/////////////////////////////////////////////////////
/// Function: isExtensionSupported
/// Params: [in]extName
///
/////////////////////////////////////////////////////
bool isExtensionSupported(const char *extName)
{
	if( core::IsEmptyString( extName ) )
		return 0;

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
/// Method: glewESInit
/// Params: None
///
/////////////////////////////////////////////////////
void glewESInit()
{
	GLEW_OES_texture_npot = isExtensionSupported( "GL_OES_texture_npot" );

	GLEW_IMG_texture_compression_pvrtc = isExtensionSupported( "GL_IMG_texture_compression_pvrtc" );

	// set this to true so that PC uses maps as it was part of the VBO spec, but later here on iOS and Android the flag
	// is set via the GL_OES_mapbuffer extension
	GLEW_OES_mapbuffer = true;

	// this is used by FBOs to render at 8 bits per pixel - on PC this is fine, for iOS/Android it's an extension
	GLEW_OES_rgb8_rgba8 = true;

	GLEW_OES_depth24	= true;
	GLEW_OES_depth32	= true;

	GLEW_OES_depth_texture = true;

#if defined(BASE_PLATFORM_iOS) || defined(BASE_PLATFORM_ANDROID) || defined(BASE_PLATFORM_RASPBERRYPI)
	GLEW_EXT_texture_filter_anisotropic = isExtensionSupported( "GL_EXT_texture_filter_anisotropic" );
	GLEW_ARB_texture_non_power_of_two = isExtensionSupported( "GL_OES_texture_npot" );

	GLEW_OES_mapbuffer = isExtensionSupported( "GL_OES_mapbuffer" ); 

	GLEW_OES_rgb8_rgba8 = isExtensionSupported( "GL_OES_rgb8_rgba8" ); 

	GLEW_OES_depth24 = isExtensionSupported( "GL_OES_depth24" );

	GLEW_OES_depth32 = isExtensionSupported( "GL_OES_depth32" );

	GLEW_OES_depth_texture = isExtensionSupported( "GL_OES_depth_texture" );
#endif // defined(BASE_PLATFORM_iOS) || defined(BASE_PLATFORM_ANDROID) || defined(BASE_PLATFORM_RASPBERRYPI)
	
#if defined(BASE_PLATFORM_MAC)
	GLEW_EXT_texture_filter_anisotropic = isExtensionSupported( "GL_EXT_texture_filter_anisotropic" );
#endif // BASE_PLATFORM_MAC
}

#endif // BASE_SUPPORT_OPENGL_GLSL
