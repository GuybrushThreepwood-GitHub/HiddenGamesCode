
#ifndef __PLATFORMOPENGLINLCUDE_H__
#define __PLATFORMOPENGLINLCUDE_H__

#ifdef BASE_SUPPORT_OPENGLES

#ifdef BASE_PLATFORM_iOS
    //#import <OpenGLES/EAGL.h>
    //#import <OpenGLES/EAGLDrawable.h>

	#import <OpenGLES/ES1/gl.h>
	#import <OpenGLES/ES1/glext.h>
#endif // BASE_PLATFORM_iOS

#ifdef BASE_PLATFORM_ANDROID
	#include <GLES/gl.h>
	#include <GLES/glext.h>
#endif // BASE_PLATFORM_ANDROID

#ifdef BASE_PLATFORM_RASPBERRYPI
	#include <GLES/gl.h>
	#include <GLES/glext.h>
	#include <EGL/egl.h>
	#include <EGL/eglext.h>
#endif // BASE_PLATFORM_RASPBERRYPI

#include "Render/FF/OpenGLES/gluSupport.h"

#endif // BASE_SUPPORT_OPENGLES

#endif // __PLATFORMOPENALINLCUDE_H__


