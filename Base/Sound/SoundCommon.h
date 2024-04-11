
#ifndef __SOUNDCOMMON_H__
#define __SOUNDCOMMON_H__

#ifdef BASE_SUPPORT_OPENAL

#ifndef __PLATFORMSOUNDBASE_H__
	#include "Sound/PlatformSoundBase.h"
#endif // __PLATFORMSOUNDBASE_H__

namespace snd
{
	const ALuint INVALID_SOUNDSOURCE		= (ALuint)(~0);
	const ALuint INVALID_SOUNDBUFFER		= (ALuint)(~0);
	
	const int STREAM_BUFFER_COUNT	= 4;

	const unsigned int SPEAKER_FRONT_LEFT				= 0x1;
	const unsigned int SPEAKER_FRONT_RIGHT				= 0x2;	
	const unsigned int SPEAKER_FRONT_CENTER				= 0x4;
	const unsigned int SPEAKER_LOW_FREQUENCY			= 0x8;
	const unsigned int SPEAKER_BACK_LEFT				= 0x10;
	const unsigned int SPEAKER_BACK_RIGHT				= 0x20;
	const unsigned int SPEAKER_FRONT_LEFT_OF_CENTER		= 0x40;
	const unsigned int SPEAKER_FRONT_RIGHT_OF_CENTER	= 0x80;
	const unsigned int SPEAKER_BACK_CENTER				= 0x100;
	const unsigned int SPEAKER_SIDE_LEFT				= 0x200;
	const unsigned int SPEAKER_SIDE_RIGHT				= 0x400;
	const unsigned int SPEAKER_TOP_CENTER				= 0x800;
	const unsigned int SPEAKER_TOP_FRONT_LEFT			= 0x1000;
	const unsigned int SPEAKER_TOP_FRONT_CENTER			= 0x2000;
	const unsigned int SPEAKER_TOP_FRONT_RIGHT			= 0x4000;
	const unsigned int SPEAKER_TOP_BACK_LEFT			= 0x8000;
	const unsigned int SPEAKER_TOP_BACK_CENTER			= 0x10000;
	const unsigned int SPEAKER_TOP_BACK_RIGHT			= 0x20000;

	const int WAV_FORMAT_PCM = 1;
	const unsigned int WAV_FORMAT_EXTENSIBLE = 0xFFFE;
	const int WAV_FORMAT_IMA4_ADPCM = 17;

	enum WAVEFILETYPE
	{
		WF_EX  = 1,
		WF_EXT = 2,
		WF_IMA4_ADPCM = 17
	};
	
	enum MusicMode
	{
		MusicMode_SingleFilePlayer=0,
		MusicMode_PlaylistPlayer=1
	};

	class MusicPauseCall
	{
		public:
			virtual void PauseMusic() {}
			virtual void UnPauseMusic() {}
	};

	struct WAVGUID 
	{
		unsigned int Data1;
		unsigned short Data2;
		unsigned short Data3;
		unsigned char  Data4[ 8 ];
	};

#pragma pack(push, 4)
	struct WAVEFileHeader
	{
		char			szRIFF[4];
		unsigned int	ulRIFFSize;
		char			szWAVE[4];
	};

	struct RIFFChunk
	{
		char			szChunkName[4];
		unsigned int	ulChunkSize;
	};

	struct WAVEFmt
	{
		unsigned short	usFormatTag;
		unsigned short	usChannels;
		unsigned int	ulSamplesPerSec;
		unsigned int	ulAvgBytesPerSec;
		unsigned short	usBlockAlign;
		unsigned short	usBitsPerSample;
		unsigned short	usSize;
		unsigned short  usReserved;
		unsigned int	ulChannelMask;
		WAVGUID            guidSubFormat;
	};
#pragma pack(pop)

	struct WAVEFormat 
	{
		unsigned short  wFormatTag;        // format type
		unsigned short  nChannels;         // number of channels (i.e. mono, stereo, etc.)
		unsigned int   nSamplesPerSec;    // sample rate
		unsigned int   nAvgBytesPerSec;   // for buffer estimation
		unsigned short  nBlockAlign;       // block size of data
	};

	struct WAVEFormatEx
	{
		unsigned short    wFormatTag;
		unsigned short    nChannels;
		unsigned int   nSamplesPerSec;
		unsigned int   nAvgBytesPerSec;
		unsigned short    nBlockAlign;
		unsigned short    wBitsPerSample;
		unsigned short    cbSize;
	};

	struct WAVEFormatExtensible 
	{
		WAVEFormatEx    Format;
		union {
			unsigned short wValidBitsPerSample;       // bits of precision
			unsigned short wSamplesPerBlock;          // valid if wBitsPerSample==0
			unsigned short wReserved;                 // If neither applies, set to zero.
		} Samples;
		unsigned int dwChannelMask;      // which channels are present in stream
		WAVGUID SubFormat;
	};

	struct WAVEFileInfo
	{
		WAVEFILETYPE	wfType;
		WAVEFormatExtensible wfEXT;		// For non-WAVEFORMATEXTENSIBLE wavefiles, the header is stored in the Format member of wfEXT
		char			*pData;
		unsigned int	ulDataSize;
		unsigned int	ulDataOffset;
	};

	struct PCMWAVEFormat 
	{
		WAVEFormat  wf;
		unsigned short        wBitsPerSample;
	};

#pragma pack (push,1) 						
	/// WAV file header
	struct WAVFileHdr                               
	{
		unsigned char  Id[4];
		unsigned int  Size;
		unsigned char  Type[4];
	};

	/// WAV file format header
	struct WAVFmtHdr                               
	{
		unsigned short Format;                              
		unsigned short Channels;
		unsigned int   SamplesPerSec;
		unsigned int   BytesPerSec;
		unsigned short BlockAlign;
		unsigned short BitsPerSample;
	};

	/// WAV file format extended header
	struct WAVFmtExHdr							
	{
		unsigned short Size;
		unsigned short SamplesPerBlock;
	};

	/// WAV file sample header
	struct WAVSmplHdr                           
	{
		unsigned int   Manufacturer;
		unsigned int   Product;
		unsigned int   SamplePeriod;                          
		unsigned int   Note;                                  
		unsigned int   FineTune;                              
		unsigned int   SMPTEFormat;
		unsigned int   SMPTEOffest;
		unsigned int   Loops;
		unsigned int   SamplerData;
		struct
		{
			unsigned int Identifier;
			unsigned int Type;
			unsigned int Start;
			unsigned int End;
			unsigned int Fraction;
			unsigned int Count;
		} Loop[1];
	};

	/// WAV file chunk header
	struct WAVChunkHdr                                
	{
		unsigned char  Id[4];
		unsigned int   Size;
	};
#pragma pack (pop)	

	enum ESoundFormat
	{
		SOUND_UNKNOWN=0,
		SOUND_WAV=1,
		SOUND_OGG=2
	};
	
	class Song
	{
		public:
			char songName[core::MAX_PATH];
			int songLength;
	};
	
	class Playlist
	{
		public:
			Playlist()
			{
				numSongs = 0;
				playListSongs = 0;
			}
		
			char playlistName[core::MAX_PATH];
		
			int numSongs;
			snd::Song* playListSongs;
	};

	class MusicCallback
	{
		public:
			MusicCallback() {}
			virtual ~MusicCallback() {}

			virtual void PlayingSongChanged() = 0;
			virtual void PlaybackStateChanged() = 0;
	};

} // namespace snd

#endif // BASE_SUPPORT_OPENAL
#endif // __SOUNDCOMMON_H__

