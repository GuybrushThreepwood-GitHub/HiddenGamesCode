
/*===================================================================
	File: AudioSystem.cpp
	Game: Firewall

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"
#include <map>
#include <vector>
#include "MathBase.h"
#include "SoundBase.h"

#include "Resources/SoundResources.h"
#include "Audio/AudioSystem.h"

AudioSystem* AudioSystem::ms_Instance = 0;

/////////////////////////////////////////////////////
/// Static Method: Initialise
/// Params: None
///
/////////////////////////////////////////////////////
void AudioSystem::Initialise( snd::MusicMode mode )
{
	DBG_ASSERT( (ms_Instance == 0) );

	// init sound manager
	snd::SoundManager::Initialise();

	DBG_MEMTRY
		ms_Instance = new AudioSystem( mode );
	DBG_MEMCATCH
}

/////////////////////////////////////////////////////
/// Static Method: Shutdown
/// Params: None
///
/////////////////////////////////////////////////////
void AudioSystem::Shutdown( void )
{
	if( ms_Instance )
	{
		delete ms_Instance;
		ms_Instance = 0;
	}

	// stop and delete sound sources
	snd::SoundManager::Shutdown();
}

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
AudioSystem::AudioSystem(snd::MusicMode mode)
: AudioSystemBase( mode )
{
	m_SoundLoader.Initialise();

	m_SFXState = true;
}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
AudioSystem::~AudioSystem()
{
	m_SoundLoader.Release();

	m_AudioNameMap.clear();
}

/////////////////////////////////////////////////////
/// Method: FindSound
/// Params: [in]resId
///
/////////////////////////////////////////////////////
ALuint AudioSystem::FindSound( int resId )
{
	// see if name exists in the map
	TNameIntMap::iterator it;

	it = m_AudioNameMap.find( resId );

	// found?
	if( it != m_AudioNameMap.end() )
		return( static_cast<ALuint>(it->second) );

	// no index (file not found)
	return(snd::INVALID_SOUNDBUFFER);
}
		
/////////////////////////////////////////////////////
/// Method: AddAudioFile
/// Params: [in]resId
///
/////////////////////////////////////////////////////
ALuint AudioSystem::AddAudioFile( int resId )
{
	// not in the list already
	ALuint bufferId = FindSound( resId );

	if( bufferId == snd::INVALID_SOUNDBUFFER )
	{
		m_SoundLoader.Initialise();
		
		const char* filename = res::GetSoundResource( resId );
		DBG_ASSERT( filename != 0 );

		m_SoundLoader.Load( filename );
		bufferId = m_SoundLoader.nBufferID;

		m_AudioNameMap.insert( TNameIntPair( resId, bufferId ) );

		m_SoundLoader.Release();
	}

	return bufferId;
}

