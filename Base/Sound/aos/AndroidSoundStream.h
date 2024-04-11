
#ifndef __ANDROIDSOUNDSTREAM_H__
#define __ANDROIDSOUNDSTREAM_H__

#ifdef BASE_PLATFORM_ANDROID
#ifdef BASE_SUPPORT_OPENAL

#ifdef BASE_SUPPORT_OGG
	#include <ogg/ogg.h>
	#include <vorbis/vorbisfile.h>
#endif // BASE_SUPPORT_OGG

namespace snd
{
	class AndroidSoundStream
	{
		public:
			/// default constructor
			AndroidSoundStream();
			/// default destructor
			virtual ~AndroidSoundStream();

			/// StreamOpen - Opens a file for streaming
			/// \param szFilename - file to load
			/// \return integer - ( SUCCESS: OK or FAIL: FAIL )
			int StreamOpen( const char *szFilename );
			/// Release - Releases any buffers and memory used by the class
			void Release( void );
			/// Display - Displays the information attached to the loaded OGG file
			void Display( void );
			/// StartPlayback - Starts playing any loaded music file
			/// \param loop - loop the stream
			/// \return bool - ( true or false )
			bool StartPlayback( bool loop );
			/// Play - Plays an OGG music file
			/// \return bool - ( true or false )
			bool Play( void );
			/// Pause - Pauses the music file
			/// \return bool - ( true or false )
			bool Pause( void );
			/// Stop - Stops the music file
			/// \return bool - ( true or false )
			bool Stop( void );
			/// IsPlaying - Starts playing the music
			/// \return bool - ( true or false )
			bool IsPlaying( void );
			/// Update - Updates the buffers playing the OGG file
			/// \return bool - ( true or false )
			bool Update( void );
			/// SetVolume - Sets the volume of the OGG file
			/// \param fVolume - volume to set
			void SetVolume( float fVolume );

		protected:

#ifdef BASE_SUPPORT_OGG
			/// OpenOGG - Loads an OGG music file
			/// \param szFilename - file to load
			/// \return integer - ( SUCCESS: OK or FAIL: FAIL )
			int OpenOGG( const char *szFilename );
#endif // BASE_SUPPORT_OGG
			/// OpenWAV - Loads an wav music file
			/// \param szFilename - file to load
			/// \return integer - ( SUCCESS: OK or FAIL: FAIL )
			int OpenWAV( const char *szFilename );

			/// Stream - Passes a buffer to read for stream data
			/// \param buffer - buffer to read from
			/// \return bool - ( true or false ) 
			bool Stream(ALuint buffer);
			/// Empty - Unqueues the music buffers
			void Empty( void );

			/// StreamIMA4 - streams ima4 adpcm data
			void StreamIMA4();

		private:
			/// file pointer
			file::TFileHandle m_FileHandle;
			/// file type
			snd::ESoundFormat m_SoundFormat;

			bool m_Loop;
			bool m_Playing;

			/// music volume value
			float m_Volume;
			/// AL buffer id list
			ALuint m_BufferID[STREAM_BUFFER_COUNT];
			/// AL source object id
			ALuint m_SourceID;
			/// AL format mono/stereo
			ALenum m_Format;

			/// wave file storage
			snd::WAVEFileInfo m_WavFileInfo;
			/// wave stream buffer
			unsigned char* m_pWaveStrmData;
			/// adpcm work buffer
			unsigned char* m_pADPCMData;
			/// wave buffer size
			unsigned long m_WaveBufferSize;
			// stream size
			unsigned long m_StreamLoadSize;

#ifdef BASE_SUPPORT_OGG
			/// vorbis stream structure
			OggVorbis_File m_OggStream;
			/// vorbis file info structure
			vorbis_info* m_VorbisInfo;
			/// vorbis file comment structure 
			vorbis_comment* m_VorbisComment;
#endif // BASE_SUPPORT_OGG

	};

} // namespace snd

#endif // BASE_SUPPORT_OPENAL
#endif // BASE_PLATFORM_ANDROID

#endif // __ANDROIDSOUNDSTREAM_H__


