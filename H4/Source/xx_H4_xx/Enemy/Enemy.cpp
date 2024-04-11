
/*===================================================================
	File: Enemy.cpp
	Game: H4

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"

#include "MathBase.h"
#include "RenderBase.h"
#include "SoundBase.h"
#include "CollisionBase.h"
#include "ModelBase.h"
#include "InputBase.h"
#include "ScriptBase.h"
#include "PhysicsBase.h"

#include "H4.h"
#include "AppConsts.h"

#include "Physics/PhysicsContact.h"
#include "Player/Player.h"

#include "Resources/SoundResources.h"
#include "Resources/SpriteResources.h"
#include "Resources/EmitterResources.h"
#include "Resources/TextureResources.h"

#include "ScriptAccess/ScriptDataHolder.h"
#include "ScriptAccess/ScriptAccess.h"

#include "GameSystems.h"

#include "Enemy/EnemyAnimationLookup.h"
#include "Enemy/Enemy.h"

namespace
{
	const float ROTATION_INCREMENT = 60.0f;
	const float ANGLE_TOLERANCE = 5.0f;

	const float CULLING_RADIUS = 1.2f;

	const float SEARCH_TIME_MIN = 1.0f;
	const float SEARCH_TIME_MAX = 2.0f;

	const float MAX_WALK_SPEED = 0.4f;
	const float MAX_RUN_SPEED = 1.8f;

	const int CHASE_COUNT = 3;

	const float DEAD_FADE_TIME = 8.0f;
	const float DEAD_SINK_SPEED = 0.1f;
	const float DEAD_SINK_STOP = -1.0f;

	const int BOUNDING_JOINT_INDEX = 1;

	const float PLAYER_DAMAGE_DISTANCE = 4.1f;
	const int PUNCH_FRAME = 17;
	const int HEADBUTT_FRAME = 10;
	const int DEATH_IMPACT_FRAME = 22;

	b2Vec2 zeroVec2(0.0f,0.0f);

	const int MESH_START = 0;
	const int MESH_END = 38;

	const float MOVE_MULTIPLY = 300.0f;

#ifdef _DEBUG
	math::Vec3 vaPoints[256];
	int pointIndex = 0;
#endif // _DEBUG

}

int Stump_Neck = 0;
int Stump_Head = 1;
int Stump_Shoulder_L = 2;
int Stump_Arm_L = 3;
int Stump_Wrist_L = 4;
int Stump_Elbow_L = 5;
int Stump_Leg_L = 6;
int Stump_Hip_L = 7;
int Stump_Shin_L = 8;
int Stump_Knee_L = 9;
int Stump_Shoulder_R = 10;
int Stump_Arm_R = 11;
int Stump_Wrist_R = 12;
int Stump_Elbow_R = 13;
int Stump_Leg_R = 14;
int Stump_Hip_R = 15;
int Stump_Shin_R = 16;
int Stump_Knee_R = 17;

int Skeleton_Head = 18;
int Skeleton_Body = 19;
int Skeleton_Arm_R = 20;
int Skeleton_Wrist_R = 21;
int Skeleton_Arm_L = 22;
int Skeleton_Wrist_L = 23;
int Skeleton_Pelvis = 24;
int Skeleton_Hip_R = 25;
int Skeleton_Hip_L = 26;
int Skeleton_Shin_R = 27;
int Skeleton_Shin_L = 28;

int Normal_Head = 29;
int Normal_Body = 30;
int Normal_Arm_R = 31;
int Normal_Wrist_R = 32;
int Normal_Arm_L = 33;
int Normal_Wrist_L = 34;
int Normal_Hip_R = 35;
int Normal_Hip_L = 36;
int Normal_Shin_R = 37;
int Normal_Shin_L = 38;

int mShadow = 39;


/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
Enemy::Enemy( NavigationMesh* pNavMesh )
	: PhysicsIdentifier( PHYSICSBASICID_ENEMY, PHYSICSCASTID_ENEMY ) 
	, NavObject( pNavMesh )
{
	int i=0;

	m_pModelData = 0;
	m_pPrimaryAnim = 0;
	m_pPolledAnim = 0;
	m_JointCount = 0;
	m_pJoints = 0;

	m_State = EnemyState_InActive;
	m_DrawState = true;
	
	m_Pos = math::Vec3( 0.0f, 0.0f, 0.0f );
	m_PrevPos = m_Pos;
	m_Movement = math::Vec3(0.0f,0.0f,0.0f);
	m_Dir = math::Vec3( 0.0f, 0.0f, 1.0f );

	m_BoundSphere.vCenterPoint = math::Vec3( 0.0f, 0.0f, 0.0f );
	m_BoundSphere.fRadius = CULLING_RADIUS;

	m_UnProjectedPoint = math::Vec2( 0.0f, 0.0f );

	m_MaxSpeed = MAX_WALK_SPEED;
	m_TargetRotation = 0.0f;
	m_RotationAngle = 0.0f;

	m_LastDeltaTime = 0.0f;

	m_NewWaypoint = false;
	m_NavMeshEnable = false;
	m_SwitchToNavMesh = false;

	m_AddKillFlag = false;
	m_DeadFadeTimer = DEAD_FADE_TIME;
	m_DeadAlpha = 255;

	m_SpawnNewGib = false;
	m_NextGibIndex = 0;
	for( i=0; i < MAX_ENEMY_GIBS; ++i )
	{
		m_Gibs[i] = 0;
		m_Gibs[i] = new Gib( );
	}

	m_LostPartCount = 0;
	for( i=0; i < MAX_ENEMY_LOST_PARTS; ++i )
	{
		m_LostParts[i] = -1;
	}

	for( i=0; i < MAX_ENEMY_EMITTERS; ++i )
		m_pEmitters[i] = 0;

	m_LightZoneIndex = -1;

	m_SpottedPlayerAudio = snd::INVALID_SOUNDBUFFER;
	m_HitPlayerAudio = snd::INVALID_SOUNDBUFFER;
	m_MissPlayerAudio = snd::INVALID_SOUNDBUFFER;
	m_DeathImpactPlayerAudio = snd::INVALID_SOUNDBUFFER;
	m_HitMissAudioStarted = false;
	m_ImpactAudioStarted = false;
}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
Enemy::~Enemy()
{
	int i=0;

	if( H4::GetHiResMode() )
	{
		renderer::RemoveTexture( m_SkinTextureIndex );
	}

	m_pModelData = 0;

	for( i=0; i < MAX_ENEMY_GIBS; ++i )
	{
		if( m_Gibs[i] != 0 )
		{
			delete m_Gibs[i];
			m_Gibs[i] = 0;
		}
	}

	for( i=0; i < MAX_ENEMY_EMITTERS; ++i )
	{
		if( m_pEmitters[i] != 0 )
		{
			delete m_pEmitters[i];
			m_pEmitters[i] = 0;
		}
	}

	if( m_pJoints != 0 )
	{
		delete[] m_pJoints;
		m_pJoints = 0;
	}

	if( m_pBody != 0 )
	{
		if( physics::PhysicsWorldB2D::GetWorld() )
			physics::PhysicsWorldB2D::GetWorld()->DestroyBody(m_pBody);

		m_pBody = 0;
	}

	snd::RemoveSound( m_SpottedPlayerAudio );
	snd::RemoveSound( m_HitPlayerAudio );
	snd::RemoveSound( m_MissPlayerAudio );
	snd::RemoveSound( m_DeathImpactPlayerAudio );
}

/////////////////////////////////////////////////////
/// Method: SetupEnemy
/// Params: [in]model, [in]smallGib, [in]largeGib, [in]skinIndex
///
/////////////////////////////////////////////////////
void Enemy::SetupEnemy( mdl::ModelHGA* model, mdl::ModelHGM* smallGib, mdl::ModelHGM* largeGib, GLuint skinIndex )
{
	m_pModelData = model;
	m_pSmallGib = smallGib;
	m_pLargeGib = largeGib;
	m_SkinTextureIndex = skinIndex;
	m_Health = ENEMY_HEALTH;

	// take a joint copy
	unsigned int i=0;
	m_JointCount = m_pModelData->GetNumBoneJoints();
	m_pJoints = new mdl::TBoneJoint[m_JointCount];
	for( i=0; i < m_JointCount; ++i )
	{
		m_pJoints[i] = m_pModelData->GetBoneJointsPtr()[i];
	}

	b2BodyDef bd;
	bd.type = b2_dynamicBody;
	bd.gravityScale = 0.0f;
	bd.awake = true;
	bd.fixedRotation = true;
	bd.linearDamping = 50.0f;
	
	// setup physics
	m_pBody = physics::PhysicsWorldB2D::GetWorld()->CreateBody(&bd);
	DBG_ASSERT_MSG( (m_pBody != 0), "Could not create enemy physics body" );

	b2CircleShape circleShape;
	b2FixtureDef fd;

	fd.friction = 0.5f;
	fd.restitution = 0.2f;
	fd.density = 50.0f;

	fd.filter.categoryBits = ENEMY_CATEGORY;
	fd.filter.maskBits = WORLDHIGH_CATEGORY | WORLDLOW_CATEGORY | ENEMY_CATEGORY | PLAYER_CATEGORY | BULLETLOW_CATEGORY;
	fd.filter.groupIndex = ENEMY_GROUP;

	circleShape.m_p = b2Vec2( 0.0f, 0.0f );
	circleShape.m_radius = 0.45f;

	fd.shape = &circleShape;

	b2Fixture* pFixture = 0;
	pFixture = m_pBody->CreateFixture( &fd );
	pFixture->SetUserData( reinterpret_cast<void *>(this) );

	// mass setup
	b2MassData massData; 
	massData.mass = 0.1f;
	massData.center = b2Vec2( 0.0f, 0.0f );
	massData.I = 1.0f;
	
	m_pBody->SetMassData( &massData );

	m_pBody->SetUserData( reinterpret_cast<void *>(this) );

	// ray data
	m_CanSeePlayer = false;
	m_RayContact.SetZero();
	m_RayNormal.SetZero();

	m_SearchTime = math::RandomNumber( SEARCH_TIME_MIN, SEARCH_TIME_MAX );
	m_CurrentSearchTime = 0.0f;

	m_ContactWithPlayer = false;

	m_NewWaypoint = false;
	m_NavMeshEnable = false;
	m_SwitchToNavMesh = false;
	m_DoorCollision = false;

	// emitters
	const res::EmitterResourceStore* er = 0;
	renderer::Texture texLoad;

	for( i=0; i < MAX_ENEMY_EMITTERS; ++i )
	{
		er = res::GetEmitterResource( math::RandomNumber(2000,2004) );
		DBG_ASSERT( er != 0 );

		m_pEmitters[i] = new efx::Emitter;
		DBG_ASSERT( (m_pEmitters[i] != 0) );

		m_pEmitters[i]->Create( *er->block );
		math::Vec3 zeroVec( 0.0f, 0.0f, 0.0f );
		m_pEmitters[i]->SetPos( zeroVec );
		m_pEmitters[i]->SetRot( zeroVec );

		m_pEmitters[i]->Disable();

		m_pEmitters[i]->SetTimeOn( -1 );
		m_pEmitters[i]->SetTimeOff( -1 );

		// grab the sprite resource
		res::SetupTexturesOnEmitter( m_pEmitters[i] );
	}

	// audio
	const char* sr = 0;

	sr = res::GetSoundResource( 300 );
	m_SpottedPlayerAudio = snd::SoundLoad( sr, m_SndLoad );

	sr = res::GetSoundResource( 301 );
	m_HitPlayerAudio = snd::SoundLoad( sr, m_SndLoad );

	sr = res::GetSoundResource( 302 );
	m_MissPlayerAudio = snd::SoundLoad( sr, m_SndLoad );

	sr = res::GetSoundResource( 303 );
	m_DeathImpactPlayerAudio = snd::SoundLoad( sr, m_SndLoad );
	
	m_HitMissAudioStarted = false;
	m_ImpactAudioStarted = false;
}

/////////////////////////////////////////////////////
/// Method: SetupSubmeshes
/// Params: None
///
/////////////////////////////////////////////////////
void Enemy::SetupSubmeshes()
{
	unsigned int i=0;

	// 
	m_PartLookup[0] = Normal_Head;
	m_PartLookup[1] = Normal_Body;
	m_PartLookup[2] = Normal_Arm_R;
	m_PartLookup[3] = Normal_Wrist_R;
	m_PartLookup[4] = Normal_Arm_L;
	m_PartLookup[5] = Normal_Wrist_L;
	m_PartLookup[6] = Normal_Hip_R;
	m_PartLookup[7] = Normal_Hip_L;
	m_PartLookup[8] = Normal_Shin_R;
	m_PartLookup[9] = Normal_Shin_L;

	// disable all
	for( i = 0; i < m_pModelData->GetNumSkinMeshes(); i++ )
		m_pModelData->SetSkinDrawState( i, false );

	// draw shadow
	if( m_State != EnemyState_InActive &&
		m_State != EnemyState_Dead )
	{
		m_pModelData->SetSkinDrawState( mShadow, true );
	}

/*	Skeleton_Head,
	Skeleton_Body,
	Skeleton_Arm_R,
	Skeleton_Wrist_R,
	Skeleton_Arm_L,
	Skeleton_Wrist_L,
	Skeleton_Pelvis,
	Skeleton_Hip_R,
	Skeleton_Hip_L,
	Skeleton_Shin_R,
	Skeleton_Shin_L,

	Normal_Head,
	Normal_Body,
	Normal_Arm_R,
	Normal_Wrist_R,
	Normal_Arm_L,
	Normal_Wrist_L,
	Normal_Hip_R,
	Normal_Hip_L,
	Normal_Shin_R,
	Normal_Shin_L,
*/
	// MAIN BODY
	if( HasLostPart( Normal_Head ) )
		m_pModelData->SetSkinDrawState( Skeleton_Head, true );
	else
		m_pModelData->SetSkinDrawState( Normal_Head, true );

	if( HasLostPart( Normal_Body ) )
	{
		m_pModelData->SetSkinDrawState( Skeleton_Body, true );
		m_pModelData->SetSkinDrawState( Skeleton_Pelvis, true );
	}
	else
		m_pModelData->SetSkinDrawState( Normal_Body, true );

	if( HasLostPart( Normal_Arm_R ) )
		m_pModelData->SetSkinDrawState( Skeleton_Arm_R, true );
	else
		m_pModelData->SetSkinDrawState( Normal_Arm_R, true );

	if( HasLostPart( Normal_Wrist_R ) )
		m_pModelData->SetSkinDrawState( Skeleton_Wrist_R, true );
	else
		m_pModelData->SetSkinDrawState( Normal_Wrist_R, true );

	if( HasLostPart( Normal_Arm_L ) )
		m_pModelData->SetSkinDrawState( Skeleton_Arm_L, true );
	else
		m_pModelData->SetSkinDrawState( Normal_Arm_L, true );

	if( HasLostPart( Normal_Wrist_L ) )
		m_pModelData->SetSkinDrawState( Skeleton_Wrist_L, true );
	else
		m_pModelData->SetSkinDrawState( Normal_Wrist_L, true );

	if( HasLostPart( Normal_Hip_R ) )
		m_pModelData->SetSkinDrawState( Skeleton_Hip_R, true );
	else
		m_pModelData->SetSkinDrawState( Normal_Hip_R, true );

	if( HasLostPart( Normal_Hip_L ) )
		m_pModelData->SetSkinDrawState( Skeleton_Hip_L, true );
	else
		m_pModelData->SetSkinDrawState( Normal_Hip_L, true );

	if( HasLostPart( Normal_Hip_R ) && HasLostPart( Normal_Hip_L ) )
		m_pModelData->SetSkinDrawState( Skeleton_Pelvis, true );

	if( HasLostPart( Normal_Shin_R ) )
		m_pModelData->SetSkinDrawState( Skeleton_Shin_R, true );
	else
		m_pModelData->SetSkinDrawState( Normal_Shin_R, true );

	if( HasLostPart( Normal_Shin_L ) )
		m_pModelData->SetSkinDrawState( Skeleton_Shin_L, true );
	else
		m_pModelData->SetSkinDrawState( Normal_Shin_L, true );

	// STUMPS
