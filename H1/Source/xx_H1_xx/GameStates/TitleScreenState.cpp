
/*===================================================================
	File: ProfileSelectState.cpp
	Game: AirCadets

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"
#include "MathBase.h"
#include "RenderBase.h"
#include "SoundBase.h"
#include "CollisionBase.h"
#include "Model/ModelCommon.h"
#include "ModelBase.h"
#include "InputBase.h"
#include "ScriptBase.h"

#include "Camera/Camera.h"
#include "Camera/DebugCamera.h"

#include "H1Consts.h"
#include "H1.h"

#include "Resources/ModelResources.h"

#include "GameStates/IBaseGameState.h"
#include "Level/Level.h"
#include "Player/Player.h"
#include "ScriptAccess/ScriptAccess.h"
#include "GameStates/TitleScreenState.h"
#include "GameStates/UI/FrontendAircraftSelectUI.h"
#include "GameStates/UI/UIIds.h"

namespace
{
	input::Input debugInput;
	const float ROTATION_PER_FRAME = 30.0f;
	const float MAX_MOVE_X = 20.0f;
	const float MOVE_X_SPEED = 35.0f;
}

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
TitleScreenState::TitleScreenState( StateManager& stateManager, InputSystem& inputSystem )
: IBaseGameState( stateManager, inputSystem )
{
	m_LastDelta = 0.0f;
	m_pVehicleModel = 0;
	m_Rotation = 0.0f;
	m_ModelShowState = false;
	m_LoadRequest = false;
	
	m_PackIndex = 0;
	m_ItemIndex = 0;
	m_LeftOrRight = -1;
	m_PosMove.setZero();
	m_PosMove.X = 0.0f;
	m_FinishedMove = false;
}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
TitleScreenState::~TitleScreenState()
{

}

/////////////////////////////////////////////////////
/// Method: Enter
/// Params: None
///
/////////////////////////////////////////////////////
void TitleScreenState::Enter()
{


}

/////////////////////////////////////////////////////
/// Method: Exit
/// Params: None
///
/////////////////////////////////////////////////////
void TitleScreenState::Exit()
{
	if( m_pVehicleModel != 0 )
	{
		res::RemoveModel( m_pVehicleModel );
		m_pVehicleModel = 0;
	}
}

/////////////////////////////////////////////////////
/// Method: TransitionIn
/// Params: None
///
/////////////////////////////////////////////////////
int TitleScreenState::TransitionIn()
{
	return(0);
}

/////////////////////////////////////////////////////
/// Method: TransitionOut
/// Params: None
///
/////////////////////////////////////////////////////
int TitleScreenState::TransitionOut()
{
	return(0);
}

/////////////////////////////////////////////////////
/// Method: Update
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
void TitleScreenState::Update( float deltaTime )
{
	m_LastDelta = deltaTime;

	m_Rotation += ROTATION_PER_FRAME*deltaTime;

	if( m_LoadRequest )
	{
		if( m_LeftOrRight == 0 ) // move left
		{
			m_PosMove.X -= MOVE_X_SPEED*deltaTime;
			if( m_PosMove.X <= -MAX_MOVE_X )
			{
				m_PosMove.X = MAX_MOVE_X;
				ClearVehicleModel();
				LoadAircraftModel();
			}
		}
		else if( m_LeftOrRight == 1 ) // move right
		{
			m_PosMove.X += MOVE_X_SPEED*deltaTime;
			if( m_PosMove.X >= MAX_MOVE_X )
			{
				m_PosMove.X = -MAX_MOVE_X;
				ClearVehicleModel();
				LoadAircraftModel();
			}
		}
	}
	else
	{
		if( m_LeftOrRight == 0 ) // move left
		{
			m_PosMove.X -= MOVE_X_SPEED*deltaTime;
			if( m_PosMove.X <= 0.0f )
			{
				m_PosMove.X = 0.0f;
				m_LeftOrRight = -1;
				m_FinishedMove = true;
			}
		}
		else if( m_LeftOrRight == 1 ) // move right
		{
			m_PosMove.X += MOVE_X_SPEED*deltaTime;
			if( m_PosMove.X >= 0.0f )
			{
				m_PosMove.X = 0.0f;
				m_LeftOrRight = -1;
				m_FinishedMove = true;
			}
		}
	}

	//if( m_DevData.allowDebugCam )
/*	{
		if( debugInput.IsKeyPressed( input::KEY_9, true ) )
		{
			if( gDebugCamera.IsEnabled() )
				gDebugCamera.Disable();	
			else
				gDebugCamera.Enable();
		}
	}	
	
	//if( m_DevData.allowDebugCam )
	{
		if( gDebugCamera.IsEnabled() )
			gDebugCamera.Update( deltaTime );
	}
*/
}

