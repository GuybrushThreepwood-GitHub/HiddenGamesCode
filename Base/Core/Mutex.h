
#ifndef __MUTEX_H__
#define __MUTEX_H__

namespace core
{
	namespace app
	{
		#ifdef BASE_PLATFORM_WINDOWS
			#define PLATFORM_MUTEX_CLASS win::WinMutex
		#endif // BASE_PLATFORM_WINDOWS
		
		#ifdef BASE_PLATFORM_iOS
			#define PLATFORM_MUTEX_CLASS ios::iOSMutex
		#endif // BASE_PLATFORM_iOS

		#ifdef BASE_PLATFORM_ANDROID
			#define PLATFORM_MUTEX_CLASS aos::AndroidMutex
		#endif // BASE_PLATFORM_ANDROID
		
		#ifdef BASE_PLATFORM_RASPBERRYPI
			#define PLATFORM_MUTEX_CLASS rpi::RaspberryPiMutex
		#endif // BASE_PLATFORM_RASPBERRYPI

		#ifdef BASE_PLATFORM_MAC
			#define PLATFORM_MUTEX_CLASS mac::MacMutex
		#endif // BASE_PLATFORM_MAC
		
		class Mutex : public PLATFORM_MUTEX_CLASS
		{
			
		};

	} // namespace app

} // namespace core

#endif // __MUTEX_H__