/*	Stump_Shoulder_L,
	Stump_Arm_L,
	Stump_Wrist_L,
	Stump_Elbow_L,
	Stump_Leg_L,
	Stump_Hip_L,
	Stump_Shin_L,
	Stump_Knee_L,
	Stump_Shoulder_R,
	Stump_Arm_R,
	Stump_Wrist_R,
	Stump_Elbow_R,
	Stump_Leg_R,
	Stump_Hip_R,
	Stump_Shin_R,
	Stump_Knee_R,
*/
	if( HasLostPart( Normal_Head ) )
	{
		if( !HasLostPart( Normal_Body ) )
			m_pModelData->SetSkinDrawState( Stump_Neck, true );
	}

	if( HasLostPart( Normal_Body ) )
	{
		if( !HasLostPart( Normal_Head ) )
			m_pModelData->SetSkinDrawState( Stump_Head, true );

		if( !HasLostPart( Normal_Arm_R ) )
			m_pModelData->SetSkinDrawState( Stump_Arm_R, true );

		if( !HasLostPart( Normal_Arm_L ) )
			m_pModelData->SetSkinDrawState( Stump_Arm_L, true );

		if( !HasLostPart( Normal_Hip_R ) )
			m_pModelData->SetSkinDrawState( Stump_Leg_R, true );

		if( !HasLostPart( Normal_Hip_L ) )
			m_pModelData->SetSkinDrawState( Stump_Leg_L, true );
	}

	// RIGHT SIDE
	if( HasLostPart( Normal_Arm_R ) )
	{
		if( !HasLostPart( Normal_Body ) )
			m_pModelData->SetSkinDrawState( Stump_Shoulder_R, true );

		if( !HasLostPart( Normal_Wrist_R ) )
			m_pModelData->SetSkinDrawState( Stump_Wrist_R, true );
	}

	if( HasLostPart( Normal_Wrist_R ) )
	{
		if( !HasLostPart( Normal_Arm_R ) )
			m_pModelData->SetSkinDrawState( Stump_Elbow_R, true );
	}

	if( HasLostPart( Normal_Hip_R ) )
	{
		if( !HasLostPart( Normal_Body ) )
			m_pModelData->SetSkinDrawState( Stump_Hip_R, true );

		if( !HasLostPart( Normal_Shin_R ) )
			m_pModelData->SetSkinDrawState( Stump_Shin_R, true );
	}

	if( HasLostPart( Normal_Shin_R ) )
	{
		if( !HasLostPart( Normal_Hip_R ) )
			m_pModelData->SetSkinDrawState( Stump_Knee_R, true );
	}

	// LEFT SIDE
	if( HasLostPart( Normal_Arm_L ) )
	{
		if( !HasLostPart( Normal_Body ) )
			m_pModelData->SetSkinDrawState( Stump_Shoulder_L, true );

		if( !HasLostPart( Normal_Wrist_L ) )
			m_pModelData->SetSkinDrawState( Stump_Wrist_L, true );
	}

	if( HasLostPart( Normal_Wrist_L ) )
	{
		if( !HasLostPart( Normal_Arm_L ) )
			m_pModelData->SetSkinDrawState( Stump_Elbow_L, true );
	}

	if( HasLostPart( Normal_Hip_L ) )
	{
		if( !HasLostPart( Normal_Body ) )
			m_pModelData->SetSkinDrawState( Stump_Hip_L, true );

		if( !HasLostPart( Normal_Shin_L ) )
			m_pModelData->SetSkinDrawState( Stump_Shin_L, true );
	}

	if( HasLostPart( Normal_Shin_L ) )
	{
		if( !HasLostPart( Normal_Hip_L ) )
			m_pModelData->SetSkinDrawState( Stump_Knee_L, true );
	}
}

