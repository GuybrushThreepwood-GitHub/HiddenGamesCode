
/*===================================================================
	File: iOSAudioSystem.cpp
	Game: Prisoner 84

	(C)Hidden Games
=====================================================================*/

#ifdef BASE_PLATFORM_iOS

#include "CoreBase.h"
#include <map>
#include <vector>
#include "MathBase.h"
#include "SoundBase.h"

#include "Resources/SoundResources.h"
#include "Audio/ios/iOSAudioSystemBase.h"

namespace
{
	snd::MusicCallback* pMusicCallback = 0;
	char CurrentPlaylistName[core::MAX_PATH];
	char CurrentSongName[core::MAX_PATH];
}

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
AudioSystemBase::AudioSystemBase(snd::MusicMode mode)
{
	m_MusicMode = mode;
	m_MusicWasPlaying = false;
	
	m_CurrentBGMTrack = -1;

	pMusicCallback = 0;

	std::memset( CurrentPlaylistName, 0, sizeof(char)*core::MAX_PATH );
	std::memset( CurrentSongName, 0, sizeof(char)*core::MAX_PATH );

	m_pMusicPlayer = nil;
	m_pMusicPlayer = [[MusicMediaPlayer alloc] initWithMode: m_MusicMode ];
		
	// should only set if playing and not paused
	m_MusicWasPaused = false;
	m_MusicWasPlaying = ([m_pMusicPlayer IsPlaying] && ![m_pMusicPlayer IsPaused]);

	m_UIBufferId = snd::INVALID_SOUNDBUFFER;

	m_SystemPaused = false;
}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
AudioSystemBase::~AudioSystemBase()
{
	pMusicCallback = 0;
	
	m_CurrentBGMTrack = -1;
	m_SFXState = true;

	if( m_pMusicPlayer != nil )
	{
		if( !m_MusicWasPlaying )
			[m_pMusicPlayer StopSong];
		
		[ m_pMusicPlayer ReleaseMusicPlayer ];
	}	

	m_UIBufferId = snd::INVALID_SOUNDBUFFER;
}

/////////////////////////////////////////////////////
/// Method: AddSfx
/// Params: [in]pSFX
///
/////////////////////////////////////////////////////
void AudioSystemBase::Update( float deltaTime )
{
	snd::SoundManager::GetInstance()->Update( deltaTime, false );
	
	if( m_pMusicPlayer != nil )
	{
		[ m_pMusicPlayer Update:deltaTime ];
	}
}

/////////////////////////////////////////////////////
/// Method: UpdatePlaylists
/// Params: 
///
/////////////////////////////////////////////////////
void AudioSystemBase::UpdatePlaylists()
{	
	if( m_pMusicPlayer != nil )
	{
		[ m_pMusicPlayer UpdatePlaylists ];
	}
}

/////////////////////////////////////////////////////
/// Method: PlayBGMTrack
/// Params: filename
///
/////////////////////////////////////////////////////
void AudioSystemBase::PlayBGMTrack( const char* filename, bool loop )
{
	if( m_pMusicPlayer != nil )
	{
		[m_pMusicPlayer PlaySong:filename loopFlag:loop ];
	}
}

/////////////////////////////////////////////////////
/// Method: StopBGMTrack
/// Params: None
///
/////////////////////////////////////////////////////
void AudioSystemBase::StopBGMTrack()
{
	if( m_pMusicPlayer != nil )
	{
		[m_pMusicPlayer StopSong ];
	}
}

/////////////////////////////////////////////////////
/// Method: ClearBGMTrack
/// Params: None
///
/////////////////////////////////////////////////////
void AudioSystemBase::ClearBGMTrack()
{

}

/////////////////////////////////////////////////////
/// Method: SetBGMTrackVolume
/// Params: None
///
/////////////////////////////////////////////////////
void AudioSystemBase::SetBGMTrackVolume( float vol )
{
	math::Clamp( &vol, 0.0f, 1.0f );
	
	if( m_pMusicPlayer != nil )
	{
		[m_pMusicPlayer SetVolume:vol];
	}
}

