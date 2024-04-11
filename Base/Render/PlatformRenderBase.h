
#ifndef __PLATFORMRENDERBASE_H__
#define __PLATFORMRENDERBASE_H__

#ifndef __PLATFORMOPENGLINLCUDE_H__

	#ifdef BASE_SUPPORT_OPENGL
		#include "Render/FF/OpenGL/PlatformOpenGLInclude.h"
	#endif // BASE_SUPPORT_OPENGL

	#ifdef BASE_SUPPORT_OPENGLES
		#include "Render/FF/OpenGLES/PlatformOpenGLInclude.h"
	#endif // BASE_SUPPORT_OPENGLES

	#ifdef BASE_SUPPORT_OPENGL_GLSL
		#include "Render/GLSL/PlatformOpenGLInclude.h"
	#endif // BASE_SUPPORT_OPENGL_GLSL

#endif // __PLATFORMOPENGLINLCUDE_H__

#endif // __PLATFORMRENDERBASE_H__


