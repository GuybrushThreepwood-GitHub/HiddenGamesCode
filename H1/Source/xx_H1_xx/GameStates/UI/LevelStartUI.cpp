
/*===================================================================
	File: LevelStartUI.cpp
	Game: AirCadets

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"
#include "MathBase.h"
#include "RenderBase.h"
#include "SoundBase.h"
#include "CollisionBase.h"
#include "Input/Input.h"
#include "ModelBase.h"
#include "ScriptBase.h"
#include "H1Consts.h"
#include "H1.h"

#include "StateManage/IState.h"

#include "Resources/ModelResources.h"
#include "Resources/StringResources.h"

#include "GameEffects/FullscreenEffects.h"

#include "GameStates/UI/UIIds.h"
#include "GameStates/UI/UIIds.h"
#include "GameStates/UI/UIBaseState.h"
#include "GameStates/UI/LevelStartUI.h"
#include "GameStates/UI/UIFileLoader.h"
#include "GameStates/UI/FrontendMainMenuUI.h"
#include "GameStates/UI/MainGameUI.h"

#include "GameStates/TitleScreenState.h"
#include "GameStates/MainGameState.h"

#include "GameStates/UI/GeneratedFiles/ingame_levelstart.hgm.h"
#include "GameStates/UI/GeneratedFiles/ingame_levelstart.hui.h"

namespace
{

}

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
LevelStartUI::LevelStartUI( StateManager& stateManager, InputSystem& inputSystem )
: IBaseGameState( stateManager, inputSystem, UI_LEVELSTART )
{
	m_UIMesh = 0;

	m_GameData = GetScriptDataHolder()->GetGameData();

	m_pProfile = 0;
	m_pProfile = ProfileManager::GetInstance()->GetProfile();
	DBG_ASSERT( m_pProfile != 0 );

	m_IsLoading = true;
	m_bLevelStarted = false;
}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
LevelStartUI::~LevelStartUI()
{

}

/////////////////////////////////////////////////////
/// Method: Enter
/// Params: None
///
/////////////////////////////////////////////////////
void LevelStartUI::Enter()
{
	math::Vec2 srcDims( static_cast<float>( core::app::GetBaseAssetsWidth() ), static_cast<float>( core::app::GetBaseAssetsHeight() ) );
	m_UIMesh = res::LoadModel(1006);
	DBG_ASSERT( m_UIMesh != 0 );

	m_Elements.Load( "assets/ui/ingame_levelstart.hui", srcDims, m_UIMesh );

	m_UIMesh->SetMeshDrawState( -1, false );
	m_Elements.ChangeElementDrawState( false );

	ScriptDataHolder* pScriptData = GetScriptDataHolder();
	DBG_ASSERT( pScriptData != 0 );

	m_UIMesh->SetMeshDrawState( GFX_BG_SKY, true );
	m_UIMesh->SetMeshDrawState( BG_LEVELNAME, true );

	m_Elements.ChangeElementText( HUI_TEXT_LEVELNAME, "%s", res::GetScriptString(60) );
	m_Elements.ChangeElementDrawState( HUI_TEXT_LEVELNAME, true );
	
	m_Elements.RefreshSelected();
    
	m_CurrentSong = 0;
	m_Playlist = 0;
	m_Song = 0;
	m_NumPlaylists = 0;
	m_NumSongsInPlaylist = 0;
	m_CurrentPlaylistIndex = -1;
	m_CurrentSongIndex = -1;	
	
	AudioSystem::GetInstance()->SetMusicCallback(this);
	AudioSystem::GetInstance()->UpdatePlaylists();

	if( !m_pProfile->adsRemoved )
	{
		core::app::SetAdBarState( true );
		#ifdef BASE_PLATFORM_WINDOWS
			core::app::SetAdBarState(  GetScriptDataHolder()->GetDevData().showPCAdBar );
		#endif // BASE_PLATFORM_WINDOWS
	}
    
    core::app::SetHandleMenuButton(true);
}

/////////////////////////////////////////////////////
/// Method: Exit
/// Params: None
///
/////////////////////////////////////////////////////
void LevelStartUI::Exit()
{
	AudioSystem::GetInstance()->SetMusicCallback(0);

	if (m_UIMesh != 0)
	{
		res::RemoveModel(m_UIMesh);
		m_UIMesh = 0;
	}
}

/////////////////////////////////////////////////////
/// Method: PrepareTransitionIn
/// Params: None
///
/////////////////////////////////////////////////////
void LevelStartUI::PrepareTransitionIn()
{
	m_FinishedTransitionIn = false;
	m_FadeTransition = 255;
}

/////////////////////////////////////////////////////
/// Method: PrepareTransitionOut
/// Params: None
///
/////////////////////////////////////////////////////
void LevelStartUI::PrepareTransitionOut()
{
	m_FinishedTransitionOut = false;
	m_FadeTransition = 0;
}

/////////////////////////////////////////////////////
/// Method: TransitionIn
/// Params: None
///
/////////////////////////////////////////////////////
int LevelStartUI::TransitionIn()
{
	if (!m_FinishedTransitionIn)
	{
		// draw normal
		Draw();

		renderer::OpenGL::GetInstance()->BlendMode(true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// draw fade
		m_FadeTransition -= 15;
		if (m_FadeTransition <= 0)
		{
			m_FadeTransition = 0;
			m_FinishedTransitionIn = true;
		}
		DrawFullscreenQuad(0.0f, m_FadeTransition);

		GameSystems::GetInstance()->DrawAchievementUI();

		renderer::OpenGL::GetInstance()->BlendMode(false, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		return(1);
	}

	// start load of the level
	m_IsLoading = true;
	m_pStateManager->ChangeSecondaryState(new MainGameState(*m_pStateManager->GetSecondaryStateManager(), m_InputSystem));

	// transition finished, make sure to do a clean draw
	Draw();
	return(0);
}

/////////////////////////////////////////////////////
/// Method: TransitionOut
/// Params: None
///
/////////////////////////////////////////////////////
int LevelStartUI::TransitionOut()
{
	if (!m_FinishedTransitionOut)
	{
		// draw normal
		Draw();

		renderer::OpenGL::GetInstance()->BlendMode(true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// draw fade
		m_FadeTransition += 15;
		if (m_FadeTransition >= 255)
		{
			m_FadeTransition = 255;
			m_FinishedTransitionOut = true;
		}
		DrawFullscreenQuad(0.0f, m_FadeTransition);

		GameSystems::GetInstance()->DrawAchievementUI();

		renderer::OpenGL::GetInstance()->BlendMode(false, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		return(1);
	}

	// game can start
	if (m_bLevelStarted &&
		GetStateManager()->GetSecondaryStateManager())
	{
		MainGameState* pGameState = static_cast<MainGameState*>(GetStateManager()->GetSecondaryStateManager()->GetCurrentState());

		pGameState->UIStartLevel();
	}

	return(0);
}

/////////////////////////////////////////////////////
/// Method: Update
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
void LevelStartUI::Update(float deltaTime)
{
	m_LastDeltaTime = deltaTime;

	// new
	if (m_InputSystem.GetUIUp())
	{
		m_Elements.SelectPreviousRow();
	}
	else
		if (m_InputSystem.GetUIDown())
		{
			m_Elements.SelectNextRow();
		}
		else
			if (m_InputSystem.GetUILeft())
			{
				m_Elements.SelectPreviousColumn();
			}
			else
				if (m_InputSystem.GetUIRight())
				{
					m_Elements.SelectNextColumn();
				}

	if (m_InputSystem.GetUISelect())
		m_Elements.SelectionPress(true);
	else
		m_Elements.SelectionPress(false);

	m_Elements.Update(TOUCH_SIZE_MENU, 2, deltaTime);

	if (!m_IsLoading)
	{
		// music
		if (m_Elements.CheckElementForTouch(HUI_BUTTON_PREVIOUS_PLAYLIST))
		{
			if (m_NumPlaylists > 0)
			{
				int prevPlaylist = m_CurrentPlaylistIndex;

				if (m_CurrentPlaylistIndex != -1 &&
					m_CurrentSongIndex != -1)
				{
					m_CurrentPlaylistIndex--;
					if (m_CurrentPlaylistIndex < 0)
					{
						if (!AudioSystem::GetInstance()->IsUsingPlaylists())
						{
							m_CurrentPlaylistIndex = -1;
							m_CurrentSongIndex = -1;

							//m_UIMesh->SetMeshDrawState(BTN_PREV_TRACK, false);
							//m_UIMesh->SetMeshDrawState(BTN_NEXT_TRACK, false);
							//m_UIMesh->SetMeshDrawState(BTN_SHUFFLE, false);
							//m_UIMesh->SetMeshDrawState(BTN_REPEAT, false);

							m_Elements.ChangeElementDrawState(HUI_GLYPH_PREV_TRACK, false);
							m_Elements.ChangeElementDrawState(HUI_GLYPH_NEXT_TRACK, false);
							m_Elements.ChangeElementDrawState(HUI_GLYPH_SHUFFLE, false);
							m_Elements.ChangeElementDrawState(HUI_GLYPH_REPEAT, false);

							m_Elements.ChangeElementDrawState(HUI_BUTTON_PREV_TRACK, false);
							m_Elements.ChangeElementDrawState(HUI_BUTTON_NEXT_TRACK, false);
							m_Elements.ChangeElementDrawState(HUI_BUTTON_SHUFFLE, false);
							m_Elements.ChangeElementDrawState(HUI_BUTTON_REPEAT, false);

						}
						else
						{
							m_CurrentPlaylistIndex = 0;

							//m_UIMesh->SetMeshDrawState(BTN_PREV_TRACK, true);
							//m_UIMesh->SetMeshDrawState(BTN_NEXT_TRACK, true);
							//m_UIMesh->SetMeshDrawState( BTN_SHUFFLE, true );
							//m_UIMesh->SetMeshDrawState( BTN_REPEAT, true );

							m_Elements.ChangeElementDrawState(HUI_GLYPH_PREV_TRACK, true);
							m_Elements.ChangeElementDrawState(HUI_GLYPH_NEXT_TRACK, true);
							m_Elements.ChangeElementDrawState(HUI_BUTTON_PREV_TRACK, true);
							m_Elements.ChangeElementDrawState(HUI_BUTTON_NEXT_TRACK, true);

							//m_Elements.ChangeElementDrawState( HUI_GLYPH_SHUFFLE, true);
							//m_Elements.ChangeElementDrawState( HUI_GLYPH_REPEAT, true);
						}
					}

					if (m_CurrentPlaylistIndex != prevPlaylist)
						m_CurrentSongIndex = 0;

					// get the first playlist and song
					if (m_CurrentPlaylistIndex != -1 &&
						m_CurrentSongIndex != -1)
					{

						m_NumSongsInPlaylist = AudioSystem::GetInstance()->GetTotalNumberOfSongsInPlaylist(m_CurrentPlaylistIndex);
						m_Playlist = AudioSystem::GetInstance()->GetPlaylistName(m_CurrentPlaylistIndex);
						m_Song = AudioSystem::GetInstance()->GetSongName(m_CurrentPlaylistIndex, m_CurrentSongIndex);

						m_Elements.ChangeElementDrawState(HUI_TEXT_PLAYLISTNAME, true);
						FormatPlaylistName(m_Playlist);

						m_Elements.ChangeElementDrawState(HUI_TEXT_SONGNAME, true);
						FormatSongName(m_Song);

					}
					else
					{
						// current
						m_Elements.ChangeElementText(HUI_TEXT_PLAYLISTNAME, res::GetScriptString(1122));

						// what's currently playing
						if (AudioSystem::GetInstance()->IsPlaying())
						{
							m_CurrentSong = AudioSystem::GetInstance()->GetCurrentSong();

							if (m_CurrentSong != 0 &&
								!core::IsEmptyString(m_CurrentSong))
							{
								m_Elements.ChangeElementDrawState(HUI_TEXT_SONGNAME, true);
								FormatSongName(m_CurrentSong);
							}
						}
						else
						{
							m_Elements.ChangeElementDrawState(HUI_TEXT_SONGNAME, false);
						}

					}
				}
			}
		}
		else
			if (m_Elements.CheckElementForTouch(HUI_BUTTON_NEXT_PLAYLIST))
			{
				if (m_NumPlaylists > 0)
				{
					int prevPlaylist = m_CurrentPlaylistIndex;

					m_CurrentPlaylistIndex++;
					if (m_CurrentPlaylistIndex >= m_NumPlaylists)
						m_CurrentPlaylistIndex = m_NumPlaylists - 1;

					if (m_CurrentPlaylistIndex != prevPlaylist)
						m_CurrentSongIndex = 0;

					//m_UIMesh->SetMeshDrawState(BTN_PREV_TRACK, true);
					//m_UIMesh->SetMeshDrawState(BTN_NEXT_TRACK, true);
					//m_UIMesh->SetMeshDrawState( BTN_SHUFFLE, true );
					//m_UIMesh->SetMeshDrawState( BTN_REPEAT, true );

					m_Elements.ChangeElementDrawState(HUI_GLYPH_PREV_TRACK, true);
					m_Elements.ChangeElementDrawState(HUI_GLYPH_NEXT_TRACK, true);
					m_Elements.ChangeElementDrawState(HUI_BUTTON_PREV_TRACK, true);
					m_Elements.ChangeElementDrawState(HUI_BUTTON_NEXT_TRACK, true);

					//m_Elements.ChangeElementDrawState( HUI_GLYPH_SHUFFLE, true);
					//m_Elements.ChangeElementDrawState( HUI_GLYPH_REPEAT, true);

					// get the first playlist and song
					m_NumSongsInPlaylist = AudioSystem::GetInstance()->GetTotalNumberOfSongsInPlaylist(m_CurrentPlaylistIndex);
					m_Playlist = AudioSystem::GetInstance()->GetPlaylistName(m_CurrentPlaylistIndex);
					m_Song = AudioSystem::GetInstance()->GetSongName(m_CurrentPlaylistIndex, m_CurrentSongIndex);

					m_Elements.ChangeElementDrawState(HUI_TEXT_PLAYLISTNAME, true);
					FormatPlaylistName(m_Playlist);

					m_Elements.ChangeElementDrawState(HUI_TEXT_SONGNAME, true);
					FormatSongName(m_Song);
				}
			}

		if (m_Elements.CheckElementForTouch(HUI_BUTTON_PREV_TRACK))
		{
			if (m_NumSongsInPlaylist > 0)
			{
				if (m_CurrentPlaylistIndex != -1 &&
					m_CurrentSongIndex != -1)
				{
					m_CurrentSongIndex--;
					if (m_CurrentSongIndex < 0)
						m_CurrentSongIndex = 0;

					// get the song
					m_Song = AudioSystem::GetInstance()->GetSongName(m_CurrentPlaylistIndex, m_CurrentSongIndex);

					m_Elements.ChangeElementDrawState(HUI_TEXT_SONGNAME, true);
					FormatSongName(m_Song);
				}
				else
				{
					/*AudioSystem::GetInstance()->SkipToPreviousSong();

					// get the song
					m_Song = AudioSystem::GetInstance()->GetCurrentSong();

					m_Elements.ChangeElementDrawState(HUI_TEXT_SONGNAME, true );
					FormatSongName(m_Song );	*/
				}
			}
			else
			{
				/*AudioSystem::GetInstance()->SkipToPreviousSong();

				// get the song
				m_Song = AudioSystem::GetInstance()->GetCurrentSong();

				m_Elements.ChangeElementDrawState(HUI_TEXT_SONGNAME, true );
				FormatSongName( m_Song );		*/
			}
		}
		else
			if (m_Elements.CheckElementForTouch(HUI_BUTTON_NEXT_TRACK))
			{
				if (m_NumSongsInPlaylist > 0)
				{
					if (m_CurrentPlaylistIndex != -1 &&
						m_CurrentSongIndex != -1)
					{
						m_CurrentSongIndex++;
						if (m_CurrentSongIndex >= m_NumSongsInPlaylist)
							m_CurrentSongIndex = m_NumSongsInPlaylist - 1;

						// get the first playlist and song
						m_Song = AudioSystem::GetInstance()->GetSongName(m_CurrentPlaylistIndex, m_CurrentSongIndex);

						m_Elements.ChangeElementDrawState(HUI_TEXT_SONGNAME, true);
						FormatSongName(m_Song);
					}
					else
					{
						/*AudioSystem::GetInstance()->SkipToNextSong();

						// get the song
						m_Song = AudioSystem::GetInstance()->GetCurrentSong();

						m_Elements.ChangeElementDrawState(HUI_TEXT_SONGNAME, true );
						FormatSongName( m_Song );	*/
					}
				}
				else
				{
					/*AudioSystem::GetInstance()->SkipToNextSong();

					// get the song
					m_Song = AudioSystem::GetInstance()->GetCurrentSong();

					m_Elements.ChangeElementDrawState(HUI_TEXT_SONGNAME, true );
					FormatSongName( m_Song );	*/
				}
			}

		if (m_Elements.CheckElementForTouch(HUI_BUTTON_PLAY))
		{
			if (m_NumPlaylists > 0)
			{
				if (m_CurrentPlaylistIndex != -1 &&
					m_CurrentSongIndex != -1)
				{
					if (AudioSystem::GetInstance()->IsPlaying() &&
						AudioSystem::GetInstance()->IsPaused())
					{
						AudioSystem::GetInstance()->ResumeSong();
					}
					else
					{
						// get the first playlist and song
						AudioSystem::GetInstance()->PlaySong(m_CurrentPlaylistIndex, m_CurrentSongIndex);
					}
				}
				else
				{
					if (AudioSystem::GetInstance()->IsPlaying() &&
						AudioSystem::GetInstance()->IsPaused())
					{
						AudioSystem::GetInstance()->ResumeSong();
					}
				}
			}
		}
		else
			if (m_Elements.CheckElementForTouch(HUI_BUTTON_PAUSE))
			{
				AudioSystem::GetInstance()->PauseSong();
			}
		/*else
		if( m_Elements.CheckElementForTouch(HUI_BUTTON_SHUFFLE) )
		{

		}
		else
		if( m_Elements.CheckElementForTouch(HUI_BUTTON_REPEAT) )
		{

		}*/

		// game
		if (m_Elements.CheckElementForTouch(HUI_BTN_NEXT))
		{
			AudioSystem::GetInstance()->PlayUIAudio();

			m_bLevelStarted = true;

			ChangeState(new MainGameUI(*m_pStateManager, m_InputSystem));
			return;
		}
		else
			if (m_Elements.CheckElementForTouch(HUI_BTN_BACK) ||
				m_InputSystem.GetMenu())
			{
				AudioSystem::GetInstance()->PlayUIAudio();

				m_bLevelStarted = false;

			m_pStateManager->ChangeSecondaryState( new TitleScreenState(*m_pStateManager->GetSecondaryStateManager(), m_InputSystem) );	
			ChangeState( new FrontendMainMenuUI(*m_pStateManager, m_InputSystem, FrontendMainMenuUI::FrontendStates_MainMenu) );
				return;
			}
	}
}