/////////////////////////////////////////////////////
/// Method: Draw
/// Params: None
///
/////////////////////////////////////////////////////
void TitleScreenState::Draw()
{
	renderer::OpenGL::GetInstance()->SetColour4ub( 255, 255, 255, 255 );

	IState* pState = reinterpret_cast<IState*>(GetStateManager()->GetSecondaryStateManager()->GetCurrentState());
	FrontendAircraftSelectUI* pUIState = 0;

	if( pState->GetId() == UI_FRONTENDAIRCRAFTSELECT )
	{
		pUIState = static_cast<FrontendAircraftSelectUI*>(GetStateManager()->GetSecondaryStateManager()->GetCurrentState());
		pUIState->DrawBG();
	}

	if( gDebugCamera.IsEnabled() )
	{
		renderer::OpenGL::GetInstance()->SetNearFarClip( 0.5f, 10000.0f );
		renderer::OpenGL::GetInstance()->SetupPerspectiveView( core::app::GetOrientationWidth(), core::app::GetOrientationHeight(), true );

		renderer::OpenGL::GetInstance()->SetLookAt( gDebugCamera.GetPosition().X, gDebugCamera.GetPosition().Y, gDebugCamera.GetPosition().Z, 
												gDebugCamera.GetTarget().X, gDebugCamera.GetTarget().Y, gDebugCamera.GetTarget().Z );
	
		snd::SoundManager::GetInstance()->SetListenerPosition( gDebugCamera.GetPosition().X, gDebugCamera.GetPosition().Y, gDebugCamera.GetPosition().Z );
		snd::SoundManager::GetInstance()->SetListenerOrientation( (float)std::sin(math::DegToRad(-gDebugCamera.GetAngle())), 0.0f, (float)std::cos(math::DegToRad(-gDebugCamera.GetAngle())),
																	0.0f, 1.0f, 0.0f );
	}
	else
	{
		renderer::OpenGL::GetInstance()->SetNearFarClip( 1.0f, 500.0f );
		renderer::OpenGL::GetInstance()->SetupPerspectiveView( core::app::GetOrientationWidth(), core::app::GetOrientationHeight(), true );

		renderer::OpenGL::GetInstance()->SetLookAt( -0.0f, 5.0f, 12.0f, 0.0f, 0.0f, 0.0f );

		snd::SoundManager::GetInstance()->SetListenerPosition( 0.0f, 2.0f, 10.0f );
		snd::SoundManager::GetInstance()->SetListenerOrientation( std::sin(math::DegToRad(180.0f)), 0.0f, std::cos(math::DegToRad(180.0f)), 0.0f, 1.0f, 0.0f );
	}

	renderer::OpenGL::GetInstance()->DepthMode( true, GL_LESS );
	renderer::OpenGL::GetInstance()->BlendMode( true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	renderer::OpenGL::GetInstance()->EnableLighting();

	math::Vec4 lightPos( 0.0f, 0.5f, 0.5f, 0.0f );
	math::Vec4 lightDiff( 0.9f, 0.9f, 0.9f, 1.0f );
	math::Vec4 lightAmb( 0.2f, 0.2f, 0.2f, 1.0f );
	math::Vec4 lightSpec( 1.0f, 1.0f, 1.0f, 1.0f );
	renderer::OpenGL::GetInstance()->EnableLight( 0 );
	renderer::OpenGL::GetInstance()->SetLightPosition( 0, lightPos );
	renderer::OpenGL::GetInstance()->SetLightDiffuse( 0, lightDiff );
	renderer::OpenGL::GetInstance()->SetLightAmbient( 0, lightAmb );
	renderer::OpenGL::GetInstance()->SetLightSpecular( 0, lightSpec );
	renderer::OpenGL::GetInstance()->SetLightAttenuation( 0, GL_CONSTANT_ATTENUATION, 0.0f );
	renderer::OpenGL::GetInstance()->SetLightAttenuation( 0, GL_LINEAR_ATTENUATION, 0.0f );
	renderer::OpenGL::GetInstance()->SetLightAttenuation( 0, GL_QUADRATIC_ATTENUATION, 0.00000001f );

	if( m_ModelShowState )
	{
		if( m_pVehicleModel != 0 )
		{
			glPushMatrix();
				glTranslatef( m_PosMove.X, m_PosMove.Y, m_PosMove.Z );
				glRotatef( m_Rotation, 0.0f, 1.0f, 0.0f );
				if( m_pVehicleModel != 0 )
					m_pVehicleModel->Draw();
			glPopMatrix();
		}
	}
	renderer::OpenGL::GetInstance()->DisableLighting();
	renderer::OpenGL::GetInstance()->BlendMode( false, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
}

/////////////////////////////////////////////////////
/// Method: ClearVehicleModel
/// Params: None
///
/////////////////////////////////////////////////////
void TitleScreenState::ClearVehicleModel( )
{
	if( m_pVehicleModel != 0 )
	{
		res::RemoveModel( m_pVehicleModel );
		m_pVehicleModel = 0;
	}
}

/////////////////////////////////////////////////////
/// Method: LoadRequest
/// Params: [in]packIndex, [in]itemIndex
///
/////////////////////////////////////////////////////
void TitleScreenState::LoadRequest( int packIndex, int itemIndex, int leftOrRight )
{
	m_LoadRequest = true;
	m_FinishedMove = false;

	m_LeftOrRight = leftOrRight;
	m_PackIndex = packIndex;
	m_ItemIndex = itemIndex;

	if( m_LeftOrRight == -1 )
	{
		m_PosMove.X = 0.0f;
		ClearVehicleModel();
		LoadAircraftModel();
		m_FinishedMove = true;
		m_LoadRequest = false;
	}
}

/////////////////////////////////////////////////////
/// Method: LoadAircraftModel
/// Params:
///
/////////////////////////////////////////////////////
void TitleScreenState::LoadAircraftModel()
{
	ScriptDataHolder::VehicleScriptData* pVehicleData = &GetScriptDataHolder()->GetVehiclePackList()[m_PackIndex]->pVehicleScriptData[m_ItemIndex];
	DBG_ASSERT( pVehicleData != 0 );

	m_pVehicleModel = res::LoadModel( pVehicleData->modelIndexUI );

	m_LoadRequest = false;

	/*IState* pUIState = reinterpret_cast<IState*>(GetStateManager()->GetSecondaryStateManager()->GetCurrentState());

	if( pUIState->GetId() == UI_FRONTENDINAPPPURCHASES )
	{
		FrontendInAppPurchaseUI* pState = static_cast<FrontendInAppPurchaseUI*>(GetStateManager()->GetSecondaryStateManager()->GetCurrentState());

		pState->SetModelLoaded();
	}*/
}
