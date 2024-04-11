

/*===================================================================
	File: LevelStartUI.h
	Game: AirCadets

	(C)Hidden Games
=====================================================================*/

#ifndef __LEVELSTARTUI_H__
#define __LEVELSTARTUI_H__

// forward declare
#include "GameStates/UI/UIBaseState.h"
#include "GameStates/IBaseGameState.h"

#include "GameStates/UI/UIFileLoader.h"
#include "Profiles/ProfileManager.h"

class LevelStartUI : public snd::MusicCallback, public IBaseGameState, public UIBaseState
{
	public:
		LevelStartUI( StateManager& stateManager, InputSystem& inputSystem );
		virtual ~LevelStartUI();

		virtual void Enter();
		virtual void Exit();
		virtual void PrepareTransitionIn();
		virtual int TransitionIn();
		virtual void PrepareTransitionOut();
		virtual int TransitionOut();

		virtual void Update( float deltaTime );
		virtual void Draw();

		virtual void PlayingSongChanged();
		virtual void PlaybackStateChanged();

		void SetFinishedLoading();

		void FormatPlaylistName(const char* playlistName);
		void FormatSongName(const char* songName);

	protected:
		float m_LastDeltaTime;

		GameData m_GameData;

		mdl::ModelHGM* m_UIMesh;
		UIFileLoader m_Elements;

		bool m_IsLoading;
		bool m_bLevelStarted;

		ProfileManager::Profile* m_pProfile;
	
		// music
		const char* m_CurrentSong;
		const char* m_Playlist;
		const char* m_Song;
		int m_NumPlaylists;
		int m_NumSongsInPlaylist;
		
		// if is using 
		int m_CurrentPlaylistIndex;
		int m_CurrentSongIndex;
	
};

#endif // __LEVELSTARTUI_H__

