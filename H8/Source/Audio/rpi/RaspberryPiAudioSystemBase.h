
#ifndef __RASPBERRYPIAUDIOSYSTEM_H__
#define __RASPBERRYPIAUDIOSYSTEM_H__

#ifdef BASE_PLATFORM_RASPBERRYPI

#include <vector>

#include "SoundBase.h"

const float MAX_AUDIO_DISTANCE = 100.0f;
const float ROLL_OFF = 0.01f;
const float REF_DISTANCE = 0.01f;

class AudioSystemBase : public snd::MusicPauseCall
{
	public:
		AudioSystemBase( snd::MusicMode mode=snd::MusicMode_SingleFilePlayer );
		virtual ~AudioSystemBase();

		virtual void Update( float deltaTime );
		void UpdatePlaylists();
	
		// BGM Player
		void PlayBGMTrack( const char* filename, bool loop=true );
		void StopBGMTrack();
		void ClearBGMTrack();
		void SetBGMTrackVolume( float vol );
		void PauseBGMTrack();
		void ResumeBGMTrack();
		void SetCurrentBGMTrack( int trackId )		{ m_CurrentBGMTrack = trackId; }
		int GetCurrentBGMTrack()					{ return m_CurrentBGMTrack; }

		// Music Player
		void SetShuffleMode( int mode );
		void SetRepeatFlag( bool state );
	
		bool IsPlaying();
		bool IsPaused();
		bool IsUsingPlaylists();
		const char* GetCurrentSong();
	
		const char* GetPlaylistName( int playlistIndex );
		const char* GetSongName( int playlistIndex, int songIndex );
	
		int GetTotalNumberOfPlaylists();
		int GetTotalNumberOfSongsInPlaylist( int playlistIndex );
		int GetCurrentPlaylistIndex();
		int GetCurrentSongIndex();
		void PlaySong( int playlistIndex, int songIndex );
		void PlaySong();
		void PauseSong();
		void ResumeSong();
		
		void StopSong();
		void SkipToPreviousSong();
		void SkipToNextSong();
		void SetMusicCallback( snd::MusicCallback* pCallback );
	
		// ui
		void SetUIBufferId( ALuint bufferId )		{ m_UIBufferId = bufferId; }
		void PlayUIAudio();

		void PlayAudio( ALuint sourceId, ALuint bufferId, math::Vec3 pos, bool sourceRel=AL_FALSE, bool loop=AL_FALSE, float pitch=1.0f, float gain=1.0f ); 
		ALuint PlayAudio( ALuint bufferId, math::Vec3 pos, bool sourceRel=AL_FALSE, bool loop=AL_FALSE, float pitch=1.0f, float gain=1.0f ); 
		void StopAudio( ALuint sourceId );
		void StopAll();

		void Pause();
		void UnPause();
		bool IsSystemPaused()						{ return m_SystemPaused; }

		// snd::MusicPauseCall
		void PauseMusic();
		void UnPauseMusic();
	
		void SetSFXState( bool state )				{ m_SFXState = state; }
		bool GetSFXState()							{ return m_SFXState; }

	protected:
		snd::MusicMode m_MusicMode;
	
		snd::SoundStream m_BGMStream;

		ALuint m_UIBufferId;

		bool m_SFXState;
		int m_CurrentBGMTrack;

		bool m_IsPlaying;
		bool m_IsPaused;

		bool m_MusicWasPlaying;
		bool m_MusicWasPaused;

		bool m_SystemPaused;
};

#endif // BASE_PLATFORM_RASPBERRYPI

#endif // __RASPBERRYPIAUDIOSYSTEM_H__

