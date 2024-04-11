

#ifndef __ANDROIDSOUND_H__
#define __ANDROIDSOUND_H__

#ifdef BASE_PLATFORM_ANDROID
#ifdef BASE_SUPPORT_OPENAL

#ifndef AL_AL_H
	#include <AL/al.h>
#endif // AL_AL_H

// forward declare
namespace math { class Vec3; }
namespace file { struct TFile; }

namespace snd
{
	const int OGG_BUFFER_READ_SIZE = (4096*20);

	class AndroidSound
	{
		public:
			/// default constructor
			AndroidSound();
			/// default destructor
			~AndroidSound();

			/// Initialise - clears out the sound data
			void Initialise( void );
			/// Release - frees andy sound memory
			void Release( void );
			/// Load - Loads a sound filename if it's valid
			/// \param szFilename - name of file to load
			/// \return integer - ( SUCCESS: OK or FAIL: FAIL )
			int Load( const char *szFilename );
			/// LoadWAV - Loads a sound filename if it's a valid WAV file
			/// \param szFilename - name of file to load
			/// \return integer - ( SUCCESS: OK or FAIL: FAIL )
			int LoadWAV( const char *szFilename );
#ifdef BASE_SUPPORT_OGG
			/// LoadOGG - Loads a sound filename if it's a valid OGG file
			/// \param szFilename - name of file to load
			/// \return integer - ( SUCCESS: OK or FAIL: FAIL )
			int LoadOGG( const char *szFilename );
#endif // BASE_SUPPORT_OGG
			/// Free - Frees any sound data
			/// \return integer - ( SUCCESS: OK or FAIL: FAIL )
			int Free( void );

			/// file format enum
			ESoundFormat eFileFormat;
			/// file size
			std::size_t nFileSize;
			/// filename of the sound
			file::TFile SoundFile;

			/// AL buffer object id
			ALuint nBufferID;

			/// sound format
			int nFormat;
			/// sound bitrate
			int nBitRate;
			/// sound frequency
			int nFrequency;
			/// number of channels
			int nChannels;
			/// bytes per second
			int nBytesPerSec;
			/// sound file size
			int nSoundSize;

		private:
			/// pointer to sound data
			char *m_Buffer;

			/// wave file storage
			snd::WAVEFileInfo m_WavFileInfo;
	};

} // namespace snd

#endif // BASE_SUPPORT_OPENAL
#endif // BASE_PLATFORM_ANDROID

#endif // __ANDROIDSOUND_H__