/////////////////////////////////////////////////////
/// Method: Draw
/// Params: None
///
/////////////////////////////////////////////////////
void LevelStartUI::Draw()
{
	renderer::OpenGL::GetInstance()->SetNearFarClip(NEAR_CLIP_ORTHO, FAR_CLIP_ORTHO);
	renderer::OpenGL::GetInstance()->SetupOrthographicView(core::app::GetOrientationWidth(), core::app::GetOrientationHeight(), true);
	renderer::OpenGL::GetInstance()->DisableLighting();

	renderer::OpenGL::GetInstance()->SetColour4ub(255, 255, 255, 255);

	renderer::OpenGL::GetInstance()->DepthMode(false, GL_ALWAYS);
	renderer::OpenGL::GetInstance()->BlendMode(true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	if (m_UIMesh != 0)
		m_UIMesh->Draw();

	m_Elements.Draw();
	m_Elements.DrawSelected();

	GameSystems::GetInstance()->DrawAchievementUI();

	renderer::OpenGL::GetInstance()->DepthMode(true, GL_LESS);
	renderer::OpenGL::GetInstance()->BlendMode(false, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

/////////////////////////////////////////////////////
/// Method: PlayingSongChanged
/// Params: None
///
/////////////////////////////////////////////////////
void LevelStartUI::PlayingSongChanged()
{
	// only change the song is not using playlists
	if (!AudioSystem::GetInstance()->IsUsingPlaylists())
	{
		// what's currently playing
		m_CurrentSong = AudioSystem::GetInstance()->GetCurrentSong();

		if (m_CurrentSong != 0 &&
			!core::IsEmptyString(m_CurrentSong))
		{
			m_Elements.ChangeElementDrawState(HUI_TEXT_SONGNAME, true);
			FormatSongName(m_CurrentSong);
		}
	}
}

/////////////////////////////////////////////////////
/// Method: PlaybackStateChanged
/// Params: None
///
/////////////////////////////////////////////////////
void LevelStartUI::PlaybackStateChanged()
{

}

/////////////////////////////////////////////////////
/// Method: SetFinishedLoading
/// Params: None
///
/////////////////////////////////////////////////////
void LevelStartUI::SetFinishedLoading()
{
	m_IsLoading = false;

	//m_UIMesh->SetMeshDrawState(BTN_QUIT, true);
	m_Elements.ChangeElementDrawState(HUI_GLYPH_EXIT, true);

	//m_UIMesh->SetMeshDrawState(BTN_START, true);
	m_Elements.ChangeElementDrawState(HUI_GLYPH_NEXT, true);

	m_UIMesh->SetMeshDrawState(BG_LEVELNAME, true);

	if (!core::app::IstvOS())
	{
		m_Elements.ChangeElementDrawState(HUI_TEXT_PLAYLISTITLE, true);

		m_NumPlaylists = AudioSystem::GetInstance()->GetTotalNumberOfPlaylists();

		// if already using playlists
		if (AudioSystem::GetInstance()->IsUsingPlaylists())
		{
			m_UIMesh->SetMeshDrawState(BG_BGM_SELECT, true);

			m_Elements.ChangeElementDrawState(HUI_BUTTON_PREVIOUS_PLAYLIST, true);
			m_Elements.ChangeElementDrawState(HUI_BUTTON_NEXT_PLAYLIST, true);

			// do we have some playlists to go through
			if (m_NumPlaylists > 0)
			{
				//m_UIMesh->SetMeshDrawState(BTN_PREV_TRACK, true);
				//m_UIMesh->SetMeshDrawState(BTN_NEXT_TRACK, true);

				m_Elements.ChangeElementDrawState(HUI_GLYPH_PREV_TRACK, true);
				m_Elements.ChangeElementDrawState(HUI_GLYPH_NEXT_TRACK, true);
				m_Elements.ChangeElementDrawState(HUI_BUTTON_PREV_TRACK, true);
				m_Elements.ChangeElementDrawState(HUI_BUTTON_NEXT_TRACK, true);

				m_Elements.ChangeElementDrawState(HUI_BUTTON_PREVIOUS_PLAYLIST, true);
				m_Elements.ChangeElementDrawState(HUI_BUTTON_NEXT_PLAYLIST, true);
				m_Elements.ChangeElementDrawState(HUI_GLYPH_PREVPLAYLIST, true);
				m_Elements.ChangeElementDrawState(HUI_GLYPH_NEXTPLAYLIST, true);

				// get the current playlist and song index
				m_CurrentPlaylistIndex = AudioSystem::GetInstance()->GetCurrentPlaylistIndex();
				m_CurrentSongIndex = AudioSystem::GetInstance()->GetCurrentSongIndex();

				if (AudioSystem::GetInstance()->IsPlaying())
				{
					// if we are playing the song
					if (m_CurrentPlaylistIndex != -1 &&
						m_CurrentSongIndex != -1)
					{
						m_NumSongsInPlaylist = AudioSystem::GetInstance()->GetTotalNumberOfSongsInPlaylist(m_CurrentPlaylistIndex);

						m_Playlist = AudioSystem::GetInstance()->GetPlaylistName(m_CurrentPlaylistIndex);
						m_Song = AudioSystem::GetInstance()->GetSongName(m_CurrentPlaylistIndex, m_CurrentSongIndex);

						m_Elements.ChangeElementDrawState(HUI_TEXT_PLAYLISTNAME, true);
						FormatPlaylistName(m_Playlist);

						m_Elements.ChangeElementDrawState(HUI_TEXT_SONGNAME, true);
						FormatSongName(m_Song);
					}
				}
				else
				{
					// get the first playlist and song
					m_NumSongsInPlaylist = AudioSystem::GetInstance()->GetTotalNumberOfSongsInPlaylist(0);
					m_Playlist = AudioSystem::GetInstance()->GetPlaylistName(0);
					m_Song = AudioSystem::GetInstance()->GetSongName(0, 0);

					m_CurrentPlaylistIndex = 0;
					m_CurrentSongIndex = 0;

					m_Elements.ChangeElementDrawState(HUI_TEXT_PLAYLISTNAME, true);
					FormatPlaylistName(m_Playlist);

					m_Elements.ChangeElementDrawState(HUI_TEXT_SONGNAME, true);
					FormatSongName(m_Song);
				}
			}
			else
			{
				// no playlists to go through
				m_Elements.ChangeElementDrawState(HUI_TEXT_PLAYLISTNAME, true);
				m_Elements.ChangeElementText(HUI_TEXT_PLAYLISTNAME, res::GetScriptString(1123));
			}
		}
		else
		{
			// not using playlists yet
			m_UIMesh->SetMeshDrawState(BG_BGM_SELECT, true);
			//m_UIMesh->SetMeshDrawState(BTN_PREV_TRACK, false);
			//m_UIMesh->SetMeshDrawState(BTN_NEXT_TRACK, false);
			//m_UIMesh->SetMeshDrawState(BTN_SHUFFLE, false);
			//m_UIMesh->SetMeshDrawState(BTN_REPEAT, false);
			m_Elements.ChangeElementDrawState(HUI_BUTTON_PREV_TRACK, false);
			m_Elements.ChangeElementDrawState(HUI_BUTTON_NEXT_TRACK, false);
			m_Elements.ChangeElementDrawState(HUI_BUTTON_SHUFFLE, false);
			m_Elements.ChangeElementDrawState(HUI_BUTTON_REPEAT, false);

			m_Elements.ChangeElementDrawState(HUI_GLYPH_PREVPLAYLIST, true);
			m_Elements.ChangeElementDrawState(HUI_GLYPH_NEXTPLAYLIST, true);
			m_Elements.ChangeElementDrawState(HUI_BUTTON_PREVIOUS_PLAYLIST, true);
			m_Elements.ChangeElementDrawState(HUI_BUTTON_NEXT_PLAYLIST, true);

			// are there any playlists
			if (m_NumPlaylists <= 0)
			{
				m_Elements.ChangeElementDrawState(HUI_TEXT_PLAYLISTNAME, true);

				if (AudioSystem::GetInstance()->IsPlaying())
				{
					m_Elements.ChangeElementText(HUI_TEXT_PLAYLISTNAME, res::GetScriptString(1122));

					// what's currently playing
					m_CurrentSong = AudioSystem::GetInstance()->GetCurrentSong();

					if (m_CurrentSong != 0 &&
						!core::IsEmptyString(m_CurrentSong))
					{
						m_Elements.ChangeElementDrawState(HUI_TEXT_SONGNAME, true);
						FormatSongName(m_CurrentSong);
					}
				}
				else
					m_Elements.ChangeElementText(HUI_TEXT_PLAYLISTNAME, res::GetScriptString(1123));

			}
			else
			{
				// there is a playlist
				m_CurrentPlaylistIndex = -1;
				m_CurrentSongIndex = -1;

				if (AudioSystem::GetInstance()->IsPlaying())
				{
					m_Elements.ChangeElementDrawState(HUI_TEXT_PLAYLISTNAME, true);
					m_Elements.ChangeElementText(HUI_TEXT_PLAYLISTNAME, res::GetScriptString(1122));

					// what's currently playing
					m_CurrentSong = AudioSystem::GetInstance()->GetCurrentSong();

					if (m_CurrentSong != 0 &&
						!core::IsEmptyString(m_CurrentSong))
					{
						m_Elements.ChangeElementDrawState(HUI_TEXT_SONGNAME, true);
						FormatSongName(m_CurrentSong);
					}
				}
				else
				{
					m_CurrentPlaylistIndex = 0;
					m_CurrentSongIndex = 0;

					//m_UIMesh->SetMeshDrawState(BTN_PREV_TRACK, true);
					//m_UIMesh->SetMeshDrawState(BTN_NEXT_TRACK, true);

					//m_UIMesh->SetMeshDrawState( BTN_SHUFFLE, true );
					//m_UIMesh->SetMeshDrawState( BTN_REPEAT, true );

					m_Elements.ChangeElementDrawState(HUI_GLYPH_PREV_TRACK, true);
					m_Elements.ChangeElementDrawState(HUI_GLYPH_NEXT_TRACK, true);
					m_Elements.ChangeElementDrawState(HUI_BUTTON_PREV_TRACK, true);
					m_Elements.ChangeElementDrawState(HUI_BUTTON_NEXT_TRACK, true);

					//m_Elements.ChangeElementDrawState( HUI_GLYPH_SHUFFLE, true);
					//m_Elements.ChangeElementDrawState( HUI_GLYPH_REPEAT, true);

					// get the first playlist and song
					m_NumSongsInPlaylist = AudioSystem::GetInstance()->GetTotalNumberOfSongsInPlaylist(0);
					m_Playlist = AudioSystem::GetInstance()->GetPlaylistName(0);
					m_Song = AudioSystem::GetInstance()->GetSongName(0, 0);

					m_Elements.ChangeElementDrawState(HUI_TEXT_PLAYLISTNAME, true);
					FormatPlaylistName(m_Playlist);

					m_Elements.ChangeElementDrawState(HUI_TEXT_SONGNAME, true);
					FormatSongName(m_Song);
				}
			}
		}
	}

	/*ScriptDataHolder* pScriptData = GetScriptDataHolder();
	DBG_ASSERT(pScriptData != 0);
	ScriptDataHolder::LevelScriptData levelData = pScriptData->GetLevelData();

	if (ProfileManager::GetInstance()->GetGameMode() == ProfileManager::CAREER_MODE)
		m_Elements.ChangeElementText(HUI_TEXT_LEVELNAME, "%d / %d : %s", m_pProfile->currentLevelIndex + 1, LEVELS_IN_A_CAREER, levelData.levelName);
	else
		m_Elements.ChangeElementText(HUI_TEXT_LEVELNAME, "%s", levelData.levelName);

	if (ProfileManager::GetInstance()->GetGameMode() == ProfileManager::ARCADE_MODE)
	{

	}
	else
	{
		m_UIMesh->SetMeshDrawState(BG_TARGET, true);
		m_Elements.ChangeElementDrawState(HUI_LABEL_TARGET, true);

		m_Elements.ChangeElementText(HUI_LABEL_TIMES, "%s", res::GetScriptString(61));
		m_Elements.ChangeElementDrawState(HUI_LABEL_TIMES, true);

		m_UIMesh->SetMeshDrawState(ICON_CUSTOMER, true);
		m_Elements.ChangeElementText(HUI_TEXT_CUSTOMERCOUNT, "%d", levelData.levelCustomerTotal);
		m_Elements.ChangeElementDrawState(HUI_TEXT_CUSTOMERCOUNT, true);
	}*/

	//m_Elements.ChangeElementDrawState(HUI_LABEL_START, true);

	if (m_NumPlaylists > 0)
	{
		//m_UIMesh->SetMeshDrawState(BTN_PLAY, true);
		//m_UIMesh->SetMeshDrawState(BTN_PAUSE, true);

		m_Elements.ChangeElementDrawState(HUI_BUTTON_PLAY, true);
		m_Elements.ChangeElementDrawState(HUI_BUTTON_PAUSE, true);
		m_Elements.ChangeElementDrawState(HUI_GLYPH_PLAY, true);
		m_Elements.ChangeElementDrawState(HUI_GLYPH_PAUSE, true);
	}

	m_Elements.ChangeElementDrawState(HUI_BTN_BACK, true);
	m_Elements.ChangeElementDrawState(HUI_BTN_NEXT, true);

	if (core::app::IstvOS())
	{
		m_Elements.ChangeElementText(HUI_TEXT_PLAYLISTNAME, "%s", res::GetScriptString(1124));
		m_Elements.ChangeElementDrawState(HUI_TEXT_PLAYLISTNAME, true);
		m_Elements.ChangeElementTextShadowState(HUI_TEXT_PLAYLISTNAME, true);
	}

	m_Elements.RefreshSelected(HUI_BTN_NEXT);
}

/////////////////////////////////////////////////////
/// Method: FormatPlaylistName
/// Params: None
///
/////////////////////////////////////////////////////
void LevelStartUI::FormatPlaylistName(const char* playlistName)
{
	if (playlistName == 0)
		m_Elements.ChangeElementText(HUI_TEXT_PLAYLISTNAME, "");
	else
	{
		// only place the maximum number of characters into the name (replace with ...)
		std::size_t len = std::strlen(playlistName);

		if (len <= 0)
			m_Elements.ChangeElementText(HUI_TEXT_PLAYLISTNAME, "");
		else
		{
			if (len < static_cast<std::size_t>(m_GameData.MAX_PLAYLISTNAME_CHARACTERS))
			{
				m_Elements.ChangeElementText(HUI_TEXT_PLAYLISTNAME, "%s", playlistName);
			}
			else
			{
				char maxString[core::MAX_PATH];
				memset(maxString, 0, sizeof(char)*core::MAX_PATH);

				snprintf(maxString, m_GameData.MAX_PLAYLISTNAME_CHARACTERS, "%s", playlistName);
				maxString[m_GameData.MAX_PLAYLISTNAME_CHARACTERS - 1] = '.';
				maxString[m_GameData.MAX_PLAYLISTNAME_CHARACTERS - 2] = '.';
				maxString[m_GameData.MAX_PLAYLISTNAME_CHARACTERS - 3] = '.';

				m_Elements.ChangeElementText(HUI_TEXT_PLAYLISTNAME, "%s", maxString);
			}
		}
	}
}

/////////////////////////////////////////////////////
/// Method: FormatSongName
/// Params: None
///
/////////////////////////////////////////////////////
void LevelStartUI::FormatSongName(const char* songName)
{
	if (songName == 0)
	{
		m_UIMesh->SetMeshDrawState(BG_TRACKNAME, false);
		m_Elements.ChangeElementText(HUI_TEXT_SONGNAME, "");
	}
	else
	{
		// only place the maximum number of characters into the name (replace with ...)
		std::size_t len = std::strlen(songName);

		if (len <= 0)
		{
			m_UIMesh->SetMeshDrawState(BG_TRACKNAME, false);
			m_Elements.ChangeElementText(HUI_TEXT_SONGNAME, "");
		}
		else
		{
			m_UIMesh->SetMeshDrawState(BG_TRACKNAME, true);
			if (len < static_cast<std::size_t>(m_GameData.MAX_SONGNAME_CHARACTERS))
			{
				m_Elements.ChangeElementText(HUI_TEXT_SONGNAME, "%s", songName);
			}
			else
			{
				char maxString[core::MAX_PATH];
				memset(maxString, 0, sizeof(char)*core::MAX_PATH);

				snprintf(maxString, m_GameData.MAX_SONGNAME_CHARACTERS, "%s", songName);
				maxString[m_GameData.MAX_SONGNAME_CHARACTERS - 1] = '.';
				maxString[m_GameData.MAX_SONGNAME_CHARACTERS - 2] = '.';
				maxString[m_GameData.MAX_SONGNAME_CHARACTERS - 3] = '.';

				m_Elements.ChangeElementText(HUI_TEXT_SONGNAME, "%s", maxString);
			}
		}
	}
}
