
/*===================================================================
	File: LevelCompleteCareerUI.cpp
	Game: Cabby

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

#include "CabbyConsts.h"
#include "Cabby.h"

#include "StateManage/IState.h"

#include "Resources/ModelResources.h"
#include "Resources/StringResources.h"

#include "GameEffects/FullscreenEffects.h"

#include "GameStates/UI/UIIds.h"
#include "GameStates/UI/UIBaseState.h"
#include "GameStates/UI/LevelCompleteCareerUI.h"
#include "GameStates/UI/UIFileLoader.h"
#include "GameStates/UI/FrontendUI.h"
#include "GameStates/UI/LevelStartUI.h"
#include "GameStates/UI/MainGameUI.h"
#include "GameStates/UI/GameCompleteUI.h"
#include "GameStates/MainGameState.h"
#include "GameStates/UnlockState.h"
#include "GameStates/TitleScreenState.h"
#include "GameStates/SpecialEndingState.h"
#include "GameStates/UI/TextFormattingFuncs.h"

#include "GameStates/UI/GeneratedFiles/level_complete_career.hgm.h"
#include "GameStates/UI/GeneratedFiles/level_complete_career.hui.h"

namespace
{
	const int CAR_UNLOCK_OFFSET = 5;
	const int NUM_CAREER_CAR_UNLOCKS = 5;
}

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
LevelCompleteCareerUI::LevelCompleteCareerUI( StateManager& stateManager, InputSystem& inputSystem )
: IBaseGameState( stateManager, inputSystem, UI_LEVELCOMPLETECAREER )
{
	m_UIMesh = 0;
}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
LevelCompleteCareerUI::~LevelCompleteCareerUI()
{

}

/////////////////////////////////////////////////////
/// Method: Enter
/// Params: None
///
/////////////////////////////////////////////////////
void LevelCompleteCareerUI::Enter()
{
	math::Vec2 srcDims( static_cast<float>( core::app::GetBaseAssetsWidth() ), static_cast<float>( core::app::GetBaseAssetsHeight() ) );
	m_UIMesh = res::LoadModel(1011);
	DBG_ASSERT( m_UIMesh != 0 );

	m_Elements.Load( "assets/ui/level_complete_career.hui", srcDims, m_UIMesh );

	m_QuitEnabled = true;

	m_pProfile = 0;
	m_pProfile = ProfileManager::GetInstance()->GetProfile();
	DBG_ASSERT( m_pProfile != 0 );

	ProfileManager::ActiveLevelData* pLevelData = ProfileManager::GetInstance()->GetActiveLevelData();
	DBG_ASSERT( pLevelData != 0 );

	m_GameMode = ProfileManager::GetInstance()->GetGameMode();
	m_GameData = GetScriptDataHolder()->GetGameData();

	m_Elements.ChangeElementText( HUI_TEXT_CUSTOMERS, "%d", pLevelData->customersTransported );
	m_Elements.ChangeElementText( HUI_TEXT_PARCELS, "%d", pLevelData->parcelsTransported );
	m_Elements.ChangeElementText( HUI_TEXT_PERFECT, "%d", pLevelData->perfectFares );
	SetCashString( m_Elements, HUI_TEXT_SHIFT_EARNINGS, pLevelData->moneyEarned );
	SetCashString( m_Elements, HUI_TEXT_TOTAL_EARNINGS, m_pProfile->currentMoney );

	m_UIMesh->SetMeshDrawState( ICON_STAR1, false );
	m_UIMesh->SetMeshDrawState( ICON_STAR2, false );

	//int currentLevel = m_pProfile->currentLevelIndex;
	//int packId = m_pProfile->careerLevels[currentLevel].packIndex;
	//int levelId = m_pProfile->careerLevels[currentLevel].levelIndex;

	m_Elements.ChangeElementDrawState( HUI_TEXT_TOTAL_EARNINGS, true );
	m_Elements.ChangeElementDrawState( HUI_LABEL_TOTAL_EARNINGS, true );
	m_Elements.ChangeElementText( HUI_LABEL_BUTTON_START, "%s", res::GetScriptString(1229) );
	m_UIMesh->SetMeshDrawState( ICON_MONEY_GREEN, true );

	/*if( m_WasNewRecord )
	{
		m_Elements.ChangeElementDrawState( HUI_TEXT_CONGRATS, true );
		m_UIMesh->SetMeshDrawState( ICON_STAR1, true );
	}
	else
	{
		m_Elements.ChangeElementDrawState( HUI_TEXT_CONGRATS, false );
		m_UIMesh->SetMeshDrawState( ICON_STAR1, false );
	}*/

	// has career ended
	if( m_pProfile->currentLevelIndex+1 >= LEVELS_IN_A_CAREER )
	{
		m_pProfile->newComplete = true;
		m_Elements.ChangeElementText( HUI_LABEL_BUTTON_START, "%s", res::GetScriptString(1230) );

		// don't show the quit button
		m_UIMesh->SetMeshDrawState( BTN_QUIT, false );
		m_Elements.ChangeElementDrawState( HUI_GLYPH_QUIT, false );
		m_QuitEnabled = false;
	}

	// sync achievements
	if( support::Scores::IsInitialised() )
		support::Scores::GetInstance()->SyncAchievements();

