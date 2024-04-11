
/*===================================================================
	File: RaspberryPiSoundStream.cpp
	Library: SoundLib

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"

#ifdef BASE_PLATFORM_RASPBERRYPI
#ifdef BASE_SUPPORT_OPENAL

#ifdef BASE_SUPPORT_OGG
	#include <ogg/ogg.h>
	#include <vorbis/vorbisfile.h>
#endif // BASE_SUPPORT_OGG

#include "Math/Vectors.h"

#include "Sound/SoundCommon.h"
#include "Sound/SoundCodecs.h"
#include "Sound/OpenAL.h"
#include "Sound/Sound.h"
#include "Sound/SoundStream.h"

#include "Sound/rpi/RaspberryPiSoundStream.h"

using snd::RaspberryPiSoundStream;

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
RaspberryPiSoundStream::RaspberryPiSoundStream()
{
	int i = 0;
	m_SoundFormat = snd::SOUND_UNKNOWN;

	m_Playing = false;
	m_Loop = false;

	for( i = 0; i < STREAM_BUFFER_COUNT; i++ )
		m_BufferID[i] = INVALID_SOUNDBUFFER;

	m_SourceID = INVALID_SOUNDSOURCE;
	m_Volume = 1.0f;

	std::memset( &m_WavFileInfo, 0, sizeof(snd::WAVEFileInfo) );
	m_pWaveStrmData = 0;
	m_pADPCMData = 0;
	m_WaveBufferSize = 0;
	m_StreamLoadSize = 0;
}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
RaspberryPiSoundStream::~RaspberryPiSoundStream()
{
	Release();
}


/////////////////////////////////////////////////////
/// Method: StreamOpen
/// Params: [in]szFilename
///
/////////////////////////////////////////////////////
int RaspberryPiSoundStream::StreamOpen( const char *szFilename  )
{    
	if( core::IsEmptyString( szFilename ) )
		return(1);

	char extStr[core::MAX_EXT];
	std::memset( extStr, 0, sizeof( char )*core::MAX_EXT );

	// get extension from filename
	//extStr = strchr( szFilename, '.' );
	core::SplitPath( szFilename, 0, 0, 0, extStr );

	// set the sound type based on extension of filename
	if( ( std::strcmp( extStr, ".WAV" ) == 0 ) || ( std::strcmp( extStr, ".wav" ) == 0 ) )
	{
		if( OpenWAV( szFilename ) )
			return(1);

		m_SoundFormat = snd::SOUND_WAV;
	}
#ifdef BASE_SUPPORT_OGG
	else if( ( std::strcmp( extStr, ".OGG" ) == 0 ) || ( std::strcmp( extStr, ".ogg" ) == 0 ) )
	{
		if( OpenOGG( szFilename ) )
			return(1);

		m_SoundFormat = snd::SOUND_OGG;
	}
#endif // BASE_SUPPORT_OGG
	else
	{
		DBGLOG( "RASPBERRYPISOUNDSTREAM: *ERROR* file %s is an unsupported file format\n", szFilename );
		return(1);
	}

	return(0);
}

#ifdef BASE_SUPPORT_OGG
/////////////////////////////////////////////////////
/// Method: OpenOGG
/// Params: [in]szFilename
///
/////////////////////////////////////////////////////
int RaspberryPiSoundStream::OpenOGG( const char *szFilename )
{
	int result;

	if( !file::FileOpen( szFilename, file::FILETYPE_BINARY_READ, &m_FileHandle ) )
	{
		DBGLOG("RASPBERRYPISOUNDSTREAM: *ERROR* Could not open Ogg file\n");
		return(1);
	}

	if( (result = ov_open_callbacks( &m_FileHandle, &m_OggStream, 0, 0, GetOGGCallbacks() )) < 0)
	//if((result = ov_open(m_FileHandle.fp, &m_OggStream, 0, 0)) < 0)
	{
		file::FileClose(&m_FileHandle);
		DBGLOG("RASPBERRYPISOUNDSTREAM: *ERROR* Could not open Ogg stream\n");
		return(1);
	}

	m_VorbisInfo = ov_info(&m_OggStream, -1);
	m_VorbisComment = ov_comment(&m_OggStream, -1);

	if(m_VorbisInfo->channels == 1)
		m_Format = AL_FORMAT_MONO16;
	else
		m_Format = AL_FORMAT_STEREO16;

    // setup the buffer and source    
	CHECK_OPENAL_ERROR( alGenBuffers(STREAM_BUFFER_COUNT, m_BufferID) )
	CHECK_OPENAL_ERROR( alGenSources(1, &m_SourceID) )

    CHECK_OPENAL_ERROR( alSourcef( m_SourceID, AL_GAIN, m_Volume ) )
	CHECK_OPENAL_ERROR( alSourcef( m_SourceID, AL_PITCH, 1.0f ) )
	CHECK_OPENAL_ERROR( alSource3f( m_SourceID, AL_POSITION, 0.0f, 0.0f, 0.0f ) )
	CHECK_OPENAL_ERROR( alSource3f( m_SourceID, AL_VELOCITY, 0.0f, 0.0f, 0.0f ) )
	CHECK_OPENAL_ERROR( alSource3f( m_SourceID, AL_DIRECTION, 0.0f, 0.0f, 0.0f ) )
	CHECK_OPENAL_ERROR( alSourcef( m_SourceID, AL_ROLLOFF_FACTOR, 0.0f ) )
	CHECK_OPENAL_ERROR( alSourcei( m_SourceID, AL_SOURCE_RELATIVE, AL_TRUE ) )	

	return(0);
}
#endif // BASE_SUPPORT_OGG

/////////////////////////////////////////////////////
/// Method: OpenWAV
/// Params: [in]szFilename
///
/////////////////////////////////////////////////////
int RaspberryPiSoundStream::OpenWAV( const char *szFilename )
{
	unsigned int i=0;
	snd::WAVEFileHeader	waveFileHeader;
	snd::RIFFChunk		riffChunk;
	snd::WAVEFmt		waveFmt;

	std::memset( &m_WavFileInfo, 0, sizeof(WAVEFileInfo) );

	// Open the wave file for reading
	if( !file::FileOpen( szFilename, file::FILETYPE_BINARY_READ, &m_FileHandle ) )
	{
		DBGLOG("RASPBERRYPISOUNDSTREAM: *ERROR* Could not open WAV file\n");
		return(1);
	}

	// Read Wave file header
	file::FileRead(&waveFileHeader, sizeof(snd::WAVEFileHeader), 1, &m_FileHandle );

	if(!strncmp(waveFileHeader.szRIFF, "RIFF", 4) && !strncmp(waveFileHeader.szWAVE, "WAVE", 4) )
	{
		while( file::FileRead( &riffChunk, sizeof(snd::RIFFChunk), &m_FileHandle ) == sizeof(snd::RIFFChunk) )
		{
			if(!strncmp(riffChunk.szChunkName, "fmt ", 4))
			{
				if(riffChunk.ulChunkSize <= sizeof(snd::WAVEFmt))
				{
					file::FileRead( &waveFmt, riffChunk.ulChunkSize, 1, &m_FileHandle );
				
					// Determine if this is a WAVEFORMATEX or WAVEFORMATEXTENSIBLE wave file
					if(waveFmt.usFormatTag == snd::WAV_FORMAT_PCM)
					{
						m_WavFileInfo.wfType = snd::WF_EX;
						std::memcpy( &m_WavFileInfo.wfEXT.Format, &waveFmt, sizeof(snd::PCMWAVEFormat) );
					}
					else if(waveFmt.usFormatTag == snd::WAV_FORMAT_EXTENSIBLE)
					{
						m_WavFileInfo.wfType = snd::WF_EXT;
						std::memcpy( &m_WavFileInfo.wfEXT, &waveFmt, sizeof(snd::WAVEFormatExtensible) );
					}
					else if( waveFmt.usFormatTag == snd::WAV_FORMAT_IMA4_ADPCM )
					{
						m_WavFileInfo.wfType = snd::WF_IMA4_ADPCM;
						std::memcpy( &m_WavFileInfo.wfEXT.Format, &waveFmt, sizeof(snd::PCMWAVEFormat) );

						m_WavFileInfo.wfEXT.Format.wBitsPerSample *= 4;
					}
				}
				else
				{
					file::FileSeek( riffChunk.ulChunkSize, file::FILESEEK_CUR, &m_FileHandle );
				}
			}
			else if(!strncmp(riffChunk.szChunkName, "data", 4))
			{
				m_WavFileInfo.ulDataSize = riffChunk.ulChunkSize;
				m_WavFileInfo.ulDataOffset = file::FileTell( &m_FileHandle );
				file::FileSeek( riffChunk.ulChunkSize, file::FILESEEK_CUR, &m_FileHandle );
			}
			else
			{
				file::FileSeek( riffChunk.ulChunkSize, file::FILESEEK_CUR, &m_FileHandle );
			}

			// Ensure that we are correctly aligned for next chunk
			if(riffChunk.ulChunkSize & 1)
				file::FileSeek( 1, file::FILESEEK_CUR, &m_FileHandle );
		}

		if( m_WavFileInfo.ulDataSize && m_WavFileInfo.ulDataOffset && 
			((m_WavFileInfo.wfType == snd::WF_EX) || (m_WavFileInfo.wfType == snd::WF_EXT) || (m_WavFileInfo.wfType == snd::WF_IMA4_ADPCM)) )
		{
			// setup the buffer and source    
			CHECK_OPENAL_ERROR( alGenBuffers(STREAM_BUFFER_COUNT, m_BufferID) )
			CHECK_OPENAL_ERROR( alGenSources(1, &m_SourceID) )

			CHECK_OPENAL_ERROR( alSourcef( m_SourceID, AL_GAIN, m_Volume ) )
			CHECK_OPENAL_ERROR( alSourcef( m_SourceID, AL_PITCH, 1.0f ) )
			CHECK_OPENAL_ERROR( alSource3f( m_SourceID, AL_POSITION, 0.0f, 0.0f, 0.0f ) )
			CHECK_OPENAL_ERROR( alSource3f( m_SourceID, AL_VELOCITY, 0.0f, 0.0f, 0.0f ) )
			CHECK_OPENAL_ERROR( alSource3f( m_SourceID, AL_DIRECTION, 0.0f, 0.0f, 0.0f ) )
			CHECK_OPENAL_ERROR( alSourcef( m_SourceID, AL_ROLLOFF_FACTOR, 0.0f ) )
			CHECK_OPENAL_ERROR( alSourcei( m_SourceID, AL_SOURCE_RELATIVE, AL_TRUE ) )	

			unsigned long	ulBytesWritten = 0;

			snd::GetWaveALBufferFormat( &m_WavFileInfo, &alGetEnumValue, reinterpret_cast<unsigned long *>(&m_Format) );

			if( (m_WavFileInfo.wfType == snd::WF_EX) || (m_WavFileInfo.wfType == snd::WF_EXT) )
			{
				// Queue 250ms of audio data
				m_WaveBufferSize = m_WavFileInfo.wfEXT.Format.nAvgBytesPerSec >> 2;

				// IMPORTANT : The Buffer Size must be an exact multiple of the BlockAlignment ...
				m_WaveBufferSize -= (m_WaveBufferSize % m_WavFileInfo.wfEXT.Format.nBlockAlign);	

				DBG_MEMTRY
					m_pWaveStrmData = new unsigned char[m_WaveBufferSize];
				DBG_MEMCATCH

				// go to beginning of stream data
				file::FileSeek(m_WavFileInfo.ulDataOffset, file::FILESEEK_BEG, &m_FileHandle );

				for (i = 0; i < STREAM_BUFFER_COUNT; i++)
				{
					unsigned long ulBufferSize = m_WaveBufferSize;
					unsigned long ulOffset = file::FileTell( &m_FileHandle );

					if ((ulOffset - m_WavFileInfo.ulDataOffset + ulBufferSize) > m_WavFileInfo.ulDataSize)
						ulBufferSize = m_WavFileInfo.ulDataSize - (ulOffset - m_WavFileInfo.ulDataOffset);

					ulBytesWritten = (unsigned long)file::FileRead( m_pWaveStrmData, ulBufferSize, &m_FileHandle );

					CHECK_OPENAL_ERROR( alBufferData(m_BufferID[i], m_Format, m_pWaveStrmData, ulBytesWritten, m_WavFileInfo.wfEXT.Format.nSamplesPerSec) )
					CHECK_OPENAL_ERROR( alSourceQueueBuffers(m_SourceID, 1, &m_BufferID[i]) )
				}
			}
			else if( m_WavFileInfo.wfType == WF_IMA4_ADPCM )
			{				
				// go to beginning of stream data
				file::FileSeek( m_WavFileInfo.ulDataOffset, file::FILESEEK_BEG, &m_FileHandle );

				for( i=0; i < STREAM_BUFFER_COUNT; ++i )
				{
					StreamIMA4();
					
					CHECK_OPENAL_ERROR( alBufferData(m_BufferID[i], m_Format, m_pWaveStrmData, m_StreamLoadSize, m_WavFileInfo.wfEXT.Format.nSamplesPerSec) )
					CHECK_OPENAL_ERROR( alSourceQueueBuffers(m_SourceID, 1, &m_BufferID[i]) )
				}
			}

			return(0);
		}
	}
	
	file::FileClose( &m_FileHandle );

	return(1);
}

/////////////////////////////////////////////////////
/// Method: Release
/// Params: None 
///
/////////////////////////////////////////////////////
void RaspberryPiSoundStream::Release( void )
{
	if( m_SourceID != INVALID_SOUNDSOURCE )
	{
		CHECK_OPENAL_ERROR( alSourceStop(m_SourceID) )
		m_Playing = false;
		Empty();
	}

	if( m_BufferID[0] != INVALID_SOUNDBUFFER )
	{
		CHECK_OPENAL_ERROR( alDeleteBuffers(snd::STREAM_BUFFER_COUNT, m_BufferID) )
	}

	if( m_SourceID != INVALID_SOUNDSOURCE )
	{
		CHECK_OPENAL_ERROR( alDeleteSources(1, &m_SourceID) )
		m_SourceID = INVALID_SOUNDSOURCE;
	}

	int i = 0;
	for( i = 0; i < STREAM_BUFFER_COUNT; i++ )
		m_BufferID[i] = INVALID_SOUNDBUFFER;

	if( m_pWaveStrmData != 0 )
	{
		delete[] m_pWaveStrmData;
		m_pWaveStrmData = 0;
	}

	if( m_pADPCMData != 0 )
	{
		delete[] m_pADPCMData;
		m_pADPCMData = 0;
	}

#ifdef BASE_SUPPORT_OGG
	if( m_SoundFormat == snd::SOUND_OGG )
		ov_clear(&m_OggStream);
#endif // BASE_SUPPORT_OGG

	m_SoundFormat = snd::SOUND_UNKNOWN;

	file::FileClose( &m_FileHandle );
}

/////////////////////////////////////////////////////
/// Method: Display
/// Params: None 
///
/////////////////////////////////////////////////////
void RaspberryPiSoundStream::Display( void )
{
#ifdef BASE_SUPPORT_OGG
	if( m_SoundFormat == snd::SOUND_OGG )
	{
		DBGLOG( "version         %d\n", m_VorbisInfo->version );
		DBGLOG( "channels        %d\n", m_VorbisInfo->channels  );
		DBGLOG( "rate (hz)       %d\n", m_VorbisInfo->rate  );
		DBGLOG( "bitrate upper   %d\n", m_VorbisInfo->bitrate_upper  );
		DBGLOG( "bitrate nominal %d\n", m_VorbisInfo->bitrate_nominal  );
		DBGLOG( "bitrate lower   %d\n", m_VorbisInfo->bitrate_lower  );
		DBGLOG( "bitrate window  %d\n", m_VorbisInfo->bitrate_window  );
		DBGLOG( "\n" );
//		DBGLOG( "vendor " << m_VorbisComment->vendor );
        
//		for(int i = 0; i < vorbisComment->comments; i++)
//			cout << "   " << vorbisComment->user_comments[i] << "\n";
	}
#endif // BASE_SUPPORT_OGG
}

/////////////////////////////////////////////////////
/// Method: StartPlayback
/// Params: None 
///
/////////////////////////////////////////////////////
bool RaspberryPiSoundStream::StartPlayback( bool loop )
{
	int i = 0;

   	if( m_SourceID == snd::INVALID_SOUNDSOURCE )
		return(false);

	if(IsPlaying())
		return(true);

	Empty();

	for( i = 0; i < STREAM_BUFFER_COUNT; i++ )
	{
		if(!Stream(m_BufferID[i]))
			return(false);
	}
				
	CHECK_OPENAL_ERROR( alSourceQueueBuffers(m_SourceID, snd::STREAM_BUFFER_COUNT, m_BufferID) )
	CHECK_OPENAL_ERROR( alSourcePlay(m_SourceID) )
	m_Loop = loop;
	m_Playing = true;
	return(true);
}

/////////////////////////////////////////////////////
/// Method: Play
/// Params: None
///
/////////////////////////////////////////////////////
bool RaspberryPiSoundStream::Play( void )
{
   	if( m_SourceID == snd::INVALID_SOUNDSOURCE )
		return(false);

	if(!IsPlaying())
	{
		CHECK_OPENAL_ERROR( alSourcePlay(m_SourceID) )
		m_Playing = true;
		return(true);
	}

	return(false);
}

/////////////////////////////////////////////////////
/// Method: Pause
/// Params: None
///
/////////////////////////////////////////////////////
bool RaspberryPiSoundStream::Pause( void )
{
   	if( m_SourceID == snd::INVALID_SOUNDSOURCE )
		return(false);

	if(IsPlaying())
	{
		CHECK_OPENAL_ERROR( alSourcePause(m_SourceID) )
		return(true);
	}

	return(false);
}

/////////////////////////////////////////////////////
/// Method: Stop
/// Params: None
///
/////////////////////////////////////////////////////
bool RaspberryPiSoundStream::Stop( void )
{
   	if( m_SourceID == snd::INVALID_SOUNDSOURCE )
		return(false);

	m_Playing = false;
	
	if(IsPlaying())
	{
		CHECK_OPENAL_ERROR( alSourceStop(m_SourceID) )
		return(true);
	}

	return(false);
}


/////////////////////////////////////////////////////
/// Method: IsPlaying
/// Params: None
///
/////////////////////////////////////////////////////
bool RaspberryPiSoundStream::IsPlaying( void )
{
	ALenum state;

   	if( m_SourceID == snd::INVALID_SOUNDSOURCE )
		return(false);
 
	CHECK_OPENAL_ERROR( alGetSourcei(m_SourceID, AL_SOURCE_STATE, &state) )

	return((state == AL_PLAYING));
}

/////////////////////////////////////////////////////
/// Method: Update
/// Params: None 
///
/////////////////////////////////////////////////////
bool RaspberryPiSoundStream::Update( void )
{
	if( m_SourceID == snd::INVALID_SOUNDSOURCE )
		return(false);

	int processed, state;
	CHECK_OPENAL_ERROR( alGetSourcei(m_SourceID, AL_SOURCE_STATE, &state) )
	
	if( !m_Playing || 
		state == AL_PAUSED )
		return(false);

	bool active = false;

	if( m_SourceID != snd::INVALID_SOUNDSOURCE )
	{
		
		CHECK_OPENAL_ERROR( alGetSourcei(m_SourceID, AL_BUFFERS_PROCESSED, &processed) )

		while(processed > 0)
		{
			ALuint buffer;
	        
			CHECK_OPENAL_ERROR( alSourceUnqueueBuffers(m_SourceID, 1, &buffer) )
			processed--;

			active = Stream(buffer);

			CHECK_OPENAL_ERROR( alSourceQueueBuffers(m_SourceID, 1, &buffer) )
		}

		// Make sure the source hasn't underrun
		if( m_Playing &&
			state != AL_PLAYING && 
			state != AL_PAUSED)
		{
			ALint queued;

			alGetSourcei(m_SourceID, AL_BUFFERS_QUEUED, &queued);
			if(queued == 0)
				return false;

			alSourcePlay(m_SourceID); 
		}
	}

	return(active);
}

/////////////////////////////////////////////////////
/// Method: Stream
/// Params: [in]buffer  
///
/////////////////////////////////////////////////////
bool RaspberryPiSoundStream::Stream(ALuint buffer)
{
	if( m_SourceID == snd::INVALID_SOUNDSOURCE )
		return(false);

#ifdef BASE_SUPPORT_OGG
	if( m_SoundFormat == snd::SOUND_OGG )
	{
		char pcm[snd::OGG_BUFFER_READ_SIZE];
		std::memset( &pcm, 0, sizeof(char)*snd::OGG_BUFFER_READ_SIZE );
		int  size = 0;
		int  section;
		int  result;

		while(size < snd::OGG_BUFFER_READ_SIZE)
		{
			result = ov_read( &m_OggStream, pcm + size, snd::OGG_BUFFER_READ_SIZE - size, core::eMachineEndian, 2, 1, &section );
	    
			if(result > 0)
				size += result;
			else
			{
				if(result < 0)
				{
					DBGLOG( "OGG Stream ERROR\n" );
					return(false);
				}
				else
				{
					// stream had finished...loop ?
					if( m_Loop )
					{
						if( ov_seekable(&m_OggStream) )
						{
							// seek back to the start of the file
							if( ov_time_seek( &m_OggStream, 0.0 ) != 0 )
								DBGLOG( "RASPBERRYPISOUNDSTREAM: *ERROR* Ogg seek error\n" );
						}
					}
					else
					{
						Stop();
						return(false);
					}
				}
			}
		}
	    
		if(size == 0)
			return(false);
	        
		CHECK_OPENAL_ERROR( alBufferData(buffer, m_Format, pcm, size, m_VorbisInfo->rate) )
	}
	else 
#endif // BASE_SUPPORT_OGG
	if( m_SoundFormat == snd::SOUND_WAV )
	{
		if( (m_WavFileInfo.wfType == snd::WF_EX) || (m_WavFileInfo.wfType == snd::WF_EXT) )
		{
			unsigned long	ulBufferSize = m_WaveBufferSize;
			unsigned long	ulBytesWritten = 0;

			unsigned long ulOffset = file::FileTell( &m_FileHandle );

			if ((ulOffset - m_WavFileInfo.ulDataOffset + ulBufferSize) > m_WavFileInfo.ulDataSize)
				ulBufferSize = m_WavFileInfo.ulDataSize - (ulOffset - m_WavFileInfo.ulDataOffset);

			ulBytesWritten = (unsigned long)file::FileRead( m_pWaveStrmData, ulBufferSize, &m_FileHandle );

			if (ulBytesWritten)
			{
				// Copy audio data to Buffer
				CHECK_OPENAL_ERROR( alBufferData(buffer, m_Format, m_pWaveStrmData, ulBytesWritten, m_WavFileInfo.wfEXT.Format.nSamplesPerSec) )
			}	
			else
			{
				// back to start of data
				file::FileSeek( m_WavFileInfo.ulDataOffset, file::FILESEEK_BEG, &m_FileHandle );

				if ((ulOffset - m_WavFileInfo.ulDataOffset + ulBufferSize) > m_WavFileInfo.ulDataSize)
					ulBufferSize = m_WavFileInfo.ulDataSize - (ulOffset - m_WavFileInfo.ulDataOffset);

				ulBytesWritten = (unsigned long)file::FileRead( m_pWaveStrmData, ulBufferSize, &m_FileHandle );

				CHECK_OPENAL_ERROR( alBufferData(buffer, m_Format, m_pWaveStrmData, ulBytesWritten, m_WavFileInfo.wfEXT.Format.nSamplesPerSec) )
			}
		}
		else if( m_WavFileInfo.wfType == snd::WF_IMA4_ADPCM )
		{
			StreamIMA4();

			if (m_StreamLoadSize)
			{
				// Copy audio data to Buffer
				CHECK_OPENAL_ERROR( alBufferData(buffer, m_Format, m_pWaveStrmData, m_StreamLoadSize, m_WavFileInfo.wfEXT.Format.nSamplesPerSec) )
			}	
			else
			{
				// back to start of data
				file::FileSeek( m_WavFileInfo.ulDataOffset, file::FILESEEK_BEG, &m_FileHandle );

				StreamIMA4();

/*				if ((ulOffset - m_WavFileInfo.ulDataOffset + ulBufferSize) > m_WavFileInfo.ulDataSize)
					ulBufferSize = m_WavFileInfo.ulDataSize - (ulOffset - m_WavFileInfo.ulDataOffset);

				ulBytesWritten = (unsigned long)file::FileRead( m_pWaveStrmData, ulBufferSize, &m_FileHandle );

				CHECK_OPENAL_ERROR( alBufferData(buffer, m_Format, m_pWaveStrmData, ulBytesWritten, m_WavFileInfo.wfEXT.Format.nSamplesPerSec) )
*/
			}
		}
	}
    
	return(true);
}

