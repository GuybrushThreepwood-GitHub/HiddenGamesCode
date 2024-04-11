
#ifndef __COREBASE_H__
#define __COREBASE_H__

#ifndef __PLATFORMBASE_H__
	#include "PlatformBase.h"
#endif // __PLATFORMBASE_H__

#ifndef __COREDEFINES_H__
	#include "Core/CoreDefines.h"
#endif // __COREDEFINES_H__

#include <cstdio>
#include <cstdarg>
#include <iostream>
#include <fstream>
#include <map> // added to stop debug new operator causing compile problems 

#ifdef BASE_SUPPORT_SDL
	#include "SDL.h"
#endif // BASE_SUPPORT_SDL

#ifdef BASE_PLATFORM_WINDOWS
	#ifndef __WININCLUDES_H__
		#include "Core/win/WinIncludes.h"
	#endif // __WININCLUDES_H__

	#ifndef __WINAPP_H__
		#include "Core/win/WinApp.h"
	#endif // __WINAPP_H__

	#ifndef __WINMUTEX_H__
		#include "Core/win/WinMutex.h"
	#endif // __WINMUTEX_H__

	#ifndef __WINTHREAD_H__
		#include "Core/win/WinThread.h"
	#endif // __WINTHREAD_H__

	#ifndef __CORECONSTS_H__
		#include "Core/CoreConsts.h"
	#endif // __CORECONSTS_H__
#endif // BASE_PLATFORM_WINDOWS

#ifdef BASE_PLATFORM_iOS
	#ifndef __IOSINCLUDES_H__
		#include "Core/ios/iOSIncludes.h"
	#endif // __IOSINCLUDES_H__

	#ifndef __CORECONSTS_H__
		#include "Core/CoreConsts.h"
	#endif // __CORECONSTS_H__

	#ifndef __IOSAPP_H__
		#include "Core/ios/iOSApp.h"
	#endif // __IOSAPP_H__

	#ifndef __IOSMUTEX_H__
		#include "Core/ios/iOSMutex.h"
	#endif // __IOSMUTEX_H__

	#ifndef __IOSTHREAD_H__
		#include "Core/ios/iOSThread.h"
	#endif // __IOSTHREAD_H__
#endif // BASE_PLATFORM_iOS

#ifdef BASE_PLATFORM_ANDROID
	#ifndef __ANDROIDINCLUDES_H__
		#include "Core/aos/AndroidIncludes.h"
	#endif // __ANDROIDINCLUDES_H__

	#ifndef __ANDROIDAPP_H__
		#include "Core/aos/AndroidApp.h"
	#endif // __ANDROIDAPP_H__

	#ifndef __ANDROIDMUTEX_H__
		#include "Core/aos/AndroidMutex.h"
	#endif // __ANDROIDMUTEX_H__

	#ifndef __ANDROIDTHREAD_H__
		#include "Core/aos/AndroidThread.h"
	#endif // __ANDROIDTHREAD_H__

	#ifndef __CORECONSTS_H__
		#include "Core/CoreConsts.h"
	#endif // __CORECONSTS_H__
#endif // BASE_PLATFORM_ANDROID

#ifdef BASE_PLATFORM_RASPBERRYPI
	#ifndef __RASPBERRYPIINCLUDES_H__
		#include "Core/rpi/RaspberryPiIncludes.h"
	#endif // __RASPBERRYPIINCLUDES_H__

	#ifndef __RASPBERRYPIAPP_H__
		#include "Core/rpi/RaspberryPiApp.h"
	#endif // __RASPBERRYPIAPP_H__

	#ifndef __RASPBERRYPIMUTEX_H__
		#include "Core/rpi/RaspberryPiMutex.h"
	#endif // __RASPBERRYPIMUTEX_H__

	#ifndef __RASPBERRYPITHREAD_H__
		#include "Core/rpi/RaspberryPiThread.h"
	#endif // __RASPBERRYPITHREAD_H__

	#ifndef __CORECONSTS_H__
		#include "Core/CoreConsts.h"
	#endif // __CORECONSTS_H__
#endif // BASE_PLATFORM_RASPBERRYPI

#ifdef BASE_PLATFORM_MAC
	#ifndef __MACINCLUDES_H__
		#include "Core/mac/MacIncludes.h"
	#endif // __MACINCLUDES_H__

	#ifndef __MACAPP_H__
		#include "Core/mac/MacApp.h"
	#endif // __MACAPP_H__

	#ifndef __MACMUTEX_H__
		#include "Core/mac/MacMutex.h"
	#endif // __MACMUTEX_H__

	#ifndef __MACTHREAD_H__
		#include "Core/mac/MacThread.h"
	#endif // __MACTHREAD_H__

	#ifndef __CORECONSTS_H__
		#include "Core/CoreConsts.h"
	#endif // __CORECONSTS_H__
#endif // BASE_PLATFORM_MAC

#ifndef __DEBUGBASE_H__
	#include "DebugBase.h"
#endif // __DEBUGBASE_H__

#ifndef __COREFUNCTIONS_H__
	#include "Core/CoreFunctions.h"
#endif // __COREFUNCTIONS_H__

#ifndef __APP_H__
	#include "Core/App.h"
#endif // __APP_H__

#ifndef __MUTEX_H__
	#include "Core/Mutex.h"
#endif // __MUTEX_H__

#ifndef __THREAD_H__
	#include "Core/Thread.h"
#endif // __THREAD_H__

#ifndef __ENDIAN_H__
	#include "Core/Endian.h"
#endif // __ENDIAN_H__

#ifndef __FILEIO_H__
	#include "Core/FileIO.h"
#endif // __FILEIO_H__

#endif // __COREBASE_H__
