
#ifndef __PLATFORMOPENGLINLCUDE_H__
#define __PLATFORMOPENGLINLCUDE_H__

#ifdef BASE_SUPPORT_OPENGL

#ifdef BASE_PLATFORM_WINDOWS
	#ifndef __GL_H__
		#include <GL/gl.h>
	#endif __GL_H__

	#ifndef __GLU_H__
		#include <GL/glu.h>
	#endif __GLU_H__

	#ifndef __glext_h_
		#include <glext.h>
	#endif __glext_h_

	#ifndef __wglext_h_
		#include <wglext.h>
	#endif __wglext_h_
#endif // BASE_PLATFORM_WINDOWS

#ifdef BASE_PLATFORM_MAC
	#include <OpenGL/OpenGL.h>
	#include <OpenGL/glu.h>
	#include <OpenGL/glext.h>
#endif // BASE_PLATFORM_MAC

#endif // BASE_SUPPORT_OPENGL

#endif // __PLATFORMOPENGLINLCUDE_H__