/////////////////////////////////////////////////////
/// Method: PreDrawSetup
/// Params: None
///
/////////////////////////////////////////////////////
void Enemy::PreDrawSetup( bool pauseFlag )
{
	if( !pauseFlag )
		UpdateJoints(m_LastDeltaTime);
}

/////////////////////////////////////////////////////
/// Method: Draw
/// Params: None
///
/////////////////////////////////////////////////////
void Enemy::Draw( )
{
	int i=0;
	bool reflection = false;

	if( m_pModelData != 0 )
	{
		if( reflection )
			m_pModelData->SetSkinDrawState( mShadow, false );

		// make sure material is correct
		if( H4::GetHiResMode() )
		{
			for( i=0; i <= 9; ++i )
				m_pModelData->ChangeTextureOnMaterialId( m_PartLookup[i], 0, m_SkinTextureIndex );

			m_pModelData->ChangeTextureOnMaterialId( Stump_Neck, 0, m_SkinTextureIndex );
			m_pModelData->ChangeTextureOnMaterialId( Stump_Head, 0, m_SkinTextureIndex );
			m_pModelData->ChangeTextureOnMaterialId( Stump_Shoulder_L, 0, m_SkinTextureIndex );
			m_pModelData->ChangeTextureOnMaterialId( Stump_Arm_L, 0, m_SkinTextureIndex );
			m_pModelData->ChangeTextureOnMaterialId( Stump_Wrist_L, 0, m_SkinTextureIndex );
			m_pModelData->ChangeTextureOnMaterialId( Stump_Elbow_L, 0, m_SkinTextureIndex );
			m_pModelData->ChangeTextureOnMaterialId( Stump_Leg_L, 0, m_SkinTextureIndex );
			m_pModelData->ChangeTextureOnMaterialId( Stump_Hip_L, 0, m_SkinTextureIndex );
			m_pModelData->ChangeTextureOnMaterialId( Stump_Shin_L, 0, m_SkinTextureIndex );
			m_pModelData->ChangeTextureOnMaterialId( Stump_Knee_L, 0, m_SkinTextureIndex );
			m_pModelData->ChangeTextureOnMaterialId( Stump_Shoulder_R, 0, m_SkinTextureIndex );
			m_pModelData->ChangeTextureOnMaterialId( Stump_Arm_R, 0, m_SkinTextureIndex );
			m_pModelData->ChangeTextureOnMaterialId( Stump_Wrist_R, 0, m_SkinTextureIndex );
			m_pModelData->ChangeTextureOnMaterialId( Stump_Elbow_R, 0, m_SkinTextureIndex );
			m_pModelData->ChangeTextureOnMaterialId( Stump_Leg_R, 0, m_SkinTextureIndex );
			m_pModelData->ChangeTextureOnMaterialId( Stump_Hip_R, 0, m_SkinTextureIndex );
			m_pModelData->ChangeTextureOnMaterialId( Stump_Shin_R, 0, m_SkinTextureIndex );
			m_pModelData->ChangeTextureOnMaterialId( Stump_Knee_R, 0, m_SkinTextureIndex );
		}
		else
		{
			for( i=0; i <= 9; ++i )
				m_pModelData->ChangeTextureOnMaterial( m_PartLookup[i], 1, m_SkinTextureIndex );
		}

		glPushMatrix();
			glTranslatef( m_Pos.X, m_Pos.Y, m_Pos.Z );
			glRotatef( m_RotationAngle, 0.0f, 1.0f, 0.0f );
			m_pModelData->Draw();
		glPopMatrix();

		if(reflection)
		{
			glFrontFace( GL_CW );

			glPushMatrix();
				glTranslatef( m_Pos.X, m_Pos.Y, m_Pos.Z );
				glRotatef( m_RotationAngle, 0.0f, 1.0f, 0.0f );
				glScalef( 1.0f, -1.0f, 1.0f );
				m_pModelData->Draw();
			glPopMatrix();

			glFrontFace( GL_CCW );
		}
	}

	for( i=0; i < MAX_ENEMY_EMITTERS; ++i )
		m_pEmitters[i]->Draw();
}

/////////////////////////////////////////////////////
/// Method: DrawGibs
/// Params: None
///
/////////////////////////////////////////////////////
void Enemy::DrawGibs()
{
	renderer::OpenGL::GetInstance()->SetColour4ub( 255,255,255, m_DeadAlpha );

	for( int i=0; i < MAX_ENEMY_GIBS; ++i )
	{
		m_Gibs[i]->Draw();
	}
}

/////////////////////////////////////////////////////
/// Method: DrawDebug
/// Params: None
///
/////////////////////////////////////////////////////
void Enemy::DrawDebug()
{
#ifdef _DEBUG
	if( m_CanSeePlayer )
		renderer::OpenGL::GetInstance()->SetColour4ub( 255,255,0,255 );
	else
		renderer::OpenGL::GetInstance()->SetColour4ub( 0,0,255,255 );

	glPushMatrix();
		glTranslatef( m_RayContact.x, 0.0f, m_RayContact.y );
		renderer::DrawSphere( 0.5f );
	glPopMatrix();

	renderer::OpenGL::GetInstance()->SetColour4ub( 255,255,255,255 );
	glPushMatrix();
		glTranslatef( m_BoundSphere.vCenterPoint.X, m_BoundSphere.vCenterPoint.Y, m_BoundSphere.vCenterPoint.Z );
		renderer::DrawSphere( m_BoundSphere.fRadius );
	glPopMatrix();

	if (m_PathActive)
	{
		renderer::OpenGL::GetInstance()->DepthMode( true, GL_ALWAYS );

		pointIndex = 0;

		glPushMatrix();
			glTranslatef(0.0f, 0.0f, 0.0f); 
			
			// For line drawing, add 0.1 to all y values to make sure 
			// the lines are visible above the Navigation Mesh
			//
			// connect the waypoins to draw lines in blue
			NavigationPath::WAYPOINT_LIST::const_iterator iter = m_Path.WaypointList().begin();
			if (iter != m_Path.WaypointList().end())
			{
				NavigationPath::WAYPOINT LastWaypoint = *iter;
				++iter;
				for (;iter != m_Path.WaypointList().end();++iter)
				{
					const NavigationPath::WAYPOINT& waypoint= *iter;

					vaPoints[pointIndex] = LastWaypoint.Position;
					vaPoints[pointIndex+1] = waypoint.Position;

					pointIndex += 2;

					LastWaypoint = waypoint;
				}
			}
			renderer::OpenGL::GetInstance()->SetColour4ub(0, 0, 255, 255);
			glVertexPointer( 3, GL_FLOAT, sizeof(math::Vec3), vaPoints );
			glDrawArrays( GL_LINES, 0, pointIndex ); 

			// draw our current heading in red
			pointIndex = 0;
			const NavigationPath::WAYPOINT& waypoint= *m_NextWaypoint;
			
			vaPoints[pointIndex] = m_Pos;
			vaPoints[pointIndex+1] = waypoint.Position;
			
			pointIndex += 2;

			renderer::OpenGL::GetInstance()->SetColour4ub(255, 0, 0, 255);
			glVertexPointer( 3, GL_FLOAT, sizeof(math::Vec3), vaPoints );
			glDrawArrays( GL_LINES, 0, pointIndex ); 
		glPopMatrix();
		
		renderer::OpenGL::GetInstance()->DepthMode( true, GL_LESS );
	}
#endif // _DEBUG
}

/////////////////////////////////////////////////////
/// Method: Update
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
void Enemy::Update( float deltaTime )
{
	unsigned int i=0;

	for( i=0; i < MAX_ENEMY_EMITTERS; ++i )
		m_pEmitters[i]->Update( deltaTime );

	for( i=0; i < MAX_ENEMY_GIBS; ++i )
	{
		m_Gibs[i]->Update(deltaTime);
	}

	if( m_State == EnemyState_InActive )
		return;

	if( m_State == EnemyState_Dead )
	{
		if( m_pBody != 0 )
		{
			physics::PhysicsWorldB2D::GetWorld()->DestroyBody(m_pBody);
			m_pBody = 0;
		}

		if( !m_ImpactAudioStarted &&
			(m_LocalPrimaryAnim.nCurrentFrame == DEATH_IMPACT_FRAME) )
		{
			if( m_DeathImpactPlayerAudio != snd::INVALID_SOUNDBUFFER )
				AudioSystem::GetInstance()->PlayAudio( m_DeathImpactPlayerAudio, m_Pos, false );
			m_ImpactAudioStarted = true;
		}

		m_BoundSphere.vCenterPoint = math::Vec3( m_Pos.X+m_pJoints[BOUNDING_JOINT_INDEX].final.m41, m_Pos.Y+m_pJoints[BOUNDING_JOINT_INDEX].final.m42, m_Pos.Z+m_pJoints[BOUNDING_JOINT_INDEX].final.m43 );
		m_BoundSphere.vCenterPoint = math::RotateAroundPoint( m_BoundSphere.vCenterPoint, m_Pos, math::DegToRad(m_RotationAngle) );

		if( m_DeadFadeTimer == DEAD_FADE_TIME )
			SetAddKillFlag( true );

		m_DeadFadeTimer -= deltaTime;
		//m_DeadAlpha -= 1;

		if( m_DeadFadeTimer <= 0.0f )
		{
			m_DeadFadeTimer = 0.0f;

			m_Pos.Y -= DEAD_SINK_SPEED*deltaTime;
			if( m_Pos.Y < DEAD_SINK_STOP )
			{
				m_State = EnemyState_InActive;
			}
		}
		
		if( m_DeadAlpha < 0 )
			m_DeadAlpha = 0;

		return;
	}

	m_LastDeltaTime = deltaTime;

	ProcessState();

	if( m_SpawnNewGib )
	{
		if( m_NextGibIndex < MAX_ENEMY_GIBS )
		{
			if( math::RandomNumber(0,1) == 0 )			
				m_Gibs[m_NextGibIndex]->SpawnGib( m_pSmallGib, m_Pos, SMALL_GIB_RADIUS );
			else
				m_Gibs[m_NextGibIndex]->SpawnGib( m_pLargeGib, m_Pos, LARGE_GIB_RADIUS );

			m_NextGibIndex++;
		}
		else
			m_NextGibIndex = MAX_ENEMY_GIBS;

		m_SpawnNewGib = false;
	}

	if( m_SwitchToNavMesh )
		UpdateNavMeshSwitch( deltaTime );
	else if( m_NavMeshEnable )
		UpdateNavMesh( deltaTime );
	else if( m_CanSeePlayer )
		UpdateDirect( deltaTime );
	else
		UpdateIdle( deltaTime );


	m_BoundSphere.vCenterPoint = math::Vec3( m_Pos.X+m_pJoints[BOUNDING_JOINT_INDEX].final.m41, m_Pos.Y+m_pJoints[BOUNDING_JOINT_INDEX].final.m42, m_Pos.Z+m_pJoints[BOUNDING_JOINT_INDEX].final.m43 );
	m_BoundSphere.vCenterPoint = math::RotateAroundPoint( m_BoundSphere.vCenterPoint, m_Pos, math::DegToRad(m_RotationAngle) );

	if( m_RotationAngle != m_TargetRotation )
	{
		if( m_TargetRotation < m_RotationAngle )
			m_RotationAngle -= ROTATION_INCREMENT*deltaTime;
		else if( m_TargetRotation > m_RotationAngle )
			m_RotationAngle += ROTATION_INCREMENT*deltaTime;

		if( std::fabs(m_RotationAngle-m_TargetRotation) < ANGLE_TOLERANCE )
				m_RotationAngle = m_TargetRotation;
	}
}

