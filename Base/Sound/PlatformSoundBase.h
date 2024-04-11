
#ifndef __PLATFORMSOUNDBASE_H__
#define __PLATFORMSOUNDBASE_H__

#ifdef BASE_SUPPORT_OPENAL

#ifndef __PLATFORMOPENALINLCUDE_H__
	#ifdef BASE_PLATFORM_WINDOWS
		#include "Sound/win/PlatformOpenALInclude.h"
	#endif // BASE_PLATFORM_WINDOWS

	#ifdef BASE_PLATFORM_iOS
		#include "Sound/ios/PlatformOpenALInclude.h"	
	#endif // BASE_PLATFORM_iOS

	#ifdef BASE_PLATFORM_ANDROID
		#include "Sound/aos/PlatformOpenALInclude.h"	
	#endif // BASE_PLATFORM_ANDROID

	#ifdef BASE_PLATFORM_RASPBERRYPI
		#include "Sound/rpi/PlatformOpenALInclude.h"	
	#endif // BASE_PLATFORM_RASPBERRYPI

	#ifdef BASE_PLATFORM_MAC
		#include "Sound/mac/PlatformOpenALInclude.h"
	#endif // BASE_PLATFORM_MAC

#endif // __PLATFORMOPENALINLCUDE_H__

#endif // BASE_SUPPORT_OPENAL

#endif // __PLATFORMSOUNDBASE_H__


