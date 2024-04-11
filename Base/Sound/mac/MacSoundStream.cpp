
/*===================================================================
	File: MacSoundStream.cpp
	Library: SoundLib

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"

#ifdef BASE_PLATFORM_MAC
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
#include "Sound/mac/MacSoundStream.h"

using snd::MacSoundStream;

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
MacSoundStream::MacSoundStream()
{
	int i = 0;
	m_SoundFormat = snd::SOUND_UNKNOWN;

	m_Playing = false;
	m_Loop = false;
	
	for( i = 0; i < STREAM_BUFFER_COUNT; i++ )
		m_BufferID[i] = INVALID_SOUNDBUFFER;

	m_SourceID = INVALID_SOUNDSOURCE;
	m_Volume = 1.0f;

	m_FileLengthInFrames = 0;
	m_PropertySize = sizeof(m_CompressedFormat);
	m_ExtFileRef = 0;
	m_pData = 0;
	m_StartFrame = 0;

}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
MacSoundStream::~MacSoundStream()
{
	Release();
}


/////////////////////////////////////////////////////
/// Method: StreamOpen
/// Params: [in]szFilename
///
/////////////////////////////////////////////////////
int MacSoundStream::StreamOpen( const char *szFilename  )
{    
	if( core::IsEmptyString( szFilename ) )
		return(1);

	char extStr[core::MAX_EXT];
	std::memset( extStr, 0, sizeof( char )*core::MAX_EXT );

	// get extension from filename
	//extStr = strchr( szFilename, '.' );
	core::SplitPath( szFilename, 0, 0, 0, extStr );

	// set the sound type based on extension of filename
	if( ( std::strcmp( extStr, ".WAV" ) == 0 ) || ( std::strcmp( extStr, ".wav" ) == 0 ) ||
	   ( std::strcmp( extStr, ".CAF" ) == 0 ) || ( std::strcmp( extStr, ".caf" ) == 0 ) )
	{
		if( OpenWAV2( szFilename ) )
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
		DBGLOG( "MACSOUNDSTREAM: *ERROR* file %s is an unsupported file format\n", szFilename );
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
int MacSoundStream::OpenOGG( const char *szFilename )
{
	int result;

	if( !file::FileOpen( szFilename, file::FILETYPE_BINARY_READ, &m_FileHandle ) )
	{
		DBGLOG("MACSOUNDSTREAM: *ERROR* Could not open Ogg file\n");
		return(1);
	}

	if( (result = ov_open_callbacks( &m_FileHandle, &m_OggStream, 0, 0, GetOGGCallbacks() )) < 0)
	//if((result = ov_open(m_FileHandle.fp, &m_OggStream, 0, 0)) < 0)
	{
		file::FileClose(&m_FileHandle);
		DBGLOG("MACSOUNDSTREAM: *ERROR* Could not open Ogg stream\n");
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
int MacSoundStream::OpenWAV( const char *szFilename )
{
	OSStatus err = noErr;
	
	// get some audio data from a wave file
	CFURLRef inFileURL = CFURLCreateWithBytes ( 0, (const UInt8 *)szFilename, std::strlen(szFilename)+1, kCFStringEncodingASCII, 0 );
	
	err = ExtAudioFileOpenURL(inFileURL, &m_ExtFileRef);
	if(err) 
	{ 
		DBGLOG("MACSOUNDSTREAM: *ERROR* ExtAudioFileOpenURL FAILED, Error = %d\n", static_cast<int>(err)); 
		if (m_ExtFileRef) 
			ExtAudioFileDispose(m_ExtFileRef);
		
		return(1);
	}
	
	// Get the audio data format
	err = ExtAudioFileGetProperty(m_ExtFileRef, kExtAudioFileProperty_FileDataFormat, &m_PropertySize, &m_CompressedFormat);
	if(err) 
	{ 
		DBGLOG("MACSOUNDSTREAM: *ERROR* ExtAudioFileGetProperty(kExtAudioFileProperty_FileDataFormat) FAILED, Error = %d\n", static_cast<int>(err));
		if (m_ExtFileRef) 
			ExtAudioFileDispose(m_ExtFileRef);
		
		return(1); 
	}
	if (m_CompressedFormat.mChannelsPerFrame > 2)  
	{ 
		DBGLOG("MACSOUNDSTREAM: *ERROR* Unsupported Format, channel count is greater than stereo\n"); 
		if (m_ExtFileRef) 
			ExtAudioFileDispose(m_ExtFileRef);
		
		return(1);
	}
	
	// Set the client format to 16 bit signed integer (native-endian) data
	// Maintain the channel count and sample rate of the original source format
	m_OutputFormat.mSampleRate = m_CompressedFormat.mSampleRate;
	m_OutputFormat.mChannelsPerFrame = m_CompressedFormat.mChannelsPerFrame;
	
	m_OutputFormat.mFormatID = kAudioFormatLinearPCM;
	m_OutputFormat.mBytesPerPacket = 2 * m_CompressedFormat.mChannelsPerFrame;
	m_OutputFormat.mFramesPerPacket = 1;
	m_OutputFormat.mBytesPerFrame = 2 * m_CompressedFormat.mChannelsPerFrame;
	m_OutputFormat.mBitsPerChannel = 16;
	m_OutputFormat.mFormatFlags = kAudioFormatFlagsNativeEndian | kAudioFormatFlagIsPacked | kAudioFormatFlagIsSignedInteger;
	
	// Set the desired client (output) data format
	err = ExtAudioFileSetProperty(m_ExtFileRef, kExtAudioFileProperty_ClientDataFormat, sizeof(m_OutputFormat), &m_OutputFormat);
	if(err) 
	{ 
		DBGLOG("MACSOUNDSTREAM: *ERROR* ExtAudioFileSetProperty(kExtAudioFileProperty_ClientDataFormat) FAILED, Error = %d\n", static_cast<int>(err)); 
		if (m_ExtFileRef) 
			ExtAudioFileDispose(m_ExtFileRef);
		
		return(1);
	}
	
	// Get the total frame count
	m_PropertySize = sizeof(m_FileLengthInFrames);
	err = ExtAudioFileGetProperty(m_ExtFileRef, kExtAudioFileProperty_FileLengthFrames, &m_PropertySize, &m_FileLengthInFrames);
	if(err) 
	{ 
		DBGLOG("MACSOUNDSTREAM: *ERROR* ExtAudioFileGetProperty(kExtAudioFileProperty_FileLengthFrames) FAILED, Error = %d\n", static_cast<int>(err)); 
		if (m_ExtFileRef) 
			ExtAudioFileDispose(m_ExtFileRef);
		
		return(1); 
	}
	
	m_Format = (m_OutputFormat.mChannelsPerFrame > 1) ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16;
	
	err = ExtAudioFileTell ( m_ExtFileRef, &m_StartFrame );	
	if(err) 
	{ 
		DBGLOG("MACSOUNDSTREAM: *ERROR* ExtAudioFileTell FAILED, Error = %d\n", static_cast<int>(err)); 
		if (m_ExtFileRef) 
			ExtAudioFileDispose(m_ExtFileRef);
		
		return(1); 
	}
	
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

	//for( i=0; i < STREAM_BUFFER_COUNT; ++i )
	//{
	//	Stream( m_BufferID[i] );
	//	CHECK_OPENAL_ERROR( alSourceQueueBuffers(m_SourceID, 1, &m_BufferID[i]) )
	//}

	return(0);
}

/////////////////////////////////////////////////////
/// Method: OpenWAV
/// Params: [in]szFilename
///
/////////////////////////////////////////////////////
int MacSoundStream::OpenWAV2( const char *szFilename )
{
	unsigned int i=0;
	snd::WAVEFileHeader	waveFileHeader;
	snd::RIFFChunk		riffChunk;
	snd::WAVEFmt		waveFmt;
	
	std::memset( &m_WavFileInfo, 0, sizeof(WAVEFileInfo) );
	
	// Open the wave file for reading
	if( !file::FileOpen( szFilename, file::FILETYPE_BINARY_READ, &m_FileHandle ) )
	{
		DBGLOG("WINSOUNDSTREAM: *ERROR* Could not open WAV file\n");
		return(1);
	}
	
	// Read Wave file header
	file::FileRead(&waveFileHeader, sizeof(snd::WAVEFileHeader), 1, &m_FileHandle );
	
	if(!strncmp(waveFileHeader.szRIFF, "RIFF", 4) &&
	   !strncmp(waveFileHeader.szWAVE, "WAVE", 4) )
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
void MacSoundStream::Release( void )
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

	if(m_ExtFileRef) 
		ExtAudioFileDispose(m_ExtFileRef);

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
void MacSoundStream::Display( void )
{
#ifdef BASE_SUPPORT_OGG
	if( m_SoundFormat == snd::SOUND_OGG )
	{
		DBGLOG( "version         %d\n", m_VorbisInfo->version );
		DBGLOG( "channels        %d\n", m_VorbisInfo->channels  );
		DBGLOG( "rate (hz)       %ld\n", m_VorbisInfo->rate  );
		DBGLOG( "bitrate upper   %ld\n", m_VorbisInfo->bitrate_upper  );
		DBGLOG( "bitrate nominal %ld\n", m_VorbisInfo->bitrate_nominal  );
		DBGLOG( "bitrate lower   %ld\n", m_VorbisInfo->bitrate_lower  );
		DBGLOG( "bitrate window  %ld\n", m_VorbisInfo->bitrate_window  );
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
bool MacSoundStream::StartPlayback( bool loop )
{
	int i = 0;

   	if( m_SourceID == snd::INVALID_SOUNDSOURCE )
		return(false);
	
	if(IsPlaying())
		return(true);

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
bool MacSoundStream::Play( void )
{
   	if( m_SourceID == snd::INVALID_SOUNDSOURCE )
		return(false);
	
	if(!IsPlaying())
	{
		CHECK_OPENAL_ERROR( alSourcePlay(m_SourceID) )
		return(true);
	}

	return(false);
}

/////////////////////////////////////////////////////
/// Method: Pause
/// Params: None
///
/////////////////////////////////////////////////////
bool MacSoundStream::Pause( void )
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
bool MacSoundStream::Stop( void )
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
bool MacSoundStream::IsPlaying( void )
{
	ALenum state;
    
	if( m_SourceID == snd::INVALID_SOUNDSOURCE )
		return false;
	
	CHECK_OPENAL_ERROR( alGetSourcei(m_SourceID, AL_SOURCE_STATE, &state) )
    
	return(state == AL_PLAYING);
}

/////////////////////////////////////////////////////
/// Method: Update
/// Params: None 
///
/////////////////////////////////////////////////////
bool MacSoundStream::Update( void )
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
bool MacSoundStream::Stream(ALuint buffer)
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
								DBGLOG( "WINSOUNDSTREAM: *ERROR* Ogg seek error\n" );
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

	}
    
	return(true);
}

/////////////////////////////////////////////////////
/// Method: StreamIMA4
/// Params: 
///
/////////////////////////////////////////////////////
void MacSoundStream::StreamIMA4()
{
	OSStatus err = noErr;
	
	// Read all the data into memory
	if(m_pData)
	{
		delete[] m_pData;
		m_pData = 0;
	}
	//const int BUFFER = 16*1024;
	UInt64 streamSize = (m_FileLengthInFrames * m_OutputFormat.mBytesPerFrame);
	
	// divide into 16k chunk
	//UInt64 chunkSize = streamSize / BUFFER;
	
	m_StreamLoadSize = streamSize;
	
	DBG_MEMTRY
		m_pData = new unsigned char[m_StreamLoadSize];
	DBG_MEMCATCH

	if (m_pData)
	{
		AudioBufferList	dataBuffer;
		dataBuffer.mNumberBuffers = 1;
		dataBuffer.mBuffers[0].mDataByteSize = m_StreamLoadSize;
		dataBuffer.mBuffers[0].mNumberChannels = m_OutputFormat.mChannelsPerFrame;
		dataBuffer.mBuffers[0].mData = m_pData;
		
		// Read the data into an AudioBufferList
		err = ExtAudioFileRead(m_ExtFileRef, (UInt32*)&m_StreamLoadSize, &dataBuffer);
		
		if( m_StreamLoadSize == 0 )
		{
			err = ExtAudioFileSeek( m_ExtFileRef, m_StartFrame );			
			if(err) 
			{ 
				DBGLOG("MACSOUNDSTREAM: *ERROR* ExtAudioFileSeek FAILED, Error = %d\n", err);
			}			
		}
		
		if(err == noErr)
		{

		}
		else 
		{ 
			// failure
			delete [] m_pData;
			m_pData = 0;
			DBGLOG("MACSOUNDSTREAM: *ERROR* ExtAudioFileRead FAILED, Error = %d\n", err); 
			
			if (m_ExtFileRef) 
				ExtAudioFileDispose(m_ExtFileRef);
			
			return;
		}	
	}
}

/////////////////////////////////////////////////////
/// Method: Empty
/// Params: None
///
/////////////////////////////////////////////////////
void MacSoundStream::Empty( void )
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
void MacSoundStream::SetVolume( float fVolume )
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
#endif // BASE_PLATFORM_MAC

