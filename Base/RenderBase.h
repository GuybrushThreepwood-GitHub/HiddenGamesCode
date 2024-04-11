
#ifndef __RENDERBASE_H__
#define __RENDERBASE_H__

#ifndef __DEBUGBASE_H__
	#include "DebugBase.h"
#endif // __DEBUGBASE_H__

#ifndef __PLATFORMRENDERBASE_H__
	#include "Render/PlatformRenderBase.h"
#endif // __PLATFORMRENDERBASE_H__

#ifndef __RENDERCONSTS_H__
	#include "Render/RenderConsts.h"
#endif // __RENDERCONSTS_H__

#ifndef __OPENGLCOMMON_H__
	#include "Render/OpenGLCommon.h"
#endif // __OPENGLCOMMON_H__

#ifndef __TEXTURESHARED_H__
	#include "Render/TextureShared.h"
#endif // __TEXTURESHARED_H__

#ifndef __TEXTURE_H__
	#include "Render/Texture.h"
#endif // __TEXTURE_H__

#ifndef __TEXTUREATLAS_H__
	#include "Render/TextureAtlas.h"
#endif // __TEXTUREATLAS_H__

/////////////////////////////////////////////////////////////////////////////// FIXED FUNCTION ///////////////////////////////////////////////////////////////////////////////
#ifdef BASE_SUPPORT_OPENGL
	#ifndef __OPENGL_H__	
		#include "Render/FF/OpenGL/OpenGL.h"
	#endif // __OPENGL_H__

	#ifndef __EXTENSIONS_H__
		#include "Render/FF/OpenGL/Extensions.h"
	#endif // __EXTENSIONS_H__

	#ifndef __TEXTURELOADANDUPLOAD_H__
		#include "Render/FF/OpenGL/TextureLoadAndUpload.h"
	#endif // __TEXTURELOADANDUPLOAD_H__

	#ifndef __SPRITE_H__
		#include "Render/FF/OpenGL/Sprite.h"
	#endif // __SPRITE_H__

	#ifndef __RENDERTOTEXTURE_H__
		#include "Render/FF/OpenGL/RenderToTexture.h"
	#endif // __RENDERTOTEXTURE_H__
#endif // BASE_SUPPORT_OPENGL

#ifdef BASE_SUPPORT_OPENGLES
	#ifndef __OPENGLES_H__
		#include "Render/FF/OpenGLES/OpenGLES.h"
	#endif // __OPENGLES_H__

	#ifndef __EXTENSIONSOES_H__
		#include "Render/FF/OpenGLES/ExtensionsOES.h"
	#endif // __EXTENSIONSOES_H__

	#ifndef __TEXTURELOADANDUPLOADOES_H__
		#include "Render/FF/OpenGLES/TextureLoadAndUploadOES.h"
	#endif // __TEXTURELOADANDUPLOADOES_H__

	#ifndef __GLUSUPPORT_H__
		#include "Render/FF/OpenGLES/gluSupport.h"
	#endif // __GLUSUPPORT_H__

	#ifndef __SPRITEOES_H__
		#include "Render/FF/OpenGLES/SpriteOES.h"
	#endif // __SPRITEOES_H__

	#ifndef __RENDERTOTEXTUREOES_H__
		#include "Render/FF/OpenGLES/RenderToTextureOES.h"
	#endif // __RENDERTOTEXTUREOES_H__
#endif // BASE_SUPPORT_OPENGLES
	
#if defined(BASE_SUPPORT_OPENGL) || defined(BASE_SUPPORT_OPENGLES)
	#ifndef __PRIMITIVES_H__
		#include "Render/FF/Primitives.h"
	#endif // __PRIMITIVES_H__
#endif // defined (BASE_SUPPORT_OPENGL) || defined(BASE_SUPPORT_OPENGLES)

/////////////////////////////////////////////////////////////////////////////// GLSL ///////////////////////////////////////////////////////////////////////////////
	
