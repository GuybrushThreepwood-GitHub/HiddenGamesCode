

#ifndef __MACSOUND_H__
#define __MACSOUND_H__

#ifdef BASE_PLATFORM_MAC
#ifdef BASE_SUPPORT_OPENAL

#ifndef AL_AL_H
	#include <OpenAL/al.h>
#endif // AL_AL_H

#import <AudioToolbox/AudioToolbox.h>
#import <AudioToolbox/ExtendedAudioFile.h>

// forward declare
namespace math { class Vec3; }
namespace file { struct TFile; }

namespace snd
{
	const int OGG_BUFFER_READ_SIZE = (4096*4);

	class MacSound
	{
		public:
			/// default constructor
			MacSound();
			/// default destructor
			~MacSound();

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
			/// LoadWAV - Loads a sound filename if it's a valid WAV file
			/// \param szFilename - name of file to load
			/// \return integer - ( SUCCESS: OK or FAIL: FAIL )
			int LoadWAV2( const char *szFilename );		
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

			SInt64							m_FileLengthInFrames;
			AudioStreamBasicDescription		m_CompressedFormat;
			UInt32							m_PropertySize;
			ExtAudioFileRef					m_ExtFileRef;
			unsigned char*					m_pData;
			SInt64							m_StartFrame;
			AudioStreamBasicDescription		m_OutputFormat;
		
			/// wave file storage
			snd::WAVEFileInfo m_WavFileInfo;		
	};

} // namespace snd

#endif // BASE_SUPPORT_OPENAL
#endif // BASE_PLATFORM_MAC

#endif // __MACSOUND_H__
