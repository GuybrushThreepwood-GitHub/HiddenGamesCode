
/*===================================================================
	File: HudUI.cpp
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

#include "Profiles/ProfileManager.h"

#include "Resources/StringResources.h"

#include "GameStates/UI/UIIds.h"
#include "GameStates/UI/UIBaseState.h"
#include "GameStates/UI/HudUI.h"
#include "GameStates/UI/UIFileLoader.h"
#include "GameStates/UI/TextFormattingFuncs.h"

#include "GameStates/UI/GeneratedFiles/hud.hgm.h"
#include "GameStates/UI/GeneratedFiles/hud.hui.h"

namespace
{

}

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
HudUI::HudUI( StateManager& stateManager, InputSystem& inputSystem )
: IBaseGameState( stateManager, inputSystem, UI_HUD )
{
	m_Player = PhysicsWorld::GetPlayer();

	m_UIMesh = 0;

	m_FinishedTransitionIn = true;
	m_FinishedTransitionOut = true;

	m_TowTruckFinished = false;
	m_TowTruckHitMiddle = false;
}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
HudUI::~HudUI()
{

}

/////////////////////////////////////////////////////
/// Method: Enter
/// Params: None
///
/////////////////////////////////////////////////////
void HudUI::Enter()
{
	math::Vec2 srcDims( static_cast<float>( core::app::GetBaseAssetsWidth() ), static_cast<float>( core::app::GetBaseAssetsHeight() ) );
	m_UIMesh = res::LoadModel(1010);
	DBG_ASSERT( m_UIMesh != 0 );

	m_Elements.Load( "assets/ui/hud.hui", srcDims, m_UIMesh );

	m_LevelData = GetScriptDataHolder()->GetLevelData();
	m_GameData = GetScriptDataHolder()->GetGameData();

	m_UIMesh->SetMeshDrawState( ICON_CARGO_CUSTOMER, false );
	m_UIMesh->SetMeshDrawState( ICON_CARGO_PARCEL, false );
	m_UIMesh->SetMeshDrawState( ICON_CARGO_SELF, false );

	m_UIMesh->SetMeshDrawState( BG_DESTINATION, false );

	m_UIMesh->SetMeshDrawState( ICON_DESTINATION_PORT, false );
	m_UIMesh->SetMeshDrawState( ICON_DESTINATION_HOME, false );

	m_UIMesh->SetMeshDrawState( GFX_TOWTRUCK, false );

	// lost/earned pop ups
	m_UIMesh->SetMeshDrawState( BG_LOST, false );
	m_UIMesh->SetMeshDrawState( BG_EARNED, false );
	m_UIMesh->SetMeshDrawState( BG_1UP, false );
	m_Elements.ChangeElementDrawState( HUI_LABEL_1UP, false );

	if( ProfileManager::GetInstance()->GetGameMode() == ProfileManager::CAREER_MODE )
		m_Elements.ChangeElementText( HUI_TEXT_CUSTOMER_COUNTER, "%d/%d", 0, m_LevelData.levelCustomerTotal );
	else
		m_Elements.ChangeElementText( HUI_TEXT_CUSTOMER_COUNTER, "%d", 0/*, m_LevelData.levelArcadeCustomers*/ );

}

/////////////////////////////////////////////////////
/// Method: Exit
/// Params: None
///
/////////////////////////////////////////////////////
void HudUI::Exit()
{
	if( m_UIMesh != 0 )
	{
		res::RemoveModel( m_UIMesh );
		m_UIMesh = 0;
	}
}

/////////////////////////////////////////////////////
/// Method: PrepareTransitionIn
/// Params: None
///
/////////////////////////////////////////////////////
void HudUI::PrepareTransitionIn()
{
	m_FinishedTransitionIn = false;
	m_FadeTransition = 255;
}

/////////////////////////////////////////////////////
/// Method: PrepareTransitionOut
/// Params: None
///
/////////////////////////////////////////////////////
void HudUI::PrepareTransitionOut()
{
	m_FinishedTransitionOut = false;
	m_FadeTransition = 0;
}

