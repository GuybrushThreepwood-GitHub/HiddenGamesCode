
#ifndef __INPUTINCLUDE_H__
#define __INPUTINCLUDE_H__

#ifdef BASE_PLATFORM_WINDOWS

	#ifndef __WININCLUDES_H__
		#include "Core/win/WinIncludes.h"
	#endif // __WININCLUDES_H__

	#ifndef __WININPUT_H__
		#include "Input/win/WinInput.h"
	#endif // __WININPUT_H__
#endif // BASE_PLATFORM_WINDOWS

#ifdef BASE_PLATFORM_iOS
	#ifndef __IOSINPUT_H__
		#include "Input/ios/iOSInput.h"
	#endif // __IOSINPUT_H__
#endif // BASE_PLATFORM_iOS

#ifdef BASE_PLATFORM_ANDROID
	#ifndef __ANDROIDINPUT_H__
		#include "Input/aos/AndroidInput.h"
	#endif // __ANDROIDINPUT_H__
#endif // BASE_PLATFORM_ANDROID

#ifdef BASE_PLATFORM_RASPBERRYPI
	#ifndef __RASPBERRYPIINPUT_H__
		#include "Input/rpi/RaspberryPiInput.h"
	#endif // __RASPBERRYPIINPUT_H__
#endif // BASE_PLATFORM_RASPBERRYPI

#ifdef BASE_PLATFORM_MAC
	#ifndef __MACINPUT_H__
		#include "Input/mac/MacInput.h"
	#endif // __RASPBERRYPIINPUT_H__
#endif // __MACINPUT_H__

#ifndef __INPUTDEVICECONTROLLER_H__
    #include "Input/InputDeviceController.h"
#endif // __INPUTDEVICECONTROLLER_H__

#ifndef __INPUTMANAGER_H__
	#include "Input/InputManager.h"
#endif // __INPUTMANAGER_H__

#endif // __INPUTINCLUDE_H__