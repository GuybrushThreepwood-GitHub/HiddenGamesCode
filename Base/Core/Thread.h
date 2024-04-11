
#ifndef __THREAD_H__
#define __THREAD_H__

namespace core
{
	namespace app
	{
		enum EThreadState
		{
			THREADSTATE_UNINITIALISED=0,
			THREADSTATE_IDLE,
			THREADSTATE_RUNNING,
			THREADSTATE_WAITING,
			THREADSTATE_FINISHED
		};

		#ifdef BASE_PLATFORM_WINDOWS
			#define PLATFORM_THREAD_CLASS win::WinThread
		#endif // BASE_PLATFORM_WINDOWS

		#ifdef BASE_PLATFORM_iOS
			#define PLATFORM_THREAD_CLASS ios::iOSThread
		#endif // BASE_PLATFORM_iOS

		#ifdef BASE_PLATFORM_ANDROID
			#define PLATFORM_THREAD_CLASS aos::AndroidThread
		#endif // BASE_PLATFORM_ANDROID
	
		#ifdef BASE_PLATFORM_RASPBERRYPI
			#define PLATFORM_THREAD_CLASS rpi::RaspberryPiThread
		#endif // BASE_PLATFORM_RASPBERRYPI

		#ifdef BASE_PLATFORM_MAC
			#define PLATFORM_THREAD_CLASS mac::MacThread
		#endif // BASE_PLATFORM_MAC
		
		class Thread : public PLATFORM_THREAD_CLASS
		{

		};

	} // namespace app

} // namespace core

#endif // __THREAD_H__

