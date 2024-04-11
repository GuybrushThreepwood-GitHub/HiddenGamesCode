
#ifndef __WININCLUDES_H__
#define __WININCLUDES_H__

#define WIN32_LEAN_AND_MEAN // Windows specific includes and defines
#define NOMINMAX // disable macro min/max

#define _CRT_SECURE_NO_DEPRECATE // don't care about secure c functions
#define _CRT_NONSTDC_NO_DEPRECATE // don't complain about non std c
#define _CRT_SECURE_NO_WARNINGS // no warnings

//	#define	_CRT_NON_CONFORMING_SWPRINTFS // stops comaplaint about non conforming swprintf without the size param

#ifdef _DEBUG
	#define _CRTDBG_MAP_ALLOC // debug calls
	//#define _CRTDBG_MAP_ALLOC_NEW
#endif // _DEBUG

#ifndef _INC_NEW
	#include <new>			// c++ new allocator
#endif // _INC_NEW

#ifdef _DEBUG
	#if (_MSC_VER < 1400)
		// VC8 defines this in crtdbg.h, but it has to come first in previous versions
		#include <malloc.h>
	#endif //(_MSC_VER < 1400)

	//#ifndef _INC_CRTDBG
		#include <crtdbg.h>
	//#endif // _INC_CRTDBG
#endif // _DEBUG

#ifdef BASE_SUPPORT_NETWORKING
	#ifndef _WINSOCK2API_
		#include <winsock2.h>	// sockets
	#endif // _WINSOCK2API_
	#ifndef _WS2TCPIP_H_
		#include <ws2tcpip.h>
	#endif // _WS2TCPIP_H_
	#ifndef __WS2BTH__H
		#include <ws2bth.h>		// bluetooth
	#endif // __WS2BTH__H
	//#include <BluetoothAPIs.h>
#endif // BASE_SUPPORT_NETWORKING

#ifndef _INC_WINDOWS	
	#include <windows.h>
#endif // _INC_WINDOWS

#ifndef _INC_SHELLAPI
	#include <shellapi.h>
#endif // _INC_SHELLAPI

// debug defined new operator that will help to show were leaks have occured
#ifdef _DEBUG
	//#define new new( _NORMAL_BLOCK, __FILE__, __LINE__ )
	#ifndef BASE_DEBUG_NEW
		#define BASE_DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
		#define new BASE_DEBUG_NEW
	#endif
#endif // _DEBUG

#ifndef _INC_MMSYSTEM
	#include <mmsystem.h>	// timer functions
#endif // _INC_MMSYSTEM

#ifndef _INC_PROCESS
	#include <process.h>
#endif // _INC_PROCESS

#endif // __WININCLUDES_H__