/////////////////////////////////////////////////////
/// Method: UpdateJoints
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
void Enemy::UpdateJoints( float deltaTime )
{
	unsigned int i = 0;
	mdl::TBoneJointKeyframe curFrame;
	mdl::TBoneJointKeyframe prevFrame;	

	math::Vec3 vPrimaryTranslate( 0.0f, 0.0f, 0.0f );
	math::Quaternion qPrimaryRotation( 0.0f, 0.0f, 0.0f, 1.0f );

	math::Vec3 vCurPos( 0.0f, 0.0f, 0.0f );
	math::Vec3 vPrevPos( 0.0f, 0.0f, 0.0f );
	math::Quaternion qPrev( 0.0f, 0.0f, 0.0f, 1.0f );
	math::Quaternion qCur( 0.0f, 0.0f, 0.0f, 1.0f );

	// setup the bone pose for this enemies animation
	if( m_pPrimaryAnim )
	{
		int nFrame = 0;
		int nStartFrame = 0;

		m_LocalPrimaryAnim.animTimer += (deltaTime*1000.0f);
		
			// find out current time from the animation timer
		m_LocalPrimaryAnim.dCurrentTime = m_LocalPrimaryAnim.animTimer;

		// should the current time be offset from the start frame (anims usually start at frame 0 though)
		float fTimeOffset = (float)((( nStartFrame )*1000) / m_LocalPrimaryAnim.fAnimationFramerate );
		m_LocalPrimaryAnim.dCurrentTime += fTimeOffset; 

		if( (m_LocalPrimaryAnim.nTotalFrames > 0) && (m_LocalPrimaryAnim.nTotalFrames < m_LocalPrimaryAnim.nTotalFrames) )
			m_LocalPrimaryAnim.fEndTime = (float)( ( (m_LocalPrimaryAnim.nTotalFrames)*1000) / m_LocalPrimaryAnim.fAnimationFramerate );
		else 
			m_LocalPrimaryAnim.fEndTime = (float)m_LocalPrimaryAnim.dTotalAnimationTime;

		// get the animation status
		if( m_LocalPrimaryAnim.dCurrentTime > m_LocalPrimaryAnim.fEndTime )
		{
			// animation has finished, check for a polled animation
			if( m_pPolledAnim )
			{
				m_pPrimaryAnim = m_pPolledAnim;
				m_LocalPrimaryAnim = m_LocalPolledAnim;

				if( m_LocalPrimaryAnim.bLooping )
				{
					m_LocalPrimaryAnim.nCurrentFrame = 0;
					//m_LocalPrimaryAnim.animTimer.Reset();
					m_LocalPrimaryAnim.animTimer = 0.0f;
					m_LocalPrimaryAnim.dCurrentTime = fTimeOffset;
				}
				else
				{
					m_LocalPrimaryAnim.dCurrentTime = m_LocalPrimaryAnim.fEndTime; 
				}

				// clear out the poll animation
				m_pPolledAnim = 0;
			}
			else
			{
				// no animation waiting, either loop or stop
				//if( !m_Paused )
				{
					if( m_LocalPrimaryAnim.bLooping )
					{
						m_LocalPrimaryAnim.nCurrentFrame = 0;
						//m_LocalPrimaryAnim.animTimer.Reset();
						m_LocalPrimaryAnim.animTimer = 0.0f;
						m_LocalPrimaryAnim.dCurrentTime = fTimeOffset;
					}
					else
					{
						m_LocalPrimaryAnim.dCurrentTime = m_LocalPrimaryAnim.fEndTime; 
					}
				}
			}
		} 

		// go through the joints and transform/rotate
		for( i = 0; i < m_JointCount; i++ )
		{
			mdl::TBoneJoint* pJoint = &m_pJoints[i];

			// allow enough time to pass before updating the frame
			nFrame = m_LocalPrimaryAnim.nCurrentFrame;
			//if( !m_Paused )
			{
				while( (nFrame < m_LocalPrimaryAnim.nTotalFrames) && (m_pPrimaryAnim->pFrames[i][nFrame].fTime < m_LocalPrimaryAnim.dCurrentTime) )
				{
					nFrame++;
				}
			}
			m_LocalPrimaryAnim.nCurrentFrame = nFrame;

			if( nFrame == 0 )
			{
				// first frame
				vPrimaryTranslate = m_pPrimaryAnim->pFrames[i][0].vTranslate;
				qPrimaryRotation = m_pPrimaryAnim->pFrames[i][0].qRotation;
			}
			else if( nFrame == m_LocalPrimaryAnim.nTotalFrames )
			{
				// last frame
				vPrimaryTranslate = m_pPrimaryAnim->pFrames[i][nFrame-1].vTranslate;
				qPrimaryRotation = m_pPrimaryAnim->pFrames[i][nFrame-1].qRotation;
			}
			else
			{
				// between first and last frame 
				curFrame = m_pPrimaryAnim->pFrames[i][nFrame];
				prevFrame = m_pPrimaryAnim->pFrames[i][nFrame-1];

				// find a point between the previous and current frame to move to
				vCurPos = m_pPrimaryAnim->pFrames[i][nFrame].vTranslate;
				vPrevPos = m_pPrimaryAnim->pFrames[i][nFrame-1].vTranslate;

				vPrimaryTranslate = Lerp( vPrevPos, vCurPos, deltaTime );

				// use quaternions
				qPrev = m_pPrimaryAnim->pFrames[i][nFrame-1].qRotation;
				qCur = m_pPrimaryAnim->pFrames[i][nFrame].qRotation;

				qPrimaryRotation = Slerp( qPrev, qCur, deltaTime );
			}

			pJoint->relative = math::Matrix4x4( qPrimaryRotation );
			pJoint->relative.SetTranslation( vPrimaryTranslate );

			// if bone has a parent, multiply the child by the parent
			if ( pJoint->nParentJointIndex != -1 )
			{
				pJoint->absolute.SetMatrix( m_pJoints[pJoint->nParentJointIndex].absolute.m  );
				pJoint->absolute = pJoint->relative * pJoint->absolute;
			}
			else
				pJoint->absolute.SetMatrix( pJoint->relative.m  );

			// assign results to a final matrix to update the verts
			pJoint->final.SetMatrix( pJoint->absolute.m );
		}
	}
	else
	{
		// go through the joints and just set the bind pose
		for( i = 0; i < m_JointCount; i++ )
		{
			mdl::TBoneJoint* pJoint = &m_pJoints[i];

			// if bone has a parent, multiply the child by the parent
			if ( pJoint->nParentJointIndex != -1 )
			{
				pJoint->absolute.SetMatrix( m_pJoints[pJoint->nParentJointIndex].absolute.m  );
				pJoint->absolute = pJoint->relative * pJoint->absolute;
			}
			else
				pJoint->absolute.SetMatrix( pJoint->relative.m  );

			// assign results to a final matrix to update the verts
			pJoint->final.SetMatrix( pJoint->absolute.m );
		}
	}
}

