
/*===================================================================
	File: UnlockState.cpp
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
#include "GameStates/UI/UIIds.h"
#include "GameStates/UnlockState.h"
#include "GameStates/UI/GameCompleteUI.h"
#include "Profiles/ProfileManager.h"

namespace
{
#ifdef BASE_PLATFORM_WINDOWS
	input::Input win32Input;
#endif // BASE_PLATFORM_WINDOWS

	const float ROTATION_PER_FRAME = 30.0f;
}

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
UnlockState::UnlockState( StateManager& stateManager, InputSystem& inputSystem )
: IBaseGameState( stateManager, inputSystem )
, m_pScriptData(0)
{
	m_LastDelta = 0.0f;

	// load from script
	m_pScriptData = GetScriptDataHolder();
	m_GameData = m_pScriptData->GetGameData();

	m_pVehicleModel = 0;
	m_Rotation = 0.0f;

	m_UnlockVehicleId = -1;

	m_pIState = 0;
}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
UnlockState::~UnlockState()
{
	if( m_pVehicleModel != 0 )
	{
		res::RemoveModel( m_pVehicleModel );
		m_pVehicleModel = 0;
	}
	m_UnlockVehicleId = -1;
}

/////////////////////////////////////////////////////
/// Method: Enter
/// Params: None
///
/////////////////////////////////////////////////////
void UnlockState::Enter()
{
	b2Vec2 areaMin( -100.0f, -100.0f );
	b2Vec2 areaMax(100.0f, 100.0f);
	b2Vec2 gravity(0.0f,0.0f);
	
	PhysicsWorld::Create( areaMin, areaMax, gravity, false );

	//renderer::OpenGL::GetInstance()->ClearColour( 0.1f, 0.1f, 0.1f, 1.0f );
}

/////////////////////////////////////////////////////
/// Method: Exit
/// Params: None
///
/////////////////////////////////////////////////////
void UnlockState::Exit()
{
	if( m_pVehicleModel != 0 )
	{
		res::RemoveModel( m_pVehicleModel );
		m_pVehicleModel = 0;
	}

	// clear the vehicle
	m_pScriptData->ReleaseVehicleList();

	PhysicsWorld::Destroy();
}

/////////////////////////////////////////////////////
/// Method: TransitionIn
/// Params: None
///
/////////////////////////////////////////////////////
int UnlockState::TransitionIn()
{

	return(0);
}

/////////////////////////////////////////////////////
/// Method: TransitionOut
/// Params: None
///
/////////////////////////////////////////////////////
int UnlockState::TransitionOut()
{
	return(0);
}

/////////////////////////////////////////////////////
/// Method: Update
/// Params: None
///
/////////////////////////////////////////////////////
void UnlockState::Update( float deltaTime )
{
	m_LastDelta = deltaTime;

	m_pIState = reinterpret_cast<IState*>(GetStateManager()->GetSecondaryStateManager()->GetCurrentState());

	if( m_pIState->GetId() == UI_GAMECOMPLETE )
	{
		GameCompleteUI* pUIState = static_cast<GameCompleteUI*>(GetStateManager()->GetSecondaryStateManager()->GetCurrentState());

		if(ProfileManager::GetInstance()->GetVehicleUnlockState() &&
			 m_UnlockVehicleId != ProfileManager::GetInstance()->GetVehicleUnlockId() &&
			 pUIState->FinishedFade() )
		{
			m_UnlockVehicleId = ProfileManager::GetInstance()->GetVehicleUnlockId();
			DBG_ASSERT( m_UnlockVehicleId != -1 );

			int packId = m_UnlockVehicleId / m_GameData.MAX_ITEMS_PER_PACK;
			int vehicleOffsetId = m_UnlockVehicleId % m_GameData.MAX_ITEMS_PER_PACK;

			const char* funcCall = GetScriptDataHolder()->GetVehiclePackList()[packId]->pPackVehicleInfo[vehicleOffsetId].setupFunction;

			// call the vehicle setup
			script::LuaCallFunction( funcCall, 0, 0 );

			// in this state there should only be one vehicle
			ScriptDataHolder::VehicleScriptData* pVehicleData = m_pScriptData->GetVehicleList()[0];
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

			m_pScriptData->GetVehicleList().pop_back();
		}

	}
	
	m_Rotation += ROTATION_PER_FRAME*deltaTime;
}

/////////////////////////////////////////////////////
/// Method: Draw
/// Params: None
///
/////////////////////////////////////////////////////
void UnlockState::Draw()
{
	ScriptDataHolder::DevScriptData devData = m_pScriptData->GetDevData();

	// default colour
	renderer::OpenGL::GetInstance()->SetColour4ub( 255, 255, 255, 255 );

	if( devData.allowDebugCam && gDebugCamera.IsEnabled() )
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
		renderer::OpenGL::GetInstance()->SetNearFarClip( 0.01f, 500.0f );
		renderer::OpenGL::GetInstance()->SetupPerspectiveView( core::app::GetOrientationWidth(), core::app::GetOrientationHeight(), true );

		renderer::OpenGL::GetInstance()->SetLookAt( -2.9f, 1.6917025f, 4.1825247f, 300.0f, -232.30847f, -452.83453f );

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

	glPushMatrix();
		glTranslatef( 0.0f, 0.0f, 0.0f );
		glRotatef( m_Rotation, 0.0f, 1.0f, 0.0f );
		if( m_pVehicleModel != 0 )
			m_pVehicleModel->Draw();
	glPopMatrix();

	renderer::OpenGL::GetInstance()->DisableLighting();
	renderer::OpenGL::GetInstance()->BlendMode( false, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
}

