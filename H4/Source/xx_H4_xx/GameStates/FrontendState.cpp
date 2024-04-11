
/*===================================================================
	File: FrontendState.cpp
	Game: H4

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"
#include <cmath>
#include "MathBase.h"
#include "RenderBase.h"
#include "SoundBase.h"
#include "CollisionBase.h"
#include "ModelBase.h"
#include "InputBase.h"
#include "ScriptBase.h"
#include "PhysicsBase.h"

#include "AppConsts.h"
#include "H4.h"

#include "Resources/ModelResources.h"
#include "Resources/StringResources.h"
#include "GameEffects/FullscreenEffects.h"

#include "GameStates/IBaseGameState.h"

#include "GameStates/LoadingState.h"
#include "GameStates/FrontendState.h"
#include "GameStates/EndingState.h"

#include "GameStates/UI/UIFileLoader.h"

#include "GameStates/UI/GeneratedFiles/frontend.hgm.h"
#include "GameStates/UI/GeneratedFiles/frontend.hui.h"

#include "GameStates/UI/GeneratedFiles/optionsfront.hgm.h"
#include "GameStates/UI/GeneratedFiles/optionsfront.hui.h"

#include "GameStates/UI/GeneratedFiles/gameover.hgm.h"
#include "GameStates/UI/GeneratedFiles/gameover.hui.h"

namespace
{
	const int LOAD_FADE = 15;
}

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
FrontendState::FrontendState( StateManager& stateManager, InputSystem& inputSystem )
: IBaseGameState( stateManager, inputSystem )
{
	m_LastDelta = 0.0f;
	m_MovementTimer = 0.0f;
	m_CurrentMovement = math::Vec3( math::RandomNumber( -10.0f, 10.0f ), math::RandomNumber( -10.0f, 10.0f ), 0.0f );
	m_Alpha = math::RandomNumber( 64, 64 );

	m_pScriptData = 0;

	m_pScriptData = GetScriptDataHolder();
	m_DevData = m_pScriptData->GetDevData();

	m_LevelName = 0;
	m_LoadTransitionState = LoadState_None;

	m_FrontendMesh = 0;
	m_OptionsMesh = 0;
	m_RankMesh = 0;

	m_TransitionAlpha = 0;

	m_CreditsList = false;

	m_NewGame = false;
	m_ContinueGame = false;
}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
FrontendState::~FrontendState()
{

}

/////////////////////////////////////////////////////
/// Method: UpdateLoadState
/// Params: None
///
/////////////////////////////////////////////////////
bool FrontendState::UpdateLoadState()
{
	switch( m_LoadTransitionState )
	{
		case LoadState_None:

		break;
		case LoadState_TransitionOut:
		{
			m_TransitionAlpha += LOAD_FADE;
			
			core::app::SetSmoothUpdate(false);
			
			if( m_TransitionAlpha > 255 )
			{
				m_TransitionAlpha = 255;
	
				if( m_NewGame )
					ChangeState( new EndingState(0, *m_pStateManager, m_InputSystem) );	
				else if( m_ContinueGame )
					ChangeState( new LoadingState(*m_pStateManager, m_InputSystem) );	

				return(true);
			}
			return(false);
		}break;

		default:
			DBG_ASSERT(0);
		break;
	}

	return(false);
}

/////////////////////////////////////////////////////
/// Method: Enter
/// Params: None
///
/////////////////////////////////////////////////////
void FrontendState::Enter()
{
	math::Vec4Lite defaultColour( 255,255,255,255 );

	renderer::OpenGL::GetInstance()->ClearColour( 0.0f, 0.0f, 0.0f, 1.0f );

	// main menu
	m_FrontendMesh = res::LoadModel( 1002 );
	DBG_ASSERT( m_FrontendMesh != 0 );

	math::Vec3 scaleFactor = math::Vec3( core::app::GetWidthScale(), core::app::GetHeightScale(), 1.0f );

	m_FrontendMesh->SetDefaultMeshColour(defaultColour);
	math::Vec2 srcAssetDims( static_cast<float>(core::app::GetBaseAssetsWidth()), static_cast<float>(core::app::GetBaseAssetsHeight()) );
	m_FrontendElements.Load( "hui/frontend.hui", srcAssetDims, m_FrontendMesh );

	// options
	m_OptionsMesh = res::LoadModel( 1003 );
	DBG_ASSERT( m_OptionsMesh != 0 );

	m_OptionsMesh->EnableDefaultMeshColour();
	m_OptionsElements.Load( "hui/optionsfront.hui", srcAssetDims, m_OptionsMesh );

	// rank
	m_RankMesh = res::LoadModel( 1008 );
	DBG_ASSERT( m_RankMesh != 0 );

	m_RankMesh->EnableDefaultMeshColour();
	m_RankElements.Load( "hui/gameover.hui", srcAssetDims, m_RankMesh );

	// load options
	GameSystems::GetInstance()->LoadOptions();
	m_CurrentOptions = GameSystems::GetInstance()->GetOptions();

	// load general data
	GameSystems::GetInstance()->LoadGeneralData();
	m_CurrentGeneralData = GameSystems::GetInstance()->GetGeneralData();

	// load best rank
	GameSystems::GetInstance()->LoadBestRank();
	m_CurrentBestRank = GameSystems::GetInstance()->GetBestRankData();

	m_InOptions = false;
	SetupState( FrontendState_Main );

	AudioSystem::GetInstance()->StopBGMTrack();
	AudioSystem::GetInstance()->ClearBGMTrack();
	AudioSystem::GetInstance()->PlayBGMTrack( "mp3/Prisoner84-title.mp3", true );
	AudioSystem::GetInstance()->SetBGMTrackVolume( 1.0f );
}

/////////////////////////////////////////////////////
/// Method: Exit
/// Params: None
///
/////////////////////////////////////////////////////
void FrontendState::Exit()
{
	GameSystems::GetInstance()->SetOptions( m_CurrentOptions );
	GameSystems::GetInstance()->SaveOptions();

	GameSystems::GetInstance()->SetBestRankData( m_CurrentBestRank );
	GameSystems::GetInstance()->SaveBestRank();

	GameSystems::GetInstance()->SetGeneralData( m_CurrentGeneralData );
	GameSystems::GetInstance()->SaveGeneralData();

	res::RemoveModel( m_FrontendMesh );
	m_FrontendMesh = 0;

	res::RemoveModel( m_OptionsMesh );
	m_OptionsMesh = 0;

	res::RemoveModel( m_RankMesh );
	m_RankMesh = 0;

	//if( !m_DevData.disableMusic )
	{
		AudioSystem::GetInstance()->StopBGMTrack();
		AudioSystem::GetInstance()->ClearBGMTrack();
	}
}

/////////////////////////////////////////////////////
/// Method: TransitionIn
/// Params: None
///
/////////////////////////////////////////////////////
int FrontendState::TransitionIn()
{
	return(0);
}

/////////////////////////////////////////////////////
/// Method: TransitionOut
/// Params: None
///
/////////////////////////////////////////////////////
int FrontendState::TransitionOut()
{
	return(0);
}

/////////////////////////////////////////////////////
/// Method: Update
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
void FrontendState::Update( float deltaTime )
{
	int i=0;
	m_LastDelta = deltaTime;

	// do any transitions
	if( UpdateLoadState() )
	{
		// returning true means the state has changed, this is not a valid class anymore
		return;
	}

	// fading out
	if( m_LoadTransitionState != LoadState_None )
		return;

	if( m_InOptions )
		m_OptionsElements.Update( TOUCH_SIZE*0.5f, 1, deltaTime );
	else
		m_FrontendElements.Update( TOUCH_SIZE, 1, deltaTime );

	m_MovementTimer += deltaTime;

	if( m_MovementTimer >= 0.1f )
	{
		m_MovementTimer = 0.0f;
		m_CurrentMovement = math::Vec3( math::RandomNumber( -10.0f, 10.0f ), math::RandomNumber( -10.0f, 10.0f ), 0.0f );
		m_Alpha = math::RandomNumber( 64, 64 );
	}

	switch( m_CurrentState )
	{
		case FrontendState_Main:
		{
			if( m_FrontendElements.CheckElementForSingleTouch(LABEL_ABOUT) )
			{
				SetupState( FrontendState_About );
			}

			if( core::app::IsGameCenterAvailable() && 
				m_FrontendElements.CheckElementForTouch(LABEL_ACHIEVEMENTS_GLYPH) )
			{
				/*if( core::app::IsNetworkAvailable() )
				{		
					if( support::Scores::IsInitialised() &&
					   support::Scores::GetInstance()->IsLoggedIn())
						support::Scores::GetInstance()->SyncAchievements();
					
					
				}*/
                
                support::Scores::GetInstance()->ShowAchievements();
			}
			else
			if( m_FrontendElements.CheckElementForSingleTouch(LABEL_MOREGAMES) )
			{
				core::app::OpenWebLink("https://github.com/GuybrushThreepwood-GitHub/HiddenGamesCode"/*res::GetScriptString(10000)*/);
			}
			else
			if( m_FrontendElements.CheckElementForSingleTouch(LABEL_RATE) )
			{
#ifndef BASE_PLATFORM_ANDROID
				if( core::app::GetAppRateURL() != 0 )
				{
					AudioSystem::GetInstance()->PlayUIAudio();

					core::app::OpenWebLink("https://github.com/GuybrushThreepwood-GitHub/HiddenGamesCode"/*core::app::GetAppRateURL()*/);
				}				
#endif // !BASE_PLATFORM_ANDROID

			}
			else
			if( m_FrontendElements.CheckElementForSingleTouch(LABEL_OPTIONS) )
			{
				m_CurrentOptions = GameSystems::GetInstance()->GetOptions();
				
				m_InOptions = true;
				SetupState( FrontendState_Options );
			}

			if( m_FrontendElements.CheckElementForSingleTouch(LABEL_START) )
			{
				m_CurrentOptions = GameSystems::GetInstance()->GetOptions();
				m_CurrentGeneralData = GameSystems::GetInstance()->GetGeneralData();
				m_CurrentBestRank = GameSystems::GetInstance()->GetBestRankData();

				SetupState( FrontendState_GameSelect );
			}

		}break;
		case FrontendState_GameSelect:
		{
			// continue game
			if( m_CurrentGeneralData.currentActiveGame == 1 )
			{
				if( m_FrontendElements.CheckElementForSingleTouch(LABEL_NEWGAME) )
				{
					SetupState( FrontendState_NewGame );
				}
				else if( m_FrontendElements.CheckElementForSingleTouch(LABEL_CONTINUE) )
				{
					m_LoadTransitionState = LoadState_TransitionOut;
					m_NewGame = false;
					m_ContinueGame = true;
					return;
				}
			}
			else
			{
				// new game
				if( m_FrontendElements.CheckElementForSingleTouch(LABEL_NEWGAME_NOSAVE) )
				{
					GameSystems::GameSaveData& saveData = GameSystems::GetInstance()->GetSaveData();	

					// clear old save
					for( i=0; i < saveData.dataBlock.itemCount; ++i )
						saveData.dataBlock.pItems[i].value = 0;

					saveData.timePlayed = 0.0f;
					saveData.numKills = 0;
					saveData.weaponIndex = -1;
					saveData.clothingIndex = 0;
					saveData.playerHealth = 100;
					saveData.pistolAmmo = 12;
					saveData.pistolAmmoTotal = 99;
					saveData.shotgunAmmo = 6;
					saveData.shotgunAmmoTotal = 99;
					saveData.inventoryTotalItems = 0;

					if( saveData.pInventoryList != 0 )
					{
						delete[] saveData.pInventoryList;
						saveData.pInventoryList = 0;
					}

					std::memset( &saveData.lastStageEntry, 0, sizeof(char)*MAX_VARIABLE_NAME );
					snprintf( saveData.lastStageEntry, MAX_VARIABLE_NAME, "stages/s01/setup.lua" );
					saveData.lastStageEntryDescriptionId = 1;
					saveData.lastStagePosRotId = 1000;

					GameSystems::GetInstance()->SetSaveData( saveData );

					if( m_DevData.enableSave )
						GameSystems::GetInstance()->SaveGameData();

					m_LoadTransitionState = LoadState_TransitionOut;
					m_NewGame = true;
					m_ContinueGame = false;
					m_CurrentGeneralData.currentActiveGame = 1;
					return;
				}
			}

			// rank
			if( m_CurrentBestRank.rankIndex != -1 )
			{
				if( m_FrontendElements.CheckElementForSingleTouch(LABEL_PASTRANKING) )
				{
					SetupState( FrontendState_Rank );
				}
			}

			// back
			if( m_FrontendElements.CheckElementForSingleTouch(BTN_BACK) )
			{
				SetupState( FrontendState_Main );
			}

		}break;
		case FrontendState_NewGame:
		{

			if( m_FrontendElements.CheckElementForSingleTouch(LABEL_YES) )
			{
				GameSystems::GameSaveData& saveData = GameSystems::GetInstance()->GetSaveData();	

				// clear old save
				for( i=0; i < saveData.dataBlock.itemCount; ++i )
					saveData.dataBlock.pItems[i].value = 0;

				saveData.timePlayed = 0.0f;
				saveData.numKills = 0;
				saveData.weaponIndex = -1;
				saveData.clothingIndex = 0;
				saveData.playerHealth = 100;
				saveData.pistolAmmo = 12;
				saveData.pistolAmmoTotal = 99;
				saveData.shotgunAmmo = 6;
				saveData.shotgunAmmoTotal = 99;
				saveData.inventoryTotalItems = 0;

				if( saveData.pInventoryList != 0 )
				{
					delete[] saveData.pInventoryList;
					saveData.pInventoryList = 0;
				}

				std::memset( &saveData.lastStageEntry, 0, sizeof(char)*MAX_VARIABLE_NAME );
				snprintf( saveData.lastStageEntry, MAX_VARIABLE_NAME, "stages/s01/setup.lua" );
				saveData.lastStageEntryDescriptionId = 1;
				saveData.lastStagePosRotId = 1000;

				GameSystems::GetInstance()->SetSaveData( saveData );

				if( m_DevData.enableSave )
					GameSystems::GetInstance()->SaveGameData();

				m_LoadTransitionState = LoadState_TransitionOut;
					
				m_NewGame = true;
				m_ContinueGame = false;
				return;
			}

			if( m_FrontendElements.CheckElementForSingleTouch(LABEL_NO) )
			{
				SetupState( FrontendState_GameSelect );
			}
		}break;
		case FrontendState_Rank:
		{
			if( m_FrontendElements.CheckElementForSingleTouch(BTN_BACK) )
			{
				SetupState( FrontendState_GameSelect );
			}
		}break;
		case FrontendState_Options:
		{
			// handedness
			if( m_OptionsElements.CheckElementForSingleTouch(BTN_LEFTHANDED) )
			{
				m_CurrentOptions.leftHanded = true;
				SetupState( FrontendState_Options );
			}
			else if( m_OptionsElements.CheckElementForSingleTouch(BTN_RIGHTHANDED) )
			{
				m_CurrentOptions.leftHanded = false;
				SetupState( FrontendState_Options );
			}

			// control type
			if( m_OptionsElements.CheckElementForSingleTouch(BTN_CONTROLS_A) )
			{
				m_CurrentOptions.controlType = 1;
				SetupState( FrontendState_Options );
			}

			// back to main menu
			if( m_OptionsElements.CheckElementForSingleTouch(OPTIONSFRONT_HUI_BTN_BACK) )
			{
				// give options back to gamesystems
				GameSystems::GetInstance()->SetOptions( m_CurrentOptions );

				// save the options
				GameSystems::GetInstance()->SaveOptions();

				m_InOptions = false;
				SetupState( FrontendState_Main );
			}

		}break;
		case FrontendState_About:
		{
			if( m_FrontendElements.CheckElementForSingleTouch(CREDITS_AREA) )
			{
				m_CreditsList = !m_CreditsList;
				SetupState( FrontendState_About );
			}

			if( m_FrontendElements.CheckElementForSingleTouch(BTN_BACK) )
			{
				SetupState( FrontendState_Main );
			}

		}break;

		default:
			DBG_ASSERT( 0 );
		break;
	}

}