/////////////////////////////////////////////////////
/// Method: UpdateSkins
/// Params: None
///
/////////////////////////////////////////////////////
void Enemy::UpdateSkins()
{
	unsigned int i = 0, j = 0;

	for ( i = 0; i < m_pModelData->GetNumSkinMeshes(); i++ )
	{
		// update each skin with the current frame pose
		mdl::SkinMesh* pSkin = &m_pModelData->GetSkinMeshListPtr()[i];

		if( !pSkin->bDraw )
			continue;

		// update the mesh if it's enabled
		for ( j = 0; j < pSkin->nNumPoints; j++ )
		{
			int nJointIndex0 = pSkin->pVertexWeights[j][0].nBoneIndex;
			float fWeight0 = pSkin->pVertexWeights[j][0].fWeight;

			int nJointIndex1 = pSkin->pVertexWeights[j][1].nBoneIndex;
			float fWeight1 = pSkin->pVertexWeights[j][1].fWeight;

			int nJointIndex2 = pSkin->pVertexWeights[j][2].nBoneIndex;
			float fWeight2 = pSkin->pVertexWeights[j][2].fWeight;

			int nJointIndex3 = pSkin->pVertexWeights[j][3].nBoneIndex;
			float fWeight3 = pSkin->pVertexWeights[j][3].fWeight;

			// move each vertex by the joints final matrix
			// nJointIndex0 and nWeight0 should always be valid, as there is always at least one bone affecting the vertex
			pSkin->pVertices[j] = math::Vec3( 0.0f, 0.0f, 0.0f );
					
			math::Vec3 vInfluence = math::TransformByMatrix4x4( pSkin->pVertexWeights[j][0].vTransformedVertex, &m_pJoints[ nJointIndex0 ].final.m[0] );
			vInfluence *= fWeight0;
			pSkin->pVertices[j] += vInfluence;

			// ADD POSSIBLE WEIGHT 2
			if( fWeight1 > 0.0f && nJointIndex1 != -1 )
			{
				// move vert
				vInfluence = math::TransformByMatrix4x4( pSkin->pVertexWeights[j][1].vTransformedVertex, &m_pJoints[ nJointIndex1 ].final.m[0]  );
				vInfluence *= fWeight1;
				pSkin->pVertices[j] += vInfluence;
			}
			// ADD POSSIBLE WEIGHT 3
			if( fWeight2 > 0.0f && nJointIndex2 != -1 )
			{
				// move vert
				vInfluence = math::TransformByMatrix4x4( pSkin->pVertexWeights[j][2].vTransformedVertex, &m_pJoints[ nJointIndex2 ].final.m[0] );
				vInfluence *= fWeight2;
				pSkin->pVertices[j] += vInfluence;
			}
			// ADD POSSIBLE WEIGHT 4
			if( fWeight3 > 0.0f && nJointIndex3 != -1 )
			{
				// move vert
				vInfluence = math::TransformByMatrix4x4( pSkin->pVertexWeights[j][3].vTransformedVertex, &m_pJoints[ nJointIndex3 ].final.m[0]  );
				vInfluence *= fWeight3;
				pSkin->pVertices[j] += vInfluence;
			}
		}
	}
}

/////////////////////////////////////////////////////
/// Method: SetAnimation
/// Params: [in]nAnimId, [in]nFrameOffset, [in]bForceChange
///
/////////////////////////////////////////////////////
void Enemy::SetAnimation( unsigned int nAnimId, int nFrameOffset, bool bForceChange )
{
	if( m_pModelData != 0 )
	{
		unsigned int i=0;

		if( m_pPrimaryAnim )
		{
			// if it asks for the same animation and the animation is looping then there's no need to change
			if( ( m_pPrimaryAnim->nAnimId == nAnimId ) && (m_pPrimaryAnim->bLooping || bForceChange==false) )
			{
				return;
			}
		}

		// forcing a change, update the primary animation pointer
		if( bForceChange )
		{
			for( i=0; i < m_pModelData->GetBoneAnimationList().size(); ++i )
			{
				mdl::BoneAnimation* pBoneAnim = m_pModelData->GetBoneAnimationList()[i];

				if( pBoneAnim->nAnimId == nAnimId  )
				{
					m_pPrimaryAnim = pBoneAnim;

					// take a copy of the frame data
					m_LocalPrimaryAnim.nAnimId					= m_pPrimaryAnim->nAnimId;
					m_LocalPrimaryAnim.fAnimationFramerate		= m_pPrimaryAnim->fAnimationFramerate;
					m_LocalPrimaryAnim.dTotalAnimationTime		= m_pPrimaryAnim->dTotalAnimationTime;
					m_LocalPrimaryAnim.nTotalJoints				= m_pPrimaryAnim->nTotalJoints;
					m_LocalPrimaryAnim.nTotalFrames				= m_pPrimaryAnim->nTotalFrames;

					m_LocalPrimaryAnim.bLooping					= m_pPrimaryAnim->bLooping;
					m_LocalPrimaryAnim.nCurrentFrame			= m_pPrimaryAnim->nCurrentFrame;
					m_LocalPrimaryAnim.dCurrentTime				= m_pPrimaryAnim->dCurrentTime;
					m_LocalPrimaryAnim.fEndTime					= m_pPrimaryAnim->fEndTime;

					if( nFrameOffset != -1 && nFrameOffset < m_LocalPrimaryAnim.nTotalFrames )
						m_LocalPrimaryAnim.nCurrentFrame = nFrameOffset;
					else
						m_LocalPrimaryAnim.nCurrentFrame = 0;
					//m_LocalPrimaryAnim.animTimer.Reset();
					//m_LocalPrimaryAnim.animTimer.Start();
					m_LocalPrimaryAnim.animTimer = 0.0f;
					return;
				}
			}
		}
		else
		{
			// not forcing a change so enable the poll animation
 			for( i=0; i < m_pModelData->GetBoneAnimationList().size(); ++i )
			{
				mdl::BoneAnimation* pBoneAnim = m_pModelData->GetBoneAnimationList()[i];

				if( pBoneAnim->nAnimId == nAnimId )
				{
					m_pPolledAnim = pBoneAnim;

					// take a copy of the frame data
					m_LocalPolledAnim.nAnimId					= m_pPolledAnim->nAnimId;
					m_LocalPolledAnim.fAnimationFramerate		= m_pPolledAnim->fAnimationFramerate;
					m_LocalPolledAnim.dTotalAnimationTime		= m_pPolledAnim->dTotalAnimationTime;
					m_LocalPolledAnim.nTotalJoints				= m_pPolledAnim->nTotalJoints;
					m_LocalPolledAnim.nTotalFrames				= m_pPolledAnim->nTotalFrames;

					m_LocalPolledAnim.bLooping					= m_pPolledAnim->bLooping;
					m_LocalPolledAnim.nCurrentFrame				= m_pPolledAnim->nCurrentFrame;
					m_LocalPolledAnim.dCurrentTime				= m_pPolledAnim->dCurrentTime;
					m_LocalPolledAnim.fEndTime					= m_pPolledAnim->fEndTime;

					if( nFrameOffset != -1 && nFrameOffset < m_LocalPolledAnim.nTotalFrames )
						m_LocalPolledAnim.nCurrentFrame = nFrameOffset;
					else
						m_LocalPolledAnim.nCurrentFrame = 0;
					//m_LocalPolledAnim.animTimer.Reset();
					//m_LocalPolledAnim.animTimer.Start();
					return;
				}
			}
		}

		// got this far, the animation doesn't exist so just set the bind pose
		m_pPrimaryAnim = 0;
	}
}

/////////////////////////////////////////////////////
/// Method: HandleContact
/// Params: [in]idNum, [in]castId, [in]point, [in]pOtherShape, [in]pOtherBody
///
/////////////////////////////////////////////////////
void Enemy::HandleContact( int idNum, int castId, const ContactPoint* contact, const b2Fixture* pOtherFixture, const b2Body* pOtherBody )
{
	int i=0;
	int numeric1 = -9999;
	//int numeric2 = -9999;
	//int numeric3 = -9999;
	//int numeric4 = -9999;
	//int numeric5 = -9999;

	switch( idNum )
	{
		case PHYSICSBASICID_WORLD:
		{
			physics::PhysicsIdentifier* pUserData = 0;
			if( pOtherBody != 0 )
			{
				pUserData = reinterpret_cast<physics::PhysicsIdentifier*>( pOtherFixture->GetUserData() );
				DBG_ASSERT_MSG( (pUserData != 0), "Could not get physics user data on enemy world contact" );
				
				numeric1 = pUserData->GetNumeric1();
				//numeric2 = pUserData->GetNumeric2();
				//numeric3 = pUserData->GetNumeric3();
				//numeric4 = pUserData->GetNumeric4();
				//numeric5 = pUserData->GetNumeric5();
			}

			if( numeric1 == NUMERIC1_WORLD_DOOR )
			{
				if( m_PathActive || m_NavMeshEnable )
				{
					m_PathActive = false;
					m_NavMeshEnable = false;
					m_DoorCollision = true;

					m_Path.WaypointList().clear();
				}
			}
		}break;

		case PHYSICSBASICID_BULLET:
		{
			// 
			Bullet* pBullet = reinterpret_cast<Bullet*>( pOtherFixture->GetUserData() );
			pBullet->SetActive(false);

			if( m_BoundSphere.SphereCollidesWithSphere( pBullet->GetCollision() ) )
			{
				// check if the stage allows gibs
				bool allowGibs = GetScriptDataHolder()->GetActiveStageData().allowGibs;
				Bullet::BulletSource whichBullet = pBullet->GetBulletSource();

				if( LosePart(whichBullet) )
					m_SpawnNewGib = true;

				if( !allowGibs )
					m_SpawnNewGib = false;

				if( m_State != EnemyState_Dead )
				{
					m_PathActive = false;

					// run after the player
					m_PrevState = EnemyState_Run;
					//else
					//	m_PrevState = EnemyState_Walk;

					SetAnimation( math::RandomNumber(EnemyAnim::FLINCH_LEFT, EnemyAnim::FLINCH_RIGHT), 0, true );

					for( i=0; i < MAX_ENEMY_EMITTERS; ++i )
					{
						math::Vec3 spawnPoint( m_Pos.X+m_pJoints[BOUNDING_JOINT_INDEX].final.m41, (m_Pos.Y+m_pJoints[BOUNDING_JOINT_INDEX].final.m42) + math::RandomNumber( -0.5f, 0.5f ), m_Pos.Z+m_pJoints[BOUNDING_JOINT_INDEX].final.m43 );
						
						m_pEmitters[i]->SetPos( spawnPoint );
						m_pEmitters[i]->Enable();

						m_pEmitters[i]->Disable();
					}

					// set the state to flinch
					m_State = EnemyState_Flinch;

					m_Movement.X = m_Movement.Z = 0.0f;
				}
				else
				{
					for( i=0; i < MAX_ENEMY_EMITTERS; ++i )
					{
						math::Vec3 spawnPoint( m_Pos.X+m_pJoints[BOUNDING_JOINT_INDEX].final.m41, (m_Pos.Y+m_pJoints[BOUNDING_JOINT_INDEX].final.m42) + math::RandomNumber( -0.5f, 0.5f ), m_Pos.Z+m_pJoints[BOUNDING_JOINT_INDEX].final.m43 );
						
						m_pEmitters[i]->SetPos( spawnPoint );
						m_pEmitters[i]->Enable();

						m_pEmitters[i]->Disable();
					}
				}
			}
		}break;

		case PHYSICSBASICID_PLAYER:
		{
			m_PathActive = false;

			math::Vec3 dir = m_PlayerPosition - m_Pos;

			m_TargetRotation = math::RadToDeg( atan2(dir.X, dir.Z) );
			m_RotationAngle = m_TargetRotation;

			if( m_State != EnemyState_Dead )
			{
				// attack the player
				m_ContactWithPlayer = true;

				if( m_State != EnemyState_Attack )
					m_State = EnemyState_Attack; 
			}

		}break;

		case PHYSICSBASICID_ENEMY:
		case PHYSICSBASICID_FLOATINGHEAD:
		case PHYSICSBASICID_CRAWLINGSPIDER:
		{

		}break;

		default:
			// colliding with something that doesn't have an id
			DBG_ASSERT_MSG( 0, "Enemy colliding with object that has no physics id" );
		break;
	}
}

