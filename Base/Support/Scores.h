
#ifndef __SCORES_H__
#define __SCORES_H__

#ifndef __SCORESCOMMON_H__
	#include "ScoresCommon.h"
#endif // __SCORESCOMMON_H__

#ifdef BASE_PLATFORM_WINDOWS
	#ifndef __WINSCORES_H__
		#include "win/WinScores.h"
	#endif // __WINSCORES_H__
#endif // BASE_PLATFORM_WINDOWS

#ifdef BASE_PLATFORM_iOS
	#ifndef __IOSSCORES_H__
		#include "ios/iOSScores.h"
	#endif // __IOSSCORES_H__
#endif // BASE_PLATFORM_iOS

#ifdef BASE_PLATFORM_ANDROID
	#ifndef __ANDROIDSCORES_H__
		#include "aos/AndroidScores.h"
	#endif // __ANDROIDSCORES_H__
#endif // BASE_PLATFORM_ANDROID

#ifdef BASE_PLATFORM_RASPBERRYPI
	#ifndef __RASPBERRYPISCORES_H__
		#include "rpi/RaspberryPiScores.h"
	#endif // __RASPBERRYPISCORES_H__
#endif // BASE_PLATFORM_RASPBERRYPI

#ifdef BASE_PLATFORM_MAC
	#ifndef __MACSCORES_H__
		#include "mac/MacScores.h"
	#endif // __MACSCORES_H__
#endif // BASE_PLATFORM_MAC

namespace support
{
    #ifdef BASE_PLATFORM_WINDOWS
		#define SCORE_PLATFORM_CLASS win::WinScores
		#define SCORE_PLATFORM_CALLBACK win::WinScores(callback)
    #endif // BASE_PLATFORM_WINDOWS

    #ifdef BASE_PLATFORM_iOS
        #define SCORE_PLATFORM_CLASS ios::iOSScores
		#define SCORE_PLATFORM_CALLBACK ios::iOSScores(callback)
    #endif // BASE_PLATFORM_iOS
		
    #ifdef BASE_PLATFORM_ANDROID
        #define SCORE_PLATFORM_CLASS aos::AndroidScores
		#define SCORE_PLATFORM_CALLBACK aos::AndroidScores(callback)
    #endif // BASE_PLATFORM_ANDROID

    #ifdef BASE_PLATFORM_RASPBERRYPI
        #define SCORE_PLATFORM_CLASS rpi::RaspberryPiScores
		#define SCORE_PLATFORM_CALLBACK rpi::RaspberryPiScores(callback)
    #endif // BASE_PLATFORM_RASPBERRYPI

	#ifdef BASE_PLATFORM_MAC
		#define SCORE_PLATFORM_CLASS mac::MacScores
		#define SCORE_PLATFORM_CALLBACK mac::MacScores(callback)
	#endif // BASE_PLATFORM_MAC
	
    class Scores : public SCORE_PLATFORM_CLASS
    {
        public:
			Scores(support::ScoresCallback* callback)
				: SCORE_PLATFORM_CALLBACK
			{

			}

            static void Create(support::ScoresCallback* callback);
            static void Destroy();
            
            static Scores *GetInstance( void ) 
            {
                DBG_ASSERT( (ms_Instance != 0) );
                
                return( ms_Instance );
            }
            static bool IsInitialised( void ) 
            {
                return( ms_Instance != 0 );
            }
            
        private:
            static Scores* ms_Instance;            
            
    };

} // namespace support

#endif // __SCORES_H__

