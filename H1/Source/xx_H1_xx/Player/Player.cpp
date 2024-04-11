
/*===================================================================
	File: Player.cpp
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

#include "Profiles/ProfileManager.h"

#include "Resources/EmitterResources.h"
#include "Resources/ModelResources.h"
#include "Resources/TextureResources.h"

#include "Player/Player.h"

namespace
{
	GLfloat m[16] = 
	{ 1.0f, 0.0f, 0.0f, 0.0f,
	  0.0f, 1.0f, 0.0f, 0.0f,
	  0.0f, 0.0f, 1.0f, 0.0f,
	  0.0f, 0.0f, 0.0f, 1.0f };

	math::Vec3 zeroVec(0.0f,0.0f,0.0f);

	const float MAX_PROP_SPEED = 2000.0f;
	const float MIN_PROP_SPEED = 0.25f;

	ALuint bulletHit = snd::INVALID_SOUNDBUFFER;
	ALuint bulletMiss = snd::INVALID_SOUNDBUFFER;

	const float RING_PITCH_START = 0.0f;
}

void PhysicsTestPlayerToWorld(void *data, dGeomID o1, dGeomID o2);

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
Player::Player( InputSystem& inputSystem, ScriptDataHolder::VehicleScriptData& vehicleData, Level& activeLevel )
	: PhysicsIdentifier( PHYSICSBASICID_PLAYER, PHYSICSCASTID_PLAYER)
	, m_InputSystem(inputSystem)
	, m_VehicleData(vehicleData)
	, m_ActiveLevel(activeLevel)
{
	int i=0;
	m_InputSystem.ResetAllInput();

	m_pScriptData = GetScriptDataHolder();
	m_GameData = m_pScriptData->GetGameData();
	m_DevData = m_pScriptData->GetDevData();

	m_LastDelta = 0.0f;
	m_pModelData = 0;
	m_pShadow = 0;

	m_PhysicsData = 0;
	m_BodyID = 0;
	m_EntitySpace = 0;

	m_DistanceToGround = 9999999.0f;

	// create model and anims
	m_pModelData = res::LoadModel( m_VehicleData.modelIndex );
	DBG_ASSERT( m_pModelData != 0 );

	m_ShadowDraw = false;

	m_ShadowPos.setZero();
	dRSetIdentity( m_ShadowOri );

	if( m_VehicleData.modelFileShadow != -1 )
	{
		m_pShadow = res::LoadModel( m_VehicleData.modelFileShadow );
		DBG_ASSERT( m_pShadow != 0 );
	}

	// PROP
	std::memset( &m_PropData, 0, sizeof(Part) );
	if( vehicleData.propModel.partResId != -1 )
		m_PropData.partModel = res::LoadModel( vehicleData.propModel.partResId );
	m_PropData.initialPos = vehicleData.propModel.initialPos;
	m_PropData.currentPos = vehicleData.propModel.initialPos;

	// RUDDER
	std::memset( &m_RudderData, 0, sizeof(Part) );
	if( vehicleData.rudder.partResId != -1 )
		m_RudderData.partModel = res::LoadModel( vehicleData.rudder.partResId );
	m_RudderData.initialPos = vehicleData.rudder.initialPos;
	m_RudderData.currentPos = vehicleData.rudder.initialPos;
	m_RudderData.currentRot.setZero();
	m_RudderData.maxRot = vehicleData.rudder.maxRot;
	m_RudderData.rotSpeed = vehicleData.rudder.rotSpeed;

	// LEFT TAIL FLAP
	std::memset( &m_LeftTailFlapData, 0, sizeof(Part) );
	if( vehicleData.leftTailFlap.partResId != -1 )
		m_LeftTailFlapData.partModel = res::LoadModel( vehicleData.leftTailFlap.partResId );
	m_LeftTailFlapData.initialPos = vehicleData.leftTailFlap.initialPos;
	m_LeftTailFlapData.currentPos = vehicleData.leftTailFlap.initialPos;
	m_LeftTailFlapData.currentRot.setZero();
	m_LeftTailFlapData.maxRot = vehicleData.leftTailFlap.maxRot;
	m_LeftTailFlapData.rotSpeed = vehicleData.leftTailFlap.rotSpeed;

	// RIGHT TAIL FLAP
	std::memset( &m_RightTailFlapData, 0, sizeof(Part) );
	if( vehicleData.rightTailFlap.partResId != -1 )
		m_RightTailFlapData.partModel = res::LoadModel( vehicleData.rightTailFlap.partResId );
	m_RightTailFlapData.initialPos = vehicleData.rightTailFlap.initialPos;
	m_RightTailFlapData.currentPos = vehicleData.rightTailFlap.initialPos;
	m_RightTailFlapData.currentRot.setZero();
	m_RightTailFlapData.maxRot = vehicleData.rightTailFlap.maxRot;
	m_RightTailFlapData.rotSpeed = vehicleData.rightTailFlap.rotSpeed;

	// LEFT WING FLAP
	std::memset( &m_LeftWingFlapData, 0, sizeof(Part) );
	if( vehicleData.leftWingFlap.partResId != -1 )
		m_LeftWingFlapData.partModel = res::LoadModel( vehicleData.leftWingFlap.partResId );
	m_LeftWingFlapData.initialPos = vehicleData.leftWingFlap.initialPos;
	m_LeftWingFlapData.currentPos = vehicleData.leftWingFlap.initialPos;
	m_LeftWingFlapData.currentRot.setZero();
	m_LeftWingFlapData.maxRot = vehicleData.leftWingFlap.maxRot;
	m_LeftWingFlapData.rotSpeed = vehicleData.leftWingFlap.rotSpeed;

	// RIGHT WING FLAP
	std::memset( &m_RightWingFlapData, 0, sizeof(Part) );
	if( vehicleData.rightWingFlap.partResId != -1 )
		m_RightWingFlapData.partModel = res::LoadModel( vehicleData.rightWingFlap.partResId );
	m_RightWingFlapData.initialPos = vehicleData.rightWingFlap.initialPos;
	m_RightWingFlapData.currentPos = vehicleData.rightWingFlap.initialPos;
	m_RightWingFlapData.currentRot.setZero();
	m_RightWingFlapData.maxRot = vehicleData.rightWingFlap.maxRot;
	m_RightWingFlapData.rotSpeed = vehicleData.rightWingFlap.rotSpeed;

	// muzzle flash
	std::memset( &m_MuzzleFlashData, 0, sizeof(Part) );
	if( vehicleData.muzzleFlash.partResId != -1 )
		m_MuzzleFlashData.partModel = res::LoadModel( vehicleData.muzzleFlash.partResId );
	m_MuzzleFlashData.initialPos = vehicleData.muzzleFlash.initialPos;
	m_MuzzleFlashData.currentPos = vehicleData.muzzleFlash.initialPos;

	std::memset( &m_Exhaust1, 0, sizeof(Effect) );
	if( vehicleData.exhaust1.useEffect )
	{
		m_Exhaust1.resId = vehicleData.exhaust1.effectResId;
		m_Exhaust1.initialPos = vehicleData.exhaust1.initialPos;
		m_Exhaust1.currentPos = vehicleData.exhaust1.initialPos;
	}

	std::memset( &m_Exhaust2, 0, sizeof(Effect) );
	if( vehicleData.exhaust2.useEffect )
	{
		m_Exhaust2.resId = vehicleData.exhaust2.effectResId;
		m_Exhaust2.initialPos = vehicleData.exhaust2.initialPos;
		m_Exhaust2.currentPos = vehicleData.exhaust2.initialPos;
	}

	std::memset( &m_WingSlice1, 0, sizeof(Effect) );
	if( vehicleData.wingSlice1.useEffect )
	{
		m_WingSlice1.resId = vehicleData.wingSlice1.effectResId;
		m_WingSlice1.initialPos = vehicleData.wingSlice1.initialPos;
		m_WingSlice1.currentPos = vehicleData.wingSlice1.initialPos;
	}

	std::memset( &m_WingSlice2, 0, sizeof(Effect) );
	if( vehicleData.wingSlice2.useEffect )
	{
		m_WingSlice2.resId = vehicleData.wingSlice2.effectResId;
		m_WingSlice2.initialPos = vehicleData.wingSlice2.initialPos;
		m_WingSlice2.currentPos = vehicleData.wingSlice2.initialPos;
	}

	m_Accelerate = m_VehicleData.accelerate;
	m_Decelerate = m_VehicleData.decelerate;
	m_SpeedRange.maxVal = m_VehicleData.maxSpeed;
	m_SpeedRange.minVal = m_VehicleData.minSpeed;

	m_Speed = m_SpeedRange.minVal + (m_SpeedRange.maxVal-m_SpeedRange.minVal)*0.5f;
	m_TargetSpeed = m_Speed;

	// by default disable everything
	//SetMeshDrawState( -1, false );

	InitialiseEmitters();

	m_LightPos = math::Vec4( 0.0f, 1.0f, 0.0f, 0.0f );
	m_LightDiffuse = math::Vec4( 1.0f, 1.0f, 1.0f, 1.0f );
	m_LightAmbient = math::Vec4( 1.0f, 1.0f, 1.0f, 1.0f );
	m_LightSpecular = math::Vec4( 0.0f, 0.0f, 0.0f, 1.0f );

	m_LightPosTarget = math::Vec4( 0.0f, 1.0f, 0.0f, 0.0f );
	m_LightDiffuseTarget = math::Vec4( 1.0f, 1.0f, 1.0f, 1.0f );
	m_LightAmbientTarget = math::Vec4( 1.0f, 1.0f, 1.0f, 1.0f );
	m_LightSpecularTarget = math::Vec4( 0.0f, 0.0f, 0.0f, 1.0f );

	m_Pos.setZero();
	m_Dir = math::Vec3( 0.0f, 0.0f, 1.0f );
	m_UpVector = math::Vec3( 0.0f, 1.0f, 0.0f );

	m_Rot.setZero();
	m_RotSpeed.setZero();
	m_TargetRot.setZero();

	m_BoundSphere.vCenterPoint = m_Pos;
	m_BoundSphere.fRadius = 2.0f;
	m_DrawDistanceSphere.vCenterPoint = m_Pos;
	m_DrawDistanceSphere.fRadius = m_GameData.SPHERE_DRAW_DISTANCE_ON_PLANE;

	// init bullet list
	m_SpawnedBullet = false;
	m_MuzzleFlashTime = 0.0f;
	m_NextBulletIndex = 0;
	for( i = 0; i < PLANE_MAXNUM_BULLETS; i++ )
	{
		m_BulletList[i].index = i;

		m_BulletList[i].SetBaseId(PHYSICSBASICID_BULLET);
		m_BulletList[i].SetCastingId(PHYSICSCASTID_BULLET);

		m_BulletList[i].active = false;
		m_BulletList[i].stateChange = false;

		m_BulletList[i].vVel.X = 0.0f;
		m_BulletList[i].vVel.Y = 0.0f;
		m_BulletList[i].vVel.Z = 0.0f;
		m_BulletList[i].life = 0.0f;
		m_BulletList[i].distanceTravelled = 0.0f;
		m_BulletList[i].geomId = 0;

		m_BulletList[i].doEffect = false;

		m_BulletList[i].pEmitter = new efx::Emitter;
		DBG_ASSERT( m_BulletList[i].pEmitter != 0 );

		const res::EmitterResourceStore* er = res::GetEmitterResource( 2 );
		DBG_ASSERT( er != 0 );

		m_BulletList[i].pEmitter->Create( *er->block );
		res::SetupTexturesOnEmitter( m_BulletList[i].pEmitter );
		m_BulletList[i].pEmitter->Reset();
	}

	m_BulletModel = res::LoadModel( 140 );
	DBG_ASSERT( m_BulletModel != 0 );

	m_ShootTimer = 0.0f;

	m_ActiveGateGroup = 0;
	m_ActiveGateGroup = activeLevel.GetActiveGateGroup();

	if( m_ActiveGateGroup != 0 )
	{
		m_Pos = m_ActiveGateGroup->spawnPos;
		m_Rot = m_ActiveGateGroup->spawnRot;
		m_TargetRot = m_ActiveGateGroup->spawnRot;
		m_ActiveGateGroup->nextGateIndex = 0;

		for( i=0; i<m_ActiveGateGroup->numGates; ++i ) 
		{
			// enable all gates with correct index
			if( m_ActiveGateGroup->gateList[i]->number == m_ActiveGateGroup->nextGateIndex )
				m_ActiveGateGroup->gateList[i]->enabled = true;
		}
	}

	// ray 
	m_pRayTest = GameSystems::GetInstance()->GetRayObject();
	DBG_ASSERT( m_pRayTest != 0 );

	m_CategoryBits = CATEGORY_PLAYER;
	m_CollideBits = ( CATEGORY_WORLD | CATEGORY_WORLDOBJECT );
	m_WorldCollided = false;

	m_IsDead = false;

	// audio
	m_AudioStarted = false;
	m_EngineBufferId = AudioSystem::GetInstance()->AddAudioFile( 1/*engine.wav*/ );
	m_EngineSrcId = snd::INVALID_SOUNDSOURCE;

	m_GunBufferId = AudioSystem::GetInstance()->AddAudioFile( 2/*gun.wav*/ );
	m_GunSrcId = snd::INVALID_SOUNDSOURCE; 

	bulletHit = AudioSystem::GetInstance()->AddAudioFile( 3/*shot_hit.wav*/ );
	bulletMiss = AudioSystem::GetInstance()->AddAudioFile( 4/*shot_bounce.wav*/ );

	m_PlayerCrashBufferId = AudioSystem::GetInstance()->AddAudioFile( 5/*crash.wav*/ );

	m_RingPitch = RING_PITCH_START;

	m_FlyingLowTime = 0.0f;
}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
Player::~Player()
{
	unsigned int i=0, j=0;

	StopAllAudio();

	for( i = 0; i < PLANE_MAXNUM_BULLETS; i++ )
	{
		if( m_BulletList[i].geomId != 0 )
		{
			//dGeomDestroy( m_BulletList[i].geomId );
			m_BulletList[i].geomId = 0;

			if( m_BulletList[i].pEmitter != 0 )
				delete m_BulletList[i].pEmitter;
		}
	}

	if( m_PhysicsData != 0 )
	{
		for( i=0; i < m_PhysicsData->totalPhysicsTriMesh; ++i )	
		{
			dGeomTriMeshDataDestroy( m_PhysicsData->triMeshList[i].triMeshDataID );
			m_PhysicsData->triMeshList[i].triMeshDataID = 0;
		}
	
		if( m_PhysicsData->boxList )
			delete[] m_PhysicsData->boxList;

		if( m_PhysicsData->sphereList )
			delete[] m_PhysicsData->sphereList;

		if( m_PhysicsData->capsuleList )
			delete[] m_PhysicsData->capsuleList;

		if( m_PhysicsData->cylinderList )
			delete[] m_PhysicsData->cylinderList;

		for( j=0; j < m_PhysicsData->totalPhysicsTriMesh; ++j )
		{
			if( m_PhysicsData->triMeshList[j].triMeshDataID != 0 )
			{
				dGeomTriMeshDataDestroy( m_PhysicsData->triMeshList[j].triMeshDataID );
				m_PhysicsData->triMeshList[j].triMeshDataID = 0;
			}

			if( m_PhysicsData->triMeshList[j].triangleData != 0 )
				delete[] m_PhysicsData->triMeshList[j].triangleData;

			if( m_PhysicsData->triMeshList[j].indices != 0 )
				delete[] m_PhysicsData->triMeshList[j].indices;

			if( m_PhysicsData->triMeshList[j].normals != 0 )
				delete[] m_PhysicsData->triMeshList[j].normals;	
		}
		if( m_PhysicsData->triMeshList )
			delete[] m_PhysicsData->triMeshList;

		if( m_PhysicsData )
		{
			delete m_PhysicsData;
			m_PhysicsData = 0;
		}
	}

	if( m_BulletModel != 0 )
	{
		res::RemoveModel( m_BulletModel );
		m_BulletModel = 0;
	}

	if( m_PropData.partModel != 0 )
	{
		res::RemoveModel( m_PropData.partModel );
		m_PropData.partModel = 0;
	}

	if( m_RudderData.partModel != 0 )
	{
		res::RemoveModel( m_RudderData.partModel );
		m_RudderData.partModel = 0;
	}

	if( m_LeftTailFlapData.partModel != 0 )
	{
		res::RemoveModel( m_LeftTailFlapData.partModel );
		m_LeftTailFlapData.partModel = 0;
	}

	if( m_RightTailFlapData.partModel != 0 )
	{
		res::RemoveModel( m_RightTailFlapData.partModel );
		m_RightTailFlapData.partModel = 0;
	}

	if( m_LeftWingFlapData.partModel != 0 )
	{
		res::RemoveModel( m_LeftWingFlapData.partModel );
		m_LeftWingFlapData.partModel = 0;
	}

	if( m_RightWingFlapData.partModel != 0 )
	{
		res::RemoveModel( m_RightWingFlapData.partModel );
		m_RightWingFlapData.partModel = 0;
	}

	if( m_MuzzleFlashData.partModel != 0 )
	{
		res::RemoveModel( m_MuzzleFlashData.partModel );
		m_MuzzleFlashData.partModel = 0;
	}

	if( m_Exhaust1.pEmitter != 0 )
	{
		delete m_Exhaust1.pEmitter;
	}

	if( m_Exhaust2.pEmitter != 0 )
	{
		delete m_Exhaust2.pEmitter;
	}

	if( m_WingSlice1.pEmitter != 0 )
	{
		delete m_WingSlice1.pEmitter;
	}

	if( m_WingSlice2.pEmitter != 0 )
	{
		delete m_WingSlice2.pEmitter;
	}

	if( m_pShadow != 0 )
	{
		res::RemoveModel( m_pShadow );
		m_pShadow = 0;
	}

	if( m_pModelData != 0 )
	{
		res::RemoveModel( m_pModelData );
		m_pModelData = 0;
	}

	m_pRayTest = 0;

}

