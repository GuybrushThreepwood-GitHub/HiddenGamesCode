
#ifndef __DEBUGBASE_H__
#define __DEBUGBASE_H__

#ifndef __ASSERTION_H__
	#include "Debug/Assertion.h"
#endif // __ASSERTION_H__

#ifndef __DEBUGLOGGING_H__
	#include "Debug/DebugLogging.h"
#endif // __DEBUGLOGGING_H__

#if defined(_DEBUG) || defined(BASE_USE_DEBUG_FONT)

	#if defined(BASE_SUPPORT_OPENGL) || defined(BASE_SUPPORT_OPENGLES)
	#ifndef __DEBUGPRINT_H__
		#include "Debug/FF/DebugPrint.h"
	#endif // __DEBUGPRINT_H__
	#endif // defined(BASE_SUPPORT_OPENGL) || defined(BASE_SUPPORT_OPENGLES)

	#ifdef BASE_SUPPORT_OPENGL_GLSL
	#ifndef __DEBUGPRINTGLSL_H__
		#include "Debug/GLSL/DebugPrintGLSL.h"
	#endif // __DEBUGPRINTGLSL_H__
	#endif // BASE_SUPPORT_OPENGL_GLSL

#endif //defined(_DEBUG) || defined(BASE_USE_DEBUG_FONT)

#endif // __DEBUGBASE_H__