/////////////////////////////////////////////////////
/// Method: PauseBGMTrack
/// Params: None
///
/////////////////////////////////////////////////////
void AudioSystemBase::PauseBGMTrack()
{
	if( m_pMusicPlayer != nil )
	{
		[m_pMusicPlayer PauseSong];
	}
}

/////////////////////////////////////////////////////
/// Method: ResumeMusicTrack
/// Params: None
///
/////////////////////////////////////////////////////
void AudioSystemBase::ResumeBGMTrack()
{
	if( m_pMusicPlayer != nil )
	{
		[m_pMusicPlayer ResumeSong];
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////
/// Method: SetShuffleMode
/// Params: [in]mode
///
/////////////////////////////////////////////////////
void AudioSystemBase::SetShuffleMode( int mode )
{

}

/////////////////////////////////////////////////////
/// Method: SetRepeatFlag
/// Params: [in]state
///
/////////////////////////////////////////////////////
void AudioSystemBase::SetRepeatFlag( bool state )
{

}

/////////////////////////////////////////////////////
/// Method: IsPlaying
/// Params: None
///
/////////////////////////////////////////////////////
bool AudioSystemBase::IsPlaying()
{
	if( m_pMusicPlayer != nil )
		return [m_pMusicPlayer IsPlaying ];

	return false;
}

/////////////////////////////////////////////////////
/// Method: IsPaused
/// Params: None
///
/////////////////////////////////////////////////////
bool AudioSystemBase::IsPaused()
{
	if( m_pMusicPlayer != nil )
		return [m_pMusicPlayer IsPaused ];

	return false;
}

/////////////////////////////////////////////////////
/// Method: IsUsingPlaylists
/// Params: None
///
/////////////////////////////////////////////////////
bool AudioSystemBase::IsUsingPlaylists()
{
	if( m_pMusicPlayer != nil )
		return [m_pMusicPlayer IsUsingPlaylist ];

	return false;
}

/////////////////////////////////////////////////////
/// Method: GetCurrentSong
/// Params: None
///
/////////////////////////////////////////////////////
const char* AudioSystemBase::GetCurrentSong()
{
	if( m_pMusicPlayer != nil )
		return [m_pMusicPlayer GetCurrentSong ];

	return 0;
}

/////////////////////////////////////////////////////
/// Method: GetPlaylistName
/// Params: [in]playlistIndex
///
/////////////////////////////////////////////////////
const char* AudioSystemBase::GetPlaylistName( int playlistIndex )
{
	if( m_pMusicPlayer != nil )
		return [m_pMusicPlayer GetPlaylistName: playlistIndex ];

	return 0;	
}

/////////////////////////////////////////////////////
/// Method: GetSongName
/// Params: [in]playlistIndex, [in]songIndex
///
/////////////////////////////////////////////////////
const char* AudioSystemBase::GetSongName( int playlistIndex, int songIndex )
{
	if( m_pMusicPlayer != nil )
		return [m_pMusicPlayer GetSongName: playlistIndex whichSong:songIndex ];

	return 0;		
}

/////////////////////////////////////////////////////
/// Method: GetTotalNumberOfPlaylists
/// Params: None
///
/////////////////////////////////////////////////////
int AudioSystemBase::GetTotalNumberOfPlaylists()
{
	if( m_pMusicPlayer != nil )
		return [m_pMusicPlayer GetTotalNumberOfPlaylists ];

	return 0;		
}

/////////////////////////////////////////////////////
/// Method: GetTotalNumberOfSongsInPlaylist
/// Params: [in]playlistIndex
///
/////////////////////////////////////////////////////
int AudioSystemBase::GetTotalNumberOfSongsInPlaylist( int playlistIndex )
{
	if( m_pMusicPlayer != nil )
		return [m_pMusicPlayer GetTotalNumberOfSongsInPlaylist: playlistIndex ];

	return 0;		
}

/////////////////////////////////////////////////////
/// Method: GetCurrentPlaylistIndex
/// Params: None
///
/////////////////////////////////////////////////////
int AudioSystemBase::GetCurrentPlaylistIndex()
{
	if( m_pMusicPlayer != nil )
		return [m_pMusicPlayer GetCurrentPlaylistIndex ];

	return -1;		
}

/////////////////////////////////////////////////////
/// Method: GetCurrentSongIndex
/// Params: None
///
/////////////////////////////////////////////////////
int AudioSystemBase::GetCurrentSongIndex()
{
	if( m_pMusicPlayer != nil )
		return [m_pMusicPlayer GetCurrentSongIndex ];

	return -1;	
}

/////////////////////////////////////////////////////
/// Method: PlaySong
/// Params: None
///
/////////////////////////////////////////////////////
void AudioSystemBase::PlaySong( int playlistIndex, int songIndex )
{
	if( m_pMusicPlayer != nil )
		[m_pMusicPlayer PlaySong: playlistIndex whichSong: songIndex ];
}

/////////////////////////////////////////////////////
/// Method: PlaySong
/// Params: None
///
/////////////////////////////////////////////////////
void AudioSystemBase::PlaySong()
{
	if( m_pMusicPlayer != nil )
		[m_pMusicPlayer PlaySong];
}

/////////////////////////////////////////////////////
/// Method: PlaySong
/// Params: None
///
/////////////////////////////////////////////////////
void AudioSystemBase::PauseSong()
{
	if( m_pMusicPlayer != nil )
		[m_pMusicPlayer PauseSong];
}

/////////////////////////////////////////////////////
/// Method: ResumeSong
/// Params: None
///
/////////////////////////////////////////////////////
void AudioSystemBase::ResumeSong()
{
	if( m_pMusicPlayer != nil )
		[m_pMusicPlayer ResumeSong];
}

/////////////////////////////////////////////////////
/// Method: StopSong
/// Params: None
///
/////////////////////////////////////////////////////
void AudioSystemBase::StopSong()
{
	if( m_pMusicPlayer != nil )
		[m_pMusicPlayer StopSong];	
}

/////////////////////////////////////////////////////
/// Method: SkipToPreviousSong
/// Params: None
///
/////////////////////////////////////////////////////
void AudioSystemBase::SkipToPreviousSong()
{
	if( m_pMusicPlayer != nil )
		[m_pMusicPlayer SelectPreviousSong];
}

/////////////////////////////////////////////////////
/// Method: SkipToNextSong
/// Params: None
///
/////////////////////////////////////////////////////
void AudioSystemBase::SkipToNextSong()
{
	if( m_pMusicPlayer != nil )
		[m_pMusicPlayer SelectNextSong];
}

/////////////////////////////////////////////////////
/// Method: SetMusicCallback
/// Params: [in]pCallback
///
/////////////////////////////////////////////////////
void AudioSystemBase::SetMusicCallback( snd::MusicCallback* pCallback )
{
	pMusicCallback = pCallback;

	if( m_pMusicPlayer != nil )
		[m_pMusicPlayer setMusicCallback:pCallback];
}

///////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////
/// Method: PlayUIAudio
/// Params: None
///
/////////////////////////////////////////////////////
void AudioSystemBase::PlayUIAudio()
{
	float soundMultiply = 1.0f;
	if(!m_SFXState)
		soundMultiply = 0.0f;

	ALuint sourceId = snd::INVALID_SOUNDSOURCE;
	sourceId = snd::SoundManager::GetInstance()->GetFreeSoundSource();

	if( sourceId != snd::INVALID_SOUNDSOURCE &&
		m_UIBufferId != snd::INVALID_SOUNDBUFFER )
	{
		CHECK_OPENAL_ERROR( alSourcei( sourceId, AL_BUFFER, m_UIBufferId ) )
		CHECK_OPENAL_ERROR( alSourcei( sourceId, AL_SOURCE_RELATIVE, AL_TRUE ) )
		CHECK_OPENAL_ERROR( alSourcei( sourceId, AL_LOOPING, AL_FALSE ) )
		CHECK_OPENAL_ERROR( alSourcef( sourceId, AL_PITCH, 1.0f ) )
		CHECK_OPENAL_ERROR( alSourcef( sourceId, AL_GAIN, 1.0f*soundMultiply ) )
		CHECK_OPENAL_ERROR( alSource3f( sourceId, AL_POSITION, 0.0f, 0.0f, 0.0f ) )
		
		CHECK_OPENAL_ERROR( alSourcef( sourceId, AL_MAX_DISTANCE, MAX_AUDIO_DISTANCE ) )
		CHECK_OPENAL_ERROR( alSourcef( sourceId, AL_ROLLOFF_FACTOR, ROLL_OFF ) )
		CHECK_OPENAL_ERROR( alSourcef( sourceId, AL_REFERENCE_DISTANCE, REF_DISTANCE ) )
		
		CHECK_OPENAL_ERROR( alSourcePlay( sourceId ) )
	}
}

/////////////////////////////////////////////////////
/// Method: PlayAudio
/// Params: [in]sourceId, [in]bufferId, [in]pos, [in]sourceRel
///
/////////////////////////////////////////////////////
void AudioSystemBase::PlayAudio( ALuint sourceId, ALuint bufferId, math::Vec3 pos, bool sourceRel, bool loop, float pitch, float gain )
{
	float soundMultiply = 1.0f;
	if(!m_SFXState)
		soundMultiply = 0.0f;

	if( sourceId == snd::INVALID_SOUNDSOURCE ||
		bufferId == snd::INVALID_SOUNDBUFFER )
		return;

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
}

/////////////////////////////////////////////////////
/// Method: PlayAudio
/// Params: [in]bufferId, [in]pos, [in]sourceRel
///
/////////////////////////////////////////////////////
ALuint AudioSystemBase::PlayAudio( ALuint bufferId, math::Vec3 pos, bool sourceRel, bool loop, float pitch, float gain )
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
	}

	if( loop )
		return(sourceId);

	return(snd::INVALID_SOUNDSOURCE);
}