/////////////////////////////////////////////////////
/// Method: Draw
/// Params: None
///
/////////////////////////////////////////////////////
void FrontendState::Draw()
{
	renderer::OpenGL::GetInstance()->ClearColour( 0.0f, 0.0f, 0.0f, 1.0f );

	renderer::OpenGL::GetInstance()->DisableLighting();
	
	renderer::OpenGL::GetInstance()->SetColour4ub( 255,255,255,255 );

	renderer::OpenGL::GetInstance()->SetNearFarClip( -10.0f, 10.0f );
	renderer::OpenGL::GetInstance()->SetupOrthographicView( core::app::GetOrientationWidth(), core::app::GetOrientationHeight(), true );
	
	if( m_InOptions )
	{
		renderer::OpenGL::GetInstance()->DepthMode( true, GL_ALWAYS );
		renderer::OpenGL::GetInstance()->BlendMode( true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

		m_OptionsMesh->Draw();

		m_OptionsElements.Draw();
	}
	else
	{
		renderer::OpenGL::GetInstance()->DepthMode( true, GL_ALWAYS );
		renderer::OpenGL::GetInstance()->BlendMode( true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

		if( m_CurrentState == FrontendState_Rank )
		{
			m_RankMesh->Draw();

			m_RankElements.Draw();
		}

		m_FrontendMesh->Draw();

		m_FrontendElements.Draw();

		// draw shifted ghost text
		if( m_CurrentState != FrontendState_About &&
			m_CurrentState != FrontendState_Rank )
		{
			m_FrontendMesh->EnableDefaultMeshColour();
			renderer::OpenGL::GetInstance()->DepthMode( true, GL_LEQUAL );
			glPushMatrix();
				math::Vec4Lite defaultColour( 255,255,255, m_Alpha );
				m_FrontendMesh->SetDefaultMeshColour( defaultColour );
				
				glTranslatef( m_CurrentMovement.X, m_CurrentMovement.Y, m_CurrentMovement.Z );
				m_FrontendMesh->Draw();

				renderer::OpenGL::GetInstance()->SetColour4ub( 255,255,255, m_Alpha );
				m_FrontendElements.Draw();
			glPopMatrix();
			m_FrontendMesh->DisableDefaultMeshColour();
		}


	}

	renderer::OpenGL::GetInstance()->DisableLighting();
		
	renderer::OpenGL::GetInstance()->DepthMode( true, GL_ALWAYS );
	renderer::OpenGL::GetInstance()->BlendMode( true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	
	// draw any load state
	switch( m_LoadTransitionState )
	{
		case LoadState_None:
		break;
		case LoadState_TransitionOut:
		{
			// fade out then load
			DrawFullscreenQuad( 0.0f, m_TransitionAlpha );
		}break;

		default:
			DBG_ASSERT(0);
		break;
	}
}

/////////////////////////////////////////////////////
/// Method: SetupState
/// Params: [in]state
///
/////////////////////////////////////////////////////
void FrontendState::SetupState( FrontendStates state )
{

	switch( state )
	{
		case FrontendState_Main:
		{
			m_FrontendMesh->SetMeshDrawState(-1, false);
			m_FrontendElements.ChangeElementDrawState(false);

			m_FrontendElements.ChangeElementDrawState(BTN_BACK, false);
			m_FrontendElements.ChangeElementDrawState(LABEL_PRISONER84_TITLE, true);
			m_FrontendElements.ChangeElementDrawState(LABEL_START, true);
			m_FrontendElements.ChangeElementDrawState(LABEL_ABOUT, true);
			m_FrontendElements.ChangeElementDrawState(LABEL_OPTIONS, true);
			m_FrontendElements.ChangeElementDrawState(LABEL_CONTINUE, false);
			m_FrontendElements.ChangeElementDrawState(LABEL_NEWGAME, false);
			m_FrontendElements.ChangeElementDrawState(LABEL_NEWGAME_NOSAVE, false);
			m_FrontendElements.ChangeElementDrawState(LABEL_PASTRANKING, false);
			m_FrontendElements.ChangeElementDrawState(LABEL_YES, false);
			m_FrontendElements.ChangeElementDrawState(LABEL_NO, false);
			m_FrontendElements.ChangeElementDrawState(LABEL_PROMPT, false);
			m_FrontendElements.ChangeElementDrawState(LABEL_BACK, false);
			m_FrontendElements.ChangeElementDrawState(LABEL_TITLE, false);
			m_FrontendElements.ChangeElementDrawState(LABEL_HEAD1, false);
			m_FrontendElements.ChangeElementDrawState(LABEL_NAME1, false);
			m_FrontendElements.ChangeElementDrawState(LABEL_HEAD2, false);
			m_FrontendElements.ChangeElementDrawState(LABEL_NAME2, false);
			m_FrontendElements.ChangeElementDrawState(LABEL_COPYRIGHT, false);
			m_FrontendElements.ChangeElementDrawState(LABEL_ACHIEVEMENTS_GLYPH, true);

			/*if( !core::app::IsGameCenterAvailable() ||
			   !core::app::IsNetworkAvailable() )
			{
				m_FrontendElements.ChangeElementDrawState( LABEL_ACHIEVEMENTS_GLYPH, false );
			}*/

			m_FrontendElements.ChangeElementDrawState( LABEL_MOREGAMES, true );

#ifndef BASE_PLATFORM_ANDROID
			m_FrontendElements.ChangeElementDrawState( LABEL_RATE, true );
#endif // !BASE_PLATFORM_ANDROID

			m_FrontendMesh->SetMeshDrawState(BG, false);
			m_FrontendMesh->SetMeshDrawState(BLOOD, true);
			m_FrontendMesh->SetMeshDrawState(TEXT_LOSTDATA, false);
			m_FrontendMesh->SetMeshDrawState(LOGO, false);
			m_FrontendMesh->SetMeshDrawState(BTN_EXIT, false);
			m_FrontendMesh->SetMeshDrawState(TITLE_BG, false);
			m_FrontendMesh->SetMeshDrawState(DIM_BOTTOM, false);
			m_FrontendMesh->SetMeshDrawState(DIM_TOP, false);

		}break;
		case FrontendState_GameSelect:
		{
			m_FrontendMesh->SetMeshDrawState(-1, false);
			m_FrontendElements.ChangeElementDrawState(false);

			m_FrontendElements.ChangeElementDrawState(BTN_BACK, true);
			m_FrontendElements.ChangeElementDrawState(LABEL_BACK, true);

			m_FrontendMesh->SetMeshDrawState(BG, false);
			m_FrontendMesh->SetMeshDrawState(BLOOD, true);
			m_FrontendMesh->SetMeshDrawState(BTN_EXIT, true);
			m_FrontendMesh->SetMeshDrawState(DIM_BOTTOM, true);
			m_FrontendMesh->SetMeshDrawState(DIM_TOP, true);

			if( m_CurrentGeneralData.currentActiveGame )
			{
				m_FrontendElements.ChangeElementDrawState(LABEL_CONTINUE, true);
				m_FrontendElements.ChangeElementDrawState(LABEL_NEWGAME, true);
			}
			else
			{
				m_FrontendElements.ChangeElementDrawState(LABEL_NEWGAME_NOSAVE, true);
			}
			
			if( m_CurrentBestRank.rankIndex != -1 )
				m_FrontendElements.ChangeElementDrawState(LABEL_PASTRANKING, true);
			else
				m_FrontendElements.ChangeElementDrawState(LABEL_PASTRANKING, false);
			
		}break;
		case FrontendState_NewGame:
		{
			m_FrontendMesh->SetMeshDrawState(-1, false);
			m_FrontendElements.ChangeElementDrawState(false);

			m_FrontendElements.ChangeElementDrawState(LABEL_YES, true);
			m_FrontendElements.ChangeElementDrawState(LABEL_NO, true);
			m_FrontendElements.ChangeElementDrawState(LABEL_PROMPT, true);

			m_FrontendMesh->SetMeshDrawState(BLOOD, true);
			m_FrontendMesh->SetMeshDrawState(TEXT_LOSTDATA, true);
			m_FrontendMesh->SetMeshDrawState(LOGO, false);

		}break;
		case FrontendState_Rank:
		{
			m_FrontendMesh->SetMeshDrawState(-1, false);
			m_FrontendElements.ChangeElementDrawState(false);

			m_FrontendMesh->SetMeshDrawState( BTN_EXIT, true );
			m_FrontendElements.ChangeElementDrawState( LABEL_BACK, true );

			// draw all meshes
			m_RankMesh->SetMeshDrawState(-1, true);
			m_RankElements.ChangeElementDrawState( false );

			m_RankElements.ChangeElementDrawState( GAMEOVER_LABEL_TIME, true );
			m_RankElements.ChangeElementDrawState( GAMEOVER_TEXT_TIME, true );
			m_RankElements.ChangeElementDrawState( GAMEOVER_LABEL_BODYCOUNT, true );
			m_RankElements.ChangeElementDrawState( GAMEOVER_TEXT_KILLS, true );
			m_RankElements.ChangeElementDrawState( GAMEOVER_TEXT_DOCUMENTS, true );
			m_RankElements.ChangeElementDrawState( GAMEOVER_TEXT_TOKENS, true );
			m_RankElements.ChangeElementDrawState( GAMEOVER_TEXT_HEADS, true );
			m_RankElements.ChangeElementDrawState( GAMEOVER_LABEL_RANK_LETTER, true );
			m_RankElements.ChangeElementDrawState( GAMEOVER_LABEL_RANKING, true );

			char text[UI_MAXSTATICTEXTBUFFER_SIZE];

			int hourAsInt = static_cast<int>((m_CurrentBestRank.timePlayed / 60.0f) / 60.0f);
			int minAsInt = static_cast<int>(m_CurrentBestRank.timePlayed / 60.0f) % 60;

			if( hourAsInt > 99 )
				hourAsInt = 99;

			if( hourAsInt < 10 )
			{
				if( minAsInt < 10 )
					snprintf( text, UI_MAXSTATICTEXTBUFFER_SIZE, "0%dh:0%dm", hourAsInt, minAsInt );
				else
					snprintf( text, UI_MAXSTATICTEXTBUFFER_SIZE, "0%dh:%dm", hourAsInt, minAsInt );
			}
			else
			{
				if( minAsInt < 10 )
					snprintf( text, UI_MAXSTATICTEXTBUFFER_SIZE, "%dh:0%dm", hourAsInt, minAsInt );
				else
					snprintf( text, UI_MAXSTATICTEXTBUFFER_SIZE, "%dh:%dm", hourAsInt, minAsInt );
			}

			m_RankElements.ChangeElementText( GAMEOVER_TEXT_TIME, text );

			if( m_CurrentBestRank.numKills > 999999 )
				m_CurrentBestRank.numKills = 999999;

			snprintf( text, UI_MAXSTATICTEXTBUFFER_SIZE, "%d", m_CurrentBestRank.numKills );
			m_RankElements.ChangeElementText( GAMEOVER_TEXT_KILLS, text );

			snprintf( text, UI_MAXSTATICTEXTBUFFER_SIZE, "%d / 5", m_CurrentBestRank.numDocs );
			m_RankElements.ChangeElementText( GAMEOVER_TEXT_DOCUMENTS, text );

			snprintf( text, UI_MAXSTATICTEXTBUFFER_SIZE, "%d / 5", m_CurrentBestRank.numTokens );
			m_RankElements.ChangeElementText( GAMEOVER_TEXT_TOKENS, text );

			snprintf( text, UI_MAXSTATICTEXTBUFFER_SIZE, "%d / 5", m_CurrentBestRank.numHeads );
			m_RankElements.ChangeElementText( GAMEOVER_TEXT_HEADS, text );

			m_RankElements.ChangeElementDrawState( GAMEOVER_LABEL_RANK_LETTER, true );

			switch(m_CurrentBestRank.rankIndex)
			{
					case 0:		
						m_RankElements.ChangeElementText( GAMEOVER_LABEL_RANK_LETTER, "S" );
						//m_RankMesh->SetMeshDrawState(GAMEOVER_SMESH, true);
					break;
					case 1:
						m_RankElements.ChangeElementText( GAMEOVER_LABEL_RANK_LETTER, "A" );
						//m_RankMesh->SetMeshDrawState(GAMEOVER_AMESH, true);
					break;
					case 2:
						m_RankElements.ChangeElementText( GAMEOVER_LABEL_RANK_LETTER, "B" );
						//m_RankMesh->SetMeshDrawState(GAMEOVER_BMESH, true);
					break;
					case 3:
						m_RankElements.ChangeElementText( GAMEOVER_LABEL_RANK_LETTER, "C" );
						//m_RankMesh->SetMeshDrawState(GAMEOVER_CMESH, true);
					break;
					default:
						DBG_ASSERT(0);
					break;
			}
		};
		case FrontendState_Options:
		{
			if( m_CurrentOptions.leftHanded )
			{
				m_OptionsMesh->SetMeshDrawState( BTN_LEFTHANDED_INACTIVE, false );
				m_OptionsMesh->SetMeshDrawState( BTN_LEFTHANDED_ACTIVE, true );

				m_OptionsMesh->SetMeshDrawState( BTN_RIGHTHANDED_INACTIVE, true );
				m_OptionsMesh->SetMeshDrawState( BTN_RIGHTHANDED_ACTIVE, false );
			}
			else
			{
				m_OptionsMesh->SetMeshDrawState( BTN_LEFTHANDED_INACTIVE, true );
				m_OptionsMesh->SetMeshDrawState( BTN_LEFTHANDED_ACTIVE, false );

				m_OptionsMesh->SetMeshDrawState( BTN_RIGHTHANDED_INACTIVE, false );
				m_OptionsMesh->SetMeshDrawState( BTN_RIGHTHANDED_ACTIVE, true );
			}

			// control type
			if( m_CurrentOptions.leftHanded )
			{
				m_OptionsMesh->SetMeshDrawState( BTN_CONTROLS_TYPEA_OFF_RH, false );
				m_OptionsMesh->SetMeshDrawState( BTN_CONTROLS_TYPEA_ON_RH, false );

				if( m_CurrentOptions.controlType == 1 )
				{
					m_OptionsMesh->SetMeshDrawState( BTN_CONTROLS_TYPEA_OFF_LH, false );
					m_OptionsMesh->SetMeshDrawState( BTN_CONTROLS_TYPEA_ON_LH, true );
				}
			}
			else
			{
				m_OptionsMesh->SetMeshDrawState( BTN_CONTROLS_TYPEA_OFF_LH, false );
				m_OptionsMesh->SetMeshDrawState( BTN_CONTROLS_TYPEA_ON_LH, false );


				if( m_CurrentOptions.controlType == 1 )
				{
					m_OptionsMesh->SetMeshDrawState( BTN_CONTROLS_TYPEA_OFF_RH, false );
					m_OptionsMesh->SetMeshDrawState( BTN_CONTROLS_TYPEA_ON_RH, true );

				}
			}

		}break;
		case FrontendState_About:
		{
			m_FrontendMesh->SetMeshDrawState(-1, false);
			m_FrontendElements.ChangeElementDrawState(false);

			m_FrontendElements.ChangeElementText(LABEL_TITLE, "%s", res::GetScriptString( 5011 ) );



			m_FrontendElements.ChangeElementDrawState(BTN_BACK, true);

			m_FrontendElements.ChangeElementDrawState(LOGO_POS, true);
			m_FrontendElements.ChangeElementText(LOGO_POS, "%d", 3 );

			m_FrontendElements.ChangeElementDrawState(LABEL_BACK, true);
			m_FrontendElements.ChangeElementDrawState(LABEL_TITLE, true);

			if( m_CreditsList )
			{
				m_FrontendElements.ChangeElementDrawState(LABEL_TESTERS_TITLE, true);
				m_FrontendElements.ChangeElementDrawState(LABEL_TESTER1, true);
				m_FrontendElements.ChangeElementDrawState(LABEL_TESTER2, true);
				m_FrontendElements.ChangeElementDrawState(LABEL_TESTER3, true);
				m_FrontendElements.ChangeElementDrawState(LABEL_TESTER4, true);
				m_FrontendElements.ChangeElementDrawState(LABEL_TESTER5, true);
				m_FrontendElements.ChangeElementDrawState(LABEL_TESTER6, true);
				m_FrontendElements.ChangeElementDrawState(LABEL_TESTER7, true);
				m_FrontendElements.ChangeElementDrawState(LABEL_TESTER8, true);
				m_FrontendElements.ChangeElementDrawState(LABEL_TESTER9, true);
				m_FrontendElements.ChangeElementDrawState(LABEL_TESTER10, true);
			}
			else
			{
				m_FrontendElements.ChangeElementDrawState(LABEL_HEAD1, true);
				m_FrontendElements.ChangeElementDrawState(LABEL_NAME1, true);
				m_FrontendElements.ChangeElementDrawState(LABEL_HEAD2, true);
				m_FrontendElements.ChangeElementDrawState(LABEL_NAME2, true);
				
			}

			m_FrontendElements.ChangeElementDrawState(LABEL_COPYRIGHT, true);

			m_FrontendMesh->SetMeshDrawState(BG, true);
			m_FrontendMesh->SetMeshDrawState(BLOOD, true);
			m_FrontendMesh->SetMeshDrawState(TEXT_LOSTDATA, false);
			m_FrontendMesh->SetMeshDrawState(LOGO, false);
			m_FrontendMesh->SetMeshDrawState(BTN_EXIT, true);
			m_FrontendMesh->SetMeshDrawState(TITLE_BG, true);
			m_FrontendMesh->SetMeshDrawState(DIM_BOTTOM, true);
			m_FrontendMesh->SetMeshDrawState(DIM_TOP, true);
		}break;

		default:
			DBG_ASSERT( 0 );
		break;
	}

	m_CurrentState = state;
}
