
/*===================================================================
	File: GameCamera.cpp
	Game: H8

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

namespace
{
	const float CAM_CATCHUP = 6.0f;
	math::Vec3 zeroVec(0.0f,0.0f,0.0f);
}

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
GameCamera::GameCamera( InputSystem& inputSystem )
	: m_InputSystem(inputSystem)
	, m_Player(0)
	, m_CameraMode(NONE)
	, m_LerpEnabled(true)
	, m_LerpSpeed(CAM_CATCHUP)
{
	//ScriptDataHolder* pScriptData = GetScriptDataHolder();
	//m_CameraData = pScriptData->GetCameraData();

	m_CurrentZoom = 50.0f;

	m_LerpPos = math::Vec3( 0.0f, 0.0f, 0.0f );
	m_LerpTarget = math::Vec3( 0.0f, 0.0f, 100.0f );

	// look down the positive Z
	m_Dir = math::Vec3( 0.0f, 0.0f, 1.0f );
}

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
GameCamera::GameCamera( InputSystem& inputSystem, Player& player )
	: m_InputSystem(inputSystem)
	, m_Player(&player)
	, m_CameraMode(FOLLOW_PLAYER)
	, m_LerpEnabled(true)
	, m_LerpSpeed(CAM_CATCHUP)
{
	//ScriptDataHolder* pScriptData = GetScriptDataHolder();
	//m_CameraData = pScriptData->GetCameraData();

	m_CurrentZoom = 50.0f;

	/*math::Vec3 playerPos = m_Player->GetPosition();*/

	m_LerpPos = math::Vec3( 0.0f, 10.0f, 0.0f );
	m_LerpTarget = math::Vec3( 0.0f, 0.0f, 0.0f );
	m_UpVector =  math::Vec3( 0.0f, 1.0f, 0.0f );
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
	
	//glRotatef( m_Rot.Z, 0.0f, 0.0f, 1.0f );

	renderer::OpenGL::GetInstance()->SetLookAt( m_Pos.X, m_Pos.Y, m_Pos.Z, 
													m_Target.X, m_Target.Y, m_Target.Z, m_UpVector.X, m_UpVector.Y, m_UpVector.Z );
}

/////////////////////////////////////////////////////
/// Method: Update
/// Params: deltaTime
///
/////////////////////////////////////////////////////
void GameCamera::Update( float deltaTime )
{
	math::Vec3 newPos( 0.0f, 0.0f, 0.0f );
	math::Vec3 newLookAt( 0.0f, 0.0f, 0.0f );

	/*math::Vec3 playerPos = m_Player->GetPosition();
	math::Vec3 playerRot = m_Player->GetAngles();
	math::Vec3 playerDir = m_Player->GetDirection();

	math::Vec3 origPos = math::Vec3( playerPos.X, playerPos.Y + 6.5f, playerPos.Z );

	math::Vec3 camPos = math::RotateAroundPoint( 0, origPos, playerPos, math::DegToRad(playerRot.X) );
	camPos = math::RotateAroundPoint( 1, camPos, playerPos, math::DegToRad(playerRot.Y) );
	//camPos = math::RotateAroundPoint( 2, camPos, playerPos, math::DegToRad(playerRot.Z) );

	SetPosition( camPos );

	math::Vec3 camTarget = camPos + (playerDir*500.0f);
	SetTarget( camTarget );*/

	/*bool liftState = m_InputSystem.GetLift();
	bool diveState = m_InputSystem.GetDive();
	bool leftState = m_InputSystem.GetLeft();
	bool rightState = m_InputSystem.GetRight();

	m_FinalDir = m_Dir;

	const float ROT_SPEED = 20.0f;
	const float ROT_SPEED_Y = 30.0f;
	const float ROT_SPEED_Z = 60.0f;

	if( liftState )
	{
		m_Rot.X -= ROT_SPEED*deltaTime;
	}
	else if( diveState )
	{
		m_Rot.X += ROT_SPEED*deltaTime;
	}

	if( leftState )
	{
		m_Rot.Y += ROT_SPEED_Y*deltaTime;
		m_Rot.Z -= ROT_SPEED_Z*deltaTime;
	}
	else if( rightState )
	{
		m_Rot.Y -= ROT_SPEED_Y*deltaTime;
		m_Rot.Z += ROT_SPEED_Z*deltaTime;
	}
	else
	{
		if( math::WithinTolerance( m_Rot.Z, 1.0f ) )
		{
			m_Rot.Z = 0.0f;
		}
		else
		{
			if( m_Rot.Z > 0.0f )
			{
				m_Rot.Z -= ROT_SPEED_Z*deltaTime;
				m_Rot.Y -= (ROT_SPEED_Y*0.5f)*deltaTime;
			}
			else if( m_Rot.Z < 0.0f  )
			{
				m_Rot.Z += ROT_SPEED_Z*deltaTime;
				m_Rot.Y += (ROT_SPEED_Y*0.5f)*deltaTime;
			}
		}	
	}

	if( m_Rot.Z > 35.0f )
		m_Rot.Z = 35.0f;
	else if( m_Rot.Z < -35.0f )
		m_Rot.Z = -35.0f;

	math::Matrix3x3 matY, matX, matZ, tmp, final;
	tmp.Identity();
	m_FinalRotMatrix.Identity();
	
	matY.SetupAxisAngle( math::Vec3( 0.0f, 1.0f, 0.0f ), math::DegToRad( m_Rot.Y ) );
	matX.SetupAxisAngle( math::Vec3( 1.0f, 0.0f, 0.0f ), math::DegToRad( m_Rot.X ) );
	matZ.SetupAxisAngle( math::Vec3( 0.0f, 0.0f, 1.0f ), math::DegToRad( m_Rot.Z ) );
	
	tmp = matX * matZ;
	
	// create a final rotation matrix
	m_FinalRotMatrix = tmp * matY;

	m_FinalDir.X = (m_Dir.X * m_FinalRotMatrix.m[0]) + (m_Dir.Y * m_FinalRotMatrix.m[3]) + (m_Dir.Z * m_FinalRotMatrix.m[6]) + zeroVec.X;
	m_FinalDir.Y = (m_Dir.X * m_FinalRotMatrix.m[1]) + (m_Dir.Y * m_FinalRotMatrix.m[4]) + (m_Dir.Z * m_FinalRotMatrix.m[7]) + zeroVec.Y;
	m_FinalDir.Z = (m_Dir.X * m_FinalRotMatrix.m[2]) + (m_Dir.Y * m_FinalRotMatrix.m[5]) + (m_Dir.Z * m_FinalRotMatrix.m[8]) + zeroVec.Z;

	m_FinalDir.normalise();

	m_Pos += (m_FinalDir*100.0f)*deltaTime;
	m_Target = m_Pos + (m_FinalDir*100.0f);*/

	if( m_LerpEnabled )
	{
		if( m_CameraMode == NONE )
		{
			m_Pos = math::Lerp( m_Pos, m_LerpPos, deltaTime*m_LerpSpeed );

			m_Target = math::Lerp( m_Target, m_LerpTarget, deltaTime*m_LerpSpeed );
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