#ifdef BASE_SUPPORT_OPENGL_GLSL
	#ifndef __SHADERSHARED_H__
		#include "Render/GLSL/ShaderShared.h"
	#endif // __SHADERSHARED_H__

	#ifndef __OPENGLSL_H__
		#include "Render/GLSL/OpenGLSL.h"
	#endif // __OPENGLSL_H__

	#ifndef __PRIMITIVESGLSL_H__
		#include "Render/GLSL/PrimitivesGLSL.h"
	#endif // __PRIMITIVESGLSL_H__

	#ifndef __TEXTURELOADANDUPLOADGLSL_H__
		#include "Render/GLSL/TextureLoadAndUploadGLSL.h"
	#endif // __TEXTURELOADANDUPLOADGLSL_H__

	#ifndef __GLEWES_H__
		#include "Render/GLSL/glewES.h"
	#endif // __GLEWES_H__

	#ifndef __RENDERTOTEXTUREGLSL_H__
		#include "Render/GLSL/RenderToTextureGLSL.h"
	#endif // __RENDERTOTEXTUREGLSL_H__

#endif // BASE_SUPPORT_OPENGL_GLSL

#if defined(BASE_SUPPORT_FREETYPE) || defined(BASE_SUPPORT_FREETYPE_UTF8)

	#ifndef __FREETYPEFONT_H__
		#include "Render/FreetypeCommon.h"
	#endif // __FREETYPEFONT_H__

	#if defined(BASE_SUPPORT_OPENGL) || defined(BASE_SUPPORT_OPENGLES)
		#ifndef __FREETYPEFONT_H__
			#include "Render/FF/FreetypeFont.h"
		#endif // __FREETYPEFONT_H__
	#endif // (BASE_SUPPORT_OPENGL) || defined(BASE_SUPPORT_OPENGLES)

	#ifdef BASE_SUPPORT_OPENGL_GLSL
		#ifndef __FREETYPEFONTGLSL_H__
			#include "Render/GLSL/FreetypeFontGLSL.h"
		#endif // __FREETYPEFONTGLSL_H__
		#ifndef __FREETYPEFONTUTF8GLSL_H__
			#include "Render/GLSL/FreetypeFontUTF8GLSL.h"
		#endif // __FREETYPEFONTUTF8GLSL_H__
	#endif // BASE_SUPPORT_OPENGL_GLSL

    #ifndef __FREETYPEFONTZZIP_H__
        #include "Render/FreetypeFontZZip.h"
    #endif // __FREETYPEFONTZZIP_H__

    #ifndef __FREETYPEZZIPACCESS_H__
        #include "Render/FreetypeZZipAccess.h"
    #endif // __FREETYPEZZIPACCESS_H__
#endif // BASE_SUPPORT_FREETYPE || BASE_SUPPORT_FREETYPE_UTF8

#ifdef BASE_SUPPORT_ADBAR
	#ifndef __ADVERTBARCOMMON_H__
		#include "Render/AdvertBarCommon.h"
	#endif // __ADVERTBARCOMMON_H__

	#if defined(BASE_SUPPORT_OPENGL) || defined(BASE_SUPPORT_OPENGLES)
		#ifndef __ADVERTBAR_H__
			#include "Render/FF/AdvertBar.h"
		#endif // __ADVERTBAR_H__
	#endif // (BASE_SUPPORT_OPENGL) || defined(BASE_SUPPORT_OPENGLES)

	#ifdef BASE_SUPPORT_OPENGL_GLSL
		#ifndef __ADVERTBARGLSL_H__
			#include "Render/GLSL/AdvertBarGLSL.h"
		#endif // __ADVERTBARGLSL_H__
	#endif // BASE_SUPPORT_OPENGL_GLSL
#endif //BASE_SUPPORT_ADBAR

#endif // __RENDERBASE_H__
