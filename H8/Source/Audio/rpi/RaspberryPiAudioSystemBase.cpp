
/*===================================================================
	File: RaspberryPiAudioSystem.cpp
	Game: H8

	(C)Hidden Games
=====================================================================*/

#ifdef BASE_PLATFORM_RASPBERRYPI

#include "CoreBase.h"
#include <map>
#include <vector>
#include "MathBase.h"
#include "SoundBase.h"

#include "Resources/SoundResources.h"
#include "Audio/rpi/RaspberryPiAudioSystemBase.h"

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
	
	m_IsPlaying = false;
	m_IsPaused = false;

	m_CurrentBGMTrack = -1;

	pMusicCallback = 0;

	std::memset( CurrentPlaylistName, 0, sizeof(char)*core::MAX_PATH );
	std::memset( CurrentSongName, 0, sizeof(char)*core::MAX_PATH );

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

	m_BGMStream.Release();

	m_UIBufferId = snd::INVALID_SOUNDBUFFER;
}

/////////////////////////////////////////////////////
/// Method: Update
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
void AudioSystemBase::Update( float deltaTime )
{
	snd::SoundManager::GetInstance()->Update( deltaTime, false );

	if( !m_BGMStream.IsPlaying() )
		m_IsPlaying = false;

	m_BGMStream.Update();
}

/////////////////////////////////////////////////////
/// Method: UpdatePlaylists
/// Params: 
///
/////////////////////////////////////////////////////
void AudioSystemBase::UpdatePlaylists()
{

}

/////////////////////////////////////////////////////
/// Method: PlayMusicTrack
/// Params: filename
///
/////////////////////////////////////////////////////
void AudioSystemBase::PlayBGMTrack( const char* filename, bool loop )
{
	// convert an mp3 to ogg lookup
	file::TFile fileSplit;
	char newPath[core::MAX_PATH+core::MAX_PATH];
	std::memset( newPath, 0, sizeof(char)*core::MAX_PATH+core::MAX_PATH );
		
	file::CreateFileStructure( filename, &fileSplit );

	if( std::strcmp( fileSplit.szFileExt, ".mp3" ) == 0 ||
		std::strcmp( fileSplit.szFileExt, ".MP3" ) == 0 )
	{
		snprintf( newPath, core::MAX_PATH+core::MAX_PATH, "%sogg/%s.ogg", fileSplit.szDrive, fileSplit.szFile );
		m_BGMStream.StreamOpen( newPath );
		m_BGMStream.StartPlayback(loop);
	}
	else
	{
		m_BGMStream.StreamOpen( filename );
		m_BGMStream.StartPlayback(loop);
	}

	m_IsPlaying = true;
}

/////////////////////////////////////////////////////
/// Method: StopBGMTrack
/// Params: None
///
/////////////////////////////////////////////////////
void AudioSystemBase::StopBGMTrack()
{
	m_BGMStream.Stop();	

	m_IsPlaying = false;
}

/////////////////////////////////////////////////////
/// Method: ClearBGMTrack
/// Params: None
///
/////////////////////////////////////////////////////
void AudioSystemBase::ClearBGMTrack()
{
	m_BGMStream.Release();

	m_IsPlaying = false;
	m_IsPaused = false;
}

/////////////////////////////////////////////////////
/// Method: SetBGMTrackVolume
/// Params: None
///
/////////////////////////////////////////////////////
void AudioSystemBase::SetBGMTrackVolume( float vol )
{
	math::Clamp( &vol, 0.0f, 1.0f );

	m_BGMStream.SetVolume( vol );
}

/////////////////////////////////////////////////////
/// Method: PauseMusicTrack
/// Params: None
///
/////////////////////////////////////////////////////
void AudioSystemBase::PauseBGMTrack()
{
	m_BGMStream.Pause();
	m_IsPaused = true;
}

/////////////////////////////////////////////////////
/// Method: ResumeMusicTrack
/// Params: None
///
/////////////////////////////////////////////////////
void AudioSystemBase::ResumeBGMTrack()
{
	m_BGMStream.Play();
	m_IsPaused = false;
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
	return m_IsPlaying;
}