/////////////////////////////////////////////////////
/// Method: ClearContact
/// Params: [in]idNum, [in]castId, [in]point, [in]pOtherShape, [in]pOtherBody
///
/////////////////////////////////////////////////////
void Enemy::ClearContact( int idNum, int castId, const ContactPoint* contact, const b2Fixture* pOtherFixture, const b2Body* pOtherBody )
{
	switch( idNum )
	{
		case PHYSICSBASICID_WORLD:
		case PHYSICSBASICID_BULLET:
		{

		}break;

		case PHYSICSBASICID_PLAYER:
		{
			if( m_State != EnemyState_Dead )
			{
				if( m_State == EnemyState_Attack )
				{
					// tell the system to look for the player again once last anim has finished
					m_ContactWithPlayer = false;
				}
			}
		}break;

		case PHYSICSBASICID_ENEMY:
		case PHYSICSBASICID_FLOATINGHEAD:
		case PHYSICSBASICID_CRAWLINGSPIDER:
		{

		}break;

		default:
			// colliding with something that doesn't have an id
			DBG_ASSERT_MSG( 0, "Enemy removing contact from object that has no physics id" );
		break;
	}
}

/////////////////////////////////////////////////////
/// Method: UpdateNavMeshSwitch
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
void Enemy::UpdateNavMeshSwitch( float deltaTime )
{
	float distance;

	if( m_State == EnemyState_Walk )
		m_MaxSpeed = MAX_WALK_SPEED;
	else if( m_State == EnemyState_Run )
		m_MaxSpeed = MAX_RUN_SPEED;

	float max_distance = m_MaxSpeed * deltaTime;

	// apply some friction to our current movement (if any)

	if( m_State != EnemyState_Flinch &&
		m_State != EnemyState_Attack )
	{
		if( m_CanSeePlayer )
		{
			// can go direct again
			m_SwitchToNavMesh = false;

			m_PathActive = false;
			m_NavMeshEnable = false;
			m_NewWaypoint = false;

			m_Path.WaypointList().clear();
		}
		else
		{
			if (m_NextWaypoint != m_Path.WaypointList().end())
			{
				// determine our desired movement vector
				m_Movement = (*m_NextWaypoint).Position;
				m_Movement -= m_Pos;

				math::Vec3 dir = Normalise( m_Movement );
				m_TargetRotation = math::RadToDeg( atan2(dir.X, dir.Z) );
				m_RotationAngle = m_TargetRotation;

				m_Movement.X *= 0.95f;
				m_Movement.Y *= 0.95f;
				m_Movement.Z *= 0.95f;
			}
			else
				DBG_ASSERT(0);
		}
	}


	// scale back movement by our max speed if needed
	distance = m_Movement.length();
	if (distance > max_distance)
	{
		m_Movement.normalise();
		m_Movement *= max_distance;
	}

	// come to a full stop when we go below a certain threshold
	if (std::fabs(m_Movement.X) < 0.0001f) 
		m_Movement.X = 0.0f;
	if (std::fabs(m_Movement.Y) < 0.0001f) 
		m_Movement.Y = 0.0f;
	if (std::fabs(m_Movement.Z) < 0.0001f) 
		m_Movement.Z = 0.0f;

	if (m_Movement.X || m_Movement.Z)
	{
		// move box2d
		b2Vec2 moveForce( m_Movement.X*MOVE_MULTIPLY, m_Movement.Z*MOVE_MULTIPLY );
		m_pBody->SetLinearVelocity( moveForce );
	}
	else
	{
		// got to the nav mesh start, switch over
		m_SwitchToNavMesh = false;
	}

	b2Vec2 bodyPos = m_pBody->GetWorldPoint(zeroVec2);

	m_pBody->SetAwake(true);
		
	// this forces a position
	m_PrevPos = m_Pos;

	m_Pos.X = bodyPos.x;
	m_Pos.Z = bodyPos.y;	
}

/////////////////////////////////////////////////////
/// Method: UpdateNavMesh
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
void Enemy::UpdateNavMesh( float deltaTime )
{
	float distance;

	if( m_State == EnemyState_Walk )
		m_MaxSpeed = MAX_WALK_SPEED*2.1f;
	else if( m_State == EnemyState_Run )
		m_MaxSpeed = MAX_RUN_SPEED*3.0f;

	float max_distance = m_MaxSpeed * deltaTime;

	if (m_PathActive)
	{
		// Move along the waypoint path
		if (m_NextWaypoint != m_Path.WaypointList().end())
		{
			// determine our desired movement vector
			m_Movement = (*m_NextWaypoint).Position;
			m_Movement -= m_Pos;

			if (m_Movement.X || m_Movement.Z)
			{
				if( m_NewWaypoint )
				{
					math::Vec3 dir = Normalise( m_Movement );
					m_TargetRotation = math::RadToDeg( atan2(dir.X, dir.Z) );
					m_RotationAngle = m_TargetRotation;

					m_NewWaypoint = false;
				}
			}
		}
		else
		{
			// we have reached the end of the path
			m_PathActive = false;
			m_Movement.X=m_Movement.Y=m_Movement.Z=0.0f;

			if( m_State != EnemyState_Idle )
			{
				m_State = EnemyState_Idle; 
				SetAnimation( math::RandomNumber(EnemyAnim::IDLE, EnemyAnim::IDLE_ALT), 0, true );
			}
		}
	}
	else
	{
		// apply some friction to our current movement (if any)
		m_Movement.X *= 0.95f;
		m_Movement.Y *= 0.95f;
		m_Movement.Z *= 0.95f;
	}

	// scale back movement by our max speed if needed
	distance = m_Movement.length();
	if (distance > max_distance)
	{
		m_Movement.normalise();
		m_Movement *= max_distance;
	}

	// come to a full stop when we go below a certain threshold
	if (std::fabs(m_Movement.X) < 0.0001f) 
		m_Movement.X = 0.0f;
	if (std::fabs(m_Movement.Y) < 0.0001f) 
		m_Movement.Y = 0.0f;
	if (std::fabs(m_Movement.Z) < 0.0001f) 
		m_Movement.Z = 0.0f;

	if (m_Movement.X || m_Movement.Z)
	{
		// Constrain any remaining Horizontal movement to the parent navigation rink
		if (m_Parent && m_PathActive)
		{
			// compute the next desired location
			math::Vec3 NextPosition = m_Pos + m_Movement;
			NavigationCell* NextCell = 0;

			// test location on the NavigationMesh and resolve collisions
			m_Parent->ResolveMotionOnMesh( m_Pos, m_CurrentCell, NextPosition, &NextCell );

			m_Pos = NextPosition;
			m_CurrentCell = NextCell;
		}
	}
	else if (m_PathActive)
	{
		// If we have no remaining movement, but the path is active,
		// we have arrived at our desired waypoint.
		// Snap to it's position and figure out where to go next
		m_Pos = (*m_NextWaypoint).Position;
		m_Movement.X = m_Movement.Y = m_Movement.Z = 0.0f;
		distance = 0.0f;
		m_NextWaypoint = m_Path.GetFurthestVisibleWayPoint(m_NextWaypoint);

		// before going to the next way point, check to see if the player is in view and cancel out and go direct
		b2Vec2 enemyPos = m_pBody->GetPosition();
		b2Vec2 playerPos = b2Vec2( m_PlayerPosition.X, m_PlayerPosition.Z );

		EnemyRayCastCallback rayCallback;
		physics::PhysicsWorldB2D::GetWorld()->RayCast( &rayCallback, enemyPos, playerPos );

		bool clearPathToPlayer = false;

		if( rayCallback.hit )
		{
			if( rayCallback.hitId == PHYSICSBASICID_PLAYER )
				clearPathToPlayer = true;
			else
				clearPathToPlayer = false;

			m_RayContact = rayCallback.hitPoint;
			m_RayNormal = rayCallback.hitNormal;
		}
		else
		{
			clearPathToPlayer = false;
		}

		if( clearPathToPlayer )
		{
			// player can be seen, bail out the navmesh
			m_CanSeePlayer = true;

			m_PathActive = false;
			m_NavMeshEnable = false;
			m_NewWaypoint = false;

			m_Path.WaypointList().clear();
		}
		else
		{
			// can't see player continue to next waypoint
			if (m_NextWaypoint == m_Path.WaypointList().end())
			{
				// end of path
				m_PathActive = false;
				m_NavMeshEnable = false;
				m_NewWaypoint = false;

				m_Movement.X=m_Movement.Y=m_Movement.Z=0.0f;

				if( m_State != EnemyState_Idle )
				{
					m_State = EnemyState_Idle; 
					SetAnimation( math::RandomNumber(EnemyAnim::IDLE, EnemyAnim::IDLE_ALT), 0, true );
				}
			}
			else
				m_NewWaypoint = true;
		}
	}

	b2Vec2 bodyPos = m_pBody->GetWorldPoint(zeroVec2);

	if(m_PathActive)
	{
		m_pBody->SetAwake(true);
		
		// this forces a position
		m_pBody->SetTransform( b2Vec2(m_Pos.X, m_Pos.Z), 0.0f );	
		m_pBody->SetLinearVelocity( zeroVec2 );
	}
	else
	{
		m_Pos.X = bodyPos.x;
		m_Pos.Z = bodyPos.y;
		
		m_PrevPos = m_Pos;
	}
}

