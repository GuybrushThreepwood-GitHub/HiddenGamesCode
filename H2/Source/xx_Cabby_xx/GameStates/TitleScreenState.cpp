
/*===================================================================
	File: ProfileSelectState.cpp
	Game: Cabby

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

#include "CabbyConsts.h"
#include "Cabby.h"

#include "Resources/ModelResources.h"

#include "GameStates/IBaseGameState.h"
#include "Physics/PhysicsWorld.h"
#include "Level/Level.h"
#include "Player/Player.h"
#include "ScriptAccess/ScriptAccess.h"
#include "GameStates/TitleScreenState.h"
#include "GameStates/UI/FrontendInAppPurchaseUI.h"
#include "GameStates/UI/UIIds.h"

namespace
{
	input::Input debugInput;
	const float ROTATION_PER_FRAME = 30.0f;
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
	m_QMarkModel = 0;
	m_RestoreModel = 0;
	m_LevelPackModel = 0;
	m_pVehicleModel = 0;
	m_Rotation = 0.0f;
	m_ModelShowState = false;
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
	b2Vec2 areaMin( -100.0f, -100.0f );
	b2Vec2 areaMax(100.0f, 100.0f);
	b2Vec2 gravity(0.0f,0.0f);
	
	PhysicsWorld::Create( areaMin, areaMax, gravity, false );

	m_QMarkModel = res::LoadModel( 150 );
	m_RestoreModel = res::LoadModel( 151 );
	m_LevelPackModel = res::LoadModel( 152 );
}

/////////////////////////////////////////////////////
/// Method: Exit
/// Params: None
///
/////////////////////////////////////////////////////
void TitleScreenState::Exit()
{
	if( m_QMarkModel != 0 )
	{
		res::RemoveModel(m_QMarkModel);
		m_QMarkModel = 0;
	}

	if( m_RestoreModel != 0 )
	{
		res::RemoveModel(m_RestoreModel);
		m_RestoreModel = 0;
	}

	if( m_LevelPackModel != 0 )
	{
		res::RemoveModel(m_LevelPackModel);
		m_LevelPackModel = 0;
	}

	if( m_pVehicleModel != 0 )
	{
		res::RemoveModel( m_pVehicleModel );
		m_pVehicleModel = 0;
	}

	PhysicsWorld::Destroy();
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

	/*if( m_DevData.allowDebugCam )
	{
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
	}*/

}