/////////////////////////////////////////////////////
/// Method: StreamIMA4
/// Params: 
///
/////////////////////////////////////////////////////
void RaspberryPiSoundStream::StreamIMA4()
{
/*	const int MAX_IMA_CHANNELS = 2;

	// queue 250ms of audio data
	unsigned long ulBufferSize = m_WavFileInfo.wfEXT.Format.nAvgBytesPerSec >> 2;

	// IMPORTANT : the buffer size must be an exact multiple of the BlockAlignment ...
	ulBufferSize -= (ulBufferSize % m_WavFileInfo.wfEXT.Format.nBlockAlign);

	// compressed buffer
	if( m_pADPCMData != 0 )
	{
		delete[] m_pADPCMData;
		m_pADPCMData = 0;
	}
				
	// find out where in the file the stream currently is
	unsigned long ulOffset = file::FileTell( &m_FileHandle );

	// keep within valid buffer range
	if ((ulOffset - m_WavFileInfo.ulDataOffset + ulBufferSize) > m_WavFileInfo.ulDataSize)
		ulBufferSize = m_WavFileInfo.ulDataSize - (ulOffset - m_WavFileInfo.ulDataOffset);

	DBG_MEMTRY
		m_pADPCMData = new unsigned char[ulBufferSize];
	DBG_MEMCATCH

	std::memset( m_pADPCMData, 0, sizeof(unsigned char)*ulBufferSize );

	// read in compressed buffer
	unsigned long readSize = 0;
	readSize = (unsigned long)file::FileRead( m_pADPCMData, ulBufferSize, &m_FileHandle );
	//DBG_ASSERT( readSize != 0 );

	// work out the final uncompressed size
	if( m_pWaveStrmData != 0 )
	{
		delete[] m_pWaveStrmData;
		m_pWaveStrmData = 0;
	}
	int blocks = (ulBufferSize/m_WavFileInfo.wfEXT.Format.nBlockAlign);
	m_StreamLoadSize = (m_WavFileInfo.wfEXT.Format.nBlockAlign-m_WavFileInfo.wfEXT.Format.nChannels)*blocks * 4;
	DBG_MEMTRY
		m_pWaveStrmData = new signed short[m_StreamLoadSize];
	DBG_MEMCATCH
	std::memset( m_pWaveStrmData, 0, sizeof(signed short)*m_StreamLoadSize );

	unsigned char *d = (unsigned char *)m_pADPCMData;
	signed short *ptr = 0;

	ptr = (signed short *)m_pWaveStrmData;

	for( int b = 0; b < blocks; b++ )
	{
		signed short predictor[MAX_IMA_CHANNELS];
		unsigned char nibble, index[MAX_IMA_CHANNELS];
		unsigned int j, chn;

		for( chn=0; chn < m_WavFileInfo.wfEXT.Format.nChannels; chn++ )
		{
			predictor[chn] = *d++;
			predictor[chn] |= *d++ << 8;

			index[chn] = *d++;
			d++;
		}

		for( j=m_WavFileInfo.wfEXT.Format.nChannels*4; j < m_WavFileInfo.wfEXT.Format.nBlockAlign; )
		{
			for( chn = 0; chn < m_WavFileInfo.wfEXT.Format.nChannels; chn++ )
			{
				signed short *ptr_ch = 0;
				unsigned int q;

				ptr_ch = ptr + chn;
				for (q=0; q<4; q++)
				{
					nibble = *d & 0xf;
					*ptr_ch = ima2linear(nibble, &predictor[chn], &index[chn]);
					ptr_ch += m_WavFileInfo.wfEXT.Format.nChannels;

					nibble = *d++ >> 4;
					*ptr_ch = ima2linear(nibble, &predictor[chn], &index[chn]);
					ptr_ch += m_WavFileInfo.wfEXT.Format.nChannels;
				}
			}
			j += m_WavFileInfo.wfEXT.Format.nChannels*4;
			ptr += m_WavFileInfo.wfEXT.Format.nChannels*8;
		}
	}
*/
}

