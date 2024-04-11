
/*===================================================================
	File: DebugPrint.h
	Library: Debug

	(C)Hidden Games
=====================================================================*/

#ifndef __DEBUGPRINT_H__
#define __DEBUGPRINT_H__

#if defined(_DEBUG) || defined(BASE_USE_DEBUG_FONT)
#if defined(BASE_SUPPORT_OPENGL) || defined(BASE_SUPPORT_OPENGLES)

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
	/// DebugPrint3D - Displays a string buffer of text in Perspective mode
	/// \param x - x world position of string
	/// \param y - y world position of string
	/// \param z - z world position of string
	/// \param szString - string data
	/// \param ... - variable string options
	void DebugPrint3D( float x, float y, float z, const char* szString, ... );
	/// DebugDestroyFont - deletes the GL font texture
	void DebugDestroyFont();
}

#endif // defined(BASE_SUPPORT_OPENGL) || defined(BASE_SUPPORT_OPENGLES)
#endif // defined(_DEBUG) || defined(BASE_USE_DEBUG_FONT)

#endif // __DEBUGPRINT_H__

