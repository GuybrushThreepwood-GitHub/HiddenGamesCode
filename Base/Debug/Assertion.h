
#ifndef __ASSERTION_H__
#define __ASSERTION_H__

#include <cassert>
#include <iostream>

//#ifdef BASE_PLATFORM_WINDOWS
//	#ifndef __WININCLUDES_H__
//		#include "Core/win/WinIncludes.h"
//	#endif // __WININCLUDES_H__
//#endif // BASE_PLATFORM_WINDOWS

#ifdef BASE_PLATFORM_iOS
	#ifndef __IOSINCLUDES_H__
		#include "Core/ios/iOSIncludes.h"
	#endif // __IOSINCLUDES_H__
#endif // BASE_PLATFORM_iOS

#ifdef BASE_PLATFORM_ANDROID
	#ifndef __ANDROIDINCLUDES_H__
		#include "Core/aos/AndroidIncludes.h"
	#endif // __ANDROIDINCLUDES_H__
#endif // BASE_PLATFORM_ANDROID

#ifdef BASE_PLATFORM_RASPBERRYPI
	#ifndef __RASPBERRYPIINCLUDES_H__
		#include "Core/rpi/RaspberryPiIncludes.h"
	#endif // __RASPBERRYPIINCLUDES_H__
#endif // BASE_PLATFORM_RASPBERRYPI

#ifdef BASE_PLATFORM_MAC
	#ifndef __MACINCLUDES_H__
		#include "Core/mac/MacIncludes.h"
	#endif // __MACINCLUDES_H__
#endif // BASE_PLATFORM_MAC

#ifdef _DEBUG
	#define ASSERTS_ENABLED
#endif // _DEBUG

namespace dbg
{
	int ReportAssert( const char* condition, const char* file, int line, const char* description, ... );
}

#ifdef BASE_PLATFORM_WINDOWS
	#define DBG_HALT() __debugbreak()
#endif // BASE_PLATFORM_WINDOWS

#if defined(BASE_PLATFORM_iOS) || defined(BASE_PLATFORM_ANDROID) || defined(BASE_PLATFORM_RASPBERRYPI) || defined(BASE_PLATFORM_MAC)
	#define DBG_HALT() kill( getpid(), SIGINT )
#endif // defined(BASE_PLATFORM_iOS) || defined(BASE_PLATFORM_ANDROID) || defined(BASE_PLATFORM_RASPBERRYPI) || defined(BASE_PLATFORM_MAC)

#define ASSERT_UNUSED(x) do { (void)sizeof(x); } while(0)

#ifdef ASSERTS_ENABLED
	#define DBG_ASSERT(cond) \
		do \
		{ \
			if (!(cond)) \
			{ \
				if (dbg::ReportAssert(#cond, __FILE__, __LINE__, 0) ) \
					DBG_HALT(); \
			} \
		} while(0)

	#define DBG_ASSERT_MSG(cond, msg, ...) \
		do \
		{ \
			if (!(cond)) \
			{ \
				if (dbg::ReportAssert(#cond, __FILE__, __LINE__, msg, ##__VA_ARGS__) ) \
					DBG_HALT(); \
			} \
		} while(0)

	//#define DBG_ASSERT_FAIL(msg, ...) \
	//	do \
	//	{ \
	//		if (dbg::ReportAssert(0, __FILE__, __LINE__, (msg), __VA_ARGS__) ) \
	//		DBG_HALT(); \
	//	} while(0)

	//#define DBG_VERIFY(cond) DBG_ASSERT(cond)
	//#define DBG_VERIFY_MSG(cond, msg, ...) DBG_ASSERT_MSG(cond, msg, ##__VA_ARGS__)
#else

	#define DBG_ASSERT(condition) \
		do { ASSERT_UNUSED(condition); } while(0)

	#define DBG_ASSERT_MSG(condition, msg, ...) \
		do { ASSERT_UNUSED(condition); ASSERT_UNUSED(msg); } while(0)

	#define DBG_ASSERT_FAIL(msg, ...) \
		do { ASSERT_UNUSED(msg); } while(0)

	#define DBG_VERIFY(cond) (void)(cond)

	#define DBG_VERIFY_MSG(cond, msg, ...) \
		do { (void)(cond); ASSERT_UNUSED(msg); } while(0)

#endif // ASSERTS_ENABLED

#endif // __ASSERTION_H__