/////////////////////////////////////////////////////
/// Method: UpdateDirect
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
void Enemy::UpdateDirect( float deltaTime )
{
	float distance;

	if( m_State == EnemyState_Walk )
		m_MaxSpeed = MAX_WALK_SPEED;
	else if( m_State == EnemyState_Run )
		m_MaxSpeed = MAX_RUN_SPEED;

	float max_distance = m_MaxSpeed * deltaTime;

	if( m_CanSeePlayer )
	{
		if( m_State != EnemyState_Flinch &&
			m_State != EnemyState_Attack )
		{
			m_Movement = math::Vec3( m_RayContact.x, 0.0f, m_RayContact.y );
			m_Movement -= m_Pos;

			math::Vec3 dir = Normalise( m_Movement );
			m_TargetRotation = math::RadToDeg( atan2(dir.X, dir.Z) );
			m_RotationAngle = m_TargetRotation;
		}
	}
	else
	{
		// apply some friction to our current movement (if any)
		m_Movement.X *= 0.95f;
		m_Movement.Y *= 0.95f;
		m_Movement.Z *= 0.95f;
	}

	// scale back movement by our max speed if needed
	distance = m_Movement.length();
	if (distance > max_distance)
	{
		m_Movement.normalise();
		m_Movement *= max_distance;
	}

	// come to a full stop when we go below a certain threshold
	if (std::fabs(m_Movement.X) < 0.0001f) 
		m_Movement.X = 0.0f;
	if (std::fabs(m_Movement.Y) < 0.0001f) 
		m_Movement.Y = 0.0f;
	if (std::fabs(m_Movement.Z) < 0.0001f) 
		m_Movement.Z = 0.0f;

	if (m_Movement.X || m_Movement.Z)
	{
		// move box2d
		b2Vec2 moveForce( m_Movement.X*MOVE_MULTIPLY, m_Movement.Z*MOVE_MULTIPLY );
		m_pBody->SetLinearVelocity( moveForce );
	}

	b2Vec2 bodyPos = m_pBody->GetWorldPoint(zeroVec2);

	m_Pos.X = bodyPos.x;
	m_Pos.Z = bodyPos.y;
		
	m_PrevPos = m_Pos;
}

/////////////////////////////////////////////////////
/// Method: UpdateIdle
/// Params: None
///
/////////////////////////////////////////////////////
void Enemy::UpdateIdle( float deltaTime )
{
	b2Vec2 bodyPos = m_pBody->GetWorldPoint(zeroVec2);

	m_PrevPos = m_Pos;

	m_Pos.X = bodyPos.x;
	m_Pos.Z = bodyPos.y;
}

