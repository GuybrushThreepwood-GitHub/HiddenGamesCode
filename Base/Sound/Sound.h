
#ifndef __SOUND_H__
#define __SOUND_H__

#ifdef BASE_SUPPORT_OPENAL

#ifndef AL_AL_H
	#include <OpenAL/al.h>
#endif // AL_AL_H

#ifdef BASE_PLATFORM_WINDOWS
#ifndef __WINSOUND_H__
	#include "win/WinSound.h"
#endif // _WINSSOUND_H__
#endif // BASE_PLATFORM_WINDOWS

#ifdef BASE_PLATFORM_iOS
#ifndef __IOSSOUND_H__
	#include "ios/iOSSound.h"
#endif // __IOSSOUND_H__
#endif // BASE_PLATFORM_iOS

#ifdef BASE_PLATFORM_ANDROID
#ifndef __ANDROIDSOUND_H__
	#include "aos/AndroidSound.h"
#endif // __ANDROIDSOUND_H__
#endif // BASE_PLATFORM_ANDROID

#ifdef BASE_PLATFORM_RASPBERRYPI
#ifndef __RASPBERRYPISOUND_H__
	#include "rpi/RaspberryPiSound.h"
#endif // __RASPBERRYPISOUND_H__
#endif // BASE_PLATFORM_RASPBERRYPI

#ifdef BASE_PLATFORM_MAC
#ifndef __MACSOUND_H__
	#include "mac/MacSound.h"
#endif // __MACSOUND_H__
#endif // BASE_PLATFORM_MAC

namespace snd
{

#ifdef BASE_PLATFORM_WINDOWS
	#define PLATFORM_SOUND_CLASS WinSound
#endif // BASE_PLATFORM_WINDOWS
	
#ifdef BASE_PLATFORM_iOS
	#define PLATFORM_SOUND_CLASS iOSSound
#endif // BASE_PLATFORM_iOS

#ifdef BASE_PLATFORM_ANDROID
	#define PLATFORM_SOUND_CLASS AndroidSound
#endif // BASE_PLATFORM_ANDROID	

#ifdef BASE_PLATFORM_RASPBERRYPI
	#define PLATFORM_SOUND_CLASS RaspberryPiSound
#endif // BASE_PLATFORM_RASPBERRYPI	

#ifdef BASE_PLATFORM_MAC
	#define PLATFORM_SOUND_CLASS MacSound
#endif // BASE_PLATFORM_MAC
	
	class Sound : public PLATFORM_SOUND_CLASS
	{
			
	};

	void SetMusicPauseCall( snd::MusicPauseCall* call );
	
	snd::MusicPauseCall* GetMusicPauseCall();
	
} // namespace snd

#endif // BASE_SUPPORT_OPENAL

#endif // __SOUND_H__


