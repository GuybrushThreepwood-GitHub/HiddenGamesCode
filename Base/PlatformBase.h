
#ifndef __PLATFORMBASE_H__
#define __PLATFORMBASE_H__

// ######## MOST IMPORTANT ########

// BASE_PLATFORM_WINDOWS		// - add at preprocessor level - Windows platform
// BASE_PLATFORM_iOS			// - add at preprocessor level - iOS platform
// BASE_PLATFORM_ANDROID		// - add at preprocessor level - android platform
// BASE_PLATFORM_RASPBERRYPI	// - add at preprocessor level - Raspberry Pi support
// BASE_PLATFORM_MAC			// - add at preprocessor level - Mac OS X platform

// BASE_NO_PNG					// - add at preprocessor level - disable any png library support
// BASE_DUMMY_APP               // - add at preprocessor level - used for non game apps

// ################################

// wchar_t support
// BASE_SUPPORT_64BIT   // support 64 bit library compilation
// BASE_SUPPORT_WCHAR   // use wchar_t
// BASE_USE_DEBUG_FONT  // - allow the debug font outside of a debug build

// render support (one or the other)
// BASE_SUPPORT_OPENGL       // support for desktop OpenGL (Fixed Function)
// BASE_SUPPORT_OPENGLES     // support for embedded OpenGL
// BASE_SUPPORT_OPENGL_GLSL  // support for OpenGL using GLSL (both desktop and embedded support)

// BASE_OES_EXTENSIONS_CHANGE

// image format support
//#define BASE_SUPPORT_BMP          // support Bitmap image file loading
//#define BASE_SUPPORT_PNG          // support PNG image file loading
//#define BASE_SUPPORT_JPEG         // support JPEG image file loading
//#define BASE_SUPPORT_PCX          // support PCX image file loading
//#define BASE_SUPPORT_DDS          // support DDS image file loading

// sound format support
// BASE_NO_OPENAL				// - add at preprocessor level - disable OpenAL library loading
// BASE_SUPPORT_OPENAL          // normal OpenAL library support
// BASE_SUPPORT_OGG				// - add at preprocessor level - support for OGG vorbis container using libvorbis and libogg

// lua script support
// BASE_SUPPORT_SCRIPTING		// - add at preprocessor level - support for LUA scripting

// physis support
// BASE_SUPPORT_BOX2D			// - add at preprocessor level - supports for Box2D library
// BASE_SUPPORT_ODE				// - add at preprocessor level - support for ODE library
// BASE_SUPPORT_BULLET			// - add at preprocessor level - support for Bullet library

// freetype library support
// BASE_SUPPORT_FREETYPE		// - add at preprocessor level - support for Freetype library

// network support
// BASE_SUPPORT_NETWORKING		// - add at preprocessor level - support for socket networking

// lets win32 pretend to render like an iOS device screen
// BASE_WINDOW_IOS				// - add at preprocessor level (win only)

//////////////////////////////////////////////////////////////////////////

#ifdef BASE_PLATFORM_WINDOWS
	#ifndef BASE_NO_OPENAL	
		#define BASE_SUPPORT_OPENAL
	#endif
	//#define BASE_SUPPORT_DDS
	#ifndef BASE_NO_PNG
		#define BASE_SUPPORT_PNG
	#endif // 
#endif // BASE_PLATFORM_WINDOWS

#ifdef BASE_PLATFORM_iOS
	#ifndef BASE_NO_OPENAL	
		#define BASE_SUPPORT_OPENAL
	#endif
	#ifndef BASE_NO_PNG
		#define BASE_SUPPORT_PNG
	#endif //
	//#define BASE_SUPPORT_JPEG
#endif // BASE_PLATFORM_iOS

#ifdef BASE_PLATFORM_ANDROID
	#ifndef BASE_NO_OPENAL	
		#define BASE_SUPPORT_OPENAL
	#endif //
	#ifndef BASE_NO_PNG
		#define BASE_SUPPORT_PNG
	#endif //
	//#define BASE_SUPPORT_JPEG
#endif // BASE_PLATFORM_ANDROID

#ifdef BASE_PLATFORM_RASPBERRYPI
	#ifndef BASE_NO_OPENAL	
		#define BASE_SUPPORT_OPENAL
	#endif //
	#ifndef BASE_NO_PNG
		#define BASE_SUPPORT_PNG
	#endif //
	//#define BASE_SUPPORT_JPEG
#endif // BASE_PLATFORM_RASPBERRYPI

#ifdef BASE_PLATFORM_MAC
	#ifndef BASE_NO_OPENAL	
		#define BASE_SUPPORT_OPENAL
	#endif //
	#ifndef BASE_NO_PNG
		#define BASE_SUPPORT_PNG
	#endif //
	//#define BASE_SUPPORT_JPEG
#endif // BASE_PLATFORM_MAC

#endif // __PLATFORMBASE_H__