/////////////////////////////////////////////////////
/// Method: ProcessState
/// Params: None
///
/////////////////////////////////////////////////////
void Enemy::ProcessState()
{
	Player& player = GameSystems::GetInstance()->GetPlayer();

	switch( m_State )
	{
		case EnemyState_InActive:
		break;

		case EnemyState_Idle:
		{
			if( m_PrevState == EnemyState_Attack )
			{
				m_CurrentSearchTime += m_LastDeltaTime;

				// look for the player every so often
				if( (m_CurrentSearchTime >= m_SearchTime) )
				{
					m_PrevState = EnemyState_InActive;

					b2Vec2 enemyPos = m_pBody->GetPosition();
					b2Vec2 playerPos = b2Vec2( m_PlayerPosition.X, m_PlayerPosition.Z );

					EnemyRayCastCallback rayCallback;
					physics::PhysicsWorldB2D::GetWorld()->RayCast( &rayCallback, enemyPos, playerPos );

					bool clearPathToPlayer = false;

					if( rayCallback.hit )
					{
						if( rayCallback.hitId == PHYSICSBASICID_PLAYER )
							clearPathToPlayer = true;
						else
							clearPathToPlayer = false;

						m_RayContact = rayCallback.hitPoint;
						m_RayNormal = rayCallback.hitNormal;
					}
					else
					{
						clearPathToPlayer = false;
					}

					// check the angle to see if it's within a decent threshold
					if( clearPathToPlayer )
					{
						m_CanSeePlayer = true;

						//if( m_LostPartCount >= CHASE_COUNT )
						{
							if( m_State != EnemyState_Run )
							{
								m_State = EnemyState_Run; 
								SetAnimation( EnemyAnim::RUN, 0, true );

								if( m_SpottedPlayerAudio != snd::INVALID_SOUNDBUFFER )
									AudioSystem::GetInstance()->PlayAudio( m_SpottedPlayerAudio, m_Pos, false );
							}
						}
						/*else
						{
							if( m_State != EnemyState_Walk )
							{
								m_State = EnemyState_Walk; 
								SetAnimation( math::RandomNumber(EnemyAnim::WALK, EnemyAnim::WALK_ALT_RAISED_ARMS), 0, true );

								if( m_SpottedPlayerAudio != snd::INVALID_SOUNDBUFFER )
									AudioSystem::GetInstance()->PlayAudio( m_SpottedPlayerAudio, m_Pos, false );
							}
						}*/
					}

					// reset timer
					m_SearchTime = math::RandomNumber( SEARCH_TIME_MIN, SEARCH_TIME_MAX );
					m_CurrentSearchTime = 0.0f;
				}
			}
			else
			{
				if( !m_CanSeePlayer )
				{
					m_CurrentSearchTime += m_LastDeltaTime;

					// look for the player every so often
					if( (m_CurrentSearchTime >= m_SearchTime) )
					{
						b2Vec2 enemyPos = m_pBody->GetPosition();
						b2Vec2 playerPos = b2Vec2( m_PlayerPosition.X, m_PlayerPosition.Z );

						EnemyRayCastCallback rayCallback;
						physics::PhysicsWorldB2D::GetWorld()->RayCast( &rayCallback, enemyPos, playerPos );

						bool clearPathToPlayer = false;

						if( rayCallback.hit )
						{
							if( rayCallback.hitId == PHYSICSBASICID_PLAYER )
								clearPathToPlayer = true;
							else
								clearPathToPlayer = false;

							m_RayContact = rayCallback.hitPoint;
							m_RayNormal = rayCallback.hitNormal;
						}
						else
						{
							clearPathToPlayer = false;
						}

						// check the angle to see if it's within a decent threshold
						if( clearPathToPlayer )
						{
							math::Vec3 dir( 0.0f, 0.0f, 0.0f );
							dir.X = std::sin( math::DegToRad(m_RotationAngle) );
							dir.Z = std::cos( math::DegToRad(m_RotationAngle) );
							math::Vec3 enemyToPlayer = m_PlayerPosition - m_Pos;
							//enemyToPlayer.normalise();

							float angleDiff = math::AngleBetweenVectors( enemyToPlayer, dir );

							const float VIEW_ANGLE = 30.0f;
							
							float LEN_RANGE = 15.0f;
							int clothingIndex = player.GetClothingIndex();

							if( clothingIndex == 5 )
								LEN_RANGE = 4.0f;

							float playerToEnemy = std::fabs( enemyToPlayer.length() );

							if( (std::fabs( angleDiff ) < math::DegToRad(VIEW_ANGLE)) &&
								(playerToEnemy <= LEN_RANGE) )
							{
								m_CanSeePlayer = true;

								if( m_State != EnemyState_Run )
								{
									m_State = EnemyState_Run; 
									SetAnimation( EnemyAnim::RUN, 0, true );

									if( m_SpottedPlayerAudio != snd::INVALID_SOUNDBUFFER )
										AudioSystem::GetInstance()->PlayAudio( m_SpottedPlayerAudio, m_Pos, false );
								}

								/*if( m_State != EnemyState_Walk )
								{
									m_State = EnemyState_Walk; 
									SetAnimation( math::RandomNumber(EnemyAnim::WALK, EnemyAnim::WALK_ALT_RAISED_ARMS), 0, true );

									if( m_SpottedPlayerAudio != snd::INVALID_SOUNDBUFFER )
										AudioSystem::GetInstance()->PlayAudio( m_SpottedPlayerAudio, m_Pos, false );
								}*/
							}
							else
								m_CanSeePlayer = false;
						}

						// reset timer
						m_SearchTime = math::RandomNumber( SEARCH_TIME_MIN, SEARCH_TIME_MAX );
						m_CurrentSearchTime = 0.0f;
					}
				}
			}
		}break;

		case EnemyState_Walk:
		case EnemyState_Run:
		{
			if( !m_NavMeshEnable )
			{
				b2Vec2 enemyPos = m_pBody->GetPosition();
				b2Vec2 playerPos = b2Vec2( m_PlayerPosition.X, m_PlayerPosition.Z );

				EnemyRayCastCallback rayCallback;
				physics::PhysicsWorldB2D::GetWorld()->RayCast( &rayCallback, enemyPos, playerPos );

				// check for a state change

				if( rayCallback.hit )
				{
					if( rayCallback.hitId == PHYSICSBASICID_PLAYER )
						m_CanSeePlayer = true;
					else
						m_CanSeePlayer = false;

					m_RayContact = rayCallback.hitPoint;
					m_RayNormal = rayCallback.hitNormal;
				}
				else
				{
					m_CanSeePlayer = false;
				}

				// go to idle or switch to nav mesh
				if( !m_CanSeePlayer )
				{
					if( m_DoorCollision )
					{
						m_State = EnemyState_Idle; 
						SetAnimation( math::RandomNumber(EnemyAnim::IDLE, EnemyAnim::IDLE_ALT), 0, true );

						m_DoorCollision = false;
					}
					else
					{
						m_Path.WaypointList().clear();

						// generate nav mesh, and chase
						CalculateStartingCell(m_Pos);

						Player& player = GameSystems::GetInstance()->GetPlayer();
						NavigateToLocation( player.GetPosition(), player.GetCurrentCell() );

						if( m_PathActive )
						{
							m_SwitchToNavMesh = true;
							m_NavMeshEnable = true;
						}
					}
				}
				else
				{
					/*if( m_State == EnemyState_Walk )
					{
						if( m_LocalPrimaryAnim.nAnimId < EnemyAnim::WALK ||
							m_LocalPrimaryAnim.nAnimId > EnemyAnim::WALK_ALT_RAISED_ARMS)
						{
							SetAnimation( math::RandomNumber(EnemyAnim::WALK, EnemyAnim::WALK_ALT_RAISED_ARMS), 0, false );
						}
					}
					else*/ if( m_State == EnemyState_Run )
					{
						if( m_LocalPrimaryAnim.nAnimId != EnemyAnim::RUN )
						{
							SetAnimation( EnemyAnim::RUN, 0, false );
						}
					}
				}
			}
		}break;

		case EnemyState_Flinch:
		{
			// has flinch anim finished playing
			if( m_LocalPrimaryAnim.nCurrentFrame == m_LocalPrimaryAnim.nTotalFrames  )
			{
				b2Vec2 enemyPos = m_pBody->GetPosition();
				b2Vec2 playerPos = b2Vec2( m_PlayerPosition.X, m_PlayerPosition.Z );

				EnemyRayCastCallback rayCallback;
				physics::PhysicsWorldB2D::GetWorld()->RayCast( &rayCallback, enemyPos, playerPos );

				// check for a state change

				if( rayCallback.hit )
				{
					if( rayCallback.hitId == PHYSICSBASICID_PLAYER )
						m_CanSeePlayer = true;
					else
						m_CanSeePlayer = false;

					m_RayContact = rayCallback.hitPoint;
					m_RayNormal = rayCallback.hitNormal;
				}
				else
				{
					m_CanSeePlayer = false;
				}

				if( m_CanSeePlayer )
				{
					// can go direct
					m_PathActive = false;
					m_SwitchToNavMesh = false;
					m_NavMeshEnable = false;
					m_NewWaypoint = false;

					m_Path.WaypointList().clear();
				}
				else
				{
					m_Path.WaypointList().clear();

					// generate nav mesh, and chase
					CalculateStartingCell(m_Pos);

					Player& player = GameSystems::GetInstance()->GetPlayer();
					NavigateToLocation( player.GetPosition(), player.GetCurrentCell() );

					if( m_PathActive )
					{
						m_SwitchToNavMesh = true;
						m_NavMeshEnable = true;
					}
				}

				if( m_PrevState == EnemyState_Run )
				{
					m_State = EnemyState_Run; 
					SetAnimation( EnemyAnim::RUN, 0, true );
				}
				/*else
				{
					m_State = EnemyState_Walk;
					SetAnimation( math::RandomNumber(EnemyAnim::WALK, EnemyAnim::WALK_ALT_RAISED_ARMS), 0, true );
				}*/

				// there should not be a polled anim
				m_pPolledAnim = 0;
			}
		}break;

		case EnemyState_Attack:
		{
			m_CanSeePlayer = false;

			m_PathActive = false;
			m_NavMeshEnable = false;
			m_NewWaypoint = false;

			Player& player = GameSystems::GetInstance()->GetPlayer();
			int clothingIndex = player.GetClothingIndex();

			m_Path.WaypointList().clear();

			// see how far the player got away
			math::Vec3 delta = m_PlayerPosition-m_Pos;
			float len = delta.lengthSqr();

			if( (m_LocalPrimaryAnim.nAnimId == EnemyAnim::ATTACK_LEFT) ||
				(m_LocalPrimaryAnim.nAnimId == EnemyAnim::ATTACK_RIGHT) )
			{
				if( m_LocalPrimaryAnim.nCurrentFrame == PUNCH_FRAME )
				{
					if( std::abs( len ) <= PLAYER_DAMAGE_DISTANCE )
					{
						// only play once on the frame
						if( !m_HitMissAudioStarted &&
							m_HitPlayerAudio != snd::INVALID_SOUNDBUFFER )
						{
							if( clothingIndex == 2 )
							{
								// naked does more damage
								player.TakeDamage(ENEMY_PUNCH_DAMAGE*2);
							}
							else if( clothingIndex == 4 )
							{
								// hidden tattoo does half damage
								player.TakeDamage(ENEMY_PUNCH_DAMAGE/2);
							}
							else
								player.TakeDamage(ENEMY_PUNCH_DAMAGE);
							
							AudioSystem::GetInstance()->PlayAudio( m_HitPlayerAudio, m_Pos, false );
						}
					}
					else
					{
						if( !m_HitMissAudioStarted &&
							m_MissPlayerAudio != snd::INVALID_SOUNDBUFFER )
						{
							AudioSystem::GetInstance()->PlayAudio( m_MissPlayerAudio, m_Pos, false );
						}

					}

					m_HitMissAudioStarted = true;
				}
			}
			else if( m_LocalPrimaryAnim.nAnimId == EnemyAnim::ATTACK_HEADBUTT )
			{
				if( m_LocalPrimaryAnim.nCurrentFrame == HEADBUTT_FRAME )
				{
					if( std::abs( len ) <= PLAYER_DAMAGE_DISTANCE )
					{
						// only play once on the frame
						if( !m_HitMissAudioStarted &&
							m_HitPlayerAudio != snd::INVALID_SOUNDBUFFER )
						{
							if( clothingIndex == 2 )
							{
								// naked does more damage
								player.TakeDamage(ENEMY_HEADBUTT_DAMAGE*2);
							}
							else if( clothingIndex == 4 )
							{
								// hidden tattoo does half damage
								player.TakeDamage(ENEMY_HEADBUTT_DAMAGE/2);
							}
							else
								player.TakeDamage(ENEMY_HEADBUTT_DAMAGE);
							
							AudioSystem::GetInstance()->PlayAudio( m_HitPlayerAudio, m_Pos, false );
						}
					}
					else
					{
						if( !m_HitMissAudioStarted &&
							m_MissPlayerAudio != snd::INVALID_SOUNDBUFFER )
						{
							AudioSystem::GetInstance()->PlayAudio( m_MissPlayerAudio, m_Pos, false );
						}
					}

					m_HitMissAudioStarted = true;
				}
			}

			if( m_ContactWithPlayer )
			{
				if( (m_LocalPrimaryAnim.nAnimId < EnemyAnim::ATTACK_HEADBUTT) ||
					(m_LocalPrimaryAnim.nAnimId > EnemyAnim::ATTACK_RIGHT) )
				{
					m_HitMissAudioStarted = false;
					SetAnimation( math::RandomNumber(EnemyAnim::ATTACK_HEADBUTT, EnemyAnim::ATTACK_RIGHT), 0, true );
				}
				else
				{
					if( m_LocalPrimaryAnim.nCurrentFrame >= m_LocalPrimaryAnim.nTotalFrames )
					{
						m_HitMissAudioStarted = false;
						SetAnimation( math::RandomNumber(EnemyAnim::ATTACK_HEADBUTT, EnemyAnim::ATTACK_RIGHT), 0, true );
					}
				}
			}
			else
			{					
				if( m_LocalPrimaryAnim.nCurrentFrame >= m_LocalPrimaryAnim.nTotalFrames )
				{
					// go for another attack
					m_PrevState = EnemyState_Attack;
					m_State = EnemyState_Idle;
					SetAnimation( EnemyAnim::IDLE_ALT, 0, false );

					m_SearchTime = 1.0f;
					m_CurrentSearchTime = 0.0f;
				}
			}

		}break;
		

		case EnemyState_Dead:
		break;

		default:
			DBG_ASSERT_MSG( 0, "unknown enemy state" );
			break;
	}
}

/////////////////////////////////////////////////////
/// Method: HasLostPart
/// Params: [in]partId
///
/////////////////////////////////////////////////////
bool Enemy::HasLostPart( int partId )
{
	int i=0;
	for( i=0; i < m_LostPartCount; ++i )
	{
		if( m_LostParts[i] == partId )
			return(true);
	}

	return(false);
}

/////////////////////////////////////////////////////
/// Method: LosePart
/// Params: [in]
///
/////////////////////////////////////////////////////
bool Enemy::LosePart( Bullet::BulletSource src )
{
	int i=0;

	int loseCount = 1;
	if( src == Bullet::BULLET_PISTOL )
	{
		int randomHit = math::RandomNumber( PISTOL_BULLET_DAMAGE, 3 );
		loseCount = randomHit;
		m_Health -= randomHit;

		//loseCount = 1;
		//m_Health -= PISTOL_BULLET_DAMAGE;
	}
	else
	{
		int randomHit = math::RandomNumber( 2, 3 );
		loseCount = randomHit;
		m_Health -= randomHit;

		//loseCount = 3;
		//m_Health -= SHOTGUN_BULLET_DAMAGE;
	}

	bool losePart = false;
	int partId = -1;

	for( i=0; i < loseCount; ++i )
	{
		while( !losePart )
		{
			int randomPartId = math::RandomNumber( 0, 9 );

			partId = m_PartLookup[randomPartId];

			if( HasLostPart( partId ) )
				continue;
			else
				losePart = true;
		}

		m_LostParts[m_LostPartCount] = partId;
		m_LostPartCount++;

		losePart = false;
	}

 	if( m_Health <= 0 )
	{
		m_Health = 0;

		if( m_State != EnemyState_Dead )
		{
			// enemy is dead
			SetAnimation( EnemyAnim::DIE, 0, true );
			m_State = EnemyState_Dead;

			m_pPolledAnim = 0;
		}
	}

	return(true);
}