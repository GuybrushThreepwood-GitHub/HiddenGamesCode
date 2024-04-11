
/*===================================================================
	File: IntroOutroState.cpp
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
#include "Resources/SoundResources.h"
#include "Resources/StringResources.h"
#include "GameEffects/FullscreenEffects.h"

#include "GameStates/IBaseGameState.h"

#include "GameStates/MainState.h"

#include "GameStates/IntroOutroState.h"
#include "GameStates/FrontendState.h"
#include "GameStates/LoadingState.h"

// generated files
#include "GameStates/UI/GeneratedFiles/hiddendocs.hgm.h" // HGM submesh list
#include "GameStates/UI/GeneratedFiles/hiddendocs.hui.h" // UI element list

#include "GameStates/UI/GeneratedFiles/gameover.hgm.h"
#include "GameStates/UI/GeneratedFiles/gameover.hui.h"

#include "GameStates/UI/GeneratedFiles/ending.h"

namespace
{
	math::Vec3 zeroVec( 0.0f, 0.0f, 0.0f );

	const int LOAD_FADE = 15;
	const int MAX_TOUCH_TESTS = 1;

	const int INTRO_PAGES = 2;
	const int ENDING_PAGES = 6;
}

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
IntroOutroState::IntroOutroState( IntroOutro state, StateManager& stateManager, InputSystem& inputSystem )
: IBaseGameState( stateManager, inputSystem )
{
	m_LastDelta = 0.0f;

	m_CurrentMode = state;

	m_LoadTransitionState = LoadState_TransitionIn;
	m_TransitionAlpha = 255;

	m_EndingMesh = 0;
	m_RankMesh = 0;

	m_IntroToLoad = false;
	m_QuitToFrontend = false;
	m_GoToDocs = false;
	m_ShowOutroRank = false;
	m_ShowOutroDocs = false;

	m_CurrentRank = 0; // S rank
	m_PageFlip = snd::INVALID_SOUNDBUFFER;
	m_BreakDownAudio = snd::INVALID_SOUNDBUFFER;

	m_ThunderTimer = 0.0f;
	m_ThunderPlayTimer = math::RandomNumber( 5.0f, 15.0f );
	m_ThunderAudio = snd::INVALID_SOUNDBUFFER;
	m_WindAudio = snd::INVALID_SOUNDBUFFER;
}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
IntroOutroState::~IntroOutroState()
{

}

/////////////////////////////////////////////////////
/// Method: Enter
/// Params: None
///
/////////////////////////////////////////////////////
void IntroOutroState::Enter()
{
	int i=0;
	math::Vec4Lite defaultColour( 255,255,255,255 );

	math::Vec3 scaleFactor = math::Vec3( core::app::GetWidthScale(), core::app::GetHeightScale(), 1.0f );

	// outro
	m_EndingMesh = res::LoadModel(1009);
	for( i=ENDING_BGMESH; i <= ENDING_BGMESH; ++i )
	{		
		m_EndingMesh->SetMeshScale(i, scaleFactor);
	}

	m_RankMesh = res::LoadModel(1008);
	math::Vec2 srcAssetDims( static_cast<float>(core::app::GetBaseAssetsWidth()), static_cast<float>(core::app::GetBaseAssetsHeight()) );
	m_RankElements.Load( "hui/gameover.hui", srcAssetDims, m_RankMesh );

	if( m_CurrentMode == IntroOutroState_Outro )
	{
		SetupRank();
		SetupGeneralData();
		
		m_ShowOutroRank = true;
		m_RankBreakDownTimer = 0.0f;
		m_BreakDownIndex = 0;
	}
	else
	{
		AudioSystem::GetInstance()->SetBGMTrackVolume( 0.1f );
	}

	// docs for intro/outro
	m_DocsModels = 0;

	m_CurrentPage = 1;
	m_NumPages = 0;

	m_DocsModels = res::LoadModel( 2009 );
	DBG_ASSERT( (m_DocsModels != 0) );

	m_DocsUI.Load( "hui/hiddendocs.hui", srcAssetDims, m_DocsModels );
	
	m_DocsModels->SetMeshDrawState( -1, false );
	m_DocsUI.ChangeElementDrawState( false );
	
	if( m_CurrentMode == IntroOutroState_Outro )
	{
		m_NumPages = ENDING_PAGES;

		m_DocsModels->SetMeshDrawState( HIDDENDOCS_CONFIDENTIAL, true );
	}
	else
	{
		m_NumPages = INTRO_PAGES;

		m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE11, true );
		m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE11, "%s", res::GetScriptString(6020) );

		m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE12, true );
		m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE12, "%s", res::GetScriptString(6021) );

		m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_NEXT, true );
	}
	
	//if( m_CurrentMode == IntroOutroState_Outro )
	//	m_DocsModels->SetMeshDrawState( HIDDENDOCS_ENDING_CLASSIFIED, false );
	//else
	//	m_DocsModels->SetMeshDrawState( HIDDENDOCS_ENDING_CLASSIFIED, false );

	// audio
	const char* sr = 0;
	snd::Sound sndLoad;

	sr = res::GetSoundResource( 190 );
	m_PageFlip = snd::SoundLoad( sr, sndLoad );
	DBG_ASSERT( (m_PageFlip != snd::INVALID_SOUNDBUFFER) );

	sr = res::GetSoundResource( 200 );
	m_BreakDownAudio = snd::SoundLoad( sr, sndLoad );
	DBG_ASSERT( (m_BreakDownAudio != snd::INVALID_SOUNDBUFFER) );

	sr = res::GetSoundResource( 201 );
	m_ThunderAudio = snd::SoundLoad( sr, sndLoad );
	DBG_ASSERT( (m_ThunderAudio != snd::INVALID_SOUNDBUFFER) );

	sr = res::GetSoundResource( 1 );
	m_WindAudio = snd::SoundLoad( sr, sndLoad );
	DBG_ASSERT( (m_WindAudio != snd::INVALID_SOUNDBUFFER) );

	if( m_CurrentMode == IntroOutroState_Intro )
	{
		AudioSystem::GetInstance()->PlayAudio( m_WindAudio, zeroVec, true, true, 1.0f, 0.1f );
		AudioSystem::GetInstance()->PlayAudio( m_ThunderAudio, zeroVec, true );
	}

	// disable screen dim
	core::app::SetDimScreenTimer( false );

}

/////////////////////////////////////////////////////
/// Method: Exit
/// Params: None
///
/////////////////////////////////////////////////////
void IntroOutroState::Exit()
{
	int i=0;

	snd::RemoveSound( m_PageFlip );
	snd::RemoveSound( m_BreakDownAudio );
	snd::RemoveSound( m_ThunderAudio );
	snd::RemoveSound( m_WindAudio );

	if( m_EndingMesh != 0 )
	{
		res::RemoveModel(m_EndingMesh);
		m_EndingMesh = 0;
	}

	if( m_DocsModels != 0 )
	{
		res::RemoveModel( m_DocsModels );
		m_DocsModels = 0;
	}

	if( m_RankMesh != 0 )
	{
		res::RemoveModel(m_RankMesh);
		m_RankMesh = 0;
	}

	if( m_CurrentMode == IntroOutroState_Outro )
	{
		// reset save
		GameSystems::GameSaveData& saveData = GameSystems::GetInstance()->GetSaveData();	

		// clear old save
		for( i=0; i < saveData.dataBlock.itemCount; ++i )
			saveData.dataBlock.pItems[i].value = 0;

		saveData.timePlayed = 0.0f;
		saveData.numKills = 0;
		saveData.weaponIndex = -1;
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

		GameSystems::GetInstance()->SaveGameData();

		// remove all data values
		GameSystems::GetInstance()->ClearDataValues();

		// complete
		GameSystems::GetInstance()->AwardAchievement(25);
	}

	// enable screen dim
	core::app::SetDimScreenTimer( true );

	if( support::Scores::IsInitialised() )
		support::Scores::GetInstance()->SyncAchievements();
}

/////////////////////////////////////////////////////
/// Method: TransitionIn
/// Params: None
///
/////////////////////////////////////////////////////
int IntroOutroState::TransitionIn()
{
	return(0);
}

/////////////////////////////////////////////////////
/// Method: TransitionOut
/// Params: None
///
/////////////////////////////////////////////////////
int IntroOutroState::TransitionOut()
{
	return(0);
}

/////////////////////////////////////////////////////
/// Method: Update
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
void IntroOutroState::Update( float deltaTime )
{
	m_LastDelta = deltaTime;

	// do any transitions
	if( UpdateLoadState() )
	{
		// returning true means the state has changed, this is not a valid class anymore
		return;
	}

	if(m_CurrentMode == IntroOutroState_Intro)
		UpdateIntroState( deltaTime );
	else 
		UpdateOutroState( deltaTime );
}

/////////////////////////////////////////////////////
/// Method: DrawIntroState
/// Params: None
///
/////////////////////////////////////////////////////
void IntroOutroState::DrawIntroState()
{
	renderer::OpenGL::GetInstance()->ClearColour( 0.0f, 0.0f, 0.0f, 1.0f );

	renderer::OpenGL::GetInstance()->SetNearFarClip( -500.0f, 100.0f );
	renderer::OpenGL::GetInstance()->SetupOrthographicView( core::app::GetOrientationWidth(), core::app::GetOrientationHeight(), true );
	renderer::OpenGL::GetInstance()->SetColour4ub( 255,255,255,255 );

	renderer::OpenGL::GetInstance()->DepthMode( true, GL_ALWAYS );
	renderer::OpenGL::GetInstance()->BlendMode( true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	renderer::OpenGL::GetInstance()->AlphaMode( false, GL_ALWAYS, 0.0f );

	m_DocsModels->Draw();

	renderer::OpenGL::GetInstance()->DepthMode( true, GL_ALWAYS );
	renderer::OpenGL::GetInstance()->BlendMode( true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	renderer::OpenGL::GetInstance()->SetNearFarClip( -100.0f, 100.0f );
	renderer::OpenGL::GetInstance()->SetupOrthographicView( core::app::GetOrientationWidth(), core::app::GetOrientationHeight(), true );

	m_DocsUI.Draw();
	
}

/////////////////////////////////////////////////////
/// Method: DrawOutroState
/// Params: None
///
/////////////////////////////////////////////////////
void IntroOutroState::DrawOutroState()
{
	
	if(m_ShowOutroRank)
	{
		renderer::OpenGL::GetInstance()->SetNearFarClip( -100.0f, 100.0f );
		renderer::OpenGL::GetInstance()->SetupOrthographicView( core::app::GetOrientationWidth(), core::app::GetOrientationHeight(), true );

		m_EndingMesh->Draw();
		m_RankMesh->Draw();

		renderer::OpenGL::GetInstance()->SetNearFarClip( -100.0f, 100.0f );
		renderer::OpenGL::GetInstance()->SetupOrthographicView( core::app::GetOrientationWidth(), core::app::GetOrientationHeight(), true );

		renderer::OpenGL::GetInstance()->SetColour4ub( 255,255,255,255 );
		m_RankElements.Draw();
	}
	else if(m_ShowOutroDocs)
	{
		renderer::OpenGL::GetInstance()->ClearColour( 0.0f, 0.0f, 0.0f, 1.0f );

		renderer::OpenGL::GetInstance()->SetNearFarClip( -500.0f, 100.0f );
		renderer::OpenGL::GetInstance()->SetupOrthographicView( core::app::GetOrientationWidth(), core::app::GetOrientationHeight(), true );
		
		renderer::OpenGL::GetInstance()->DepthMode( true, GL_ALWAYS );
		renderer::OpenGL::GetInstance()->BlendMode( true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
		renderer::OpenGL::GetInstance()->AlphaMode( false, GL_ALWAYS, 0.0f );

		m_DocsModels->Draw();

		renderer::OpenGL::GetInstance()->DepthMode( true, GL_ALWAYS );
		renderer::OpenGL::GetInstance()->BlendMode( true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

		renderer::OpenGL::GetInstance()->SetNearFarClip( -100.0f, 100.0f );
		renderer::OpenGL::GetInstance()->SetupOrthographicView( core::app::GetOrientationWidth(), core::app::GetOrientationHeight(), true );

		m_DocsUI.Draw();
	}
}

/////////////////////////////////////////////////////
/// Method: UpdateIntroState
/// Params: None
///
/////////////////////////////////////////////////////
void IntroOutroState::UpdateIntroState( float deltaTime )
{
	// update UI elements
	m_DocsUI.Update( TOUCH_SIZE*0.5f, MAX_TOUCH_TESTS, deltaTime );

	m_ThunderTimer += deltaTime;

	if( m_ThunderTimer >= m_ThunderPlayTimer )
	{
		AudioSystem::GetInstance()->PlayAudio( m_ThunderAudio, zeroVec, true );

		m_ThunderPlayTimer = math::RandomNumber( 5.0f, 15.0f );
		m_ThunderTimer = 0.0f;
	}

	if( m_CurrentPage >= m_NumPages )
	{
		if( ScreenTap() )
		{
			m_IntroToLoad = true;
			m_LoadTransitionState = LoadState_TransitionOut;
			return;
		}
	}

	if( m_CurrentPage < m_NumPages )
	{
		if( m_DocsUI.CheckElementForSingleTouch(HIDDENDOCS_UI_BTN_NEXT) )
		{
			m_CurrentPage++;
			if( m_CurrentPage == INTRO_PAGES )
			{
				m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE11, "%s", res::GetScriptString(6022) );
				m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE12, "%s", res::GetScriptString(6023) );

				m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_NEXT, false );
			}

			AudioSystem::GetInstance()->PlayAudio( m_PageFlip, zeroVec, true );
		}
	}
}

/////////////////////////////////////////////////////
/// Method: UpdateOutroState
/// Params: None
///
/////////////////////////////////////////////////////
void IntroOutroState::UpdateOutroState( float deltaTime )
{
	if(m_ShowOutroRank)
	{
		if( m_BreakDownIndex != 4 )
			m_RankBreakDownTimer += deltaTime;

		if( m_RankBreakDownTimer >= 1.0f )
		{
			if( m_BreakDownIndex == 0 )
			{
				// show the time 
				m_RankMesh->SetMeshDrawState(GAMEOVER_PANEL1, true);
				m_RankElements.ChangeElementDrawState( GAMEOVER_TEXT_TIME, true );
				m_RankElements.ChangeElementDrawState( GAMEOVER_LABEL_TIME, true );
	
				AudioSystem::GetInstance()->PlayAudio( m_BreakDownAudio, zeroVec, true );

				m_BreakDownIndex++;
				m_RankBreakDownTimer = 0.0f;
			}
			else if( m_BreakDownIndex == 1 )
			{
				// show the body count
				m_RankMesh->SetMeshDrawState(GAMEOVER_PANEL2, true);
				m_RankElements.ChangeElementDrawState( GAMEOVER_TEXT_KILLS, true );
				m_RankElements.ChangeElementDrawState( GAMEOVER_LABEL_BODYCOUNT, true );

				AudioSystem::GetInstance()->PlayAudio( m_BreakDownAudio, zeroVec, true );

				m_BreakDownIndex++;
				m_RankBreakDownTimer = 0.0f;

			}
			else if( m_BreakDownIndex == 2 )
			{
				// show the collections
				m_RankMesh->SetMeshDrawState(GAMEOVER_PANEL3, true);
				m_RankMesh->SetMeshDrawState(GAMEOVER_DOCUMENTS, true);
				m_RankMesh->SetMeshDrawState(GAMEOVER_TOKENS, true);
				m_RankMesh->SetMeshDrawState(GAMEOVER_HEADS, true);

				m_RankElements.ChangeElementDrawState( GAMEOVER_TEXT_DOCUMENTS, true );
				m_RankElements.ChangeElementDrawState( GAMEOVER_TEXT_TOKENS, true );
				m_RankElements.ChangeElementDrawState( GAMEOVER_TEXT_HEADS, true );

				AudioSystem::GetInstance()->PlayAudio( m_BreakDownAudio, zeroVec, true );

				m_BreakDownIndex++;
				m_RankBreakDownTimer = 0.0f;
			}
			else if( m_BreakDownIndex == 3 )
			{
				m_RankMesh->SetMeshDrawState(GAMEOVER_BLOOD, true);
				m_RankElements.ChangeElementDrawState( GAMEOVER_LABEL_RANK_LETTER, true );
				m_RankElements.ChangeElementDrawState( GAMEOVER_LABEL_RANKING, true );

				// show the rank
				switch(m_CurrentRank)
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

				AudioSystem::GetInstance()->PlayAudio( m_BreakDownAudio, zeroVec, true );

				m_BreakDownIndex++;
				m_RankBreakDownTimer = 2.0f;
			}
		}


		if( m_BreakDownIndex == 4 )
		{
			if( ScreenTap() )
			{
				GameSystems::GetInstance()->SetEndGameState(false);

				m_LoadTransitionState = LoadState_TransitionOut;
				m_GoToDocs = true;
			}
		}
	}
	else if( m_ShowOutroDocs )
	{
		math::Vec3 zeroVec( 0.0f, 0.0f, 0.0f );

		// update UI elements
		m_DocsUI.Update( TOUCH_SIZE*0.5f, MAX_TOUCH_TESTS, deltaTime );

		if( m_CurrentPage >= m_NumPages )
		{
			if( ScreenTap() )
			{
				m_QuitToFrontend = true;
				m_LoadTransitionState = LoadState_TransitionOut;
				return;
			}
		}

		if( m_CurrentPage < m_NumPages )
		{
			if( ScreenTap() )
			{
				m_CurrentPage++;
				switch( m_CurrentPage )
				{
					case 1:
					{
						m_DocsUI.ChangeElementDrawState( false );

						m_DocsModels->SetMeshDrawState( HIDDENDOCS_CONFIDENTIAL, true );
					}break;
					case 2:
					{
						int textOffset = 0;
						if( !GameSystems::GetInstance()->GetDataValue("criminal_docs_1") )
							textOffset = 1000;

						m_DocsModels->SetMeshDrawState( HIDDENDOCS_CONFIDENTIAL, false );

						m_DocsUI.ChangeElementDrawState( false );

						m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE4, true );
						m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE4, "%s", res::GetScriptString(6160+textOffset) );

						m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE5, true );
						m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE5, "%s", res::GetScriptString(6161+textOffset) );
					}break;
					case 3:
					{
						int textOffset = 0;
						if( !GameSystems::GetInstance()->GetDataValue("criminal_docs_2") )
							textOffset = 1000;

						m_DocsUI.ChangeElementDrawState( false );

						m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE4, true );
						m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE4, "%s", res::GetScriptString(6162+textOffset) );

						m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE5, true );
						m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE5, "%s", res::GetScriptString(6163+textOffset) );

						m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE6, true );
						m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE6, "%s", res::GetScriptString(6164+textOffset) );

						m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE7, true );
						m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE7, "%s", res::GetScriptString(6165+textOffset) );

					}break;
					case 4:
					{
						int textOffset = 0;
						if( !GameSystems::GetInstance()->GetDataValue("criminal_docs_3") )
							textOffset = 1000;

						m_DocsUI.ChangeElementDrawState( false );

						m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE4, true );
						m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE4, "%s", res::GetScriptString(6166+textOffset) );

						m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE5, true );
						m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE5, "%s", res::GetScriptString(6167+textOffset) );

						m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE6, true );
						m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE6, "%s", res::GetScriptString(6168+textOffset) );
					}break;
					case 5:
					{
						int textOffset = 0;
						if( !GameSystems::GetInstance()->GetDataValue("criminal_docs_4") )
							textOffset = 1000;

						m_DocsUI.ChangeElementDrawState( false );

						m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE4, true );
						m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE4, "%s", res::GetScriptString(6169+textOffset) );

						m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE5, true );
						m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE5, "%s", res::GetScriptString(6170+textOffset) );

						m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE6, true );
						m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE6, "%s", res::GetScriptString(6171+textOffset) );
					}break;
					case 6:
					{
						int textOffset = 0;
						if( !GameSystems::GetInstance()->GetDataValue("criminal_docs_5") )
							textOffset = 1000;

						m_DocsUI.ChangeElementDrawState( false );

						m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE3, true );
						m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE3, "%s", res::GetScriptString(6172+textOffset) );

						m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE4, true );
						m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE4, "%s", res::GetScriptString(6173+textOffset) );

						m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE5, true );
						m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE5, "%s", res::GetScriptString(6174+textOffset) );

						m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE6, true );
						m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE6, "%s", res::GetScriptString(6175+textOffset) );

						m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE8, true );
						m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE8, "%s", res::GetScriptString(6176+textOffset) );
					}break;
				}

				AudioSystem::GetInstance()->PlayAudio( m_PageFlip, zeroVec, true );
			}
		}
	}
}

/////////////////////////////////////////////////////
/// Method: Draw
/// Params: None
///
/////////////////////////////////////////////////////
void IntroOutroState::Draw()
{
	renderer::OpenGL::GetInstance()->ClearColour( 0.0f, 0.0f, 0.0f, 1.0f );

	renderer::OpenGL::GetInstance()->DisableLighting();

	renderer::OpenGL::GetInstance()->SetNearFarClip( -100.0f, 100.0f );
	renderer::OpenGL::GetInstance()->SetupOrthographicView( core::app::GetOrientationWidth(), core::app::GetOrientationHeight(), true );
	
	renderer::OpenGL::GetInstance()->SetColour4ub( 255,255,255,255 );
	renderer::OpenGL::GetInstance()->DepthMode( true, GL_ALWAYS );
	renderer::OpenGL::GetInstance()->BlendMode( true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	if(m_CurrentMode == IntroOutroState_Intro)
		DrawIntroState();
	else 
		DrawOutroState();

	renderer::OpenGL::GetInstance()->DepthMode( true, GL_ALWAYS );
	renderer::OpenGL::GetInstance()->BlendMode( true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	// draw any load state
	switch( m_LoadTransitionState )
	{
		case LoadState_None:
		break;
		case LoadState_TransitionIn:
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
/// Method: UpdateLoadState
/// Params: None
///
/////////////////////////////////////////////////////
bool IntroOutroState::UpdateLoadState()
{
	switch( m_LoadTransitionState )
	{
		case LoadState_None:

		break;
		case LoadState_TransitionIn:
		{
			m_TransitionAlpha -= LOAD_FADE;
			
			core::app::SetSmoothUpdate(false);
			
			if( m_TransitionAlpha < 0 )
			{
				m_TransitionAlpha = 0;
				core::app::SetSmoothUpdate(true);

				m_LoadTransitionState = LoadState_None;
			}
			return(false);
		}break;
		case LoadState_TransitionOut:
		{
			m_TransitionAlpha += LOAD_FADE;
			
			core::app::SetSmoothUpdate(false);
			
			if( m_TransitionAlpha > 255 )
			{
				m_TransitionAlpha = 255;
				core::app::SetSmoothUpdate(true);

				m_LoadTransitionState = LoadState_None;

				if( m_GoToDocs )
				{
					m_GoToDocs = false;
					m_ShowOutroRank = false;
					m_ShowOutroDocs = true;

					m_LoadTransitionState = LoadState_TransitionIn;
				}

				if( m_QuitToFrontend )
				{
					ChangeState( new FrontendState(*m_pStateManager, m_InputSystem) );
					return(true);
				}
				else if( m_IntroToLoad )
				{
					ChangeState( new LoadingState(*m_pStateManager, m_InputSystem) );
					return(true);
				}
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
/// Method: SetupRank
/// Params: None
///
/////////////////////////////////////////////////////
void IntroOutroState::SetupRank()
{
	GameSystems::GetInstance()->LoadBestRank();
	GameSystems::BestRankData& rankData = GameSystems::GetInstance()->GetBestRankData();

	GameSystems::GameSaveData& gameSave = GameSystems::GetInstance()->GetSaveData();

	float hourTime = ((gameSave.timePlayed / 60.0f) / 60.0f);
	if( hourTime > 2.0f ) // over 2 hours
		m_CurrentRank++; 

	int docCount = 0;
	// criminal docs
	docCount += GameSystems::GetInstance()->GetDataValue( "criminal_docs_1" );
	docCount += GameSystems::GetInstance()->GetDataValue( "criminal_docs_2" );
	docCount += GameSystems::GetInstance()->GetDataValue( "criminal_docs_3" );
	docCount += GameSystems::GetInstance()->GetDataValue( "criminal_docs_4" );
	docCount += GameSystems::GetInstance()->GetDataValue( "criminal_docs_5" );

	if( docCount < 5 )
		m_CurrentRank++;

	int tokenCount = 0;
	// tokens
	tokenCount += GameSystems::GetInstance()->GetDataValue( "hidden_token_1" );
	tokenCount += GameSystems::GetInstance()->GetDataValue( "hidden_token_2" );
	tokenCount += GameSystems::GetInstance()->GetDataValue( "hidden_token_3" );
	tokenCount += GameSystems::GetInstance()->GetDataValue( "hidden_token_4" );
	tokenCount += GameSystems::GetInstance()->GetDataValue( "hidden_token_5" );

	if( tokenCount < 5 )
		m_CurrentRank++;

	int headCount = 0;
	// head hunter count
	headCount += GameSystems::GetInstance()->GetDataValue( "head_hunter_1" );
	headCount += GameSystems::GetInstance()->GetDataValue( "head_hunter_2" );
	headCount += GameSystems::GetInstance()->GetDataValue( "head_hunter_3" );
	headCount += GameSystems::GetInstance()->GetDataValue( "head_hunter_4" );
	headCount += GameSystems::GetInstance()->GetDataValue( "head_hunter_5" );

	if( headCount < 5 )
		m_CurrentRank++;

	if( rankData.rankIndex == -1 )
	{
		// first rank
		rankData.rankIndex = m_CurrentRank;
		
		rankData.timePlayed = gameSave.timePlayed;
		rankData.numKills = gameSave.numKills;
		
		rankData.numDocs = docCount;
		rankData.numTokens = tokenCount;
		rankData.numHeads = headCount;
	}
	else
	{
		if( m_CurrentRank < rankData.rankIndex )
		{
			// better rank
			rankData.rankIndex = m_CurrentRank;
			
			rankData.timePlayed = gameSave.timePlayed;
			rankData.numKills = gameSave.numKills;
			
			rankData.numDocs = docCount;
			rankData.numTokens = tokenCount;
			rankData.numHeads = headCount;
		}
		else if( m_CurrentRank == rankData.rankIndex )
		{
			// same rank

			// only save if it's a better time
			if( gameSave.timePlayed < rankData.timePlayed )
			{
				rankData.timePlayed = gameSave.timePlayed;
				rankData.numKills = gameSave.numKills;
			}
		}
		else
		{
			// nothing to do
		}
	}

	char text[UI_MAXSTATICTEXTBUFFER_SIZE];

	int hourAsInt = static_cast<int>((gameSave.timePlayed / 60.0f) / 60.0f);
	int minAsInt = static_cast<int>(gameSave.timePlayed / 60.0f) % 60;

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
	m_RankElements.ChangeElementDrawState( GAMEOVER_TEXT_TIME, false );

	m_RankElements.ChangeElementDrawState( GAMEOVER_LABEL_TIME, false );
	m_RankElements.ChangeElementDrawState( GAMEOVER_LABEL_BODYCOUNT, false );

	if( gameSave.numKills > 999999 )
		gameSave.numKills = 999999;

	snprintf( text, UI_MAXSTATICTEXTBUFFER_SIZE, "%d", gameSave.numKills );
	m_RankElements.ChangeElementText( GAMEOVER_TEXT_KILLS, text );
	m_RankElements.ChangeElementDrawState( GAMEOVER_TEXT_KILLS, false );

	snprintf( text, UI_MAXSTATICTEXTBUFFER_SIZE, "%d / 5", docCount );
	m_RankElements.ChangeElementText( GAMEOVER_TEXT_DOCUMENTS, text );
	m_RankElements.ChangeElementDrawState( GAMEOVER_TEXT_DOCUMENTS, false );

	snprintf( text, UI_MAXSTATICTEXTBUFFER_SIZE, "%d / 5", tokenCount );
	m_RankElements.ChangeElementText( GAMEOVER_TEXT_TOKENS, text );
	m_RankElements.ChangeElementDrawState( GAMEOVER_TEXT_TOKENS, false );

	snprintf( text, UI_MAXSTATICTEXTBUFFER_SIZE, "%d / 5", headCount );
	m_RankElements.ChangeElementText( GAMEOVER_TEXT_HEADS, text );
	m_RankElements.ChangeElementDrawState( GAMEOVER_TEXT_HEADS, false );

	GameSystems::GetInstance()->SetBestRankData(rankData);
	GameSystems::GetInstance()->SaveBestRank();

	// setup the mesh
	m_RankMesh->SetMeshDrawState(-1, false);

	m_RankMesh->SetMeshDrawState(GAMEOVER_BG, true);

	m_RankElements.ChangeElementDrawState( false );
	m_RankElements.ChangeElementDrawState( GAMEOVER_LABEL_CONGRATULATIONS, true );

	if( m_CurrentRank > 3 )
		m_CurrentRank = 3;

	// S rank
	if( rankData.rankIndex == 0 )
	{
		GameSystems::GetInstance()->AwardAchievement(21);
	}

	if( hourAsInt == 0 )
	{
		if( minAsInt <= 30 )
		{
			GameSystems::GetInstance()->AwardAchievement(24);
		}
	}

	if( gameSave.numKills >= 100 )
	{
		GameSystems::GetInstance()->AwardAchievement(22);
	}
	else if( gameSave.numKills <= 10 ) 
	{
		GameSystems::GetInstance()->AwardAchievement(23);
	}

	if( support::Scores::IsInitialised() )
		support::Scores::GetInstance()->SyncAchievements();
}

/////////////////////////////////////////////////////
/// Method: SetupGeneralData
/// Params: None
///
/////////////////////////////////////////////////////
void IntroOutroState::SetupGeneralData()
{
	GameSystems::GetInstance()->LoadGeneralData();

	GameSystems::GeneralData& generalData = GameSystems::GetInstance()->GetGeneralData();

	generalData.numCompletes++;
	generalData.currentActiveGame = 0;
	generalData.fpsModeUnlocked = 1;

	GameSystems::GetInstance()->SetGeneralData(generalData);

	GameSystems::GetInstance()->SaveGeneralData();
}

/////////////////////////////////////////////////////
/// Method: ScreenTap
/// Params: None
///
/////////////////////////////////////////////////////
bool IntroOutroState::ScreenTap()
{
	if( input::gInputState.TouchesData[input::FIRST_TOUCH].bRelease )
	{
		return(true);
	}

	return(false);
}