/////////////////////////////////////////////////////
/// Method: Empty
/// Params: None
///
/////////////////////////////////////////////////////
void RaspberryPiSoundStream::Empty( void )
{
	int queued;
	if( m_SourceID == snd::INVALID_SOUNDSOURCE )
		return;

	CHECK_OPENAL_ERROR( alGetSourcei(m_SourceID, AL_BUFFERS_QUEUED, &queued) )
    
	if( queued > 0 )
	{
		while(queued--)
		{
			ALuint buffer;
		
			CHECK_OPENAL_ERROR( alSourceUnqueueBuffers(m_SourceID, 1, &buffer) )
		}
	}
}

/////////////////////////////////////////////////////
/// Method: SetVolume
/// Params: [in]fVolume
///
/////////////////////////////////////////////////////
void RaspberryPiSoundStream::SetVolume( float fVolume )
{
	if( m_SourceID == snd::INVALID_SOUNDSOURCE )
		return;

	if( fVolume > 2.0f )
		fVolume = 2.0f;

	if( fVolume < 0.0f )
		fVolume = 0.0f;

	m_Volume = fVolume;

	CHECK_OPENAL_ERROR( alSourcef( m_SourceID, AL_GAIN, m_Volume ) )
}

#endif // BASE_SUPPORT_OPENAL
#endif // BASE_PLATFORM_RASPBERRYPI

