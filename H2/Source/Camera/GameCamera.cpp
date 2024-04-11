
/*===================================================================
	File: GameCamera.cpp
	Game: Cabby

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"
#include "MathBase.h"
#include "CollisionBase.h"
#include "RenderBase.h"
#include "ModelBase.h"
#include "ScriptBase.h"

#include "Input/InputInclude.h"
#include "Input/Input.h"

#include "ScriptAccess/ScriptDataHolder.h"

#include "Camera.h"

#include "GameCamera.h"

const float CAM_CATCHUP = 6.0f;

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
GameCamera::GameCamera()
	: m_Player(0)
	, m_CameraMode(NONE)
	//, m_pCamPoint(0)
	, m_LerpEnabled(true)
	, m_LerpSpeed(CAM_CATCHUP)
{
	//ScriptDataHolder* pScriptData = GetScriptDataHolder();
	//m_CameraData = pScriptData->GetCameraData();

	m_CurrentZoom = 50.0f;//m_CameraData.cam_zdistance;

	m_LerpPos = math::Vec3( 0.0f, 0.0f, 10.0f );
	m_LerpTarget = math::Vec3( 0.0f, 0.0f, 0.0f );
}

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
GameCamera::GameCamera( Player& player )
	: m_Player(&player)
	, m_CameraMode(FOLLOW_PLAYER)
	//, m_pCamPoint(0)
	, m_LerpEnabled(true)
	, m_LerpSpeed(CAM_CATCHUP)
{
	//ScriptDataHolder* pScriptData = GetScriptDataHolder();
	//m_CameraData = pScriptData->GetCameraData();

	m_CurrentZoom = 50.0f;//m_CameraData.cam_zdistance;

	math::Vec3 playerPos = m_Player->GetPosition();
	m_LerpPos = math::Vec3( playerPos.X, playerPos.Y, playerPos.Z );
	m_LerpTarget = math::Vec3( playerPos.X, playerPos.Y, playerPos.Z-1.0f );
}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
GameCamera::~GameCamera()
{

}

/////////////////////////////////////////////////////
/// Method: SetupCamera
/// 
///
/////////////////////////////////////////////////////
void GameCamera::SetupCamera()
{
	renderer::OpenGL::GetInstance()->SetLookAt( m_Pos.X, m_Pos.Y, m_Pos.Z, 
													m_Target.X, m_Target.Y, m_Target.Z );
}

/////////////////////////////////////////////////////
/// Method: Update
/// Params: deltaTime
///
/////////////////////////////////////////////////////
void GameCamera::Update( float deltaTime )
{
	//m_LerpSpeed = CAM_CATCHUP;

	math::Vec3 newPos( 0.0f, 0.0f, 0.0f );
	math::Vec3 newLookAt( 0.0f, 0.0f, 0.0f );

	//const float LERP_TOLERANCE = 0.01f;

	if( m_LerpEnabled )
	{
		if( m_CameraMode == NONE )
		{
			//bool posArrive = math::WithinTolerance( m_Pos, m_LerpPos, LERP_TOLERANCE );

			//if( !posArrive )
				m_Pos = math::Lerp( m_Pos, m_LerpPos,  deltaTime*m_LerpSpeed );

			//bool targetArrive = math::WithinTolerance( m_LerpTarget, m_Target, LERP_TOLERANCE );
			//if( !targetArrive )
				m_Target = math::Lerp( m_Target, m_LerpTarget,  deltaTime*m_LerpSpeed );
		}
	}
}

/////////////////////////////////////////////////////
/// Method: RotateAroundPoint
/// 
///
/////////////////////////////////////////////////////
void GameCamera::RotateAroundPoint(math::Vec3& vCenter, float x, float y, float z)
{	
	math::Vec3 vVector = m_LerpPos - vCenter;
	float sinVal;
	float cosVal;

	if(x != 0.0f)
	{
		math::sinCos( &sinVal, &cosVal, x );

		m_LerpPos.Y = (float)(vCenter.Y + sinVal*vVector.Y + cosVal*vVector.X);
		m_LerpPos.Z = (float)(vCenter.Z - sinVal*vVector.X + cosVal*vVector.Z);
	}
	if(y != 0.0f) 
	{
		math::sinCos( &sinVal, &cosVal, y );

		m_LerpPos.X = (float)(vCenter.X + cosVal*vVector.X + sinVal*vVector.Z);
		m_LerpPos.Z = (float)(vCenter.Z - sinVal*vVector.X + cosVal*vVector.Z);

		m_Angle += math::RadToDeg(y);
	}
	if(z != 0.0f)
	{
		math::sinCos( &sinVal, &cosVal, z );

		m_LerpPos.X = (float)(vCenter.X + cosVal*vVector.Y - sinVal*vVector.X);		
		m_LerpPos.Y = (float)(vCenter.Y + sinVal*vVector.Y + cosVal*vVector.X);
	}

	if( !m_LerpEnabled )
		m_Pos = m_LerpPos;
}