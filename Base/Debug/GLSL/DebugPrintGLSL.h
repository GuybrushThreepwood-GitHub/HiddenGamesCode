
/*===================================================================
	File: DebugPrint.h
	Library: Debug

	(C)Hidden Games
=====================================================================*/

#ifndef __DEBUGPRINTGLSL_H__
#define __DEBUGPRINTGLSL_H__

#if defined(_DEBUG) || defined(BASE_USE_DEBUG_FONT)
#ifdef BASE_SUPPORT_OPENGL_GLSL

// forward delcare
namespace math { class Vec3; }
namespace renderer { class OpenGL; }

namespace dbg
{
	/// DebugCreateFont - Creates a GL 8x8 font texture
	void DebugCreateFont( renderer::OpenGL* openGLContext=0 );
	/// DebugSetFontColour - Sets the colour of the debug font
	/// \param r -  red component
	/// \param g -  green component
	/// \param b -  blue component
	/// \param a -  alpha component
	void DebugSetFontColour( unsigned char r, unsigned char g, unsigned char b, unsigned char a );
	/// DebugPrint - Displays a string buffer of text in Orthographic mode
	/// \param x - x window position of string
	/// \param y - y window position of string
	/// \param szString - string data
	/// \param ... - variable string options
	void DebugPrint( int x, int y, const char* szString, ... );
	/// DebugDestroyFont - deletes the GL font texture
	void DebugDestroyFont();
}

#endif // BASE_SUPPORT_OPENGL_GLSL
#endif // defined(_DEBUG) || defined(BASE_USE_DEBUG_FONT)

#endif // __DEBUGPRINTGLSL_H__

