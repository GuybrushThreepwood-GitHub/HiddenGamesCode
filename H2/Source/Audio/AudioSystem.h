
#ifndef __AUDIOSYSTEM_H__
#define __AUDIOSYSTEM_H__

#include <vector>

#include "SoundBase.h"

#ifdef BASE_PLATFORM_WINDOWS
	#include "win/WinAudioSystemBase.h"
#endif // BASE_PLATFORM_WINDOWS

#if defined(BASE_PLATFORM_iOS) && !defined(BASE_PLATFORM_tvOS)
	#include "ios/iOSAudioSystemBase.h"
#endif // BASE_PLATFORM_iOS

#ifdef BASE_PLATFORM_tvOS 
    #include "tos/tvOSAudioSystemBase.h"
#endif // BASE_PLATFORM_tvOS

#ifdef BASE_PLATFORM_ANDROID
	#include "aos/AndroidAudioSystemBase.h"
#endif // BASE_PLATFORM_ANDROID

#ifdef BASE_PLATFORM_RASPBERRYPI
	#include "rpi/RaspberryPiAudioSystemBase.h"
#endif // BASE_PLATFORM_RASPBERRYPI

#ifdef BASE_PLATFORM_MAC
	#include "mac/MacAudioSystemBase.h"
#endif // BASE_PLATFORM_MAC

typedef std::map< int, ALuint> TNameIntMap;
typedef std::pair< int, ALuint> TNameIntPair;

class AudioSystem : public AudioSystemBase
{
	public:
		AudioSystem( snd::MusicMode mode=snd::MusicMode_SingleFilePlayer );
		virtual ~AudioSystem();

		static void Initialise( snd::MusicMode mode=snd::MusicMode_SingleFilePlayer );
		static void Shutdown( void );

		//ALuint FindSound( const char* soundName );
		//ALuint AddAudioFile( const char* file );
		ALuint FindSound( int resId );
		ALuint AddAudioFile( int resId );

		static AudioSystem *GetInstance( void ) 
		{
			DBG_ASSERT( (ms_Instance != 0) );

			return( ms_Instance );
		}

		static bool IsInitialised( void ) 
		{
			return( ms_Instance != 0 );
		}

	private:
		static AudioSystem* ms_Instance;

		snd::Sound m_SoundLoader;
		TNameIntMap m_AudioNameMap;
};

#endif // __AUDIOSYSTEM_H__

