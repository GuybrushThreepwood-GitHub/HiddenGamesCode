
#ifndef __PLATFORMOPENGLINLCUDE_H__
#define __PLATFORMOPENGLINLCUDE_H__

#ifdef BASE_SUPPORT_OPENGL_GLSL 

#ifdef BASE_PLATFORM_WINDOWS
	#ifndef __glew_h__
		#include "GL/glew.h"
	#endif // __glew_h__
#endif // BASE_PLATFORM_WINDOWS

#ifdef BASE_PLATFORM_iOS
	#import <OpenGLES/ES2/gl.h>
	#import <OpenGLES/ES2/glext.h>
#endif // BASE_PLATFORM_iOS

#ifdef BASE_PLATFORM_ANDROID
	#include <GLES2/gl2.h>
	#include <GLES2/gl2ext.h>
#endif // BASE_PLATFORM_ANDROID

#ifdef BASE_PLATFORM_RASPBERRYPI
	#include <GLES2/gl2.h>
	#include <GLES2/gl2ext.h>
	#include <EGL/egl.h>
	#include <EGL/eglext.h>	
#endif // BASE_PLATFORM_RASPBERRYPI

#ifdef BASE_PLATFORM_MAC
	#include <OpenGL/OpenGL.h>
	#include <OpenGL/gl.h>
	#include <OpenGL/glext.h>
#endif // BASE_PLATFORM_MAC

#endif // BASE_SUPPORT_OPENGL_GLSL 

#endif // __PLATFORMOPENGLINLCUDE_H__