#ifdef CABBY_FREE
	core::app::SetAdBarState( true );
	#ifdef BASE_PLATFORM_WINDOWS
		core::app::SetAdBarState(  GetScriptDataHolder()->GetDevData().showPCAdBar );
	#endif // BASE_PLATFORM_WINDOWS
#endif // CABBY_FREE
	
#ifdef BASE_SUPPORT_KIIP
	if( m_pProfile->currentLevelIndex+1 >= LEVELS_IN_A_CAREER )
	{
		// no need for insurance award
		if( m_pProfile->currentLivesLost <= 0 )
			GameSystems::GetInstance()->AwardKiipAchievement(1);
		
		if( m_pProfile->totalMoneyEarned >= 500000 )
			GameSystems::GetInstance()->AwardKiipAchievement(2);
		
		// career complete
		GameSystems::GetInstance()->AwardKiipAchievement(3);
	
		// 5+ careers
		if( m_pProfile->numberOfCompletes+1 >= 5 )
			GameSystems::GetInstance()->AwardKiipAchievement(4);
	}
#endif // BASE_SUPPORT_KIIP

	m_Elements.RefreshSelected(HUI_BUTTON_START);
    
    core::app::SetHandleMenuButton(true);
}

/////////////////////////////////////////////////////
/// Method: Exit
/// Params: None
///
/////////////////////////////////////////////////////
void LevelCompleteCareerUI::Exit()
{
	if( m_UIMesh != 0 )
	{
		res::RemoveModel( m_UIMesh );
		m_UIMesh = 0;
	}

	// sync achievements
	if( support::Scores::IsInitialised() )
		support::Scores::GetInstance()->SyncAchievements();
}

/////////////////////////////////////////////////////
/// Method: PrepareTransitionIn
/// Params: None
///
/////////////////////////////////////////////////////
void LevelCompleteCareerUI::PrepareTransitionIn()
{
	m_FinishedTransitionIn = false;
	m_FadeTransition = 255;
}

/////////////////////////////////////////////////////
/// Method: PrepareTransitionOut
/// Params: None
///
/////////////////////////////////////////////////////
void LevelCompleteCareerUI::PrepareTransitionOut()
{
	m_FinishedTransitionOut = false;
	m_FadeTransition = 0;
}

