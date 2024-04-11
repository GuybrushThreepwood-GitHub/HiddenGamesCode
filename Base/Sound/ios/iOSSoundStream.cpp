
/*===================================================================
	File: iOSSoundStream.cpp
	Library: SoundLib

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"

#ifdef BASE_PLATFORM_iOS
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
#include "Sound/ios/iOSSoundStream.h"

using snd::iOSSoundStream;

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
iOSSoundStream::iOSSoundStream()
{
	int i = 0;
	m_SoundFormat = snd::SOUND_UNKNOWN;

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
iOSSoundStream::~iOSSoundStream()
{
	Release();
}


/////////////////////////////////////////////////////
/// Method: StreamOpen
/// Params: [in]szFilename
///
/////////////////////////////////////////////////////
int iOSSoundStream::StreamOpen( const char *szFilename  )
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
		DBGLOG( "IOSSOUNDSTREAM: *ERROR* file %s is an unsupported file format\n", szFilename );
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
int iOSSoundStream::OpenOGG( const char *szFilename )
{
	int result;

	if( !file::FileOpen( szFilename, file::FILETYPE_BINARY_READ, &m_FileHandle ) )
	{
		DBGLOG("IOSSOUNDSTREAM: *ERROR* Could not open Ogg file\n");
		return(1);
	}

	if( (result = ov_open_callbacks( &m_FileHandle, &m_OggStream, 0, 0, GetOGGCallbacks() )) < 0)
	//if((result = ov_open(m_FileHandle.fp, &m_OggStream, 0, 0)) < 0)
	{
		file::FileClose(&m_FileHandle);
		DBGLOG("IOSSOUNDSTREAM: *ERROR* Could not open Ogg stream\n");
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
int iOSSoundStream::OpenWAV( const char *szFilename )
{
	OSStatus err = noErr;
	
	// get some audio data from a wave file
	CFURLRef inFileURL = CFURLCreateWithBytes ( 0, (const UInt8 *)szFilename, std::strlen(szFilename)+1, kCFStringEncodingASCII, 0 );
	
	err = ExtAudioFileOpenURL(inFileURL, &m_ExtFileRef);
	if(err) 
	{ 
        DBGLOG("IOSSOUNDSTREAM: *ERROR* ExtAudioFileOpenURL FAILED, Error = %d\n", (int)err); 
		if (m_ExtFileRef) 
			ExtAudioFileDispose(m_ExtFileRef);
		
		return(1);
	}
	
	// Get the audio data format
	err = ExtAudioFileGetProperty(m_ExtFileRef, kExtAudioFileProperty_FileDataFormat, &m_PropertySize, &m_CompressedFormat);
	if(err) 
	{ 
        DBGLOG("IOSSOUNDSTREAM: *ERROR* ExtAudioFileGetProperty(kExtAudioFileProperty_FileDataFormat) FAILED, Error = %d\n", (int)err); 
		if (m_ExtFileRef) 
			ExtAudioFileDispose(m_ExtFileRef);
		
		return(1); 
	}
	if (m_CompressedFormat.mChannelsPerFrame > 2)  
	{ 
		DBGLOG("IOSSOUNDSTREAM: *ERROR* Unsupported Format, channel count is greater than stereo\n"); 
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
        DBGLOG("IOSSOUNDSTREAM: *ERROR* ExtAudioFileSetProperty(kExtAudioFileProperty_ClientDataFormat) FAILED, Error = %d\n", (int)err); 
		if (m_ExtFileRef) 
			ExtAudioFileDispose(m_ExtFileRef);
		
		return(1);
	}
	
	// Get the total frame count
	m_PropertySize = sizeof(m_FileLengthInFrames);
	err = ExtAudioFileGetProperty(m_ExtFileRef, kExtAudioFileProperty_FileLengthFrames, &m_PropertySize, &m_FileLengthInFrames);
	if(err) 
	{ 
        DBGLOG("IOSSOUNDSTREAM: *ERROR* ExtAudioFileGetProperty(kExtAudioFileProperty_FileLengthFrames) FAILED, Error = %d\n", (int)err); 
		if (m_ExtFileRef) 
			ExtAudioFileDispose(m_ExtFileRef);
		
		return(1); 
	}
	
	m_Format = (m_OutputFormat.mChannelsPerFrame > 1) ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16;
	
	err = ExtAudioFileTell ( m_ExtFileRef, &m_StartFrame );	
	if(err) 
	{ 
        DBGLOG("IOSSOUNDSTREAM: *ERROR* ExtAudioFileTell FAILED, Error = %d\n", (int)err); 
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
/// Method: Release
/// Params: None 
///
/////////////////////////////////////////////////////
void iOSSoundStream::Release( void )
{
	if( m_SourceID != INVALID_SOUNDSOURCE )
	{
		CHECK_OPENAL_ERROR( alSourceStop(m_SourceID) )
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
void iOSSoundStream::Display( void )
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
bool iOSSoundStream::StartPlayback( void )
{
	int i = 0;

	if(IsPlaying())
		return(true);

	for( i = 0; i < STREAM_BUFFER_COUNT; i++ )
	{
		if(!Stream(m_BufferID[i]))
			return(false);
	}
				
	CHECK_OPENAL_ERROR( alSourceQueueBuffers(m_SourceID, snd::STREAM_BUFFER_COUNT, m_BufferID) )
	CHECK_OPENAL_ERROR( alSourcePlay(m_SourceID) )

	return(true);
}

/////////////////////////////////////////////////////
/// Method: Play
/// Params: None
///
/////////////////////////////////////////////////////
bool iOSSoundStream::Play( void )
{
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
bool iOSSoundStream::Pause( void )
{
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
bool iOSSoundStream::Stop( void )
{
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
bool iOSSoundStream::IsPlaying( void )
{
	ALenum state;
    
   	if( m_SourceID == snd::INVALID_SOUNDSOURCE )
		return(false);
	
	CHECK_OPENAL_ERROR( alGetSourcei(m_SourceID, AL_SOURCE_STATE, &state) )
    
	return(state == AL_PLAYING);
}

/////////////////////////////////////////////////////
/// Method: Update
/// Params: None 
///
/////////////////////////////////////////////////////
bool iOSSoundStream::Update( void )
{
   	if( m_SourceID == snd::INVALID_SOUNDSOURCE )
		return(false);
	
	if( !IsPlaying() )
		return(false);

	int processed;
	bool active = false;

	CHECK_OPENAL_ERROR( alGetSourcei(m_SourceID, AL_BUFFERS_PROCESSED, &processed) )

	while(processed--)
	{
		ALuint buffer;
        
		CHECK_OPENAL_ERROR( alSourceUnqueueBuffers(m_SourceID, 1, &buffer) )

		Stream(buffer);

		CHECK_OPENAL_ERROR( alSourceQueueBuffers(m_SourceID, 1, &buffer) )

		active = true;
	}

	return(active);
}

/////////////////////////////////////////////////////
/// Method: Stream
/// Params: [in]buffer  
///
/////////////////////////////////////////////////////
bool iOSSoundStream::Stream(ALuint buffer)
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
					if( ov_seekable(&m_OggStream) )
					{
						// seek back to the start of the file
						if( ov_time_seek( &m_OggStream, 0.0 ) != 0 )
							DBGLOG( "IOSSOUNDSTREAM: *ERROR* Ogg seek error\n" );
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
void iOSSoundStream::StreamIMA4()
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
                DBGLOG("IOSSOUNDSTREAM: *ERROR* ExtAudioFileSeek FAILED, Error = %d\n", (int)err); 
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
            DBGLOG("IOSSOUNDSTREAM: *ERROR* ExtAudioFileRead FAILED, Error = %d\n", (int)err); 
			
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
void iOSSoundStream::Empty( void )
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
void iOSSoundStream::SetVolume( float fVolume )
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
#endif // BASE_PLATFORM_iOS