/////////////////////////////////////////////////////
/// Method: Setup
/// Params: None
///
/////////////////////////////////////////////////////
void Player::Setup()
{
	int i=0;

	dSpaceID physicsSpaceId = 0;
	physicsSpaceId = physics::PhysicsWorldODE::GetSpace();

	m_EntitySpace = physicsSpaceId;

	CreatePhysics( true );

	for( i = 0; i < PLANE_MAXNUM_BULLETS; i++ )
	{
		m_BulletList[i].geomId = dCreateSphere( m_EntitySpace, m_GameData.BULLET_SIZE );

		dGeomSetCategoryBits( m_BulletList[i].geomId, CATEGORY_BULLET );
		dGeomSetCollideBits( m_BulletList[i].geomId, CATEGORY_NOTHING );

		dGeomSetData( m_BulletList[i].geomId, &m_BulletList[i] );
		dGeomDisable( m_BulletList[i].geomId );
	}
}

/////////////////////////////////////////////////////
/// Method: Draw
/// Params: None
///
/////////////////////////////////////////////////////
void Player::Draw()
{
	renderer::OpenGL::GetInstance()->BlendMode( false, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	if( !m_IsDead )
	{

		m[0] = m_Orientation[0];
		m[1] = m_Orientation[4];
		m[2] = m_Orientation[8];
		m[3] = 0.0f;
		m[4] = m_Orientation[1];
		m[5] = m_Orientation[5];
		m[6] = m_Orientation[9];
		m[7] = 0.0f;
		m[8] = m_Orientation[2];
		m[9] = m_Orientation[6];
		m[10] = m_Orientation[10];
		m[11] = 0.0f;
		m[12] = m_Pos[0];
		m[13] = m_Pos[1];
		m[14] = m_Pos[2];
		m[15] = 1.0f;

		glPushMatrix();
			glMultMatrixf(m);

			if( m_SpawnedBullet )
			{
				math::Vec4 pos( m_MuzzleFlashData.initialPos.X, m_MuzzleFlashData.initialPos.Y+3.0f, m_MuzzleFlashData.initialPos.Z, 1.0f );
				math::Vec4 diff( 1.0f, 1.0f, 0.0f, 1.0f );
				math::Vec4 amb( 1.0f, 1.0f, 0.0f, 1.0f );

				renderer::OpenGL::GetInstance()->SetLightPosition( 1, pos );
				renderer::OpenGL::GetInstance()->SetLightDiffuse( 1, diff );
				renderer::OpenGL::GetInstance()->SetLightAmbient( 1, amb );

				renderer::OpenGL::GetInstance()->SetLightAttenuation( 1, GL_CONSTANT_ATTENUATION, 6.0f );
				renderer::OpenGL::GetInstance()->SetLightAttenuation( 1, GL_LINEAR_ATTENUATION, 0.0f );
				renderer::OpenGL::GetInstance()->SetLightAttenuation( 1, GL_QUADRATIC_ATTENUATION, 0.0f );
				renderer::OpenGL::GetInstance()->EnableLight(1);
			}
			else
				renderer::OpenGL::GetInstance()->DisableLight(1);

			m_pModelData->Draw();

			if( m_PropData.partModel != 0 )
			{
				glPushMatrix();
					glTranslatef( m_PropData.initialPos.X, m_PropData.initialPos.Y, m_PropData.initialPos.Z );
					glRotatef( m_PropData.currentRot.Z, 0.0f, 0.0f, 1.0f );
					m_PropData.partModel->Draw();
				glPopMatrix();
			}

			if( m_RudderData.partModel != 0 )
			{
				glPushMatrix();
					glTranslatef( m_RudderData.initialPos.X, m_RudderData.initialPos.Y, m_RudderData.initialPos.Z );
					glRotatef( m_RudderData.currentRot.Y, 0.0f, 1.0f, 0.0f );
					m_RudderData.partModel->Draw();
				glPopMatrix();
			}

			if( m_LeftTailFlapData.partModel != 0 )
			{
				glPushMatrix();
					glTranslatef( m_LeftTailFlapData.initialPos.X, m_LeftTailFlapData.initialPos.Y, m_LeftTailFlapData.initialPos.Z );
					glRotatef( m_LeftTailFlapData.currentRot.X, 1.0f, 0.0f, 0.0f );
					m_LeftTailFlapData.partModel->Draw();
				glPopMatrix();
			}

			if( m_RightTailFlapData.partModel != 0 )
			{
				glPushMatrix();
					glTranslatef( m_RightTailFlapData.initialPos.X, m_RightTailFlapData.initialPos.Y, m_RightTailFlapData.initialPos.Z );
					glRotatef( m_RightTailFlapData.currentRot.X, 1.0f, 0.0f, 0.0f );
					m_RightTailFlapData.partModel->Draw();
				glPopMatrix();
			}

			if( m_LeftWingFlapData.partModel != 0 )
			{
				glPushMatrix();
					glTranslatef( m_LeftWingFlapData.initialPos.X, m_LeftWingFlapData.initialPos.Y, m_LeftWingFlapData.initialPos.Z );
					glRotatef( m_LeftWingFlapData.currentRot.X, 1.0f, 0.0f, 0.0f );
					m_LeftWingFlapData.partModel->Draw();
				glPopMatrix();
			}

			if( m_RightWingFlapData.partModel != 0 )
			{
				glPushMatrix();
					glTranslatef( m_RightWingFlapData.initialPos.X, m_RightWingFlapData.initialPos.Y, m_RightWingFlapData.initialPos.Z );
					glRotatef( m_RightWingFlapData.currentRot.X, 1.0f, 0.0f, 0.0f );
					m_RightWingFlapData.partModel->Draw();
				glPopMatrix();
			}

		glPopMatrix();
	}

	renderer::OpenGL::GetInstance()->DisableLight(1);

	renderer::OpenGL::GetInstance()->BlendMode( true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	DrawBullets();

	/*GLfloat *mdl = renderer::OpenGL::GetInstance()->GetModelViewMatrix();
	GLfloat *proj = renderer::OpenGL::GetInstance()->GetProjectionMatrix();
	GLint *viewport = renderer::OpenGL::GetInstance()->GetViewport();

	gluUtil::gluProjectf( m_Pos.X, m_Pos.Y, m_Pos.Z,
				mdl, proj, viewport,
				&m_UnProjectedPoint.X, &m_UnProjectedPoint.Y, &m_UnProjectedPoint.Z );*/
}

/////////////////////////////////////////////////////
/// Method: DrawAlpha
/// 
///
/////////////////////////////////////////////////////
void Player::DrawAlpha()
{			
	m[0] = m_Orientation[0];
	m[1] = m_Orientation[4];
	m[2] = m_Orientation[8];
	m[3] = 0.0f;
	m[4] = m_Orientation[1];
	m[5] = m_Orientation[5];
	m[6] = m_Orientation[9];
	m[7] = 0.0f;
	m[8] = m_Orientation[2];
	m[9] = m_Orientation[6];
	m[10] = m_Orientation[10];
	m[11] = 0.0f;
	m[12] = m_Pos[0];
	m[13] = m_Pos[1];
	m[14] = m_Pos[2];
	m[15] = 1.0f;

	bool lightingState = renderer::OpenGL::GetInstance()->GetLightingState();
	if( lightingState )
		renderer::OpenGL::GetInstance()->DisableLighting();

	renderer::OpenGL::GetInstance()->SetColour4ub( 255,255,255,255 );

	renderer::OpenGL::GetInstance()->AlphaMode( true, GL_GREATER, 0.3f );
	renderer::OpenGL::GetInstance()->BlendMode( true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	if( !m_IsDead &&
		m_SpawnedBullet )
	{	
		glPushMatrix();
			glMultMatrixf(m);

			if( m_MuzzleFlashData.partModel != 0 )
			{
				glPushMatrix();
					glTranslatef( m_MuzzleFlashData.initialPos.X, m_MuzzleFlashData.initialPos.Y, m_MuzzleFlashData.initialPos.Z );
					m_MuzzleFlashData.partModel->Draw();
				glPopMatrix();
			}
		glPopMatrix();
	}
	renderer::OpenGL::GetInstance()->AlphaMode( false, GL_ALWAYS, 0.0f );

	DrawEmitters();

	if( !m_IsDead &&
		m_ShadowDraw &&
		m_pShadow != 0 )
	{
		renderer::OpenGL::GetInstance()->DepthMode( true, GL_LEQUAL );

		m[0] = m_ShadowOri[0];
		m[1] = m_ShadowOri[4];
		m[2] = m_ShadowOri[8];
		m[3] = 0.0f;
		m[4] = m_ShadowOri[1];
		m[5] = m_ShadowOri[5];
		m[6] = m_ShadowOri[9];
		m[7] = 0.0f;
		m[8] = m_ShadowOri[2];
		m[9] = m_ShadowOri[6];
		m[10] = m_ShadowOri[10];
		m[11] = 0.0f;
		m[12] = m_ShadowPos[0];
		m[13] = m_ShadowPos[1];
		m[14] = m_ShadowPos[2];
		m[15] = 1.0f;

		glPushMatrix();	
			glMultMatrixf(m);
			glRotatef( m_Rot.Y, 0.0f, 1.0f, 0.0f );
			m_pShadow->Draw();
		glPopMatrix();

		renderer::OpenGL::GetInstance()->DepthMode( true, GL_LESS );
	}

	renderer::OpenGL::GetInstance()->AlphaMode( false, GL_ALWAYS, 0.0f );
	renderer::OpenGL::GetInstance()->BlendMode( false, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	if( lightingState )
		renderer::OpenGL::GetInstance()->EnableLighting();
	
}

/////////////////////////////////////////////////////
/// Method: DrawBullets
/// 
///
/////////////////////////////////////////////////////
void Player::DrawBullets( void )
{
	int i = 0;
	bool lightState = renderer::OpenGL::GetInstance()->GetLightingState();
	
	if( lightState )
		renderer::OpenGL::GetInstance()->DisableLighting();
	
	// draw active bullets
	for( i = 0; i < PLANE_MAXNUM_BULLETS; i++ )
	{
		if( m_BulletList[i].pEmitter != 0 )
			m_BulletList[i].pEmitter->Draw( );

		if( m_BulletList[i].active )
		{
			if( m_BulletModel != 0 )
			{
				glPushMatrix();
					glTranslatef( m_BulletList[i].vPos.X, m_BulletList[i].vPos.Y, m_BulletList[i].vPos.Z );
					glRotatef( m_BulletList[i].vRot.Y, 0.0f, 1.0f, 0.0f );
					glRotatef( m_BulletList[i].vRot.X, 1.0f, 0.0f, 0.0f );
					glRotatef( m_BulletList[i].vRot.Z, 0.0f, 0.0f, 1.0f );
					m_BulletModel->Draw();
				glPopMatrix();
			}
		}
	}

	if( lightState )
		renderer::OpenGL::GetInstance()->EnableLighting();
}

/////////////////////////////////////////////////////
/// Method: DrawDebug
/// Params: None
///
/////////////////////////////////////////////////////
void Player::DrawDebug()
{
	unsigned int i=0;
	const dReal* pos = 0;
	const dReal* rot = 0;

	bool lightState = renderer::OpenGL::GetInstance()->GetLightingState();
	bool textureState = renderer::OpenGL::GetInstance()->GetTextureState();
	
	if( lightState )
		renderer::OpenGL::GetInstance()->DisableLighting();
	if( textureState )
		renderer::OpenGL::GetInstance()->DisableTexturing();

	renderer::OpenGL::GetInstance()->SetColour4ub( 255,255,255,255 );
	
	for( i=0; i < m_PhysicsData->totalPhysicsBox; ++i )
	{
		pos = dGeomGetPosition( m_PhysicsData->boxList[i].geomId );
		rot = dGeomGetRotation( m_PhysicsData->boxList[i].geomId );

		physics::DrawPhysicsGeometry( m_PhysicsData->boxList[i].geomId, pos, rot );
	}

	// Sphere Geoms
	for( i=0; i < m_PhysicsData->totalPhysicsSphere; ++i )
	{
		m_PhysicsData->sphereList[i].geomId = CreateGeom( dSphereClass, m_EntitySpace, zeroVec, m_PhysicsData->sphereList[i].radius );
		pos = dGeomGetPosition( m_PhysicsData->sphereList[i].geomId );
		rot = dGeomGetRotation( m_PhysicsData->sphereList[i].geomId );

		physics::DrawPhysicsGeometry( m_PhysicsData->sphereList[i].geomId, pos, rot );
	}

	// Capsule Geoms
	for( i=0; i < m_PhysicsData->totalPhysicsCapsule; ++i )
	{
		pos = dGeomGetPosition( m_PhysicsData->capsuleList[i].geomId );
		rot = dGeomGetRotation( m_PhysicsData->capsuleList[i].geomId );

		physics::DrawPhysicsGeometry( m_PhysicsData->capsuleList[i].geomId, pos, rot );
	}

	// Cylinder Geoms
	for( i=0; i < m_PhysicsData->totalPhysicsCylinder; ++i )
	{
		pos = dGeomGetPosition( m_PhysicsData->cylinderList[i].geomId );
		rot = dGeomGetRotation( m_PhysicsData->cylinderList[i].geomId );

		physics::DrawPhysicsGeometry( m_PhysicsData->cylinderList[i].geomId, pos, rot );
	}

	/*glLineWidth( 3.0f );
	renderer::OpenGL::GetInstance()->SetColour4ub( 255, 255, 0, 255 );
		glBegin(GL_LINES);
			glVertex3f( m_Pos.X, m_Pos.Y, m_Pos.Z );
			glVertex3f( m_Pos.X+(m_FinalDir.X*50.0f), m_Pos.Y+(m_FinalDir.Y*50.0f), m_Pos.Z+(m_FinalDir.Z*50.0f) );
		glEnd();
	glLineWidth( 1.0f );*/

	if( m_pRayTest->totalHits > 0 )
	{
		renderer::OpenGL::GetInstance()->SetColour4ub( 255, 255, 0, 255 );
		for( i=0; i < static_cast<unsigned int>(m_pRayTest->totalHits); ++i )
		{
			glPushMatrix();
				glTranslatef(m_pRayTest->rayHits[i].pos.X, m_pRayTest->rayHits[i].pos.Y, m_pRayTest->rayHits[i].pos.Z );
				renderer::DrawSphere( 5.0f );
			glPopMatrix();
		}
	}

	renderer::OpenGL::GetInstance()->SetColour4ub(255, 255, 255, 255 );
	for( i = 0; i < PLANE_MAXNUM_BULLETS; i++ )
	{
		if( m_BulletList[i].active )
		{
			const dReal* pos = dGeomGetPosition( m_BulletList[i].geomId  );
			dMatrix3 mat;
			dRSetIdentity( mat );

			physics::DrawPhysicsGeometry( m_BulletList[i].geomId, pos, mat );
		}
	}

	renderer::OpenGL::GetInstance()->SetColour4ub( 255,255,255,255 );
	glPushMatrix();
		glTranslatef( m_BoundSphere.vCenterPoint.X, m_BoundSphere.vCenterPoint.Y, m_BoundSphere.vCenterPoint.Z );
		renderer::DrawSphere( m_BoundSphere.fRadius );
	glPopMatrix();

	if( lightState )
		renderer::OpenGL::GetInstance()->EnableLighting();
	if( textureState )
		renderer::OpenGL::GetInstance()->EnableTexturing();	
}

/////////////////////////////////////////////////////
/// Method: Update
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
void Player::Update( float deltaTime )
{
	static bool firstUpdate = true;
	m_LastDelta = deltaTime;

	// ray check for shadow
	m_pRayTest->hasHits = false;
	m_pRayTest->totalHits = 0;
	m_pRayTest->nextHitIndex = 0;

	m_pRayTest->rayPos = m_Pos;
	m_pRayTest->rayDir = math::Vec3( 0.0f, -1.0f, 0.0f );

	dGeomRaySet (m_pRayTest->rayGeom, m_pRayTest->rayPos.X, m_pRayTest->rayPos.Y, m_pRayTest->rayPos.Z, 
			m_pRayTest->rayDir.X, m_pRayTest->rayDir.Y, m_pRayTest->rayDir.Z );

	dGeomRaySetLength (m_pRayTest->rayGeom, 500.0f);

	dGeomSetCategoryBits( m_pRayTest->rayGeom, CATEGORY_RAYCAST );
	dGeomSetCollideBits( m_pRayTest->rayGeom, CATEGORY_WORLD );

	// check against world
	if( !firstUpdate )
		dSpaceCollide(physics::PhysicsWorldODE::GetSpace(), 0, &PhysicsTestPlayerToWorld);

	if( !m_WorldCollided && !m_IsDead)
	{
		math::Vec2 analogueVal = m_InputSystem.GetAnalogueValues();

		m_FinalDir = m_Dir;
		m_FinalUp = m_UpVector;

#if defined(BASE_PLATFORM_WINDOWS) || defined(BASE_PLATFORM_tvOS)
		bool accelState = m_InputSystem.GetAccel();
		bool decelState = m_InputSystem.GetDecel();

		if( accelState )
		{
			m_TargetSpeed += m_Accelerate*deltaTime;
			if( m_TargetSpeed > m_VehicleData.maxSpeed )
				m_TargetSpeed = m_VehicleData.maxSpeed;
		}
		else
		if( decelState )
		{
			m_TargetSpeed -= m_Decelerate*deltaTime;
			if( m_TargetSpeed < m_VehicleData.minSpeed )
				m_TargetSpeed = m_VehicleData.minSpeed;
		}
#endif // BASE_PLATFORM_WINDOWS

		if( math::WithinTolerance( m_Speed, m_TargetSpeed, 0.5f ) )
		{
			m_Speed = m_TargetSpeed;
		}
		else
		if( m_Speed < m_TargetSpeed )
		{
			m_Speed += m_Accelerate*deltaTime;
			if( m_Speed > m_VehicleData.maxSpeed )
				m_Speed = m_VehicleData.maxSpeed;
		}
		else
		if( m_Speed > m_TargetSpeed )
		{
			m_Speed -= m_Decelerate*deltaTime;
			if( m_Speed < m_VehicleData.minSpeed )
				m_Speed = m_VehicleData.minSpeed;
		}
				
		float range = m_SpeedRange.maxVal - m_SpeedRange.minVal;
		float val = m_Speed - m_SpeedRange.minVal;
		float percent = (val / range); // 0.0-1.0

		if( percent < MIN_PROP_SPEED )
			percent = MIN_PROP_SPEED;

		m_PropellorSpeed = MAX_PROP_SPEED * percent;

		if( m_EngineSrcId != snd::INVALID_SOUNDSOURCE )
		{
			float pitch = percent + 0.35f;

			if( pitch < 0.65f )
				pitch = 0.65f;
			if( pitch > 1.1f )
				pitch = 1.1f;

			alSourcef( m_EngineSrcId, AL_PITCH, pitch );
		}

		m_PropData.currentRot.Z += m_PropellorSpeed*deltaTime;

		if( m_PropData.currentRot.Z > 360.0f )
			m_PropData.currentRot.Z += -360.0f;
		
		if( analogueVal.Y != 0.0f )
		{
			m_TargetRot.X += (analogueVal.Y*m_VehicleData.rotSpeedX)*deltaTime;

			m_LeftTailFlapData.currentRot.X += (-analogueVal.Y*m_LeftTailFlapData.rotSpeed.X)*deltaTime;
			m_RightTailFlapData.currentRot.X += (-analogueVal.Y*m_RightTailFlapData.rotSpeed.X)*deltaTime;
		}
		//else if( analogueVal.Y > 0.0f  )
		//{
		//	m_TargetRot.X += m_VehicleData.rotSpeedX*deltaTime;
		//
		//	m_LeftTailFlapData.currentRot.X += m_LeftTailFlapData.rotSpeed.X*deltaTime;
		//	m_RightTailFlapData.currentRot.X += m_RightTailFlapData.rotSpeed.X*deltaTime;
		//}
		if( analogueVal.Y == 0.0f )
		//else
		{
			// LEFT TAIL FLAP
			if( math::WithinTolerance( m_LeftTailFlapData.currentRot.X, m_GameData.FLAP_RESET_AUTO_SNAP ) )
			{
				m_LeftTailFlapData.currentRot.X = 0.0f;
			}
			else
			{
				if( m_LeftTailFlapData.currentRot.X > 0.0f )
				{
					if( m_LeftTailFlapData.currentRot.X > 2.0f )
						m_LeftTailFlapData.currentRot.X -= (m_LeftTailFlapData.rotSpeed.X*2.0f)*deltaTime;
					else
						m_LeftTailFlapData.currentRot.X -= (m_LeftTailFlapData.rotSpeed.X)*deltaTime;
				}
				else if( m_LeftTailFlapData.currentRot.X < 0.0f  )
				{
					if( m_LeftTailFlapData.currentRot.X < -2.0f )
						m_LeftTailFlapData.currentRot.X += (m_LeftTailFlapData.rotSpeed.X*2.0f)*deltaTime;
					else
						m_LeftTailFlapData.currentRot.X += (m_LeftTailFlapData.rotSpeed.X)*deltaTime;
				}
			}

			// RIGHT TAIL FLAP
			if( math::WithinTolerance( m_RightTailFlapData.currentRot.X, m_GameData.FLAP_RESET_AUTO_SNAP ) )
			{
				m_RightTailFlapData.currentRot.X = 0.0f;
			}
			else
			{
				if( m_RightTailFlapData.currentRot.X > 0.0f )
				{
					if( m_RightTailFlapData.currentRot.X > 2.0f )
						m_RightTailFlapData.currentRot.X -= (m_RightTailFlapData.rotSpeed.X*2.0f)*deltaTime;
					else
						m_RightTailFlapData.currentRot.X -= (m_RightTailFlapData.rotSpeed.X)*deltaTime;
				}
				else if( m_RightTailFlapData.currentRot.X < 0.0f  )
				{
					if( m_RightTailFlapData.currentRot.X < -2.0f )
						m_RightTailFlapData.currentRot.X += (m_RightTailFlapData.rotSpeed.X*2.0f)*deltaTime;
					else
						m_RightTailFlapData.currentRot.X += (m_RightTailFlapData.rotSpeed.X)*deltaTime;
				}
			}
		}

		if( analogueVal.X != 0.0f )
		{
			m_TargetRot.Y += (-analogueVal.X*m_VehicleData.rotSpeedY)*deltaTime;
			m_TargetRot.Z += (analogueVal.X*m_VehicleData.rotSpeedZ)*deltaTime;

			m_RudderData.currentRot.Y += (analogueVal.X*m_RudderData.rotSpeed.Y)*deltaTime;

			m_LeftWingFlapData.currentRot.X += (-analogueVal.X*m_LeftWingFlapData.rotSpeed.X)*deltaTime;
			m_RightWingFlapData.currentRot.X += (analogueVal.X*m_RightWingFlapData.rotSpeed.X)*deltaTime;

		}
		//else if( analogueVal.X > 0.0f )
		//{
		//	m_TargetRot.Y -= m_VehicleData.rotSpeedY*deltaTime;
		//	m_TargetRot.Z += m_VehicleData.rotSpeedZ*deltaTime;

		//	m_RudderData.currentRot.Y += m_RudderData.rotSpeed.Y*deltaTime;

		//	m_LeftWingFlapData.currentRot.X -= m_LeftWingFlapData.rotSpeed.X*deltaTime;
		//	m_RightWingFlapData.currentRot.X += m_RightWingFlapData.rotSpeed.X*deltaTime;
		//}
		if( analogueVal.X == 0.0f )
		{
			if( math::WithinTolerance( m_TargetRot.Z, m_GameData.FLAP_RESET_AUTO_SNAP ) )
			{
				m_TargetRot.Z = 0.0f;
			}
			else
			{
				if( m_TargetRot.Z > 0.0f )
				{
					if( m_TargetRot.Z > m_GameData.FLAP_RESET_AUTO_SNAP*1.5f  )
						m_TargetRot.Z -= (m_VehicleData.rotSpeedZ*1.5f)*deltaTime;
					else
						m_TargetRot.Z -= (m_VehicleData.rotSpeedZ)*deltaTime;
					//m_TargetRot.Y -= (m_VehicleData.rotSpeedY*0.5f)*deltaTime;
				}
				else if( m_TargetRot.Z < 0.0f  )
				{
					if( m_TargetRot.Z < -m_GameData.FLAP_RESET_AUTO_SNAP*1.5f  )
						m_TargetRot.Z += (m_VehicleData.rotSpeedZ*1.5f)*deltaTime;
					else
						m_TargetRot.Z += (m_VehicleData.rotSpeedZ)*deltaTime;
					//m_TargetRot.Y += (m_VehicleData.rotSpeedY*0.5f)*deltaTime;
				}
			}

			// RUDDER
			if( math::WithinTolerance( m_RudderData.currentRot.Y, m_GameData.FLAP_RESET_AUTO_SNAP ) )
			{
				m_RudderData.currentRot.Y = 0.0f;
			}
			else
			{
				if( m_RudderData.currentRot.Y > 0.0f )
				{
					if( m_RudderData.currentRot.Y > m_GameData.FLAP_RESET_AUTO_SNAP*2.0f  )
						m_RudderData.currentRot.Y -= (m_RudderData.rotSpeed.Y*2.0f)*deltaTime;
					else
						m_RudderData.currentRot.Y -= (m_RudderData.rotSpeed.Y)*deltaTime;
				}
				else if( m_RudderData.currentRot.Y < 0.0f  )
				{
					if( m_RudderData.currentRot.Y < -m_GameData.FLAP_RESET_AUTO_SNAP*2.0f  )
						m_RudderData.currentRot.Y += (m_RudderData.rotSpeed.Y*2.0f)*deltaTime;
					else
						m_RudderData.currentRot.Y += (m_RudderData.rotSpeed.Y)*deltaTime;
				}
			}

			// LEFT WING FLAP
			if( math::WithinTolerance( m_LeftWingFlapData.currentRot.X, m_GameData.FLAP_RESET_AUTO_SNAP ) )
			{
				m_LeftWingFlapData.currentRot.X = 0.0f;
			}
			else
			{
				if( m_LeftWingFlapData.currentRot.X > 0.0f )
				{
					if( m_LeftWingFlapData.currentRot.X > m_GameData.FLAP_RESET_AUTO_SNAP*1.5f )
						m_LeftWingFlapData.currentRot.X -= (m_LeftWingFlapData.rotSpeed.X*1.2f)*deltaTime;
					else
						m_LeftWingFlapData.currentRot.X -= (m_LeftWingFlapData.rotSpeed.X)*deltaTime;
				}
				else if( m_LeftWingFlapData.currentRot.X < 0.0f  )
				{
					if( m_LeftWingFlapData.currentRot.X < -m_GameData.FLAP_RESET_AUTO_SNAP*1.5f  )
						m_LeftWingFlapData.currentRot.X += (m_LeftWingFlapData.rotSpeed.X*1.2f)*deltaTime;
					else
						m_LeftWingFlapData.currentRot.X += (m_LeftWingFlapData.rotSpeed.X)*deltaTime;
				}
			}

			// RIGHT WING FLAP
			if( math::WithinTolerance( m_RightWingFlapData.currentRot.X, m_GameData.FLAP_RESET_AUTO_SNAP ) )
			{
				m_RightWingFlapData.currentRot.X = 0.0f;
			}
			else
			{
				if( m_RightWingFlapData.currentRot.X > 0.0f )
				{
					if( m_RightWingFlapData.currentRot.X > m_GameData.FLAP_RESET_AUTO_SNAP*1.5f )
						m_RightWingFlapData.currentRot.X -= (m_RightWingFlapData.rotSpeed.X*1.2f)*deltaTime;
					else
						m_RightWingFlapData.currentRot.X -= (m_RightWingFlapData.rotSpeed.X)*deltaTime;
				}
				else if( m_RightWingFlapData.currentRot.X < 0.0f  )
				{
					if( m_RightWingFlapData.currentRot.X < -m_GameData.FLAP_RESET_AUTO_SNAP*1.5f  )
						m_RightWingFlapData.currentRot.X += (m_RightWingFlapData.rotSpeed.X*1.2f)*deltaTime;
					else
						m_RightWingFlapData.currentRot.X += (m_RightWingFlapData.rotSpeed.X)*deltaTime;
				}
			}
		}
	
		if( m_TargetRot.X > m_VehicleData.maxRotXAngle )
			m_TargetRot.X = m_VehicleData.maxRotXAngle;
		else if( m_TargetRot.X < -m_VehicleData.maxRotXAngle )
			m_TargetRot.X = -m_VehicleData.maxRotXAngle;

		if( m_TargetRot.Z > m_VehicleData.maxRotZAngle )
			m_TargetRot.Z = m_VehicleData.maxRotZAngle;
		else if( m_TargetRot.Z < -m_VehicleData.maxRotZAngle )
			m_TargetRot.Z = -m_VehicleData.maxRotZAngle;

		const float LERP_SPEED = 2.0f;
		m_Rot.X = math::Lerp( m_Rot.X, m_TargetRot.X, LERP_SPEED*deltaTime );
		m_Rot.Y = math::Lerp( m_Rot.Y, m_TargetRot.Y, LERP_SPEED*deltaTime );
		m_Rot.Z = math::Lerp( m_Rot.Z, m_TargetRot.Z, LERP_SPEED*deltaTime );

		dQuaternion qRotX, qRotY,qRotZ;
		dQuaternion qTmp;

		dRSetIdentity( m_Orientation );
		dQFromAxisAndAngle( qRotY, 0.0f, 1.0f, 0.0f, math::DegToRad(m_Rot.Y) );
		dQFromAxisAndAngle( qRotX, 1.0f, 0.0f, 0.0f, math::DegToRad(m_Rot.X) );
		dQFromAxisAndAngle( qRotZ, 0.0f, 0.0f, 1.0f, math::DegToRad(m_Rot.Z) );
	
		dQMultiply0( qTmp, qRotX, qRotZ );
		dQMultiply0( m_qPlaneOri, qRotY, qTmp );
		dRfromQ( m_Orientation, m_qPlaneOri );


		m_FinalDir.X = (m_Dir.X * m_Orientation[0]) + (m_Dir.Y * m_Orientation[1]) + (m_Dir.Z * m_Orientation[2]);
		m_FinalDir.Y = (m_Dir.X * m_Orientation[4]) + (m_Dir.Y * m_Orientation[5]) + (m_Dir.Z * m_Orientation[6]);
		m_FinalDir.Z = (m_Dir.X * m_Orientation[8]) + (m_Dir.Y * m_Orientation[9]) + (m_Dir.Z * m_Orientation[10]);

		m_FinalDir.normalise();

		m_FinalUp.X = (m_UpVector.X * m_Orientation[0]) + (m_UpVector.Y * m_Orientation[1]) + (m_UpVector.Z * m_Orientation[2]);
		m_FinalUp.Y = (m_UpVector.X * m_Orientation[4]) + (m_UpVector.Y * m_Orientation[5]) + (m_UpVector.Z * m_Orientation[6]);
		m_FinalUp.Z = (m_UpVector.X * m_Orientation[8]) + (m_UpVector.Y * m_Orientation[9]) + (m_UpVector.Z * m_Orientation[10]);

		m_FinalUp.normalise();
	}

	// do the movement
	m_Pos += (m_FinalDir*m_Speed)*deltaTime;

	if( m_Pos.X < m_GameData.MIN_BOUNDING_X )
		m_Pos.X = m_GameData.MIN_BOUNDING_X;
	if( m_Pos.X > m_GameData.MAX_BOUNDING_X )
		m_Pos.X = m_GameData.MAX_BOUNDING_X;

	if( m_Pos.Y > m_GameData.MAX_BOUNDING_Y )
		m_Pos.Y = m_GameData.MAX_BOUNDING_Y;

	if( m_Pos.Z < m_GameData.MIN_BOUNDING_Z )
		m_Pos.Z = m_GameData.MIN_BOUNDING_Z;
	if( m_Pos.Z > m_GameData.MAX_BOUNDING_Z )
		m_Pos.Z = m_GameData.MAX_BOUNDING_Z;

	CalculateRudderAndFlaps();

	m_BoundSphere.vCenterPoint = m_Pos;
	m_DrawDistanceSphere.vCenterPoint = m_Pos;

	// normal model update
	m_pModelData->Update( deltaTime );

	CreatePhysics( false );

	UpdateEmitters( deltaTime );
	UpdateBullets( deltaTime );

	if( m_pRayTest->totalHits > 0 )
	{
		dQuaternion qTerrainOri;

		m_ShadowDraw = true;

		float angle = 0.0f;

		// move the position slightly higher
		m_ShadowPos.X = m_pRayTest->rayHits[0].pos[0] + (m_pRayTest->rayHits[0].n[0]*m_GameData.SHADOW_OFFSET);
		m_ShadowPos.Y = m_pRayTest->rayHits[0].pos[1] + std::abs(m_pRayTest->rayHits[0].n[1]*m_GameData.SHADOW_OFFSET);
		m_ShadowPos.Z = m_pRayTest->rayHits[0].pos[2] + (m_pRayTest->rayHits[0].n[2]*m_GameData.SHADOW_OFFSET);

		math::Vec3 cross;
		math::Vec3 worldUp( 0.0f, 1.0f, 0.0f );

		// rotate the heading by the node rotation
		math::Vec3 normal( m_pRayTest->rayHits[0].n[0], std::abs(m_pRayTest->rayHits[0].n[1]), m_pRayTest->rayHits[0].n[2] );

		// up vector
		angle = math::AngleBetweenVectors(worldUp,normal);
		cross = math::CrossProduct(normal,worldUp);
		cross.normalise();

		// terrain slope orientation
		dQSetIdentity( qTerrainOri );
		dQFromAxisAndAngle( qTerrainOri, cross.X, cross.Y, cross.Z, angle );

		// use this orientation for the model
		dRSetIdentity( m_ShadowOri );
		dRfromQ( m_ShadowOri, qTerrainOri );

		m_DistanceToGround = std::abs( (m_Pos - m_ShadowPos).length() );
	}	
	else
	{
		m_ShadowDraw = false;

		m_DistanceToGround = 999999.0f;
	}

	// reset
	m_WorldCollided = false;
	firstUpdate = false;

	if( !m_IsDead &&
		m_DistanceToGround <= m_GameData.ACH_FLYING_LOW_HEIGHT )
	{
		m_FlyingLowTime += deltaTime;

		if( m_FlyingLowTime >= m_GameData.ACH_FLYING_LOW_TIME )
			GameSystems::GetInstance()->AwardAchievement( 67, -1 );
	}
	else
		m_FlyingLowTime = 0.0f;
}

/////////////////////////////////////////////////////
/// Method: PlayerCrashed
/// Params: None
///
/////////////////////////////////////////////////////
void Player::PlayerCrashed()
{
	if( !m_IsDead )
	{
		// show explosion, set any timers
		math::Vec3 spawnPos, spawnRot;

		math::Vec3 oldPos = m_Pos;

		GameSystems::GetInstance()->SpawnExplosion( m_Pos );
		GameSystems::GetInstance()->SpawnFire(  m_Pos );

		if( GameSystems::GetInstance()->GetGameMode() != GameSystems::GAMEMODE_RINGRUN )
		{
			m_ActiveLevel.GetNearestSpawn( m_Pos, &spawnPos, &spawnRot );
			SetRespawnPosition( spawnPos, spawnRot );
		}

		m_Speed = 0.0f;

		m_IsDead = true;

		m_FlyingLowTime = 0.0f;

		StopAllAudio();

		AudioSystem::GetInstance()->PlayAudio( m_PlayerCrashBufferId, zeroVec, AL_TRUE, AL_FALSE, 1.0f, 0.2f );
	}
}

/////////////////////////////////////////////////////
/// Method: Respawn
/// Params: None
///
/////////////////////////////////////////////////////
void Player::Respawn()
{
	int i=0;
	m_InputSystem.ResetAllInput();

	m_Pos = m_RespawnPosition; 
	m_Rot = m_RespawnRot;

	m_TargetRot = m_RespawnRot;

	m_PropData.initialPos = m_VehicleData.propModel.initialPos;
	m_PropData.currentPos = m_VehicleData.propModel.initialPos;

	// RUDDER
	m_RudderData.currentPos = m_VehicleData.rudder.initialPos;
	m_RudderData.currentRot.setZero();

	// LEFT TAIL FLAP
	m_LeftTailFlapData.currentPos = m_VehicleData.leftTailFlap.initialPos;
	m_LeftTailFlapData.currentRot.setZero();

	// RIGHT TAIL FLAP
	m_RightTailFlapData.currentPos = m_VehicleData.rightTailFlap.initialPos;

	// LEFT WING FLAP
	m_LeftWingFlapData.currentPos = m_VehicleData.leftWingFlap.initialPos;
	m_LeftWingFlapData.currentRot.setZero();

	// RIGHT WING FLAP
	m_RightWingFlapData.currentPos = m_VehicleData.rightWingFlap.initialPos;
	m_RightWingFlapData.currentRot.setZero();

	m_Accelerate = m_VehicleData.accelerate;
	m_Decelerate = m_VehicleData.decelerate;

	m_Speed = m_SpeedRange.minVal + (m_SpeedRange.maxVal-m_SpeedRange.minVal)*0.5f;
	m_TargetSpeed = m_Speed;

	m_IsDead = false;

	// ray check for shadow
	m_pRayTest->hasHits = false;
	m_pRayTest->totalHits = 0;
	m_pRayTest->nextHitIndex = 0;

	m_pRayTest->rayPos = m_Pos;
	m_pRayTest->rayDir = math::Vec3( 0.0f, -1.0f, 0.0f );

	dGeomRaySet (m_pRayTest->rayGeom, m_pRayTest->rayPos.X, m_pRayTest->rayPos.Y, m_pRayTest->rayPos.Z, 
			m_pRayTest->rayDir.X, m_pRayTest->rayDir.Y, m_pRayTest->rayDir.Z );

	dGeomRaySetLength (m_pRayTest->rayGeom, 100.0f);

	dGeomSetCategoryBits( m_pRayTest->rayGeom, CATEGORY_RAYCAST );
	dGeomSetCollideBits( m_pRayTest->rayGeom, CATEGORY_WORLD );

	if( !m_WorldCollided && !m_IsDead)
	{
		math::Vec2 analogueVal;

		m_FinalDir = m_Dir;
		m_FinalUp = m_UpVector;

		m_Speed = m_TargetSpeed;

		if( m_PropData.currentRot.Z > 360.0f )
			m_PropData.currentRot.Z += -360.0f;

		dQuaternion qRotX, qRotY,qRotZ;
		dQuaternion qTmp;

		dRSetIdentity( m_Orientation );
		dQFromAxisAndAngle( qRotY, 0.0f, 1.0f, 0.0f, math::DegToRad(m_Rot.Y) );
		dQFromAxisAndAngle( qRotX, 1.0f, 0.0f, 0.0f, math::DegToRad(m_Rot.X) );
		dQFromAxisAndAngle( qRotZ, 0.0f, 0.0f, 1.0f, math::DegToRad(m_Rot.Z) );
	
		dQMultiply0( qTmp, qRotX, qRotZ );
		dQMultiply0( m_qPlaneOri, qRotY, qTmp );
		dRfromQ( m_Orientation, m_qPlaneOri );

		m_FinalDir.X = (m_Dir.X * m_Orientation[0]) + (m_Dir.Y * m_Orientation[1]) + (m_Dir.Z * m_Orientation[2]);
		m_FinalDir.Y = (m_Dir.X * m_Orientation[4]) + (m_Dir.Y * m_Orientation[5]) + (m_Dir.Z * m_Orientation[6]);
		m_FinalDir.Z = (m_Dir.X * m_Orientation[8]) + (m_Dir.Y * m_Orientation[9]) + (m_Dir.Z * m_Orientation[10]);

		m_FinalDir.normalise();

		m_FinalUp.X = (m_UpVector.X * m_Orientation[0]) + (m_UpVector.Y * m_Orientation[1]) + (m_UpVector.Z * m_Orientation[2]);
		m_FinalUp.Y = (m_UpVector.X * m_Orientation[4]) + (m_UpVector.Y * m_Orientation[5]) + (m_UpVector.Z * m_Orientation[6]);
		m_FinalUp.Z = (m_UpVector.X * m_Orientation[8]) + (m_UpVector.Y * m_Orientation[9]) + (m_UpVector.Z * m_Orientation[10]);

		m_FinalUp.normalise();
	}

	CalculateRudderAndFlaps();

	m_BoundSphere.vCenterPoint = m_Pos;
	m_DrawDistanceSphere.vCenterPoint = m_Pos;

	CreatePhysics( false );

	if( m_pRayTest->totalHits > 0 )
	{
		dQuaternion qTerrainOri;

		m_ShadowDraw = true;

		float angle = 0.0f;

		// move the position slightly higher
		m_ShadowPos.X = m_pRayTest->rayHits[0].pos[0] + (m_pRayTest->rayHits[0].n[0]*m_GameData.SHADOW_OFFSET);
		m_ShadowPos.Y = m_pRayTest->rayHits[0].pos[1] + std::abs(m_pRayTest->rayHits[0].n[1]*m_GameData.SHADOW_OFFSET);
		m_ShadowPos.Z = m_pRayTest->rayHits[0].pos[2] + (m_pRayTest->rayHits[0].n[2]*m_GameData.SHADOW_OFFSET);

		math::Vec3 cross;
		math::Vec3 worldUp( 0.0f, 1.0f, 0.0f );

		// rotate the heading by the node rotation
		math::Vec3 normal( m_pRayTest->rayHits[0].n[0], std::abs(m_pRayTest->rayHits[0].n[1]), m_pRayTest->rayHits[0].n[2] );

		// up vector
		angle = math::AngleBetweenVectors(worldUp,normal);
		cross = math::CrossProduct(normal,worldUp);
		cross.normalise();

		// terrain slope orientation
		dQSetIdentity( qTerrainOri );
		dQFromAxisAndAngle( qTerrainOri, cross.X, cross.Y, cross.Z, angle );

		// use this orientation for the model
		dRSetIdentity( m_ShadowOri );
		dRfromQ( m_ShadowOri, qTerrainOri );
	}	
	else
		m_ShadowDraw = false;

	// reset
	m_WorldCollided = false;
	
	// bullets
	m_SpawnedBullet = 0.0f;
	m_ShootTimer = 0.0f;
	m_MuzzleFlashTime = 0.0f;
	m_NextBulletIndex = 0;
	for( i = 0; i < PLANE_MAXNUM_BULLETS; i++ )
	{
		if( m_BulletList[i].active )
		{
			m_BulletList[i].active = false;
			m_BulletList[i].stateChange = false;

			dGeomSetCategoryBits( m_BulletList[i].geomId, CATEGORY_BULLET );
			dGeomSetCollideBits( m_BulletList[i].geomId, CATEGORY_NOTHING );
			dGeomDisable( m_BulletList[i].geomId );
		}
	}

	if( m_Exhaust1.pEmitter != 0 )
	{
		m_Exhaust1.currentPos.X = (m_Exhaust1.initialPos.X * m_Orientation[0]) + (m_Exhaust1.initialPos.Y * m_Orientation[1]) + (m_Exhaust1.initialPos.Z * m_Orientation[2]);
		m_Exhaust1.currentPos.Y = (m_Exhaust1.initialPos.X * m_Orientation[4]) + (m_Exhaust1.initialPos.Y * m_Orientation[5]) + (m_Exhaust1.initialPos.Z * m_Orientation[6]);
		m_Exhaust1.currentPos.Z = (m_Exhaust1.initialPos.X * m_Orientation[8]) + (m_Exhaust1.initialPos.Y * m_Orientation[9]) + (m_Exhaust1.initialPos.Z * m_Orientation[10]);

		m_Exhaust1.pEmitter->SetPos( m_Exhaust1.currentPos );
		m_Exhaust1.pEmitter->SetSpawnDir1( -m_FinalDir*20.0f );
		m_Exhaust1.pEmitter->SetSpawnDir2( -m_FinalDir*20.0f );
		m_Exhaust1.pEmitter->Reset();
	}

	if( m_Exhaust2.pEmitter != 0 )
	{
		m_Exhaust2.currentPos.X = (m_Exhaust2.initialPos.X * m_Orientation[0]) + (m_Exhaust2.initialPos.Y * m_Orientation[1]) + (m_Exhaust2.initialPos.Z * m_Orientation[2]);
		m_Exhaust2.currentPos.Y = (m_Exhaust2.initialPos.X * m_Orientation[4]) + (m_Exhaust2.initialPos.Y * m_Orientation[5]) + (m_Exhaust2.initialPos.Z * m_Orientation[6]);
		m_Exhaust2.currentPos.Z = (m_Exhaust2.initialPos.X * m_Orientation[8]) + (m_Exhaust2.initialPos.Y * m_Orientation[9]) + (m_Exhaust2.initialPos.Z * m_Orientation[10]);

		m_Exhaust2.pEmitter->SetPos( m_Exhaust2.currentPos );
		m_Exhaust2.pEmitter->SetSpawnDir1( -m_FinalDir*20.0f );
		m_Exhaust2.pEmitter->SetSpawnDir2( -m_FinalDir*20.0f );
		m_Exhaust2.pEmitter->Reset();
	}

	if( m_WingSlice1.pEmitter != 0 )
	{
		m_WingSlice1.currentPos.X = (m_WingSlice1.initialPos.X * m_Orientation[0]) + (m_WingSlice1.initialPos.Y * m_Orientation[1]) + (m_WingSlice1.initialPos.Z * m_Orientation[2]);
		m_WingSlice1.currentPos.Y = (m_WingSlice1.initialPos.X * m_Orientation[4]) + (m_WingSlice1.initialPos.Y * m_Orientation[5]) + (m_WingSlice1.initialPos.Z * m_Orientation[6]);
		m_WingSlice1.currentPos.Z = (m_WingSlice1.initialPos.X * m_Orientation[8]) + (m_WingSlice1.initialPos.Y * m_Orientation[9]) + (m_WingSlice1.initialPos.Z * m_Orientation[10]);

		m_WingSlice1.pEmitter->SetPos( zeroVec );
		m_WingSlice1.pEmitter->Reset();
	}

	if( m_WingSlice2.pEmitter != 0 )
	{
		m_WingSlice2.currentPos.X = (m_WingSlice2.initialPos.X * m_Orientation[0]) + (m_WingSlice2.initialPos.Y * m_Orientation[1]) + (m_WingSlice2.initialPos.Z * m_Orientation[2]);
		m_WingSlice2.currentPos.Y = (m_WingSlice2.initialPos.X * m_Orientation[4]) + (m_WingSlice2.initialPos.Y * m_Orientation[5]) + (m_WingSlice2.initialPos.Z * m_Orientation[6]);
		m_WingSlice2.currentPos.Z = (m_WingSlice2.initialPos.X * m_Orientation[8]) + (m_WingSlice2.initialPos.Y * m_Orientation[9]) + (m_WingSlice2.initialPos.Z * m_Orientation[10]);

		m_WingSlice2.pEmitter->SetPos( zeroVec );
		m_WingSlice2.pEmitter->Reset();
	}

	m_FlyingLowTime = 0.0f;
}

/////////////////////////////////////////////////////
/// Method: ResetRingPitch
/// Params: None
///
/////////////////////////////////////////////////////
void Player::ResetRingPitch()
{
	m_RingPitch = RING_PITCH_START;
}

/////////////////////////////////////////////////////
/// Method: CheckAgainstLevel
/// Params: [in]activeLevel
///
/////////////////////////////////////////////////////
void Player::CheckAgainstLevel( Level& activeLevel )
{
	int i=0;

	m_ActiveGateGroup = activeLevel.GetActiveGateGroup();
	m_Sights = activeLevel.GetSights();

	if( m_Sights != 0 )
	{
		std::vector<Level::Gate>::iterator it = m_Sights->begin();

		while( it != m_Sights->end() ) 
		{
			if( (*it).enabled )
			{
				bool collision = false;
				if( (*it).collisionType == 0 ) // sphere
					collision = m_BoundSphere.SphereCollidesWithSphere((*it).s);
				else
					collision = (*it).obb.OBBCollidesWithSphere( m_BoundSphere );

				if( collision )
				{
					(*it).enabled = false;
					if( (*it).achievementId == 52 )
					{
						// barn 
						GameSystems::GetInstance()->AwardAchievement( (*it).achievementId + m_pScriptData->GetDefaultVehicle(), (*it).textId );

						// full speed barn
						if( m_Speed >= m_VehicleData.maxSpeed )
						{
							GameSystems::GetInstance()->AwardAchievement( 55 + m_pScriptData->GetDefaultVehicle(), (*it).textId );
						}
					}
					else
					{
						GameSystems::GetInstance()->AwardAchievement( (*it).achievementId, (*it).textId );
					}
				}
			}
			// next
			it++;
		}
	}

	if( m_ActiveGateGroup != 0 )
	{
		int counter = 0;
		GameSystems::GetInstance()->SetGatesTotal( m_ActiveGateGroup->numGates );

		for( i=0; i<m_ActiveGateGroup->numGates; ++i ) 
		{
			// enable all gates with correct index
			int matchNum = m_ActiveGateGroup->nextGateIndex;
			int matchNum1 = -100;//m_ActiveGateGroup->nextGateIndex+1;
			
			if( m_ActiveGateGroup->nextGateIndex+1 > m_ActiveGateGroup->highestGateNum )
			{
				matchNum1 = -100;//0;
			}

			if( m_ActiveGateGroup->gateList[i]->number == matchNum ||
				m_ActiveGateGroup->gateList[i]->number == matchNum1 )
			{
				// normal gate
				m_ActiveGateGroup->enabled = true;
				m_ActiveGateGroup->gateList[i]->enabled = true;
				
				if( m_ActiveGateGroup->gateList[i]->collisionType == 0 )
				{
					if( m_ActiveGateGroup->gateList[i]->number == matchNum &&
						m_BoundSphere.SphereCollidesWithSphere( m_ActiveGateGroup->gateList[i]->s ) )
					{
						if( m_ActiveGateGroup->gateList[i]->type == 0 ) // ring
						{
							m_RespawnPosition = m_ActiveGateGroup->gateList[i]->pos; 
							m_RespawnRot = m_ActiveGateGroup->gateList[i]->rot;

							m_ActiveGateGroup->nextGateIndex++;
							if( m_ActiveGateGroup->nextGateIndex > m_ActiveGateGroup->highestGateNum )
							{
								m_ActiveGateGroup->nextGateIndex = 0;
								counter = GameSystems::GetInstance()->GetLapsCompleted();
								GameSystems::GetInstance()->SetLapsComplete(counter+1);

								GameSystems::GetInstance()->SetGatesComplete( 0 );
								m_RingPitch = RING_PITCH_START;
							}
							else
							{
								counter = GameSystems::GetInstance()->GetGatesComplete();
								GameSystems::GetInstance()->SetGatesComplete( counter+1 );

								GameSystems::GetInstance()->PlayRingPassAudio( m_RingPitch+=m_GameData.RING_PITCH_INC );
							}
						}
						else if( m_ActiveGateGroup->gateList[i]->type == 1 ) // speed up
						{

						}
						else if( m_ActiveGateGroup->gateList[i]->type == 2 ) // sightsee
						{

						}
					}
				}
				else if( m_ActiveGateGroup->gateList[i]->collisionType == 1 )
				{
					if( m_ActiveGateGroup->gateList[i]->number == matchNum &&
						m_ActiveGateGroup->gateList[i]->obb.OBBCollidesWithSphere( m_BoundSphere ) )
					{
						if( m_ActiveGateGroup->gateList[i]->type == 0 ) // ring
						{
							m_RespawnPosition = m_ActiveGateGroup->gateList[i]->pos; 
							m_RespawnRot = m_ActiveGateGroup->gateList[i]->rot;

							m_ActiveGateGroup->nextGateIndex++;
							if( m_ActiveGateGroup->nextGateIndex > m_ActiveGateGroup->highestGateNum )
							{
								m_ActiveGateGroup->nextGateIndex = 0;
								counter = GameSystems::GetInstance()->GetLapsCompleted();
								GameSystems::GetInstance()->SetLapsComplete(counter+1);

								GameSystems::GetInstance()->SetGatesComplete( 0 );
								m_RingPitch = RING_PITCH_START;
							}
							else
							{
								counter = GameSystems::GetInstance()->GetGatesComplete();
								GameSystems::GetInstance()->SetGatesComplete( counter+1 );

								GameSystems::GetInstance()->PlayRingPassAudio( m_RingPitch+=m_GameData.RING_PITCH_INC );
							}
						}
						else if( m_ActiveGateGroup->gateList[i]->type == 1 ) // speed up
						{

						}
						else if( m_ActiveGateGroup->gateList[i]->type == 2 ) // sightsee
						{

						}
					}
				}
			}
			else
			{
				// keep gate disabled
				m_ActiveGateGroup->gateList[i]->enabled = false;
			}
		}
	}
}

/////////////////////////////////////////////////////
/// Method: StartAudio
/// Params: None
///
/////////////////////////////////////////////////////
void Player::StartAudio()
{
	if( !m_AudioStarted )
	{
		if( m_EngineSrcId != snd::INVALID_SOUNDSOURCE )
		{
			AudioSystem::GetInstance()->StopAudio( m_EngineSrcId );
			m_EngineSrcId = snd::INVALID_SOUNDSOURCE;
		}

		if( m_EngineSrcId == snd::INVALID_SOUNDSOURCE )
		{
			m_EngineSrcId = snd::SoundManager::GetInstance()->GetFreeSoundSource();
			AudioSystem::GetInstance()->PlayAudio( m_EngineSrcId, m_EngineBufferId, math::Vec3(0.0f,0.0f,0.0f), AL_TRUE, AL_TRUE, 1.0f, 0.2f );
		}

		m_AudioStarted = true;
	}
}

/////////////////////////////////////////////////////
/// Method: StopAllAudio
/// Params: None
///
/////////////////////////////////////////////////////
void Player::StopAllAudio()
{
	if( m_AudioStarted )
	{
		if( m_EngineSrcId != snd::INVALID_SOUNDSOURCE )
		{
			AudioSystem::GetInstance()->StopAudio( m_EngineSrcId );
			m_EngineSrcId = snd::INVALID_SOUNDSOURCE;
		}

		if( m_GunSrcId != snd::INVALID_SOUNDSOURCE )
		{
			AudioSystem::GetInstance()->StopAudio( m_GunSrcId );
			m_GunSrcId = snd::INVALID_SOUNDSOURCE;
		}

		m_AudioStarted = false;
	}
}

/////////////////////////////////////////////////////
/// Method: InitialiseEmitters
/// Params: None
///
/////////////////////////////////////////////////////
void Player::InitialiseEmitters()
{
	if( m_Exhaust1.resId != -1 )
	{
		const res::EmitterResourceStore *er = res::GetEmitterResource( m_Exhaust1.resId );
		DBG_ASSERT( er != 0 );

		m_Exhaust1.pEmitter = new efx::Emitter;
		DBG_ASSERT( m_Exhaust1.pEmitter != 0 );

		m_Exhaust1.pEmitter->Create( *er->block );
		res::SetupTexturesOnEmitter( m_Exhaust1.pEmitter );

		m_Exhaust1.pEmitter->SetBoundingCheckState(false);
		m_Exhaust1.pEmitter->SetPos( zeroVec );
		m_Exhaust1.pEmitter->Enable();
	}

	if( m_Exhaust2.resId != -1 )
	{
		const res::EmitterResourceStore *er = res::GetEmitterResource( m_Exhaust2.resId );
		DBG_ASSERT( er != 0 );

		m_Exhaust2.pEmitter = new efx::Emitter;
		DBG_ASSERT( m_Exhaust2.pEmitter != 0 );

		m_Exhaust2.pEmitter->Create( *er->block );
		res::SetupTexturesOnEmitter( m_Exhaust2.pEmitter );

		m_Exhaust2.pEmitter->SetBoundingCheckState(false);
		m_Exhaust2.pEmitter->SetPos( zeroVec );
		m_Exhaust2.pEmitter->Enable();
	}

	if( m_WingSlice1.resId != -1 )
	{
		const res::EmitterResourceStore *er = res::GetEmitterResource( m_WingSlice1.resId );
		DBG_ASSERT( er != 0 );

		m_WingSlice1.pEmitter = new efx::Emitter;
		DBG_ASSERT( m_WingSlice1.pEmitter != 0 );

		m_WingSlice1.pEmitter->Create( *er->block );
		res::SetupTexturesOnEmitter( m_WingSlice1.pEmitter );

		m_WingSlice1.pEmitter->SetBoundingCheckState(false);
		m_WingSlice1.pEmitter->SetPos( zeroVec );
		m_WingSlice1.pEmitter->Enable();
	}

	if( m_WingSlice2.resId != -1 )
	{
		const res::EmitterResourceStore *er = res::GetEmitterResource( m_WingSlice2.resId );
		DBG_ASSERT( er != 0 );

		m_WingSlice2.pEmitter = new efx::Emitter;
		DBG_ASSERT( m_WingSlice2.pEmitter != 0 );

		m_WingSlice2.pEmitter->Create( *er->block );
		res::SetupTexturesOnEmitter( m_WingSlice2.pEmitter );

		m_WingSlice2.pEmitter->SetBoundingCheckState(false);
		m_WingSlice2.pEmitter->SetPos( zeroVec );
		m_WingSlice2.pEmitter->Enable();
	}
}

/////////////////////////////////////////////////////
/// Method: DrawEmitters
/// Params: None
///
/////////////////////////////////////////////////////
void Player::DrawEmitters()
{
	if( !m_IsDead )
	{
		if( m_Exhaust1.pEmitter != 0 )
		{
			glPushMatrix();
				glTranslatef( m_Pos.X, m_Pos.Y, m_Pos.Z );
				m_Exhaust1.pEmitter->Draw();
			glPopMatrix();
		}

		if( m_Exhaust2.pEmitter != 0 )
		{
			glPushMatrix();
				glTranslatef( m_Pos.X, m_Pos.Y, m_Pos.Z );
				m_Exhaust2.pEmitter->Draw();
			glPopMatrix();
		}


			if( m_WingSlice1.pEmitter != 0 )
			{
				glPushMatrix();
					glTranslatef( m_WingSlice1.currentPos.X, m_WingSlice1.currentPos.Y, m_WingSlice1.currentPos.Z );
					glMultMatrixf(m);
					m_WingSlice1.pEmitter->Draw();
				glPopMatrix();
			}

			if( m_WingSlice2.pEmitter != 0 )
			{
				glPushMatrix();
					glTranslatef( m_WingSlice2.currentPos.X, m_WingSlice2.currentPos.Y, m_WingSlice2.currentPos.Z );
					glMultMatrixf(m);
					m_WingSlice2.pEmitter->Draw();
				glPopMatrix();
			}
	}
}

/////////////////////////////////////////////////////
/// Method: UpdateEmitters
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
void Player::UpdateEmitters(float deltaTime)
{
	if( m_Exhaust1.pEmitter != 0 )
	{
		m_Exhaust1.currentPos.X = (m_Exhaust1.initialPos.X * m_Orientation[0]) + (m_Exhaust1.initialPos.Y * m_Orientation[1]) + (m_Exhaust1.initialPos.Z * m_Orientation[2]);
		m_Exhaust1.currentPos.Y = (m_Exhaust1.initialPos.X * m_Orientation[4]) + (m_Exhaust1.initialPos.Y * m_Orientation[5]) + (m_Exhaust1.initialPos.Z * m_Orientation[6]);
		m_Exhaust1.currentPos.Z = (m_Exhaust1.initialPos.X * m_Orientation[8]) + (m_Exhaust1.initialPos.Y * m_Orientation[9]) + (m_Exhaust1.initialPos.Z * m_Orientation[10]);

		m_Exhaust1.pEmitter->SetPos( m_Exhaust1.currentPos );
		m_Exhaust1.pEmitter->SetSpawnDir1( -m_FinalDir*20.0f );
		m_Exhaust1.pEmitter->SetSpawnDir2( -m_FinalDir*20.0f );
		m_Exhaust1.pEmitter->Update(deltaTime);
	}

	if( m_Exhaust2.pEmitter != 0 )
	{
		m_Exhaust2.currentPos.X = (m_Exhaust2.initialPos.X * m_Orientation[0]) + (m_Exhaust2.initialPos.Y * m_Orientation[1]) + (m_Exhaust2.initialPos.Z * m_Orientation[2]);
		m_Exhaust2.currentPos.Y = (m_Exhaust2.initialPos.X * m_Orientation[4]) + (m_Exhaust2.initialPos.Y * m_Orientation[5]) + (m_Exhaust2.initialPos.Z * m_Orientation[6]);
		m_Exhaust2.currentPos.Z = (m_Exhaust2.initialPos.X * m_Orientation[8]) + (m_Exhaust2.initialPos.Y * m_Orientation[9]) + (m_Exhaust2.initialPos.Z * m_Orientation[10]);

		m_Exhaust2.pEmitter->SetPos( m_Exhaust2.currentPos );
		m_Exhaust2.pEmitter->SetSpawnDir1( -m_FinalDir*20.0f );
		m_Exhaust2.pEmitter->SetSpawnDir2( -m_FinalDir*20.0f );
		m_Exhaust2.pEmitter->Update(deltaTime);
	}


	if( m_Rot.Z > 10.0f ||
		m_Rot.Z < -10.0f )
	{
		if( m_WingSlice1.pEmitter != 0 )
		{
			m_WingSlice1.pEmitter->Enable();
		}

		if( m_WingSlice2.pEmitter != 0 )
		{
			m_WingSlice2.pEmitter->Enable();
		}
	}
	else
	{
		if( m_WingSlice1.pEmitter != 0 )
		{
			m_WingSlice1.pEmitter->Disable();
		}

		if( m_WingSlice2.pEmitter != 0 )
		{
			m_WingSlice2.pEmitter->Disable();
		}
	}

	if( m_WingSlice1.pEmitter != 0 )
	{
		m_WingSlice1.currentPos.X = (m_WingSlice1.initialPos.X * m_Orientation[0]) + (m_WingSlice1.initialPos.Y * m_Orientation[1]) + (m_WingSlice1.initialPos.Z * m_Orientation[2]);
		m_WingSlice1.currentPos.Y = (m_WingSlice1.initialPos.X * m_Orientation[4]) + (m_WingSlice1.initialPos.Y * m_Orientation[5]) + (m_WingSlice1.initialPos.Z * m_Orientation[6]);
		m_WingSlice1.currentPos.Z = (m_WingSlice1.initialPos.X * m_Orientation[8]) + (m_WingSlice1.initialPos.Y * m_Orientation[9]) + (m_WingSlice1.initialPos.Z * m_Orientation[10]);

		m_WingSlice1.pEmitter->SetPos( zeroVec );
		m_WingSlice1.pEmitter->Update(deltaTime);
	}

	if( m_WingSlice2.pEmitter != 0 )
	{
		m_WingSlice2.currentPos.X = (m_WingSlice2.initialPos.X * m_Orientation[0]) + (m_WingSlice2.initialPos.Y * m_Orientation[1]) + (m_WingSlice2.initialPos.Z * m_Orientation[2]);
		m_WingSlice2.currentPos.Y = (m_WingSlice2.initialPos.X * m_Orientation[4]) + (m_WingSlice2.initialPos.Y * m_Orientation[5]) + (m_WingSlice2.initialPos.Z * m_Orientation[6]);
		m_WingSlice2.currentPos.Z = (m_WingSlice2.initialPos.X * m_Orientation[8]) + (m_WingSlice2.initialPos.Y * m_Orientation[9]) + (m_WingSlice2.initialPos.Z * m_Orientation[10]);

		m_WingSlice2.pEmitter->SetPos( zeroVec );
		m_WingSlice2.pEmitter->Update(deltaTime);
	}

}

/////////////////////////////////////////////////////
/// Method: UpdateBullets
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
void Player::UpdateBullets(float deltaTime)
{
	int i=0;

	bool shootState = m_InputSystem.GetShoot();

	// update the bullets
	if( !m_IsDead &&
		shootState )
	{
		m_ShootTimer += deltaTime;

		if( m_ShootTimer >= m_VehicleData.bulletRate )
		{
			//if( m_BulletList[m_NextBulletIndex].active == false )
			{
				m_BulletList[m_NextBulletIndex].vPos = m_Pos + math::RandomNumber( -0.25f, 0.25f );
				m_BulletList[m_NextBulletIndex].vStartPoint = m_BulletList[m_NextBulletIndex].vPos;

				m_BulletList[m_NextBulletIndex].vVel.X = (m_FinalDir.X*m_VehicleData.bulletSpeed);
				m_BulletList[m_NextBulletIndex].vVel.Y = (m_FinalDir.Y*m_VehicleData.bulletSpeed);
				m_BulletList[m_NextBulletIndex].vVel.Z = (m_FinalDir.Z*m_VehicleData.bulletSpeed);

				m_BulletList[m_NextBulletIndex].vRot = m_Rot;

				m_BulletList[m_NextBulletIndex].life = m_VehicleData.bulletLife;
				m_BulletList[m_NextBulletIndex].distanceTravelled = 0.0f;

				m_BulletList[m_NextBulletIndex].active = true;
				m_BulletList[m_NextBulletIndex].stateChange = false;

				dGeomSetCategoryBits( m_BulletList[m_NextBulletIndex].geomId, CATEGORY_BULLET );
				unsigned int bits = CATEGORY_WORLD | CATEGORY_WORLDOBJECT;
				dGeomSetCollideBits( m_BulletList[m_NextBulletIndex].geomId, bits );
				dGeomEnable( m_BulletList[m_NextBulletIndex].geomId );
					
				dGeomSetPosition( m_BulletList[m_NextBulletIndex].geomId, m_BulletList[m_NextBulletIndex].vPos.X, m_BulletList[m_NextBulletIndex].vPos.Y, m_BulletList[m_NextBulletIndex].vPos.Z  );

				m_ShootTimer = 0.0f;
				m_MuzzleFlashTime = 0.0f;
				m_SpawnedBullet = true;

				AudioSystem::GetInstance()->PlayAudio( m_GunBufferId, zeroVec, AL_TRUE, AL_FALSE, 1.0f, 0.1f );

				m_NextBulletIndex++;
				if( m_NextBulletIndex >= PLANE_MAXNUM_BULLETS )
					m_NextBulletIndex = 0;
			}
		}
	}
	else
	{
		//m_ShootTimer = m_VehicleData.bulletRate;
	}

	if( m_MuzzleFlashTime >= 0.05f )
	{
		m_SpawnedBullet = false;
		m_MuzzleFlashTime = 0.05f;
	}
	m_MuzzleFlashTime += deltaTime;

	for( i = 0; i < PLANE_MAXNUM_BULLETS; i++ )
	{
		if( m_BulletList[i].pEmitter != 0 )
			m_BulletList[i].pEmitter->Update( deltaTime );

		if( m_BulletList[i].active )
		{
			m_BulletList[i].vPos += m_BulletList[i].vVel*deltaTime;
			m_BulletList[i].life -= 1.0f*deltaTime;

			dGeomSetPosition( m_BulletList[i].geomId, m_BulletList[i].vPos.X, m_BulletList[i].vPos.Y, m_BulletList[i].vPos.Z  );

			math::Vec3 vDistance = m_BulletList[i].vPos - m_BulletList[i].vStartPoint;
			m_BulletList[i].distanceTravelled = vDistance.length();

			if( (m_BulletList[i].distanceTravelled >= m_VehicleData.maxBulletDistance) ||
				m_BulletList[i].life <= 0.0f ||
				m_BulletList[i].stateChange )
			{
				if( m_BulletList[i].stateChange )
				{
					if( m_BulletList[i].doEffect )
					{
						m_BulletList[i].pEmitter->SetPos( m_BulletList[i].vPos );
						m_BulletList[i].pEmitter->Enable();
						m_BulletList[i].pEmitter->Disable();
					}
				}

				m_BulletList[i].active = false;
				m_BulletList[i].stateChange = false;
				m_BulletList[i].doEffect = false;

				dGeomSetCategoryBits( m_BulletList[i].geomId, CATEGORY_BULLET );
				dGeomSetCollideBits( m_BulletList[i].geomId, CATEGORY_NOTHING );
				dGeomDisable( m_BulletList[i].geomId );
			}
		}
	}
}

/////////////////////////////////////////////////////
/// Method: LoadPhysicsData
/// Params: [in]szFilename
///
/////////////////////////////////////////////////////
int Player::LoadPhysicsData( const char *szFilename )
{
	file::TFileHandle fileHandle;
	file::TFile physFile;
	unsigned int i=0, j=0;

	if( core::IsEmptyString( szFilename ) )
		return(1);

	if( file::FileOpen( szFilename, file::FILETYPE_BINARY_READ, &fileHandle ) == false )
	{
		DBGLOG( "PHYSICSRESOURCES: *ERROR* Could not open %s file\n", szFilename );
		return(1);
	}

	unsigned int boxIndex = 0;
	unsigned int sphereIndex = 0;
	unsigned int capsuleIndex = 0;
	unsigned int cylinderIndex = 0;
	unsigned int triMeshIndex = 0;

	res::PhysicsBlock* pBlock = 0;

	//m_AABB.Reset();

	// can create the physics m_PhysicsData
	m_PhysicsData = new res::PhysicsBlock;
	DBG_ASSERT( m_PhysicsData != 0 );

	pBlock = m_PhysicsData;

	DBG_ASSERT( pBlock != 0 );

	std::memset( pBlock, 0, sizeof(res::PhysicsBlock) );

	file::FileReadUInt( &pBlock->totalPhysicsBox, sizeof(unsigned int), 1, &fileHandle );
	file::FileReadUInt( &pBlock->totalPhysicsSphere, sizeof(unsigned int), 1, &fileHandle );
	file::FileReadUInt( &pBlock->totalPhysicsCapsule, sizeof(unsigned int), 1, &fileHandle );
	file::FileReadUInt( &pBlock->totalPhysicsCylinder, sizeof(unsigned int), 1, &fileHandle );
	file::FileReadUInt( &pBlock->totalPhysicsTriMesh, sizeof(unsigned int), 1, &fileHandle );
	file::FileReadUInt( &pBlock->totalPhysicsObjects, sizeof(unsigned int), 1, &fileHandle );

	// allocations
	DBG_ASSERT( pBlock->boxList == 0 );
	if( pBlock->totalPhysicsBox > 0 )
	{
		pBlock->boxList = new res::PhysicsBoxShape[pBlock->totalPhysicsBox];
		DBG_ASSERT( pBlock->boxList != 0 );
	}

	DBG_ASSERT( pBlock->sphereList == 0 );
	if( pBlock->totalPhysicsSphere > 0 )
	{
		pBlock->sphereList = new res::PhysicsSphereShape[pBlock->totalPhysicsSphere];
		DBG_ASSERT( pBlock->sphereList != 0 );
	}

	DBG_ASSERT( pBlock->capsuleList == 0 );
	if( pBlock->totalPhysicsCapsule > 0 )
	{
		pBlock->capsuleList = new res::PhysicsCapsuleShape[pBlock->totalPhysicsCapsule];
		DBG_ASSERT( pBlock->capsuleList != 0 );
	}

	DBG_ASSERT( pBlock->cylinderList == 0 );
	if( pBlock->totalPhysicsCylinder > 0 )
	{
		pBlock->cylinderList = new res::PhysicsCylinderShape[pBlock->totalPhysicsCylinder];
		DBG_ASSERT( pBlock->cylinderList != 0 );
	}

	DBG_ASSERT( pBlock->triMeshList == 0 );
	if( pBlock->totalPhysicsTriMesh > 0 )
	{
		pBlock->triMeshList = new res::PhysicsTriMesh[pBlock->totalPhysicsTriMesh];
		DBG_ASSERT( pBlock->triMeshList != 0 );
	}

	math::Vec3 pos, rot;

	for( i=0; i < pBlock->totalPhysicsObjects; ++i )
	{
		unsigned int type=0;
		file::FileReadUInt( &type, sizeof(unsigned int), 1, &fileHandle );

		switch( type )
		{
			case ODETYPE_BOX: // polyBox
				{
					res::PhysicsBoxShape* pBoxPtr = &pBlock->boxList[boxIndex];
					DBG_ASSERT( pBoxPtr != 0 );

					file::FileReadInt( &pBoxPtr->numeric1, sizeof(int), 1, &fileHandle );
					file::FileReadInt( &pBoxPtr->numeric2, sizeof(int), 1, &fileHandle );
					file::FileReadInt( &pBoxPtr->numeric3, sizeof(int), 1, &fileHandle );
					file::FileReadInt( &pBoxPtr->numeric4, sizeof(int), 1, &fileHandle );
					file::FileReadInt( &pBoxPtr->numeric5, sizeof(int), 1, &fileHandle );

					file::FileReadFloat( &pBoxPtr->pos, sizeof(float), 3, &fileHandle );

					file::FileReadFloat( &pBoxPtr->rot, sizeof(float), 3, &fileHandle );

					file::FileReadFloat( &pBoxPtr->dims, sizeof(float), 3, &fileHandle );

					++boxIndex;
				}break;
			case ODETYPE_SPHERE: // sphere
				{
					res::PhysicsSphereShape* pSpherePtr = &pBlock->sphereList[sphereIndex];
					DBG_ASSERT( pSpherePtr != 0 );

					file::FileReadInt( &pSpherePtr->numeric1, sizeof(int), 1, &fileHandle );
					file::FileReadInt( &pSpherePtr->numeric2, sizeof(int), 1, &fileHandle );
					file::FileReadInt( &pSpherePtr->numeric3, sizeof(int), 1, &fileHandle );
					file::FileReadInt( &pSpherePtr->numeric4, sizeof(int), 1, &fileHandle );
					file::FileReadInt( &pSpherePtr->numeric5, sizeof(int), 1, &fileHandle );

					file::FileReadFloat( &pSpherePtr->pos, sizeof(float), 3, &fileHandle );

					file::FileReadFloat( &pSpherePtr->radius, sizeof(float), 1, &fileHandle );

					++sphereIndex;
				}break;
			case ODETYPE_CAPSULE:
				{
					res::PhysicsCapsuleShape* pCapsulePtr = &pBlock->capsuleList[capsuleIndex];
					DBG_ASSERT( pCapsulePtr != 0 );

					file::FileReadInt( &pCapsulePtr->numeric1, sizeof(int), 1, &fileHandle );
					file::FileReadInt( &pCapsulePtr->numeric2, sizeof(int), 1, &fileHandle );
					file::FileReadInt( &pCapsulePtr->numeric3, sizeof(int), 1, &fileHandle );
					file::FileReadInt( &pCapsulePtr->numeric4, sizeof(int), 1, &fileHandle );
					file::FileReadInt( &pCapsulePtr->numeric5, sizeof(int), 1, &fileHandle );

					file::FileReadFloat( &pCapsulePtr->pos, sizeof(float), 3, &fileHandle );

					file::FileReadFloat( &pCapsulePtr->rot, sizeof(float), 3, &fileHandle );

					file::FileReadFloat( &pCapsulePtr->radius, sizeof(float), 1, &fileHandle );
					file::FileReadFloat( &pCapsulePtr->length, sizeof(float), 1, &fileHandle );

					++capsuleIndex;
				}break;
			case ODETYPE_CYLINDER:
				{
					res::PhysicsCylinderShape* pCylinderPtr = &pBlock->cylinderList[cylinderIndex];
					DBG_ASSERT( pCylinderPtr != 0 );

					file::FileReadInt( &pCylinderPtr->numeric1, sizeof(int), 1, &fileHandle );
					file::FileReadInt( &pCylinderPtr->numeric2, sizeof(int), 1, &fileHandle );
					file::FileReadInt( &pCylinderPtr->numeric3, sizeof(int), 1, &fileHandle );
					file::FileReadInt( &pCylinderPtr->numeric4, sizeof(int), 1, &fileHandle );
					file::FileReadInt( &pCylinderPtr->numeric5, sizeof(int), 1, &fileHandle );

					file::FileReadFloat( &pCylinderPtr->pos, sizeof(float), 3, &fileHandle );

					file::FileReadFloat( &pCylinderPtr->rot, sizeof(float), 3, &fileHandle );

					file::FileReadFloat( &pCylinderPtr->radius, sizeof(float), 1, &fileHandle );

					file::FileReadFloat( &pCylinderPtr->length, sizeof(float), 1, &fileHandle );

					++cylinderIndex;
				}break;
			case ODETYPE_TRIMESH: // mesh
				{
					res::PhysicsTriMesh* pTriMesh = &pBlock->triMeshList[triMeshIndex];
					DBG_ASSERT( pTriMesh != 0 );

					pTriMesh->totalMeshTriangles = 0;
					pTriMesh->indices = 0;
					pTriMesh->triangleData = 0;
					pTriMesh->normals = 0;
					pTriMesh->triMeshDataID = 0;

					file::FileReadInt( &pTriMesh->numeric1, sizeof(int), 1, &fileHandle );
					file::FileReadInt( &pTriMesh->numeric2, sizeof(int), 1, &fileHandle );
					file::FileReadInt( &pTriMesh->numeric3, sizeof(int), 1, &fileHandle );
					file::FileReadInt( &pTriMesh->numeric4, sizeof(int), 1, &fileHandle );
					file::FileReadInt( &pTriMesh->numeric5, sizeof(int), 1, &fileHandle );

					file::FileReadFloat( &pTriMesh->pos, sizeof(float), 3, &fileHandle );
					file::FileReadFloat( &pTriMesh->rot, sizeof(float), 3, &fileHandle );

					unsigned int totalMeshTriangles = 0;
					file::FileReadUInt( &totalMeshTriangles, sizeof(unsigned int), 1, &fileHandle );

					unsigned int polyMeshCount = 0;
					file::FileReadUInt( &polyMeshCount, sizeof(unsigned int), 1, &fileHandle );

					// allocate data
					pTriMesh->totalMeshTriangles = totalMeshTriangles;
					pTriMesh->triangleData = new math::Vec3[totalMeshTriangles*3];
					pTriMesh->indices = new int[totalMeshTriangles*3];
					pTriMesh->normals = new math::Vec3[totalMeshTriangles];

					DBG_ASSERT( pTriMesh->triangleData != 0 );
					DBG_ASSERT( pTriMesh->indices != 0 );

					int pointIndex = 0;
					int normalCount = 0;
					for( j = 0; j < totalMeshTriangles; ++j )
					{
						file::FileReadFloat( &pTriMesh->triangleData[pointIndex+0], sizeof(float), 3, &fileHandle );
						file::FileReadFloat( &pTriMesh->triangleData[pointIndex+1], sizeof(float), 3, &fileHandle );
						file::FileReadFloat( &pTriMesh->triangleData[pointIndex+2], sizeof(float), 3, &fileHandle );

						// calc normal
						math::Vec3 edge1 = pTriMesh->triangleData[(pointIndex+1)] - pTriMesh->triangleData[(pointIndex+0)];
						math::Vec3 edge2 = pTriMesh->triangleData[(pointIndex+2)] - pTriMesh->triangleData[(pointIndex+0)];

						pTriMesh->normals[normalCount] = CrossProduct( edge1, edge2 );
						pTriMesh->normals[normalCount].normalise();

						normalCount++;

						pointIndex+=3;
					}

					for( j = 0; j < pTriMesh->totalMeshTriangles*3; j++ )
						pTriMesh->indices[j] = j;

					++triMeshIndex;

				}break;
			
			default:
				DBG_ASSERT_MSG( 0, "Unknown ode physics type in file" );
			break;
		}
	}

	file::FileClose( &fileHandle );

	return(0);
}

/////////////////////////////////////////////////////
/// Method: CreateGeom
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
dGeomID Player::CreateGeom( int shapeClass, dSpaceID spaceId, const math::Vec3& dims, float radius, float length, dTriMeshDataID triMeshDataId)
{
	switch( shapeClass )
	{
		case dBoxClass:
			return dCreateBox( spaceId, dims.Width, dims.Height, dims.Depth );
		break;
		case dSphereClass:
			return dCreateSphere( spaceId, radius );
		break;
		case dCapsuleClass:
			return dCreateCapsule( spaceId, radius, length );
		break;
		case dCylinderClass:
			return dCreateCylinder( spaceId, radius, length );
		break;
		case dTriMeshClass:
			return dCreateTriMesh( spaceId, triMeshDataId, 0, 0, 0 );
		break;

		default:
			DBG_ASSERT(0);
			break;
	}		

	// bad shape class?
	DBG_ASSERT(0);
	return 0;
}

/////////////////////////////////////////////////////
/// Method: CreatePhysics
/// Params: None
///
/////////////////////////////////////////////////////
void Player::CreatePhysics( bool create )
{
	unsigned int i = 0;

	dMatrix3 mat;
	dQuaternion qGeomOri, qFinalOri;

	DBG_ASSERT( m_PhysicsData != 0 );

		if( m_PhysicsData != 0 )
		{
			// Box Geoms
			for( i=0; i < m_PhysicsData->totalPhysicsBox; ++i )
			{
				dGeomID geom = 0;
				if( create )
				{
					geom = CreateGeom( dBoxClass, m_EntitySpace, m_PhysicsData->boxList[i].dims );
					DBG_ASSERT_MSG( (geom != 0), "Could not create ode box" );

					m_PhysicsData->boxList[i].geomId = geom;
				}
				else
					geom = m_PhysicsData->boxList[i].geomId;

				// rotate the placement point of each geom
				math::Vec3 placePoint = math::Vec3( m_PhysicsData->boxList[i].pos[0], m_PhysicsData->boxList[i].pos[1], m_PhysicsData->boxList[i].pos[2]);
				math::Vec3 finalPoint = zeroVec;

				finalPoint.X = (placePoint.X * m_Orientation[0]) + (placePoint.Y * m_Orientation[1]) + (placePoint.Z * m_Orientation[2]) + m_Pos.X;
				finalPoint.Y = (placePoint.X * m_Orientation[4]) + (placePoint.Y * m_Orientation[5]) + (placePoint.Z * m_Orientation[6]) + m_Pos.Y;
				finalPoint.Z = (placePoint.X * m_Orientation[8]) + (placePoint.Y * m_Orientation[9]) + (placePoint.Z * m_Orientation[10]) + m_Pos.Z;

				dGeomSetPosition( geom, finalPoint.X, finalPoint.Y, finalPoint.Z );

				dRSetIdentity( mat );	
				dRFromEulerAngles( mat, 
									m_PhysicsData->boxList[i].rot.X, 
									m_PhysicsData->boxList[i].rot.Y, 
									m_PhysicsData->boxList[i].rot.Z );

				// rotate the geom
				dQSetIdentity( qGeomOri );
				dQfromR( qGeomOri, mat );

				// multiply by the geom orientation
				dQMultiply0( qFinalOri, m_qPlaneOri , qGeomOri );

				dGeomSetQuaternion( geom, qFinalOri );

				dGeomSetData( geom, this );	
				dGeomSetCategoryBits( geom, m_CategoryBits );
				dGeomSetCollideBits( geom, m_CollideBits );

				dReal aabb[6];
				dGeomGetAABB( geom, aabb );

				// The aabb array has elements (minx, maxx, miny, maxy, minz, maxz)
				math::Vec3 minPoint( aabb[0], aabb[2], aabb[4] );
				math::Vec3 maxPoint( aabb[1], aabb[3], aabb[5] );

				collision::AABB AABB;
				AABB.vBoxMin.X = minPoint.X;
				AABB.vBoxMin.Y = minPoint.Y;
				AABB.vBoxMin.Z = minPoint.Z;

				AABB.vBoxMax.X = maxPoint.X;
				AABB.vBoxMax.Y = maxPoint.Y;
				AABB.vBoxMax.Z = maxPoint.Z;

				AABB.vCenter = (minPoint + maxPoint)*0.5f;

				m_AABB += AABB;
			}

			// Sphere Geoms
			for( i=0; i < m_PhysicsData->totalPhysicsSphere; ++i )
			{
				dGeomID geom = 0;
				if( create )
				{
					geom = CreateGeom( dSphereClass, m_EntitySpace, zeroVec, m_PhysicsData->sphereList[i].radius );
					DBG_ASSERT_MSG( (geom != 0), "Could not create ode sphere" );

					m_PhysicsData->sphereList[i].geomId = geom;
				}
				else
					geom = m_PhysicsData->sphereList[i].geomId;
		
				// rotate the placement point of each geom
				math::Vec3 placePoint = math::Vec3( m_PhysicsData->sphereList[i].pos[0], m_PhysicsData->sphereList[i].pos[1], m_PhysicsData->sphereList[i].pos[2]);
				math::Vec3 finalPoint = zeroVec;

				finalPoint.X = (placePoint.X * m_Orientation[0]) + (placePoint.Y * m_Orientation[1]) + (placePoint.Z * m_Orientation[2]) + m_Pos.X;
				finalPoint.Y = (placePoint.X * m_Orientation[4]) + (placePoint.Y * m_Orientation[5]) + (placePoint.Z * m_Orientation[6]) + m_Pos.Y;
				finalPoint.Z = (placePoint.X * m_Orientation[8]) + (placePoint.Y * m_Orientation[9]) + (placePoint.Z * m_Orientation[10]) + m_Pos.Z;

				dGeomSetPosition( geom, finalPoint.X, finalPoint.Y, finalPoint.Z );

				dRSetIdentity( mat );	

				// rotate the geom
				dQSetIdentity( qGeomOri );
				dQfromR( qGeomOri, mat );

				// multiply by the geom orientation
				dQMultiply0( qFinalOri, m_qPlaneOri , qGeomOri );

				dGeomSetQuaternion( geom, qFinalOri );

				dGeomSetData( geom, this );
				dGeomSetCategoryBits( geom, m_CategoryBits );
				dGeomSetCollideBits( geom, m_CollideBits );

				dReal aabb[6];
				dGeomGetAABB( geom, aabb );

				// The aabb array has elements (minx, maxx, miny, maxy, minz, maxz)
				math::Vec3 minPoint( aabb[0], aabb[2], aabb[4] );
				math::Vec3 maxPoint( aabb[1], aabb[3], aabb[5] );

				collision::AABB AABB;
				AABB.vBoxMin.X = minPoint.X;
				AABB.vBoxMin.Y = minPoint.Y;
				AABB.vBoxMin.Z = minPoint.Z;

				AABB.vBoxMax.X = maxPoint.X;
				AABB.vBoxMax.Y = maxPoint.Y;
				AABB.vBoxMax.Z = maxPoint.Z;

				AABB.vCenter = (minPoint + maxPoint)*0.5f;

				m_AABB += AABB;
			}

			// Capsule Geoms
			for( i=0; i < m_PhysicsData->totalPhysicsCapsule; ++i )
			{
				dGeomID geom = 0;
				if( create )
				{
					geom = CreateGeom( dCapsuleClass, m_EntitySpace, zeroVec, m_PhysicsData->capsuleList[i].radius, m_PhysicsData->capsuleList[i].length );
					DBG_ASSERT_MSG( (geom != 0), "Could not create ode capsule" );

					m_PhysicsData->capsuleList[i].geomId = geom;
				}
				else
					geom = m_PhysicsData->capsuleList[i].geomId;

				// rotate the placement point of each geom
				math::Vec3 placePoint = math::Vec3( m_PhysicsData->capsuleList[i].pos[0], m_PhysicsData->capsuleList[i].pos[1], m_PhysicsData->capsuleList[i].pos[2]);
				math::Vec3 finalPoint = zeroVec;

				finalPoint.X = (placePoint.X * m_Orientation[0]) + (placePoint.Y * m_Orientation[1]) + (placePoint.Z * m_Orientation[2]) + m_Pos.X;
				finalPoint.Y = (placePoint.X * m_Orientation[4]) + (placePoint.Y * m_Orientation[5]) + (placePoint.Z * m_Orientation[6]) + m_Pos.Y;
				finalPoint.Z = (placePoint.X * m_Orientation[8]) + (placePoint.Y * m_Orientation[9]) + (placePoint.Z * m_Orientation[10]) + m_Pos.Z;

				dGeomSetPosition( geom, finalPoint.X, finalPoint.Y, finalPoint.Z );

				dRSetIdentity( mat );	
				dRFromEulerAngles( mat, 
									m_PhysicsData->capsuleList[i].rot.X, 
									m_PhysicsData->capsuleList[i].rot.Y, 
									m_PhysicsData->capsuleList[i].rot.Z );

				// rotate the geom
				dQSetIdentity( qGeomOri );
				dQfromR( qGeomOri, mat );

				// multiply by the geom orientation
				dQMultiply0( qFinalOri, m_qPlaneOri , qGeomOri );

				dGeomSetQuaternion( geom, qFinalOri );

				dGeomSetData( geom, this );
				dGeomSetCategoryBits( geom, m_CategoryBits );
				dGeomSetCollideBits( geom, m_CollideBits );

				dReal aabb[6];
				dGeomGetAABB( geom, aabb );

				// The aabb array has elements (minx, maxx, miny, maxy, minz, maxz)
				math::Vec3 minPoint( aabb[0], aabb[2], aabb[4] );
				math::Vec3 maxPoint( aabb[1], aabb[3], aabb[5] );

				collision::AABB AABB;
				AABB.vBoxMin.X = minPoint.X;
				AABB.vBoxMin.Y = minPoint.Y;
				AABB.vBoxMin.Z = minPoint.Z;

				AABB.vBoxMax.X = maxPoint.X;
				AABB.vBoxMax.Y = maxPoint.Y;
				AABB.vBoxMax.Z = maxPoint.Z;

				AABB.vCenter = (minPoint + maxPoint)*0.5f;

				m_AABB += AABB;
			}

			// Cylinder Geoms
			for( i=0; i < m_PhysicsData->totalPhysicsCylinder; ++i )
			{
				dGeomID geom = 0;
				if( create )
				{
					geom = CreateGeom( dCylinderClass, m_EntitySpace, zeroVec, m_PhysicsData->cylinderList[i].radius, m_PhysicsData->cylinderList[i].length );
					DBG_ASSERT_MSG( (geom != 0), "Could not create ode cylinder" );

					m_PhysicsData->cylinderList[i].geomId = geom;
				}
				else
					geom = m_PhysicsData->cylinderList[i].geomId;


				// rotate the placement point of each geom
				math::Vec3 placePoint = math::Vec3( m_PhysicsData->cylinderList[i].pos[0], m_PhysicsData->cylinderList[i].pos[1], m_PhysicsData->cylinderList[i].pos[2]);
				math::Vec3 finalPoint = zeroVec;

				finalPoint.X = (placePoint.X * m_Orientation[0]) + (placePoint.Y * m_Orientation[1]) + (placePoint.Z * m_Orientation[2]) + m_Pos.X;
				finalPoint.Y = (placePoint.X * m_Orientation[4]) + (placePoint.Y * m_Orientation[5]) + (placePoint.Z * m_Orientation[6]) + m_Pos.Y;
				finalPoint.Z = (placePoint.X * m_Orientation[8]) + (placePoint.Y * m_Orientation[9]) + (placePoint.Z * m_Orientation[10]) + m_Pos.Z;

				dGeomSetPosition( geom, finalPoint.X, finalPoint.Y, finalPoint.Z );

				dRSetIdentity( mat );	
				dRFromEulerAngles( mat, 
									m_PhysicsData->cylinderList[i].rot.X, 
									m_PhysicsData->cylinderList[i].rot.Y, 
									m_PhysicsData->cylinderList[i].rot.Z );

				// rotate the geom
				dQSetIdentity( qGeomOri );
				dQfromR( qGeomOri, mat );

				// multiply by the geom orientation
				dQMultiply0( qFinalOri, m_qPlaneOri , qGeomOri );

				dGeomSetQuaternion( geom, qFinalOri );

				dGeomSetData( geom, this );
				dGeomSetCategoryBits( geom, m_CategoryBits );
				dGeomSetCollideBits( geom, m_CollideBits );

				dReal aabb[6];
				dGeomGetAABB( geom, aabb );

				// The aabb array has elements (minx, maxx, miny, maxy, minz, maxz)
				math::Vec3 minPoint( aabb[0], aabb[2], aabb[4] );
				math::Vec3 maxPoint( aabb[1], aabb[3], aabb[5] );

				collision::AABB AABB;
				AABB.vBoxMin.X = minPoint.X;
				AABB.vBoxMin.Y = minPoint.Y;
				AABB.vBoxMin.Z = minPoint.Z;

				AABB.vBoxMax.X = maxPoint.X;
				AABB.vBoxMax.Y = maxPoint.Y;
				AABB.vBoxMax.Z = maxPoint.Z;

				AABB.vCenter = (minPoint + maxPoint)*0.5f;

				m_AABB += AABB;
			}

			// triMesh Geoms
			for( i=0; i < m_PhysicsData->totalPhysicsTriMesh; ++i )
			{
				dGeomID geom = 0;
				if( create )
				{
					m_PhysicsData->triMeshList[i].triMeshDataID = dGeomTriMeshDataCreate();
					geom = CreateGeom( dTriMeshClass, m_EntitySpace, zeroVec, 0.0f, 0.0f, m_PhysicsData->triMeshList[i].triMeshDataID );
					DBG_ASSERT_MSG( (geom != 0), "Could not create tri-mesh" );

					//dGeomTriMeshDataBuildSingle( m_PhysicsData->triMeshList[i].triMeshDataID, m_PhysicsData->triMeshList[i].triangleData, sizeof(math::Vec3), m_PhysicsData->triMeshList[i].totalMeshTriangles*3, m_PhysicsData->triMeshList[i].indices, m_PhysicsData->triMeshList[i].totalMeshTriangles*3, sizeof(int) );
					dGeomTriMeshDataBuildSingle1( m_PhysicsData->triMeshList[i].triMeshDataID, m_PhysicsData->triMeshList[i].triangleData, sizeof(math::Vec3), m_PhysicsData->triMeshList[i].totalMeshTriangles*3, m_PhysicsData->triMeshList[i].indices, m_PhysicsData->triMeshList[i].totalMeshTriangles*3, sizeof(int), m_PhysicsData->triMeshList[i].normals );

					m_PhysicsData->triMeshList[i].geomId = geom;
				}
				else
					geom = m_PhysicsData->triMeshList[i].geomId;

				dGeomSetPosition( geom, m_PhysicsData->triMeshList[i].pos[0] + m_Pos.X, m_PhysicsData->triMeshList[i].pos[1] + m_Pos.Y, m_PhysicsData->triMeshList[i].pos[2] + m_Pos.Z );
		
				dRSetIdentity( mat );	
				dRFromEulerAngles( mat, 
									m_PhysicsData->triMeshList[i].rot.X, 
									m_PhysicsData->triMeshList[i].rot.Y, 
									m_PhysicsData->triMeshList[i].rot.Z );

				// rotate the geom
				dQSetIdentity( qGeomOri );
				dQfromR( qGeomOri, mat );

				// multiply by the geom orientation
				dQMultiply0( qFinalOri, m_qPlaneOri , qGeomOri );

				dGeomSetQuaternion( geom, qFinalOri );

				dGeomSetData( geom, this );
				dGeomSetCategoryBits( geom, m_CategoryBits );
				dGeomSetCollideBits( geom, m_CollideBits );

				dReal aabb[6];
				dGeomGetAABB( geom, aabb );

				// The aabb array has elements (minx, maxx, miny, maxy, minz, maxz)
				math::Vec3 minPoint( aabb[0], aabb[2], aabb[4] );
				math::Vec3 maxPoint( aabb[1], aabb[3], aabb[5] );

				collision::AABB AABB;
				AABB.vBoxMin.X = minPoint.X;
				AABB.vBoxMin.Y = minPoint.Y;
				AABB.vBoxMin.Z = minPoint.Z;

				AABB.vBoxMax.X = maxPoint.X;
				AABB.vBoxMax.Y = maxPoint.Y;
				AABB.vBoxMax.Z = maxPoint.Z;

				AABB.vCenter = (minPoint + maxPoint)*0.5f;

				m_AABB += AABB;
			}
		}
}

/////////////////////////////////////////////////////
/// Method: CalculateRudderAndFlaps
/// Params: 
///
/////////////////////////////////////////////////////
void Player::CalculateRudderAndFlaps()
{
	// RUDDER
	if( m_RudderData.currentRot.Y > m_RudderData.maxRot.Y )
		m_RudderData.currentRot.Y = m_RudderData.maxRot.Y;

	if( m_RudderData.currentRot.Y < -m_RudderData.maxRot.Y )
		m_RudderData.currentRot.Y = -m_RudderData.maxRot.Y;

	// LEFT TAIL FLAP
	if( m_LeftTailFlapData.currentRot.X > m_LeftTailFlapData.maxRot.X )
		m_LeftTailFlapData.currentRot.X = m_LeftTailFlapData.maxRot.X;

	if( m_LeftTailFlapData.currentRot.X < -m_LeftTailFlapData.maxRot.X )
		m_LeftTailFlapData.currentRot.X = -m_LeftTailFlapData.maxRot.X;

	// RIGHT TAIL FLAP
	if( m_RightTailFlapData.currentRot.X > m_RightTailFlapData.maxRot.X )
		m_RightTailFlapData.currentRot.X = m_RightTailFlapData.maxRot.X;

	if( m_RightTailFlapData.currentRot.X < -m_RightTailFlapData.maxRot.X )
		m_RightTailFlapData.currentRot.X = -m_RightTailFlapData.maxRot.X;

	// LEFT WING FLAP
	if( m_LeftWingFlapData.currentRot.X > m_LeftWingFlapData.maxRot.X )
		m_LeftWingFlapData.currentRot.X = m_LeftWingFlapData.maxRot.X;

	if( m_LeftWingFlapData.currentRot.X < -m_LeftWingFlapData.maxRot.X )
		m_LeftWingFlapData.currentRot.X = -m_LeftWingFlapData.maxRot.X;

	// RIGHT WING FLAP
	if( m_RightWingFlapData.currentRot.X > m_RightWingFlapData.maxRot.X )
		m_RightWingFlapData.currentRot.X = m_RightWingFlapData.maxRot.X;

	if( m_RightWingFlapData.currentRot.X < -m_RightWingFlapData.maxRot.X )
		m_RightWingFlapData.currentRot.X = -m_RightWingFlapData.maxRot.X;
}

/////////////////////////////////////////////////////
/// Function: PhysicsTestPlayerToWorld
/// Params: [in]data, [in]o1, [in]o2
///
/////////////////////////////////////////////////////
void PhysicsTestPlayerToWorld (void *data, dGeomID o1, dGeomID o2)
{
	int i=0;

	dContact contact[MAX_CONTACTS];

	// run the collision
	int numCollisions = dCollide( o1, o2, MAX_CONTACTS, &contact[0].geom, sizeof(dContact) );

	if( numCollisions <= 0 ) 
		return;

	physics::PhysicsIdentifier* pGeomData1 = reinterpret_cast<physics::PhysicsIdentifier*>(dGeomGetData(o1));
	physics::PhysicsIdentifier* pGeomData2 = reinterpret_cast<physics::PhysicsIdentifier*>(dGeomGetData(o2));

	// check for a contact point between a ray and selectable
	DBG_ASSERT( (pGeomData1 != 0) && (pGeomData2 != 0) );

	RayObject* pRay = 0;

	// only want world and rays
	if( (pGeomData1->GetBaseId() == PHYSICSBASICID_RAYTESTER || 
		pGeomData2->GetBaseId() == PHYSICSBASICID_RAYTESTER) ) 
	{
		for( i=0; i < 1; ++i )
		{
			bool newHit = false;

			if( pGeomData1->GetBaseId() == PHYSICSBASICID_RAYTESTER )
			{
				pRay = reinterpret_cast<RayObject*>(dGeomGetData(o1)); 

				if( pRay->totalHits < MAX_RAY_HITS )
				{
					pRay->rayHits[pRay->nextHitIndex].pos = math::Vec3( contact[i].geom.pos[0], contact[i].geom.pos[1], contact[i].geom.pos[2] );
					pRay->rayHits[pRay->nextHitIndex].otherGeom = o2;
					pRay->rayHits[pRay->nextHitIndex].n = math::Vec3( contact[i].geom.normal[0], contact[i].geom.normal[1], contact[i].geom.normal[2] );
					pRay->rayHits[pRay->nextHitIndex].pData = dGeomGetData(o2);

					//if( FacingCamera( math::Normalise(pRay->rayDir), pRay->rayHits[pRay->nextHitIndex].n ) )
					{
						newHit = true;

						pRay->hasHits = true;
						pRay->totalHits++;

						math::Vec3 distVec = pRay->rayHits[pRay->nextHitIndex].pos - pRay->rayPos;
						pRay->rayHits[pRay->nextHitIndex].distance = distVec.length();
					}
				}
			}
			else
			{
				pRay = reinterpret_cast<RayObject*>(dGeomGetData(o2)); 

				if( pRay->totalHits < MAX_RAY_HITS )
				{
					pRay->rayHits[pRay->nextHitIndex].pos = math::Vec3( contact[i].geom.pos[0], contact[i].geom.pos[1], contact[i].geom.pos[2] );
					pRay->rayHits[pRay->nextHitIndex].otherGeom = o1;
					pRay->rayHits[pRay->nextHitIndex].n = math::Vec3( contact[i].geom.normal[0], contact[i].geom.normal[1], contact[i].geom.normal[2] );
					pRay->rayHits[pRay->nextHitIndex].pData = dGeomGetData(o1);

					//if( FacingCamera( math::Normalise(pRay->rayDir), pRay->rayHits[pRay->nextHitIndex].n ) )
					{
						newHit = true;

						pRay->hasHits = true;
						pRay->totalHits++;

						math::Vec3 distVec = pRay->rayHits[pRay->nextHitIndex].pos - pRay->rayPos;
						pRay->rayHits[pRay->nextHitIndex].distance = distVec.length();
					}
				}
			}

			if( newHit == true )
				pRay->nextHitIndex++;

			if( pRay->nextHitIndex >= MAX_RAY_HITS )
				pRay->nextHitIndex = 0;
		}
	}
	else if( (pGeomData1->GetBaseId() == PHYSICSBASICID_BULLET || 
		pGeomData2->GetBaseId() == PHYSICSBASICID_BULLET) )
	{
		for( i=0; i < 1; ++i )
		{
			// cast to the bullet
			if( pGeomData1->GetBaseId() == PHYSICSBASICID_BULLET )
			{
				Player::Bullet* pBullet = reinterpret_cast<Player::Bullet*>(dGeomGetData(o1));
				if( pBullet->active &&
					!pBullet->stateChange )
				{
					pBullet->stateChange = true;
					pBullet->vPos = math::Vec3( contact[0].geom.pos[0], contact[0].geom.pos[1], contact[0].geom.pos[2] );
					pBullet->doEffect = true;
					if( pGeomData2->GetBaseId() == PHYSICSBASICID_WORLDOBJECT )
					{
						WorldObject* pObj = reinterpret_cast<WorldObject*>(dGeomGetData(o2));

						if( !pObj->IsDestroyed() && pObj->CanDestroy() )
							AudioSystem::GetInstance()->PlayAudio( bulletHit, zeroVec, AL_TRUE, AL_FALSE, 0.5f, 0.1f );
						else
							AudioSystem::GetInstance()->PlayAudio( bulletMiss, zeroVec, AL_TRUE, AL_FALSE, 0.5f, 0.1f );

						if( !pObj->IsDestroyed() && pObj->CanDestroy() )
						{
							int damage = pObj->GetDamage();
							if( damage > 0 )
								damage--;
							pObj->SetDamage( damage );

							pBullet->doEffect = false;
						}
					}
				}
				
				DBGLOG( "BULLET CONTACT: %d\n", pBullet->index );
			}
			else if( pGeomData2->GetBaseId() == PHYSICSBASICID_BULLET )
			{
				Player::Bullet* pBullet = reinterpret_cast<Player::Bullet*>(dGeomGetData(o2));
				if( pBullet->active &&
					!pBullet->stateChange)
				{
					pBullet->stateChange = true;
					pBullet->vPos = math::Vec3( contact[0].geom.pos[0], contact[0].geom.pos[1], contact[0].geom.pos[2] );
					pBullet->doEffect = true;

					if( pGeomData1->GetBaseId() == PHYSICSBASICID_WORLDOBJECT )
					{
						WorldObject* pObj = reinterpret_cast<WorldObject*>(dGeomGetData(o1));

						if( !pObj->IsDestroyed() && pObj->CanDestroy() )
							AudioSystem::GetInstance()->PlayAudio( bulletHit, zeroVec, AL_TRUE, AL_FALSE, 0.5f, 0.1f );
						else
							AudioSystem::GetInstance()->PlayAudio( bulletMiss, zeroVec, AL_TRUE, AL_FALSE, 0.5f, 0.1f );

						if( !pObj->IsDestroyed() && pObj->CanDestroy() )
						{
							int damage = pObj->GetDamage();
							if( damage > 0 )
								damage--;
							pObj->SetDamage( damage );

							pBullet->doEffect = false;
						}
					}
				}

				DBGLOG( "BULLET CONTACT: %d\n", pBullet->index );
			}
			else
				DBGLOG( ">>>>>>>>>>>>>\n" );
		}
	}
	else if( (pGeomData1->GetBaseId() == PHYSICSBASICID_PLAYER || 
		pGeomData2->GetBaseId() == PHYSICSBASICID_PLAYER) )
	{
		// 
		for( i=0; i < 1; ++i )
		{
			// cast to the player
			if( pGeomData1->GetBaseId() == PHYSICSBASICID_PLAYER )
			{
				Player* pPlayer = reinterpret_cast<Player*>(dGeomGetData(o1));
				//if(pPlayer->I )
				{
					if( pGeomData2->GetBaseId() == PHYSICSBASICID_WORLDOBJECT )
					{
						WorldObject* pObj = reinterpret_cast<WorldObject*>(dGeomGetData(o2));

						if( /*!pObj->IsDestroyed() &&*/ !pPlayer->IsDead() )
						{
							DBGLOG( "PLAYER TO WORLDOBJECT\n" );

							pPlayer->PlayerCrashed();

							if( !pObj->IsDestroyed() && pObj->CanDestroy() )
								pObj->SetDamage( 0 );
						}
					}
					else if( pGeomData2->GetBaseId() == PHYSICSBASICID_WORLD )
					{
						if( !pPlayer->HasWorldCollision() && !pPlayer->IsDead() )
						{
							pPlayer->PlayerCrashed();

							DBGLOG( "PLAYER TO WORLD1\n" );
							/*math::Vec3 normal = math::Vec3( contact[i].geom.normal[0], -contact[i].geom.normal[1], contact[i].geom.normal[2] );
							math::Vec3 dir = pPlayer->GetDirection();
							math::Vec3 angles = pPlayer->GetAngles();
							math::Vec3 playerPos = pPlayer->GetPosition();
							playerPos.Y = contact[i].geom.pos[1];

							//-2*(V dot N)*N + V 

							float angle = -2.0f * math::DotProduct(normal, dir);
							DBG_ASSERT( angle != 0 );
							math::Vec3 newVec = (normal + dir) * angle;

							newVec.normalise();
							dir.Y = -dir.Y;
							pPlayer->SetDirection( newVec );
							angles.X = -angles.X;
							pPlayer->SetAngles(angles);
							pPlayer->SetPosition(playerPos);
							pPlayer->SetWorldCollision( true );*/
						}
					}
				}
			}
			else if( pGeomData2->GetBaseId() == PHYSICSBASICID_PLAYER )
			{
				Player* pPlayer = reinterpret_cast<Player*>(dGeomGetData(o2));
				//if( )
				{
					if( pGeomData1->GetBaseId() == PHYSICSBASICID_WORLDOBJECT )
					{
						WorldObject* pObj = reinterpret_cast<WorldObject*>(dGeomGetData(o1));

						if( /*!pObj->IsDestroyed() &&*/ !pPlayer->IsDead() )
						{
							DBGLOG( "PLAYER TO WORLDOBJECT\n" );

							pPlayer->PlayerCrashed();

							if( !pObj->IsDestroyed() && pObj->CanDestroy() )
								pObj->SetDamage( 0 );
						}
					}
					else if( pGeomData1->GetBaseId() == PHYSICSBASICID_WORLD )
					{	
						if( !pPlayer->HasWorldCollision() && !pPlayer->IsDead() )
						{
							pPlayer->PlayerCrashed();

							DBGLOG( "PLAYER TO WORLD2\n" );
							/*math::Vec3 normal = math::Vec3( contact[i].geom.normal[0], -contact[i].geom.normal[1], contact[i].geom.normal[2] );
							math::Vec3 dir = pPlayer->GetDirection();
							math::Vec3 angles = pPlayer->GetAngles();
							math::Vec3 playerPos = pPlayer->GetPosition();
							playerPos.Y = contact[i].geom.pos[1];

							//-2*(V dot N)*N + V 

							float angle = -2.0f * math::DotProduct(normal, dir);
							DBG_ASSERT( angle != 0 );
							math::Vec3 newVec = (normal + dir) * angle;

							newVec.normalise();
							dir.Y = -dir.Y;
							pPlayer->SetDirection( newVec );
							angles.X = -angles.X;
							pPlayer->SetAngles(angles);
							pPlayer->SetPosition(playerPos);
							pPlayer->SetWorldCollision( true );*/
						}
					}
				}
			}
		}
	}
}