/////////////////////////////////////////////////////
/// Method: TransitionIn
/// Params: None
///
/////////////////////////////////////////////////////
int LevelCompleteCareerUI::TransitionIn()
{
	if( !m_FinishedTransitionIn )
	{
		// draw normal
		Draw();

		renderer::OpenGL::GetInstance()->BlendMode( true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

		// draw fade
		m_FadeTransition -= 15;
		if( m_FadeTransition <= 0 )
		{
			m_FadeTransition = 0;
			m_FinishedTransitionIn = true;
		}
		DrawFullscreenQuad( 0.0f, m_FadeTransition );

		GameSystems::GetInstance()->DrawAchievementUI();

		renderer::OpenGL::GetInstance()->BlendMode( false, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

		return(1);
	}

	// play success
	ALuint sourceId = snd::INVALID_SOUNDSOURCE;
	ALuint bufferId = snd::INVALID_SOUNDBUFFER;
	sourceId = snd::SoundManager::GetInstance()->GetFreeSoundSource();
	bufferId = AudioSystem::GetInstance()->FindSound( 101/*"complete.wav"*/ );

	if( sourceId != snd::INVALID_SOUNDSOURCE &&
		bufferId != snd::INVALID_SOUNDBUFFER )
	{
		AudioSystem::GetInstance()->PlayAudio( sourceId, bufferId, math::Vec3(0.0f,0.0f,0.0f), AL_TRUE );
	}

	// transition finished, make sure to do a clean draw
	Draw();
	return(0);
}

/////////////////////////////////////////////////////
/// Method: TransitionOut
/// Params: None
///
/////////////////////////////////////////////////////
int LevelCompleteCareerUI::TransitionOut()
{
	if( !m_FinishedTransitionOut )
	{
		// draw normal
		Draw();

		renderer::OpenGL::GetInstance()->BlendMode( true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

		// draw fade
		m_FadeTransition += 15;
		if( m_FadeTransition >= 255 )
		{
			m_FadeTransition = 255;
			m_FinishedTransitionOut = true;
		}
		DrawFullscreenQuad( 0.0f, m_FadeTransition );

		GameSystems::GetInstance()->DrawAchievementUI();

		renderer::OpenGL::GetInstance()->BlendMode( false, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

		return(1);
	}
	return(0);
}

/////////////////////////////////////////////////////
/// Method: Update
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
void LevelCompleteCareerUI::Update( float deltaTime )
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

	m_Elements.Update( TOUCH_SIZE_MENU, 2, deltaTime );

	if( m_QuitEnabled &&
		(m_Elements.CheckElementForTouch( HUI_BUTTON_QUIT ) ||
       m_InputSystem.GetMenu()) )
	{
		AudioSystem::GetInstance()->PlayUIAudio();

		// increase the level
		m_pProfile->careerLevels[m_pProfile->currentLevelIndex].complete = true;
		m_pProfile->currentLevelIndex += 1;

		ProfileManager::GetInstance()->SaveProfile();

		m_pStateManager->ChangeSecondaryState( new TitleScreenState(*m_pStateManager->GetSecondaryStateManager(), m_InputSystem) );		
		ChangeState( new FrontendUI(*m_pStateManager, m_InputSystem, FrontendUI::FrontendStates_MainMenu) );
		return;
	}

	if( m_Elements.CheckElementForTouch( HUI_BUTTON_START ) )
	{
		bool goToEnd = false;
		AudioSystem::GetInstance()->PlayUIAudio();

		//bool* pUnlock = 0;

		// increase the level
		m_pProfile->careerLevels[m_pProfile->currentLevelIndex].complete = true;
		m_pProfile->currentLevelIndex += 1;

		// is the game over?
		if( m_pProfile->currentLevelIndex >= LEVELS_IN_A_CAREER )
		{
			// no need for insurance award
			if( m_pProfile->currentLivesLost <= 0 )
				GameSystems::GetInstance()->AwardAchievement(22);

			// 10+ perfect fares award
			if( m_pProfile->currentPerfectFares >= 10 )
				GameSystems::GetInstance()->AwardAchievement(23);

			// travis bickle award
			if( m_pProfile->currentCustomersLost == 4 )
				GameSystems::GetInstance()->AwardAchievement(24);

#if !defined(CABBY_FREE)
			// unlock the plonker mobile
			if( m_pProfile->currentMoney <= 15000 )
			{
				if( m_pProfile->Pack1vehicle1 != true )
				{
					ProfileManager::GetInstance()->AddVehicleToUnlocks( 10 );
					m_pProfile->Pack1vehicle1 = true;
				}
				GameSystems::GetInstance()->AwardAchievement(41);
			}
#endif // !defined(CABBY_FREE)

			m_pProfile->currentLevelIndex	= 0;
			m_pProfile->currentFuel			= 100.0f;
			m_pProfile->currentLives		= m_GameData.DEFAULT_LIVES_COUNT;
			m_pProfile->currentPerfectFares	= 0;
			m_pProfile->currentCustomersLost= 0;
			m_pProfile->currentLivesLost	= 0;
			m_pProfile->currentMoney		= 0;
			m_pProfile->extraLifeTotal		= 0;

			// set the flag to make sure, continue career allows vehicle select again
			m_pProfile->newComplete = true;
			m_pProfile->careerActive = false;

			m_pProfile->numberOfCompletes += 1;

			// workaholic achievement
			if( m_pProfile->numberOfCompletes >= 50 )
				GameSystems::GetInstance()->AwardAchievement(19);

			// money in the bank achievement
			if( m_pProfile->totalMoneyEarned >= 250000 )
				GameSystems::GetInstance()->AwardAchievement(25);

#if !defined(CABBY_FREE)
			bool* pVehicleUnlock = &m_pProfile->bondLotusUnlocked;
			if( m_pProfile->numberOfCompletes <= CAR_UNLOCK_OFFSET )
			{
				// unlock awards (full version only)
				
				// move the pointer
				pVehicleUnlock += (m_pProfile->numberOfCompletes-1);
				if( *pVehicleUnlock != true )
				{
					*pVehicleUnlock = true;

					// add the car
					ProfileManager::GetInstance()->AddVehicleToUnlocks( CAR_UNLOCK_OFFSET + (m_pProfile->numberOfCompletes-1) );
				}
				
				switch( m_pProfile->numberOfCompletes )
				{
					case 1:
					{
						// lond botus award
						GameSystems::GetInstance()->AwardAchievement(4);
					}break;
					case 2:
					{
						// team a van award
						GameSystems::GetInstance()->AwardAchievement(5);
					}break;
					case 3:
					{
						// Ghoul Hunter award
						GameSystems::GetInstance()->AwardAchievement(6);
					}break;
					case 4:
					{
						// spinner award
						GameSystems::GetInstance()->AwardAchievement(7);
					}break;
					case 5:
					{
						// ledorean award
						GameSystems::GetInstance()->AwardAchievement(8);
					}break;
					default:
					break;
				}
			}
			else // make sure all cars are unlocked and awarded when enough completes are done
			{
				int i=0;
				for( i=0; i < NUM_CAREER_CAR_UNLOCKS; ++i )
				{
					if( *pVehicleUnlock != true )
					{
						*pVehicleUnlock = true;

						// add the car
						ProfileManager::GetInstance()->AddVehicleToUnlocks( CAR_UNLOCK_OFFSET + i );

						// make sure it was achieved
						GameSystems::GetInstance()->AwardAchievement(4+i);
					}

					// next
					pVehicleUnlock++;
				}
			}

			// unlock the flying beetle
			if( m_pProfile->totalCustomersTransported >= 100 )
			{
				if( m_pProfile->Pack1vehicle2 != true )
				{
					ProfileManager::GetInstance()->AddVehicleToUnlocks( 11 );
					m_pProfile->Pack1vehicle2 = true;
				}
				GameSystems::GetInstance()->AwardAchievement(42);
			}

			// unlock the whodunit
			if( m_pProfile->totalCustomersLost >= 100 )
			{
				if( m_pProfile->Pack1vehicle3 != true )
				{
					ProfileManager::GetInstance()->AddVehicleToUnlocks( 12 );
					m_pProfile->Pack1vehicle3 = true;
				}
				GameSystems::GetInstance()->AwardAchievement(43);
			}

			// unlock the police box
			if( m_pProfile->totalMoneyEarned >= 231963 )
			{
				if( m_pProfile->Pack1vehicle4 != true )
				{
					ProfileManager::GetInstance()->AddVehicleToUnlocks( 13 );
					m_pProfile->Pack1vehicle4 = true;
				}

				GameSystems::GetInstance()->AwardAchievement(44);
			}

			// unlock the homie
			if( m_pProfile->totalMoneyEarned >= 82000 )
			{
				if( m_pProfile->Pack1vehicle5 != true )
				{
					ProfileManager::GetInstance()->AddVehicleToUnlocks( 14 );
					m_pProfile->Pack1vehicle5 = true;
				}
				GameSystems::GetInstance()->AwardAchievement(45);
			}
#endif // !defined(CABBY_FREE)


#ifdef BASE_PLATFORM_ANDROID
	#if defined(CABBY_FREE)
			bool* pVehicleUnlock = &m_pProfile->bondLotusUnlocked;
			if( m_pProfile->numberOfCompletes <= CAR_UNLOCK_OFFSET )
			{
				// unlock awards (free version)
				
				// move the pointer
				pVehicleUnlock += (m_pProfile->numberOfCompletes-1);
				if( *pVehicleUnlock != true )
				{
					*pVehicleUnlock = true;

					// add the car
					ProfileManager::GetInstance()->AddVehicleToUnlocks( CAR_UNLOCK_OFFSET + (m_pProfile->numberOfCompletes-1) );
				}
				
				switch( m_pProfile->numberOfCompletes )
				{
					case 1:
					{
						// lond botus award
						GameSystems::GetInstance()->AwardAchievement(4);
					}break;
					case 2:
					{
						// team a van award
						GameSystems::GetInstance()->AwardAchievement(5);
					}break;
					case 3:
					{
						// Ghoul Hunter award
						GameSystems::GetInstance()->AwardAchievement(6);
					}break;
					case 4:
					{
						// spinner award
						GameSystems::GetInstance()->AwardAchievement(7);
					}break;
					case 5:
					{
						// ledorean award
						GameSystems::GetInstance()->AwardAchievement(8);
					}break;
					default:
					break;
				}
			}
			else // make sure all cars are unlocked and awarded when enough completes are done
			{
				int i=0;
				for( i=0; i < NUM_CAREER_CAR_UNLOCKS; ++i )
				{
					if( *pVehicleUnlock != true )
					{
						*pVehicleUnlock = true;

						// add the car
						ProfileManager::GetInstance()->AddVehicleToUnlocks( CAR_UNLOCK_OFFSET + i );

						// make sure it was achieved
						GameSystems::GetInstance()->AwardAchievement(4+i);
					}

					// next
					pVehicleUnlock++;
				}
			}
	#endif // (CABBY_FREE)
#endif // BASE_PLATFORM_ANDROID

			// go to credits
			goToEnd = true;
		}

		ProfileManager::GetInstance()->SaveProfile();

		if( goToEnd )
		{
			if( ProfileManager::GetInstance()->GetVehicleUnlockState() )
				m_pStateManager->ChangeSecondaryState( new UnlockState(*m_pStateManager->GetSecondaryStateManager(), m_InputSystem) );		
			else
				m_pStateManager->ChangeSecondaryState( new SpecialEndingState(*m_pStateManager->GetSecondaryStateManager(), m_InputSystem) );	

			ChangeState( new GameCompleteUI(*m_pStateManager, m_InputSystem, ProfileManager::CAREER_MODE) );
			return;
		}
		else
		{
			int currentLevel = m_pProfile->currentLevelIndex;
			int packId = m_pProfile->careerLevels[currentLevel].packIndex;
			int levelId = m_pProfile->careerLevels[currentLevel].levelIndex;

			// next level
			script::LuaCallFunction( GetScriptDataHolder()->GetLevelPackList()[packId]->pPackLevelInfo[levelId].levelLoadFuncName, 0, 0 );	
			ChangeState( new LevelStartUI(*m_pStateManager, m_InputSystem) );
			return;
		}
	}
}

/////////////////////////////////////////////////////
/// Method: Draw
/// Params: None
///
/////////////////////////////////////////////////////
void LevelCompleteCareerUI::Draw()
{
	renderer::OpenGL::GetInstance()->SetNearFarClip( NEAR_CLIP_ORTHO, FAR_CLIP_ORTHO );
	renderer::OpenGL::GetInstance()->SetupOrthographicView( core::app::GetOrientationWidth(), core::app::GetOrientationHeight(), true );
	renderer::OpenGL::GetInstance()->DisableLighting();

	renderer::OpenGL::GetInstance()->SetColour4ub( 255, 255, 255, 255 );

	renderer::OpenGL::GetInstance()->DepthMode( false, GL_ALWAYS );
	renderer::OpenGL::GetInstance()->BlendMode( true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	
	if( m_UIMesh != 0 )
		m_UIMesh->Draw();
	
	m_Elements.Draw();
	m_Elements.DrawSelected();

	GameSystems::GetInstance()->DrawAchievementUI();
	
	renderer::OpenGL::GetInstance()->DepthMode( true, GL_LESS );
	renderer::OpenGL::GetInstance()->BlendMode( false, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
}
