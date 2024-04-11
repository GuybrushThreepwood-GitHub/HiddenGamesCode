
#ifndef __SOUNDSTREAM_H__
#define __SOUNDSTREAM_H__

#ifdef BASE_SUPPORT_OPENAL

#ifdef BASE_SUPPORT_OGG
	#include <ogg/ogg.h>
	#include <vorbis/vorbisfile.h>
#endif // BASE_SUPPORT_OGG

#ifdef BASE_PLATFORM_WINDOWS
#ifndef __WINSOUNDSTREAM_H__
	#include "win/WinSoundStream.h"
#endif // __WINSOUNDSTREAM_H__
#endif // BASE_PLATFORM_WINDOWS

#ifdef BASE_PLATFORM_iOS
#ifndef __IOSSOUNDSTREAM_H__
	#include "ios/iOSSoundStream.h"
#endif // __IOSSOUNDSTREAM_H__
#endif // BASE_PLATFORM_iOS

#ifdef BASE_PLATFORM_ANDROID
#ifndef __ANDROIDSOUNDSTREAM_H__
	#include "aos/AndroidSoundStream.h"
#endif // __ANDROIDSOUNDSTREAM_H__
#endif // BASE_PLATFORM_ANDROID

#ifdef BASE_PLATFORM_RASPBERRYPI
#ifndef __RASPBERRYPISOUNDSTREAM_H__
	#include "rpi/RaspberryPiSoundStream.h"
#endif // __RASPBERRYPISOUNDSTREAM_H__
#endif // BASE_PLATFORM_RASPBERRYPI

#ifdef BASE_PLATFORM_MAC
#ifndef __MACSOUNDSTREAM_H__
	#include "mac/MacSoundStream.h"
#endif // __MACSOUNDSTREAM_H__
#endif // BASE_PLATFORM_MAC

namespace snd
{

#ifdef BASE_PLATFORM_WINDOWS
	#define PLATFORM_SOUNDSTREAM_CLASS WinSoundStream
#endif // BASE_PLATFORM_WINDOWS
	
#ifdef BASE_PLATFORM_iOS
	#define PLATFORM_SOUNDSTREAM_CLASS iOSSoundStream
#endif // BASE_PLATFORM_iOS

#ifdef BASE_PLATFORM_ANDROID
	#define PLATFORM_SOUNDSTREAM_CLASS AndroidSoundStream
#endif // BASE_PLATFORM_ANDROID
	
#ifdef BASE_PLATFORM_RASPBERRYPI
	#define PLATFORM_SOUNDSTREAM_CLASS RaspberryPiSoundStream
#endif // BASE_PLATFORM_RASPBERRYPI

#ifdef BASE_PLATFORM_MAC
	#define PLATFORM_SOUNDSTREAM_CLASS MacSoundStream
#endif // BASE_PLATFORM_MAC
	
	class SoundStream : public PLATFORM_SOUNDSTREAM_CLASS
	{
			
	};

} // namespace snd


#endif // BASE_SUPPORT_OPENAL

#endif // __SOUNDSTREAM_H__

