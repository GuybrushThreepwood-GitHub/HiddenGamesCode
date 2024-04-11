
/*===================================================================
	File: AudioSystem.cpp
	Game: AirCadets

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

	m_OneShotList.clear();

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
/// Method: Update
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
void AudioSystem::Update( float deltaTime )
{
	AudioSystemBase::Update( deltaTime );

	// look for one shots to remove
	if( m_OneShotList.size() != 0 )
	{
		std::vector<OneShot>::iterator it = m_OneShotList.begin();

		while( it != m_OneShotList.end() )
		{
			if( (*it).sourceId != snd::INVALID_SOUNDSOURCE &&
				(*it).bufferId != snd::INVALID_SOUNDBUFFER )
			{
				ALenum state;
				CHECK_OPENAL_ERROR( alGetSourcei( (*it).sourceId, AL_SOURCE_STATE, &state) )
				
				if( state == AL_INITIAL || state == AL_STOPPED )
				{
					//CHECK_OPENAL_ERROR( alSourcei( (*it).sourceId, AL_BUFFER, 0 ) );
					
					snd::RemoveSound( (*it).bufferId );

					it = m_OneShotList.erase( it );
					continue;
				}
			}

			it++;
		}
	}
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

/////////////////////////////////////////////////////
/// Method: PlayAndRemove
/// Params: 
///
/////////////////////////////////////////////////////
void AudioSystem::PlayAndRemove( ALuint bufferId, math::Vec3 pos, bool sourceRel, bool loop, float pitch, float gain )
{
	float soundMultiply = 1.0f;
		gain = 2.0f;
	if(!m_SFXState)
		soundMultiply = 0.0f;

	ALuint sourceId = snd::SoundManager::GetInstance()->GetFreeSoundSource();

	if( sourceId != snd::INVALID_SOUNDSOURCE &&
		bufferId != snd::INVALID_SOUNDBUFFER )
	{
		CHECK_OPENAL_ERROR( alSourcei( sourceId, AL_BUFFER, bufferId ) )
		CHECK_OPENAL_ERROR( alSourcei( sourceId, AL_SOURCE_RELATIVE, sourceRel ) )
		CHECK_OPENAL_ERROR( alSourcei( sourceId, AL_LOOPING, loop ) )
		CHECK_OPENAL_ERROR( alSourcef( sourceId, AL_PITCH, pitch ) )
		CHECK_OPENAL_ERROR( alSourcef( sourceId, AL_GAIN, gain*soundMultiply ) )
		CHECK_OPENAL_ERROR( alSource3f( sourceId, AL_POSITION, pos.X, pos.Y, pos.Z ) )

		CHECK_OPENAL_ERROR( alSourcef( sourceId, AL_MAX_DISTANCE, MAX_AUDIO_DISTANCE ) )
		CHECK_OPENAL_ERROR( alSourcef( sourceId, AL_ROLLOFF_FACTOR, ROLL_OFF ) )
		CHECK_OPENAL_ERROR( alSourcef( sourceId, AL_REFERENCE_DISTANCE, REF_DISTANCE ) )

		CHECK_OPENAL_ERROR( alSourcePlay( sourceId ) )

		// add for removal
		OneShot oneShot;
		oneShot.sourceId = sourceId;
		oneShot.bufferId = bufferId;

		m_OneShotList.push_back(oneShot);
	}
}

/////////////////////////////////////////////////////
/// Method: ClearOneShotList
/// Params: 
///
/////////////////////////////////////////////////////
void AudioSystem::ClearOneShotList()
{
	m_OneShotList.clear();
}
