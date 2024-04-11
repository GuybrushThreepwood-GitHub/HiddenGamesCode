
/*===================================================================
	File: SoundLoadAndUpload.cpp
	Library: Sound

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"

#ifdef BASE_SUPPORT_OPENAL

#include <cstring>
#include <cmath>

#include "Math/Vectors.h"

#include "Sound/OpenAL.h"
#include "Sound/Sound.h"
#include "Sound/SoundManager.h"
#include "Sound/SoundLoadAndUpload.h"

namespace
{
	struct lstr
	{
		bool operator() (std::string s1, std::string s2) const
		{
			return( (s1.compare(s2)) < 0);
		}
	};

	struct SoundStore
	{
		int refCount;
		ALuint bufferId;
	};

	typedef std::map< std::string, SoundStore, struct lstr> TNameSoundMap;
	typedef std::pair< std::string, SoundStore> TNameSoundPair;

	TNameSoundMap SoundMap;
}

/////////////////////////////////////////////////////
/// Function: ClearSoundMap
/// Params: 
///
/////////////////////////////////////////////////////
void snd::ClearSoundMap()
{
	TNameSoundMap::iterator it;

	it = SoundMap.begin();
	
	while( it != SoundMap.end() )
	{
#ifdef _DEBUG
		DBGLOG( "*WARNING* Sound %s still active\n", it->first.c_str() );
#endif // _DEBUG

		if( it->second.bufferId != snd::INVALID_SOUNDBUFFER )
		{
			if( snd::SoundManager::IsInitialised() )
			{
				snd::SoundManager::GetInstance()->Stop( it->second.bufferId );
			}

			alDeleteBuffers( 1, &it->second.bufferId );
		}

		it++;
	}

	SoundMap.clear();
}

/////////////////////////////////////////////////////
/// Function: SoundLoad
/// Params: [in]szFilename
///
/////////////////////////////////////////////////////
ALuint snd::SoundLoad( const char *szFilename, Sound& sound )
{
	file::TFile fileData;
	if( szFilename )
	{	
		file::CreateFileStructure( szFilename, &fileData );

		TNameSoundMap::iterator it;
		it = SoundMap.find( std::string(fileData.szFile) );

		// did iterator not hit the end
		if( it != SoundMap.end() )
		{
			it->second.refCount++;
			return( static_cast<int>(it->second.bufferId) );
		}
	}

	// load the sound
	if( sound.Load( szFilename ) == 0 )
	{
		if( szFilename )
		{
			SoundStore addSound;
			addSound.refCount = 1;
			addSound.bufferId = sound.nBufferID;

			SoundMap.insert( TNameSoundPair( std::string(fileData.szFile), addSound ) );
		}

		// sound loaded
		return(sound.nBufferID);
	}

	return( snd::INVALID_SOUNDBUFFER );
}

/////////////////////////////////////////////////////
/// Function: RemoveSound
/// Params: 
///
/////////////////////////////////////////////////////
void snd::RemoveSound( ALuint bufferId )
{
	TNameSoundMap::iterator it;

	it = SoundMap.begin();
	
	while( it != SoundMap.end() )
	{
		if( it->second.bufferId == bufferId )
		{
			it->second.refCount--;

			if( it->second.refCount < 1 )
			{
				if( it->second.bufferId != snd::INVALID_SOUNDBUFFER )
				{
					if( snd::SoundManager::IsInitialised() )
					{
						snd::SoundManager::GetInstance()->Stop( it->second.bufferId );
					}

					alDeleteBuffers( 1, &it->second.bufferId );
				}

				SoundMap.erase( it );

				return;
			}
		}

		it++;
	}
}

#endif // BASE_SUPPORT_OPENAL