/////////////////////////////////////////////////////
/// Method: StopAudio
/// Params: [in]sourceId
///
/////////////////////////////////////////////////////
void AudioSystemBase::StopAudio( ALuint sourceId )
{
	CHECK_OPENAL_ERROR( alSourceStop( sourceId ) )
}

/////////////////////////////////////////////////////
/// Method: StopAll
/// Params: None
///
/////////////////////////////////////////////////////
void AudioSystemBase::StopAll()
{
	snd::SoundManager::GetInstance()->StopAll();
}

/////////////////////////////////////////////////////
/// Method: Pause
/// Params: None
///
/////////////////////////////////////////////////////
void AudioSystemBase::Pause()
{
	snd::SoundManager::GetInstance()->Pause();

	m_SystemPaused = true;
}

/////////////////////////////////////////////////////
/// Method: UnPause
/// Params: None
///
/////////////////////////////////////////////////////
void AudioSystemBase::UnPause()
{
	snd::SoundManager::GetInstance()->UnPause();

	m_SystemPaused = false;
}

/////////////////////////////////////////////////////
/// Method: PauseMusic
/// Params: None
///
/////////////////////////////////////////////////////
void AudioSystemBase::PauseMusic()
{
	if( m_pMusicPlayer != nil )
	{
		if( !m_MusicWasPlaying )
		{
			if( [m_pMusicPlayer IsPlaying] && ![m_pMusicPlayer IsPaused] )
			{
				[m_pMusicPlayer PauseSong];
				m_MusicWasPaused = true;
			}
			else 
			{
				m_MusicWasPaused = false;
			}
		}
	}

}

/////////////////////////////////////////////////////
/// Method: UnPauseMusic
/// Params: None
///
/////////////////////////////////////////////////////
void AudioSystemBase::UnPauseMusic()
{
	if( m_pMusicPlayer != nil )
	{
		if( !m_MusicWasPlaying && m_MusicWasPaused)
			[m_pMusicPlayer PlaySong];
	}

}

#endif // BASE_PLATFORM_iOS