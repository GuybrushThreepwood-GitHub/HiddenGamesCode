

/*===================================================================
	File: AndroidSound.cpp
	Library: Sound

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"

#ifdef BASE_PLATFORM_ANDROID
#ifdef BASE_SUPPORT_OPENAL

#ifdef BASE_SUPPORT_OGG
	#include <ogg/ogg.h>
	#include <vorbis/vorbisfile.h>
#endif // BASE_SUPPORT_OGG

#include "Math/Vectors.h"

#include "Sound/OpenAL.h"
#include "Sound/SoundCommon.h"
#include "Sound/SoundCodecs.h"
#include "Sound/aos/AndroidSound.h"

using snd::AndroidSound;

/////////////////////////////////////////////////////
/// Default constructor
/// 
///
/////////////////////////////////////////////////////
AndroidSound::AndroidSound()
{
	Initialise();
}

/////////////////////////////////////////////////////
/// Default destructor
/// 
///
/////////////////////////////////////////////////////
AndroidSound::~AndroidSound()
{
	Release();
}

/////////////////////////////////////////////////////
/// Method: Initialise
/// Params: 
///
/////////////////////////////////////////////////////
void AndroidSound::Initialise( void )
{
	eFileFormat = SOUND_UNKNOWN;

	nBufferID = snd::INVALID_SOUNDBUFFER;

	std::memset( &SoundFile, 0, sizeof( file::TFile ) );
	nBitRate = 0;
	nFrequency = 0;
	nChannels = 0;
	nBytesPerSec = 0;
	nSoundSize = 0;

	m_Buffer = 0;

	std::memset( &m_WavFileInfo, 0, sizeof(snd::WAVEFileInfo) );
}

/////////////////////////////////////////////////////
/// Method: Release
/// Params: 
///
/////////////////////////////////////////////////////
void AndroidSound::Release( void )
{
	if( m_Buffer )
	{
		delete[] m_Buffer;
		m_Buffer = 0;
	}
}

/////////////////////////////////////////////////////
/// Method: Load
/// Params: [in]szFilename
///
/////////////////////////////////////////////////////
int AndroidSound::Load( const char *szFilename )
{
	char extStr[core::MAX_EXT];
	std::memset( extStr, 0, sizeof( char )*core::MAX_EXT );

	Initialise();

	// get extension from filename
	core::SplitPath( szFilename, 0, 0, 0, extStr );

	// set the sound type based on extension of filename
	if( ( std::strcmp( extStr, ".WAV" ) == 0 ) || ( std::strcmp( extStr, ".wav" ) == 0 ) )
	{
		return( LoadWAV( szFilename ) );
	}
#ifdef BASE_SUPPORT_OGG
	else if( ( std::strcmp( extStr, ".OGG" ) == 0 ) || ( std::strcmp( extStr, ".ogg" ) == 0 ) )
	{
		return( LoadOGG( szFilename ) );
	}
#endif // BASE_SUPPORT_OGG

	DBGLOG( "ANDROIDSOUND: *ERROR* file %s is an unsupported file format\n", szFilename );

	return(1);
}

/////////////////////////////////////////////////////
/// Method: LoadWAV
/// Params: [in]szFilename
///
/////////////////////////////////////////////////////
int AndroidSound::LoadWAV( const char *szFilename )
{
	unsigned int i=0;
	file::TFileHandle fileHandle;
	snd::WAVEFileHeader	waveFileHeader;
	snd::RIFFChunk		riffChunk;
	snd::WAVEFmt		waveFmt;

	std::memset( &m_WavFileInfo, 0, sizeof(WAVEFileInfo) );

	// Open the wave file for reading
	if( !file::FileOpen( szFilename, file::FILETYPE_BINARY_READ, &fileHandle ) )
	{
		DBGLOG("ANDROIDSOUND: *ERROR* Could not open WAV file\n");
		return(1);
	}

	// Read Wave file header
	file::FileRead(&waveFileHeader, sizeof(snd::WAVEFileHeader), 1, &fileHandle );

	if(!strncmp(waveFileHeader.szRIFF, "RIFF", 4) && !strncmp(waveFileHeader.szWAVE, "WAVE", 4) )
	{
		while( file::FileRead( &riffChunk, sizeof(snd::RIFFChunk), &fileHandle ) == sizeof(snd::RIFFChunk) )
		{
			if(!strncmp(riffChunk.szChunkName, "fmt ", 4))
			{
				if(riffChunk.ulChunkSize <= sizeof(snd::WAVEFmt))
				{
					file::FileRead( &waveFmt, riffChunk.ulChunkSize, 1, &fileHandle );
				
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
					file::FileSeek( riffChunk.ulChunkSize, file::FILESEEK_CUR, &fileHandle );
				}
			}
			else if(!strncmp(riffChunk.szChunkName, "data", 4))
			{
				m_WavFileInfo.ulDataSize = riffChunk.ulChunkSize;
				m_WavFileInfo.ulDataOffset = file::FileTell( &fileHandle );
				file::FileSeek( riffChunk.ulChunkSize, file::FILESEEK_CUR, &fileHandle );
			}
			else
			{
				file::FileSeek( riffChunk.ulChunkSize, file::FILESEEK_CUR, &fileHandle );
			}

			// Ensure that we are correctly aligned for next chunk
			if(riffChunk.ulChunkSize & 1)
				file::FileSeek( 1, file::FILESEEK_CUR, &fileHandle );
		}

		if( m_WavFileInfo.ulDataSize && m_WavFileInfo.ulDataOffset && 
			((m_WavFileInfo.wfType == snd::WF_EX) || (m_WavFileInfo.wfType == snd::WF_EXT) || (m_WavFileInfo.wfType == snd::WF_IMA4_ADPCM)) )
		{
			// setup the buffer   
			CHECK_OPENAL_ERROR( alGenBuffers(1, &nBufferID) );

			unsigned long ulBytesWritten = 0;

			snd::GetWaveALBufferFormat( &m_WavFileInfo, &alGetEnumValue, reinterpret_cast<unsigned long *>(&nFormat) );

			//nBitRate = m_WavFileInfo.wfEXT.wBitsPerSample.;
			//nFrequency = m_WavFileInfo.wfEXT.Format.;
			nChannels = m_WavFileInfo.wfEXT.Format.nChannels;
			nBytesPerSec = m_WavFileInfo.wfEXT.Format.nSamplesPerSec;
			nSoundSize = m_WavFileInfo.ulDataSize;

			if( (m_WavFileInfo.wfType == snd::WF_EX) || (m_WavFileInfo.wfType == snd::WF_EXT) )
			{
				DBG_MEMTRY
					m_Buffer = new char[nSoundSize];
				DBG_MEMCATCH

				// go to beginning of data
				file::FileSeek( m_WavFileInfo.ulDataOffset, file::FILESEEK_BEG, &fileHandle );

				// read linear
				ulBytesWritten = (unsigned long)file::FileRead( m_Buffer, nSoundSize, &fileHandle );

				// create the AL buffer
				CHECK_OPENAL_ERROR( alBufferData(nBufferID, nFormat, m_Buffer, ulBytesWritten, m_WavFileInfo.wfEXT.Format.nSamplesPerSec) )

				// cleanup
				Free();
			}
			else if( m_WavFileInfo.wfType == WF_IMA4_ADPCM )
			{
				// setup the buffer   
				CHECK_OPENAL_ERROR( alGenBuffers(1, &nBufferID) );

				snd::GetWaveALBufferFormat( &m_WavFileInfo, &alGetEnumValue, reinterpret_cast<unsigned long *>(&nFormat) );

				const int MAX_IMA_CHANNELS = 2;
				char* pADPCMData = 0;
	
				// go to beginning of data
				file::FileSeek( m_WavFileInfo.ulDataOffset, file::FILESEEK_BEG, &fileHandle );

				// find out where in the file the stream currently is
				unsigned long ulOffset = file::FileTell( &fileHandle );

				DBG_MEMTRY
					pADPCMData = new char[nSoundSize];
				DBG_MEMCATCH

				// read in compressed buffer
				unsigned long readSize = 0;
				readSize = (unsigned long)file::FileRead( pADPCMData, nSoundSize, &fileHandle );
				//DBG_ASSERT( readSize != 0 );

				// work out the final uncompressed size
				int blocks = (nSoundSize/m_WavFileInfo.wfEXT.Format.nBlockAlign);

				nSoundSize = (m_WavFileInfo.wfEXT.Format.nBlockAlign-m_WavFileInfo.wfEXT.Format.nChannels)*blocks * 4;

				DBG_MEMTRY
					m_Buffer = new char[nSoundSize];
				DBG_MEMCATCH

				unsigned char *d = (unsigned char *)pADPCMData;
				signed short *ptr = 0;

				ptr = (signed short *)m_Buffer;

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

				// compressed buffer
				if( pADPCMData != 0 )
				{
					delete[] pADPCMData;
					pADPCMData = 0;
				}

				// create the AL buffer
				CHECK_OPENAL_ERROR( alBufferData(nBufferID, nFormat, m_Buffer, nSoundSize, m_WavFileInfo.wfEXT.Format.nSamplesPerSec) )

				// cleanup
				Free();
			}
		}
	}
	
	file::FileClose( &fileHandle );

	//DBGLOG( "ANDROIDSOUND: SOUND %s Loaded\t\t(bitRate:%d, Freq:%d, Channels:%d, bytes/sec:%d, size:%d)\n", SoundFile.szFilename,
	//									nBitRate, nFrequency, nChannels, nBytesPerSec, nSoundSize );
	return(0);
}

#ifdef BASE_SUPPORT_OGG
/////////////////////////////////////////////////////
/// Method: LoadOGG
/// Params: [in]szFilename
///
/////////////////////////////////////////////////////
int AndroidSound::LoadOGG( const char *szFilename )
{
	file::TFileHandle fileHandle;
	OggVorbis_File	oggStream;
	vorbis_info		*vorbisInfo = 0;
	vorbis_comment	*vorbisComment = 0;

	char tempPCM[OGG_BUFFER_READ_SIZE];
	
    int bitStream;
	int nReadBytes = 0;
	int nTotalPCMRead = 0;

	std::memset( &tempPCM, 0, sizeof(char)*OGG_BUFFER_READ_SIZE );

	if( szFilename )
	{
		if( core::IsEmptyString( szFilename ) )
			return(1);

		if( !file::FileOpen( szFilename, file::FILETYPE_BINARY_READ, &fileHandle ) )
		{
			DBGLOG( "ANDROIDSOUND: *ERROR* failed loading ogg: %s\n", szFilename );
			return(1);
		}
		else
		{
			/// filename of the sound
			file::CreateFileStructure( szFilename, &SoundFile );

			eFileFormat = SOUND_OGG;

			// find length of file
			nFileSize = file::FileSize(&fileHandle);

			int result = -1;
			if( (result = ov_open_callbacks( &fileHandle, &oggStream, 0, 0, GetOGGCallbacks() )) < 0)
			//if((result = ov_open(fileHandle.fp, &oggStream, 0, 0)) < 0)
			{
				file::FileClose(&fileHandle);
				DBGLOG( "ANDROIDSOUND: *ERROR* Could not open Ogg stream\n" );
				return(1);
			}
			vorbisInfo = ov_info(&oggStream, -1);
			vorbisComment = ov_comment(&oggStream, -1);

			if(vorbisInfo->channels == 1)
				nFormat = AL_FORMAT_MONO16;
			else
				nFormat = AL_FORMAT_STEREO16;

			nBitRate		= vorbisInfo->bitrate_nominal;
			nChannels		= vorbisInfo->channels;
			nFrequency		= vorbisInfo->rate;
			nBytesPerSec	= 0;

			// (2 bytes for 16bit audio)
			nTotalPCMRead = vorbisInfo->channels * 2 * static_cast<int>(ov_pcm_total( &oggStream, -1 ));
			// create the buffer 
			nSoundSize = nTotalPCMRead;
			
			DBG_MEMTRY
				m_Buffer = new char[nSoundSize];
			DBG_MEMCATCH
			
			if(m_Buffer == 0)
			{
				DBGLOG( "ANDROIDSOUND: *ERROR* Could not allocate OGG sound buffer\n" );
				file::FileClose(&fileHandle);
				return(1);
			}

			// reset the read position
			nReadBytes = 0;
			nTotalPCMRead = 0;
			// Keep reading until all is read
			do
			{
				// Read up to a buffer's worth of decoded sound data
				nReadBytes = ov_read(&oggStream, reinterpret_cast<char *>(m_Buffer+nTotalPCMRead), OGG_BUFFER_READ_SIZE, core::eMachineEndian, 2, 1, &bitStream);

				if( nReadBytes < 0 )
				{
					ov_clear(&oggStream);
					DBGLOG( "ANDROIDSOUND: *ERROR* ogg decode buffer failed\n" );
					delete[] m_Buffer;
					m_Buffer = 0;
					file::FileClose(&fileHandle);
					return(1);
				}

				nTotalPCMRead += nReadBytes;

			}while(nReadBytes > 0);
		}
		
		CHECK_OPENAL_ERROR( alBufferData(nBufferID, nFormat, m_Buffer, nSoundSize, nFrequency) )
	}

	// clear the ogg buffer and close the file
	ov_clear(&oggStream);
	file::FileClose(&fileHandle);

	//DBGLOG( "ANDROIDSOUND: SOUND %s Loaded\t\t(bitRate:%d, Freq:%d, Channels:%d, bytes/sec:%d, size:%d)\n", SoundFile.szFilename,
	//									nBitRate, nFrequency, nChannels, nBytesPerSec, nSoundSize );
	return(0);
}
#endif // BASE_SUPPORT_OGG

/////////////////////////////////////////////////////
/// Method: Free
/// Params: None
///
/////////////////////////////////////////////////////
int AndroidSound::Free( void )
{
	if( m_Buffer )
	{
		delete[] m_Buffer;
		m_Buffer = 0;
	}

	return(0);
}

#endif // BASE_SUPPORT_OPENAL
#endif // BASE_PLATFORM_ANDROID