/////////////////////////////////////////////////////
/// Method: Draw
/// Params: None
///
/////////////////////////////////////////////////////
void TitleScreenState::Draw()
{
	renderer::OpenGL::GetInstance()->SetColour4ub( 255, 255, 255, 255 );

	/*if( gDebugCamera.IsEnabled() )
	{
		renderer::OpenGL::GetInstance()->SetNearFarClip( 0.5f, 10000.0f );
		renderer::OpenGL::GetInstance()->SetupPerspectiveView( core::app::GetOrientationWidth(), core::app::GetOrientationHeight(), true );

		renderer::OpenGL::GetInstance()->SetLookAt( gDebugCamera.GetPosition().X, gDebugCamera.GetPosition().Y, gDebugCamera.GetPosition().Z, 
												gDebugCamera.GetTarget().X, gDebugCamera.GetTarget().Y, gDebugCamera.GetTarget().Z );
	
		snd::SoundManager::GetInstance()->SetListenerPosition( gDebugCamera.GetPosition().X, gDebugCamera.GetPosition().Y, gDebugCamera.GetPosition().Z );
		snd::SoundManager::GetInstance()->SetListenerOrientation( (float)std::sin(math::DegToRad(-gDebugCamera.GetAngle())), 0.0f, (float)std::cos(math::DegToRad(-gDebugCamera.GetAngle())),
																	0.0f, 1.0f, 0.0f );
	}
	else*/
	{
		renderer::OpenGL::GetInstance()->SetNearFarClip( 0.01f, 500.0f );
		renderer::OpenGL::GetInstance()->SetupPerspectiveView( core::app::GetOrientationWidth(), core::app::GetOrientationHeight(), true );

		renderer::OpenGL::GetInstance()->SetLookAt( -0.0f, 1.3344823f, 8.0707846f, 0.0f, -40.665539f, -93.640244f );

		snd::SoundManager::GetInstance()->SetListenerPosition( 0.0f, 2.0f, 10.0f );
		snd::SoundManager::GetInstance()->SetListenerOrientation( std::sin(math::DegToRad(180.0f)), 0.0f, std::cos(math::DegToRad(180.0f)), 0.0f, 1.0f, 0.0f );
	}

	renderer::OpenGL::GetInstance()->DepthMode( true, GL_LESS );

	renderer::OpenGL::GetInstance()->BlendMode( true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	renderer::OpenGL::GetInstance()->EnableLighting();

	math::Vec4 lightPos( 0.0f, 1.0f, 5.0f, 1.0f );
	math::Vec4 lightDiff( 1.0f, 1.0f, 1.0f, 1.0f );
	math::Vec4 lightAmb( 1.0f, 1.0f, 1.0f, 1.0f );
	math::Vec4 lightSpec( 1.0f, 1.0f, 1.0f, 1.0f );
	renderer::OpenGL::GetInstance()->EnableLight( 0 );
	renderer::OpenGL::GetInstance()->SetLightPosition( 0, lightPos );
	renderer::OpenGL::GetInstance()->SetLightDiffuse( 0, lightDiff );
	renderer::OpenGL::GetInstance()->SetLightAmbient( 0, lightAmb );
	renderer::OpenGL::GetInstance()->SetLightSpecular( 0, lightSpec );
	renderer::OpenGL::GetInstance()->SetLightAttenuation( 0, GL_CONSTANT_ATTENUATION, 1.0f );
	renderer::OpenGL::GetInstance()->SetLightAttenuation( 0, GL_LINEAR_ATTENUATION, 0.0f );
	renderer::OpenGL::GetInstance()->SetLightAttenuation( 0, GL_QUADRATIC_ATTENUATION, 0.0f );

	if( m_ModelShowState )
	{
		if( m_pVehicleModel != 0 )
		{
			glPushMatrix();
				glTranslatef( 0.0f, 0.0f, 0.0f );
				glRotatef( m_Rotation, 0.0f, 1.0f, 0.0f );
				if( m_pVehicleModel != 0 )
					m_pVehicleModel->Draw();
			glPopMatrix();
		}
		else
		{
			glPushMatrix();
				glTranslatef( 0.0f, -0.25f, 0.0f );
				glRotatef( m_Rotation, 0.0f, 1.0f, 0.0f );
				if( m_ModelIndex == 0 )
				{					
					if( m_RestoreModel != 0 )
						m_RestoreModel->Draw();
				}
				else
				if( m_ModelIndex == 1 )
				{
					if( m_LevelPackModel != 0 )
						m_LevelPackModel->Draw();
				}
				else
				{
					if( m_QMarkModel != 0 )
						m_QMarkModel->Draw();
				}
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
void TitleScreenState::LoadRequest( int packIndex, int itemIndex )
{
	const char* funcCall = GetScriptDataHolder()->GetVehiclePackList()[packIndex]->pPackVehicleInfo[itemIndex].setupFunction;

	// call the vehicle setup
	script::LuaCallFunction( funcCall, 0, 0 );

	// in this state there should only be one vehicle
	ScriptDataHolder::VehicleScriptData* pVehicleData = GetScriptDataHolder()->GetVehicleList()[0];
	DBG_ASSERT( pVehicleData != 0 );

	m_pVehicleModel = res::LoadModel( pVehicleData->modelIndex );

	// by default disable everything
	m_pVehicleModel->SetMeshDrawState( -1, false );

	// main body
	m_pVehicleModel->SetMeshDrawState( pVehicleData->mainBodyMesh, true );

	// permanent meshes
	unsigned int i=0;
	if( pVehicleData->hasPermanentMeshes )
	{
		for( i=0; i < static_cast<unsigned int>(pVehicleData->permanentSubMeshCount); ++i )
			m_pVehicleModel->SetMeshDrawState( pVehicleData->permanentSubMeshList[i], true );
	}

	// remove the vehicle, it's not to be used
	if( pVehicleData->pLandingGearDef != 0 )
	{
		delete pVehicleData->pLandingGearDef;
		pVehicleData->pLandingGearDef = 0;
	}
	PhysicsWorld::GetWorld()->DestroyBody( pVehicleData->pBody );
	delete pVehicleData;

	GetScriptDataHolder()->GetVehicleList().pop_back();

	IState* pUIState = reinterpret_cast<IState*>(GetStateManager()->GetSecondaryStateManager()->GetCurrentState());

	if( pUIState->GetId() == UI_FRONTENDINAPPPURCHASES )
	{
		FrontendInAppPurchaseUI* pState = static_cast<FrontendInAppPurchaseUI*>(GetStateManager()->GetSecondaryStateManager()->GetCurrentState());

		pState->SetModelLoaded();
	}
}
