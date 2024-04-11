
/*===================================================================
	File: FrontendAircraftSelectUI.cpp
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
#include "Resources/IAPList.h"

#include "GameEffects/FullscreenEffects.h"

#include "GameStates/UI/UIIds.h"
#include "GameStates/UI/UIBaseState.h"
#include "GameStates/UI/FrontendAircraftSelectUI.h"
#include "GameStates/UI/FrontendLevelSelectUI.h"
#include "GameStates/UI/LevelStartUI.h"
#include "GameStates/UI/FrontendMainMenuUI.h"
#include "GameStates/TitleScreenState.h"
#include "GameStates/UI/UIFileLoader.h"

#include "GameStates/UI/GeneratedFiles/frontend_aircraftselect.hgm.h"
#include "GameStates/UI/GeneratedFiles/frontend_aircraftselect.hui.h"

namespace
{
}

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
FrontendAircraftSelectUI::FrontendAircraftSelectUI( StateManager& stateManager, InputSystem& inputSystem )
: IBaseGameState( stateManager, inputSystem, UI_FRONTENDAIRCRAFTSELECT )
{
	m_UIMesh = 0;
	m_pScriptData = 0;

	m_VehiclePackList.clear();
	m_ChangedPack = false;

	m_VehiclePackIndex = 0;
	m_VehicleSelection = 0;
	m_SelectedVehiclePack = 0;
	m_SelectedVehicle = 0;
	m_ChangedVehicle = false;
}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
FrontendAircraftSelectUI::~FrontendAircraftSelectUI()
{

}

/////////////////////////////////////////////////////
/// Method: Enter
/// Params: None
///
/////////////////////////////////////////////////////
void FrontendAircraftSelectUI::Enter()
{
	math::Vec2 srcDims( static_cast<float>( core::app::GetBaseAssetsWidth() ), static_cast<float>( core::app::GetBaseAssetsHeight() ) );
	m_UIMesh = res::LoadModel(1000);
	DBG_ASSERT( m_UIMesh != 0 );

	m_Elements.Load( "assets/ui/frontend_aircraftselect.hui", srcDims, m_UIMesh );

	m_pProfile = 0;
	m_pProfile = ProfileManager::GetInstance()->GetProfile();
	DBG_ASSERT( m_pProfile != 0 );

	m_pScriptData = GetScriptDataHolder();
	m_GameData = m_pScriptData->GetGameData();

	m_VehiclePackList = m_pScriptData->GetVehiclePackList();
	SetupPlaneSelect();
	
	if( !m_pProfile->adsRemoved )
	{
		core::app::SetAdBarState( true );
		#ifdef BASE_PLATFORM_WINDOWS
			core::app::SetAdBarState(  m_pScriptData->GetDevData().showPCAdBar );
		#endif // BASE_PLATFORM_WINDOWS
	}
    
    core::app::SetHandleMenuButton(true);
}

/////////////////////////////////////////////////////
/// Method: Exit
/// Params: None
///
/////////////////////////////////////////////////////
void FrontendAircraftSelectUI::Exit()
{
	if( m_UIMesh != 0 )
	{
		res::RemoveModel( m_UIMesh );
		m_UIMesh = 0;
	}
	
	m_Elements.Release();

	TitleScreenState* pState = static_cast<TitleScreenState*>(GetStateManager()->GetSecondaryStateManager()->GetCurrentState());
	if( pState != 0 )
	{
		pState->ClearVehicleModel( );
		pState->SetShowModelState( false, -1 );
	}
}

/////////////////////////////////////////////////////
/// Method: PrepareTransitionIn
/// Params: None
///
/////////////////////////////////////////////////////
void FrontendAircraftSelectUI::PrepareTransitionIn()
{
	m_FinishedTransitionIn = false;
	m_FadeTransition = 255;

	TitleScreenState* pState = static_cast<TitleScreenState*>(GetStateManager()->GetSecondaryStateManager()->GetCurrentState());
	if( pState != 0 )
	{
		pState->LoadRequest( m_SelectedVehiclePack, m_SelectedVehicle, -1 );
		pState->SetShowModelState( true, -1 );
	}
}

/////////////////////////////////////////////////////
/// Method: PrepareTransitionOut
/// Params: None
///
/////////////////////////////////////////////////////
void FrontendAircraftSelectUI::PrepareTransitionOut()
{
	m_FinishedTransitionOut = false;
	m_FadeTransition = 0;
}

/////////////////////////////////////////////////////
/// Method: TransitionIn
/// Params: None
///
/////////////////////////////////////////////////////
int FrontendAircraftSelectUI::TransitionIn()
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
int FrontendAircraftSelectUI::TransitionOut()
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
void FrontendAircraftSelectUI::Update( float deltaTime )
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

	UpdatePlaneSelect(deltaTime);

}

/////////////////////////////////////////////////////
/// Method: Draw
/// Params: None
///
/////////////////////////////////////////////////////
void FrontendAircraftSelectUI::Draw()
{
	renderer::OpenGL::GetInstance()->SetNearFarClip( NEAR_CLIP_ORTHO, FAR_CLIP_ORTHO );
	renderer::OpenGL::GetInstance()->SetupOrthographicView( core::app::GetOrientationWidth(), core::app::GetOrientationHeight(), true );
	renderer::OpenGL::GetInstance()->DisableLighting();

	renderer::OpenGL::GetInstance()->SetColour4ub( 255, 255, 255, 255 );

	renderer::OpenGL::GetInstance()->DepthMode( false, GL_LESS );
	renderer::OpenGL::GetInstance()->BlendMode( true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	if( m_UIMesh != 0 )
		m_UIMesh->Draw();

	renderer::OpenGL::GetInstance()->SetColour4ub(255,255,255,255);
	m_Elements.Draw();
	m_Elements.DrawSelected();

	renderer::OpenGL::GetInstance()->DepthMode( true, GL_LESS );
	renderer::OpenGL::GetInstance()->BlendMode( false, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
}

/////////////////////////////////////////////////////
/// Method: Draw
/// Params: None
///
/////////////////////////////////////////////////////
void FrontendAircraftSelectUI::DrawBG()
{
	renderer::OpenGL::GetInstance()->SetNearFarClip( NEAR_CLIP_ORTHO, FAR_CLIP_ORTHO );
	renderer::OpenGL::GetInstance()->SetupOrthographicView( core::app::GetOrientationWidth(), core::app::GetOrientationHeight(), true );
	renderer::OpenGL::GetInstance()->DisableLighting();

	renderer::OpenGL::GetInstance()->SetColour4ub( 255, 255, 255, 255 );

	renderer::OpenGL::GetInstance()->DepthMode( false, GL_LESS );
	renderer::OpenGL::GetInstance()->BlendMode( true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	m_UIMesh->SetMeshDrawState( GFX_BG_SKY, true );

	if( m_UIMesh != 0 )
		m_UIMesh->Draw();

	renderer::OpenGL::GetInstance()->SetColour4ub(255,255,255,255);
	m_Elements.Draw();
	m_Elements.DrawSelected();

	m_UIMesh->SetMeshDrawState( GFX_BG_SKY, false );

	renderer::OpenGL::GetInstance()->DepthMode( true, GL_LESS );
	renderer::OpenGL::GetInstance()->BlendMode( false, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
}


/////////////////////////////////////////////////////
/// Method: SetupPlaneSelect
/// Params: None
///
/////////////////////////////////////////////////////
void FrontendAircraftSelectUI::SetupPlaneSelect()
{
	m_UIMesh->SetMeshDrawState( -1, false );
	m_Elements.ChangeElementDrawState( false );

	m_UIMesh->SetMeshDrawState( GFX_BORDER_BOTTOM, true );
	m_UIMesh->SetMeshDrawState( GFX_BG_PAGINATION, true );

	m_Elements.ChangeElementDrawState( HUI_LABEL_AIRCRAFT, true );

	m_Elements.ChangeElementDrawState( HUI_GLYPH_BACK, true );
	m_Elements.ChangeElementDrawState(HUI_BTN_BACK, true);

	m_Elements.ChangeElementDrawState( HUI_GLYPH_MINUS, true );
	m_Elements.ChangeElementDrawState(HUI_BTN_MINUS, true);

	m_Elements.ChangeElementDrawState( HUI_GLYPH_PLUS, true );
	m_Elements.ChangeElementDrawState(HUI_BTN_PLUS, true);

	m_Elements.ChangeElementDrawState( HUI_GLYPH_NEXT, true );
	m_Elements.ChangeElementDrawState(HUI_BTN_NEXT, true);

	m_Elements.ChangeElementDrawState( HUI_TEXT_PAGECOUNTER, true );
	m_Elements.ChangeElementText( HUI_TEXT_PAGECOUNTER, "%d/%d", (m_SelectedVehicle+1), m_VehiclePackList[m_VehiclePackIndex]->packVehicleCount  );

	m_Elements.ChangeElementDrawState( HUI_TEXT_NAME_AIRCRAFTSELECT, true );

	ScriptDataHolder::VehicleScriptData* pVehicleData = &m_VehiclePackList[m_VehiclePackIndex]->pVehicleScriptData[m_SelectedVehicle];
	DBG_ASSERT( pVehicleData != 0 );

	m_Elements.ChangeElementText( HUI_TEXT_NAME_AIRCRAFTSELECT, "%s", res::GetScriptString(pVehicleData->aircraftName) );

	m_Elements.RefreshSelected(HUI_BTN_NEXT);
}		

/////////////////////////////////////////////////////
/// Method: UpdatePlaneSelect
/// Params: None
///
/////////////////////////////////////////////////////
int FrontendAircraftSelectUI::UpdatePlaneSelect( float deltaTime )
{
	if( m_Elements.CheckElementForTouch(HUI_BTN_MINUS) )
	{
		if( (m_SelectedVehicle) > 0 )
		{
			AudioSystem::GetInstance()->PlayUIAudio();

			m_SelectedVehicle--;

			m_Elements.ChangeElementDrawState( HUI_GLYPH_NEXT, true );

			TitleScreenState* pState = static_cast<TitleScreenState*>(GetStateManager()->GetSecondaryStateManager()->GetCurrentState());
			if( pState != 0 )
			{
				pState->LoadRequest( m_SelectedVehiclePack, m_SelectedVehicle, 1 );
				pState->SetShowModelState( true, -1 );
			}

			m_Elements.ChangeElementDrawState( HUI_TEXT_PAGECOUNTER, true );
			m_Elements.ChangeElementText( HUI_TEXT_PAGECOUNTER, "%d/%d", (m_SelectedVehicle+1), m_VehiclePackList[m_VehiclePackIndex]->packVehicleCount  );

			ScriptDataHolder::VehicleScriptData* pVehicleData = &m_VehiclePackList[m_VehiclePackIndex]->pVehicleScriptData[m_SelectedVehicle];
			DBG_ASSERT( pVehicleData != 0 );

			m_Elements.ChangeElementText( HUI_TEXT_NAME_AIRCRAFTSELECT, "%s", res::GetScriptString(pVehicleData->aircraftName) );
		}
	}
	else
	if( m_Elements.CheckElementForTouch(HUI_BTN_PLUS) )
	{
		if( (m_SelectedVehicle+1) < m_VehiclePackList[m_VehiclePackIndex]->packVehicleCount )
		{
			AudioSystem::GetInstance()->PlayUIAudio();

			m_SelectedVehicle++;

			m_Elements.ChangeElementDrawState( HUI_GLYPH_NEXT, true );

			TitleScreenState* pState = static_cast<TitleScreenState*>(GetStateManager()->GetSecondaryStateManager()->GetCurrentState());
			if( pState != 0 )
			{
				pState->LoadRequest( m_SelectedVehiclePack, m_SelectedVehicle, 0 );
				pState->SetShowModelState( true, -1 );
			}

			m_Elements.ChangeElementDrawState( HUI_TEXT_PAGECOUNTER, true );
			m_Elements.ChangeElementText( HUI_TEXT_PAGECOUNTER, "%d/%d", (m_SelectedVehicle+1), m_VehiclePackList[m_VehiclePackIndex]->packVehicleCount  );

			ScriptDataHolder::VehicleScriptData* pVehicleData = &m_VehiclePackList[m_VehiclePackIndex]->pVehicleScriptData[m_SelectedVehicle];
			DBG_ASSERT( pVehicleData != 0 );

			m_Elements.ChangeElementText( HUI_TEXT_NAME_AIRCRAFTSELECT, "%s", res::GetScriptString(pVehicleData->aircraftName) );
		}
	}
	else
	if( m_Elements.CheckElementForTouch(HUI_BTN_NEXT) )
	{
		if( m_SelectedVehicle != -1 )
		{
			AudioSystem::GetInstance()->PlayUIAudio();

			if( VehicleSelected() )
				return 1 ;
		}
	}
	else
	if( m_Elements.CheckElementForTouch(HUI_BTN_BACK) ||
		m_InputSystem.GetMenu() )
	{
		AudioSystem::GetInstance()->PlayUIAudio();

		ChangeState( new FrontendMainMenuUI(*m_pStateManager, m_InputSystem, FrontendMainMenuUI::FrontendStates_MainMenu) );
		return 1;
	}

	return 0;
}	

/////////////////////////////////////////////////////
/// Method: VehicleSelected
/// Params: None
///
/////////////////////////////////////////////////////
int FrontendAircraftSelectUI::VehicleSelected()
{
	AudioSystem::GetInstance()->PlayUIAudio();

	// STORE SELECTED VEHICLE
	int vehicleId = (m_SelectedVehiclePack*m_GameData.MAX_ITEMS_PER_PACK) + m_SelectedVehicle;

	m_pScriptData->SetDefaultVehicle(vehicleId);

	// selected vehicle, now start career mode
	ProfileManager::GetInstance()->SetModeVehicle(vehicleId);

	// arcade mode allows selection of level	
	ChangeState( new FrontendLevelSelectUI(*m_pStateManager, m_InputSystem, FrontendLevelSelectUI::FrontendItemSelectStates_LevelSelect) );
	return 1;
}