/////////////////////////////////////////////////////
/// Method: IsPaused
/// Params: None
///
/////////////////////////////////////////////////////
bool AudioSystemBase::IsPaused()
{
	return m_IsPaused;
}

/////////////////////////////////////////////////////
/// Method: IsUsingPlaylists
/// Params: None
///
/////////////////////////////////////////////////////
bool AudioSystemBase::IsUsingPlaylists()
{
	return false;
}

/////////////////////////////////////////////////////
/// Method: GetCurrentSong
/// Params: None
///
/////////////////////////////////////////////////////
const char* AudioSystemBase::GetCurrentSong()
{
	return 0;
}

/////////////////////////////////////////////////////
/// Method: GetPlaylistName
/// Params: [in]playlistIndex
///
/////////////////////////////////////////////////////
const char* AudioSystemBase::GetPlaylistName( int playlistIndex )
{
	return 0;	
}

/////////////////////////////////////////////////////
/// Method: GetSongName
/// Params: [in]playlistIndex, [in]songIndex
///
/////////////////////////////////////////////////////
const char* AudioSystemBase::GetSongName( int playlistIndex, int songIndex )
{
	return 0;		
}

/////////////////////////////////////////////////////
/// Method: GetTotalNumberOfPlaylists
/// Params: None
///
/////////////////////////////////////////////////////
int AudioSystemBase::GetTotalNumberOfPlaylists()
{
	return 0;		
}

/////////////////////////////////////////////////////
/// Method: GetTotalNumberOfSongsInPlaylist
/// Params: [in]playlistIndex
///
/////////////////////////////////////////////////////
int AudioSystemBase::GetTotalNumberOfSongsInPlaylist( int playlistIndex )
{
	return 0;		
}

/////////////////////////////////////////////////////
/// Method: GetCurrentPlaylistIndex
/// Params: None
///
/////////////////////////////////////////////////////
int AudioSystemBase::GetCurrentPlaylistIndex()
{
	return -1;		
}

/////////////////////////////////////////////////////
/// Method: GetCurrentSongIndex
/// Params: None
///
/////////////////////////////////////////////////////
int AudioSystemBase::GetCurrentSongIndex()
{
	return -1;	
}

/////////////////////////////////////////////////////
/// Method: PlaySong
/// Params: None
///
/////////////////////////////////////////////////////
void AudioSystemBase::PlaySong( int playlistIndex, int songIndex )
{

}

/////////////////////////////////////////////////////
/// Method: PlaySong
/// Params: None
///
/////////////////////////////////////////////////////
void AudioSystemBase::PlaySong()
{

}

/////////////////////////////////////////////////////
/// Method: PlaySong
/// Params: None
///
/////////////////////////////////////////////////////
void AudioSystemBase::PauseSong()
{

}

/////////////////////////////////////////////////////
/// Method: ResumeSong
/// Params: None
///
/////////////////////////////////////////////////////
void AudioSystemBase::ResumeSong()
{

}

/////////////////////////////////////////////////////
/// Method: StopSong
/// Params: None
///
/////////////////////////////////////////////////////
void AudioSystemBase::StopSong()
{

}

/////////////////////////////////////////////////////
/// Method: SkipToPreviousSong
/// Params: None
///
/////////////////////////////////////////////////////
void AudioSystemBase::SkipToPreviousSong()
{

}

/////////////////////////////////////////////////////
/// Method: SkipToNextSong
/// Params: None
///
/////////////////////////////////////////////////////
void AudioSystemBase::SkipToNextSong()
{

}

/////////////////////////////////////////////////////
/// Method: SetMusicCallback
/// Params: [in]pCallback
///
/////////////////////////////////////////////////////
void AudioSystemBase::SetMusicCallback( snd::MusicCallback* pCallback )
{
	pMusicCallback = pCallback;
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

}

/////////////////////////////////////////////////////
/// Method: UnPauseMusic
/// Params: None
///
/////////////////////////////////////////////////////
void AudioSystemBase::UnPauseMusic()
{

}

#endif // BASE_PLATFORM_RASPBERRYPI