/////////////////////////////////////////////////////
/// Method: TransitionIn
/// Params: None
///
/////////////////////////////////////////////////////
int HudUI::TransitionIn()
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

		renderer::OpenGL::GetInstance()->BlendMode( false, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

		return(1);
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
int HudUI::TransitionOut()
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
void HudUI::Update( float deltaTime )
{
	m_LastDeltaTime = deltaTime;

	m_Elements.Update( TOUCH_SIZE_MENU, 2, deltaTime );

	m_Player = PhysicsWorld::GetPlayer();
	if( m_Player == 0 || m_Player->IsDead() )
		return;

	if( m_Player->HasPassenger() )
	{
		m_UIMesh->SetMeshDrawState( ICON_CARGO_CUSTOMER, true );
		m_UIMesh->SetMeshDrawState( ICON_CARGO_PARCEL, false );
		m_UIMesh->SetMeshDrawState( ICON_CARGO_SELF, false );

		m_UIMesh->SetMeshDrawState( BG_DESTINATION, true );

		m_UIMesh->SetMeshDrawState( ICON_DESTINATION_PORT, true );
		m_UIMesh->SetMeshDrawState( ICON_DESTINATION_HOME, false );

		m_Elements.ChangeElementDrawState( HUI_TEXT_PORTNUMBER, true );
		m_Elements.ChangeElementText( HUI_TEXT_PORTNUMBER, "%d", m_Player->GetPassengerDestination() );
	}
	else if( m_Player->HasCargo() )
	{
		m_UIMesh->SetMeshDrawState( ICON_CARGO_CUSTOMER, false );
		m_UIMesh->SetMeshDrawState( ICON_CARGO_PARCEL, true );
		m_UIMesh->SetMeshDrawState( ICON_CARGO_SELF, false );

		m_UIMesh->SetMeshDrawState( BG_DESTINATION, true );

		m_UIMesh->SetMeshDrawState( ICON_DESTINATION_PORT, true );
		m_UIMesh->SetMeshDrawState( ICON_DESTINATION_HOME, false );

		m_Elements.ChangeElementDrawState( HUI_TEXT_PORTNUMBER, true );
		m_Elements.ChangeElementText( HUI_TEXT_PORTNUMBER, "%d", m_Player->GetCargoDestination() );
	}
	else if( ProfileManager::GetInstance()->GetGameMode() == ProfileManager::CAREER_MODE &&
		m_Player->GetCustomersComplete() >= m_Player->GetLevelCustomerTotal() )
	{
		m_UIMesh->SetMeshDrawState( ICON_CARGO_CUSTOMER, false );
		m_UIMesh->SetMeshDrawState( ICON_CARGO_PARCEL, false );
		m_UIMesh->SetMeshDrawState( ICON_CARGO_SELF, true );

		m_UIMesh->SetMeshDrawState( BG_DESTINATION, true );

		m_UIMesh->SetMeshDrawState( ICON_DESTINATION_PORT, false );
		m_UIMesh->SetMeshDrawState( ICON_DESTINATION_HOME, true );

		m_Elements.ChangeElementDrawState( HUI_TEXT_PORTNUMBER, false );
	}
	else
	{
		m_UIMesh->SetMeshDrawState( ICON_CARGO_CUSTOMER, false );
		m_UIMesh->SetMeshDrawState( ICON_CARGO_PARCEL, false );
		m_UIMesh->SetMeshDrawState( ICON_CARGO_SELF, false );

		m_UIMesh->SetMeshDrawState( BG_DESTINATION, false );

		m_UIMesh->SetMeshDrawState( ICON_DESTINATION_PORT, false );
		m_UIMesh->SetMeshDrawState( ICON_DESTINATION_HOME, false );

		m_Elements.ChangeElementDrawState( HUI_TEXT_PORTNUMBER, false );
	}

	if( ProfileManager::GetInstance()->GetGameMode() == ProfileManager::CAREER_MODE )
		m_Elements.ChangeElementText( HUI_TEXT_CUSTOMER_COUNTER, "%d/%d", m_Player->GetCustomersComplete(), m_LevelData.levelCustomerTotal );
	else
		m_Elements.ChangeElementText( HUI_TEXT_CUSTOMER_COUNTER, "%d", m_Player->GetCustomersComplete()/*, m_LevelData.levelArcadeCustomers*/ );

	if( m_Player->ShowCustomerReward() )
	{
		if( m_Player->CustomerReward() < 0 )
		{
			m_UIMesh->SetMeshDrawState( BG_LOST, true );
			m_UIMesh->SetMeshDrawState( BG_EARNED, false );
			
			m_Elements.ChangeElementText( HUI_TEXT_INCOME_OUTCOME, "%d", m_Player->CustomerReward() );
		}
		else
		{
			m_UIMesh->SetMeshDrawState( BG_LOST, false );
			m_UIMesh->SetMeshDrawState( BG_EARNED, true );

			m_Elements.ChangeElementText( HUI_TEXT_INCOME_OUTCOME, "+%d", m_Player->CustomerReward() );
		}
		
		
		m_Elements.ChangeElementDrawState( HUI_TEXT_INCOME_OUTCOME, true );
	}
	else
	{
			m_UIMesh->SetMeshDrawState( BG_LOST, false );
			m_UIMesh->SetMeshDrawState( BG_EARNED, false );

			m_Elements.ChangeElementDrawState( HUI_TEXT_INCOME_OUTCOME, false );
	}

	if( m_Player->ShowExtraLifeReward() )
	{
		m_UIMesh->SetMeshDrawState( BG_1UP, true );
		m_Elements.ChangeElementDrawState( HUI_LABEL_1UP, true );
	}
	else
	{
		m_UIMesh->SetMeshDrawState( BG_1UP, false );
		m_Elements.ChangeElementDrawState( HUI_LABEL_1UP, false );
	}

	SetLivesString( m_Elements, HUI_TEXT_LIVES, m_Player->GetLivesCount() );
	SetCashStringNoSymbol( m_Elements, HUI_TEXT_BANKBALANCE, m_Player->GetTotalMoney() );

	if( m_Player->IsRefueling() )
	{
		m_UIMesh->SetMeshDrawState( ICON_FUEL_REFUELLING, true );
		m_UIMesh->SetMeshDrawState( ICON_FUEL_NORMAL, false );
	}
	else
	{
		m_UIMesh->SetMeshDrawState( ICON_FUEL_REFUELLING, false );
		m_UIMesh->SetMeshDrawState( ICON_FUEL_NORMAL, true );
	}
}

/////////////////////////////////////////////////////
/// Method: Draw
/// Params: None
///
/////////////////////////////////////////////////////
void HudUI::Draw()
{
	m_Player = PhysicsWorld::GetPlayer();
	if( m_Player == 0 || m_Player->IsDead() )
		return;

	math::Vec3 pos;
	math::Vec2 fuelGuageDims( m_GameData.FUEL_BAR_WIDTH, m_GameData.FUEL_BAR_HEIGHT );

	renderer::OpenGL::GetInstance()->SetNearFarClip( NEAR_CLIP_ORTHO, FAR_CLIP_ORTHO );
	renderer::OpenGL::GetInstance()->SetupOrthographicView( core::app::GetOrientationWidth(), core::app::GetOrientationHeight(), true );
	renderer::OpenGL::GetInstance()->DisableLighting();

	renderer::OpenGL::GetInstance()->SetColour4ub( 255, 255, 255, 255 );

	renderer::OpenGL::GetInstance()->DepthMode( false, GL_ALWAYS );
	renderer::OpenGL::GetInstance()->BlendMode( true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	if( m_UIMesh != 0 )
		m_UIMesh->Draw();

	pos = m_Elements.GetElementPosition( HUI_GFX_FUELLGAUGE_START );
	DrawFuelGauge( pos, fuelGuageDims, static_cast<float>( m_Player->GetFuelCount() ), static_cast<float>( m_GameData.LOW_FUEL ), static_cast<float>( m_GameData.MEDIUM_FUEL ), m_Player->IsRefueling() );

	m_Elements.Draw();

	renderer::OpenGL::GetInstance()->DepthMode( true, GL_LESS );
	renderer::OpenGL::GetInstance()->BlendMode( false, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
}

/////////////////////////////////////////////////////
/// Method: InitTowTruck
/// Params: None
///
/////////////////////////////////////////////////////
void HudUI::InitTowTruck()
{
	m_TowTruckFinished = false;
	m_TowTruckHitMiddle = false;

	m_UIMesh->SetMeshDrawState( GFX_TOWTRUCK, true );

	if( renderer::OpenGL::GetInstance()->GetIsRotated() )
	{
		m_TowTruckPos.X = static_cast<float>( core::app::GetOrientationHeight() ) + 20.0f;
		m_TowTruckPos.Y = static_cast<float>( core::app::GetOrientationWidth() ) * 0.5f;
	}
	else
	{
		m_TowTruckPos.X = static_cast<float>( core::app::GetOrientationWidth() ) + 20.0f;
		m_TowTruckPos.Y = static_cast<float>( core::app::GetOrientationHeight() ) * 0.5f;
	}

	m_TowTruckPos.Z = 0.0f;

	m_TowEndX = -50.0f;
}

/////////////////////////////////////////////////////
/// Method: UpdateTowTruck
/// Params: None
///
/////////////////////////////////////////////////////
void HudUI::UpdateTowTruck( float deltaTime )
{
	if( !m_TowTruckFinished )
	{
		m_TowTruckPos.X -= m_GameData.TOWTRUCK_FLYBY_SPEED*deltaTime;

		if( m_TowTruckPos.X <= m_TowEndX )
		{
			m_UIMesh->SetMeshDrawState( GFX_TOWTRUCK, false );
			m_TowTruckFinished = true;
		}
		
		if( !m_TowTruckHitMiddle )
		{
			float middle = 0.0f;
			if( renderer::OpenGL::GetInstance()->GetIsRotated() )
				middle = static_cast<float>( core::app::GetOrientationHeight() ) * 0.5f;
			else
				middle= static_cast<float>( core::app::GetOrientationWidth() ) * 0.5f;

			if( m_TowTruckPos.X < middle )
			{
				m_Player->AddMoney( -m_GameData.TOW_TRUCK_CHARGE, true );

				// play audio
				ALuint sourceId = snd::SoundManager::GetInstance()->GetFreeSoundSource();
				ALuint bufferId = AudioSystem::GetInstance()->FindSound( 105/*"money_big.wav"*/ );

				if( sourceId != snd::INVALID_SOUNDSOURCE &&
					bufferId != snd::INVALID_SOUNDBUFFER )
					AudioSystem::GetInstance()->PlayAudio( sourceId, bufferId, math::Vec3(0.0f,0.0f,0.0f), AL_TRUE, AL_FALSE );	

				m_TowTruckHitMiddle = true;

				// tow truck award
#ifdef CABBY_LITE
			// ignore
#else
				GameSystems::GetInstance()->AwardAchievement(1);
#endif // CABBY_LITE
					
			}
		}

		m_UIMesh->SetMeshTranslation( GFX_TOWTRUCK, m_TowTruckPos );

	}
}	

/////////////////////////////////////////////////////
/// Method: DrawTowTruck
/// Params: None
///
/////////////////////////////////////////////////////
void HudUI::DrawTowTruck()
{
	renderer::OpenGL::GetInstance()->DepthMode( false, GL_ALWAYS );
	m_UIMesh->DrawMeshId( GFX_TOWTRUCK );
	renderer::OpenGL::GetInstance()->DepthMode( true, GL_LESS );
}

/////////////////////////////////////////////////////
/// Method: SetGameTime
/// Params: [in]timer
///
/////////////////////////////////////////////////////
void HudUI::SetGameTime( float timer )
{
	char text[UI_MAXSTATICTEXTBUFFER_SIZE];

	int minAsInt = static_cast<int>(timer / 60.0f);
	int secsAsInt = static_cast<int>(timer) % 60;

	if( minAsInt > 99 )
		minAsInt = 99;

	if( minAsInt < 10 )
	{
		if( secsAsInt < 10 )
			snprintf( text, UI_MAXSTATICTEXTBUFFER_SIZE, "0%d:0%d", minAsInt, secsAsInt );
		else
			snprintf( text, UI_MAXSTATICTEXTBUFFER_SIZE, "0%d:%d", minAsInt, secsAsInt );
	}
	else
	{
		if( secsAsInt < 10 )
			snprintf( text, UI_MAXSTATICTEXTBUFFER_SIZE, "%d:0%d", minAsInt, secsAsInt );
		else
			snprintf( text, UI_MAXSTATICTEXTBUFFER_SIZE, "%d:%d", minAsInt, secsAsInt );
	}

	m_Elements.ChangeElementText( HUI_TEXT_TIMER, "%s", text );
}
