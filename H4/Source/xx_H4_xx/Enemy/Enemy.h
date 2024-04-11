
/*===================================================================
	File: Enemy.h
	Game: H4

	(C)Hidden Games
=====================================================================*/

#ifndef __ENEMY_H__
#define __ENEMY_H__

#include "Physics/PhysicsIds.h"

#include "Audio/AudioSystem.h"

#include "NavigationMesh/NavObject.h"
#include "Enemy/Gib.h"

#include "GameConsts.h"

const int MAX_ENEMY_EMITTERS = 5;

extern int Stump_Neck;
extern int Stump_Head;
extern int Stump_Shoulder_L;
extern int Stump_Arm_L;
extern int Stump_Wrist_L;
extern int Stump_Elbow_L;
extern int Stump_Leg_L;
extern int Stump_Hip_L;
extern int Stump_Shin_L;
extern int Stump_Knee_L;
extern int Stump_Shoulder_R;
extern int Stump_Arm_R;
extern int Stump_Wrist_R;
extern int Stump_Elbow_R ;
extern int Stump_Leg_R;
extern int Stump_Hip_R;
extern int Stump_Shin_R;
extern int Stump_Knee_R;

extern int Skeleton_Head;
extern int Skeleton_Body;
extern int Skeleton_Arm_R;
extern int Skeleton_Wrist_R;
extern int Skeleton_Arm_L;
extern int Skeleton_Wrist_L;
extern int Skeleton_Pelvis;
extern int Skeleton_Hip_R;
extern int Skeleton_Hip_L;
extern int Skeleton_Shin_R;
extern int Skeleton_Shin_L;

extern int Normal_Head;
extern int Normal_Body;
extern int Normal_Arm_R;
extern int Normal_Wrist_R;
extern int Normal_Arm_L;
extern int Normal_Wrist_L;
extern int Normal_Hip_R;
extern int Normal_Hip_L;
extern int Normal_Shin_R;
extern int Normal_Shin_L;

extern int mShadow;

class EnemyRayCastCallback : public b2RayCastCallback
{
	public:
		EnemyRayCastCallback()
		{
			hit = false;
		}

		float32 ReportFixture(	b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float32 fraction)
		{
			const b2Body* pBody = 0;
			physics::PhysicsIdentifier* pData = 0;

			pBody = fixture->GetBody();

			if( pBody != 0 )
				pData = reinterpret_cast<physics::PhysicsIdentifier*>( pBody->GetUserData() );

			// all physics data should have the PhysicsIdentifier inherited
			DBG_ASSERT_MSG( (pData != 0), "Physics ray contact Body has no PhysicsIdentifier" );

			switch( pData->GetBaseId() )
			{
				case PHYSICSBASICID_WORLD:
				{
					// is this a low/high/blocking part of the world
					hit = true;
					hitId = PHYSICSBASICID_WORLD;

					hitPoint = point;
					hitNormal = normal;

					return fraction;
				}break;

				case PHYSICSBASICID_PLAYER:
				{
					// got here, so can see the player
					hit = true;
					hitId = PHYSICSBASICID_PLAYER;

					hitPoint = point;
					hitNormal = normal;

					return(fraction);
				}break;


				case PHYSICSBASICID_BULLET:
				case PHYSICSBASICID_ENEMY:
				case PHYSICSBASICID_FLOATINGHEAD:
				case PHYSICSBASICID_CRAWLINGSPIDER:
				case PHYSICSBASICID_GIB:
				{
					// ignore
					return(-1.0f);
				}break;

				default:
					// ray colliding with something that doesn't have an id
					DBG_ASSERT_MSG( 0, "Enemy ray cast contact from object that has no physics id" );
				break;
			}

			// shouldn't get here
			return(0.0f);
		}
		
	public:
		bool hit;
		int hitId;
		b2Vec2 hitPoint;
		b2Vec2 hitNormal;
};

class Enemy : public physics::PhysicsIdentifier, public NavObject
{
	public:
		enum EnemyState
		{
			EnemyState_InActive=0,

			EnemyState_Idle,
			EnemyState_Walk,
			EnemyState_Run,
			EnemyState_Flinch,

			EnemyState_Attack,

			EnemyState_Dead
		};

	public:
		Enemy( NavigationMesh* pNavMesh );
		virtual ~Enemy();

		void SetupEnemy( mdl::ModelHGA* model, mdl::ModelHGM* smallGib, mdl::ModelHGM* largeGib, GLuint skinIndex );
		void SetupSubmeshes();

		void PreDrawSetup( bool pauseFlag );
		void Draw();
		void DrawGibs();

		void DrawDebug();
		void Update( float deltaTime );
		void UpdateJoints( float deltaTime );
		void UpdateSkins();
		
		void SetAnimation( unsigned int nAnimId, int nFrameOffset = -1, bool bForceChange = true );
		void HandleContact( int idNum, int castId, const ContactPoint* contact, const b2Fixture* pOtherFixture, const b2Body* pOtherBody );
		void ClearContact( int idNum, int castId, const ContactPoint* contact, const b2Fixture* pOtherFixture, const b2Body* pOtherBody );

		void SetDrawState( bool state );
		bool GetDrawState();

		void SetState( EnemyState state );
		EnemyState GetState();

		void SetPosition( math::Vec3& pos );
		math::Vec3& GetPosition();

		float GetRotation()									{ return m_RotationAngle; }
		void SetTargetRotation( float angle, bool force=false );
		float GetTargetRotation()							{ return m_TargetRotation;}

		void NavigateToLocation(const math::Vec3& pos, NavigationCell* cell);
		void DisablePath()									{ m_PathActive = false; }

		collision::Sphere& GetBoundingSphere()				{ return m_BoundSphere; }
		math::Vec2& GetUnprojection()						{ return m_UnProjectedPoint; }

		void SetPlayerPosition( const math::Vec3& pos )		{ m_PlayerPosition = pos; }
		void SetInFrustum( bool state )						{ m_InFrustum = state; }
		bool InFrustum()									{ return m_InFrustum; }
	
		void SetLightZoneIndex( int index )					{ m_LightZoneIndex = index; }
		int GetLightZoneIndex()								{ return m_LightZoneIndex; }

		void SetAddKillFlag( bool state )					{ m_AddKillFlag = state; }
		bool GetAddKillFlag()								{ return m_AddKillFlag; }

	private:
		void UpdateNavMeshSwitch( float deltaTime );
		void UpdateNavMesh( float deltaTime );
		void UpdateDirect( float deltaTime );
		void UpdateIdle( float deltaTime );

		void ProcessState();

		bool LosePart( Bullet::BulletSource src );
		bool HasLostPart( int partId );
		
	private:
		EnemyState m_State;
		EnemyState m_PrevState;

		int m_Health;
		bool m_AddKillFlag;

		bool m_DrawState;
		math::Vec3 m_Pos;
		math::Vec3 m_PrevPos;

		math::Vec3 m_Dir;

		math::Vec3 m_Movement;

		float m_LastDeltaTime;
		int m_LightZoneIndex;

		struct LocalAnim
		{
			/// time
			float animTimer;
			// animation identifier
			unsigned int nAnimId;
			/// framerate of the animation
			float fAnimationFramerate;
			/// total amount of time this animation plays for
			double dTotalAnimationTime;
			/// total number of joints exported into this file
			unsigned int nTotalJoints;
			/// total number of frames in this animation
			int nTotalFrames;
			/// timer for tracking this animation
			//core::app::CTimer animTimer;
			/// looping flag
			bool bLooping;
			/// current active frame
			int nCurrentFrame;
			/// current time
			double dCurrentTime;
			/// end of the animation time
			float fEndTime;
		};

		mdl::ModelHGA* m_pModelData;
		mdl::ModelHGM* m_pSmallGib;
		mdl::ModelHGM* m_pLargeGib;

		/// current primary animation
		mdl::BoneAnimation *m_pPrimaryAnim;
		LocalAnim	m_LocalPrimaryAnim;

		/// current polled animation
		mdl::BoneAnimation *m_pPolledAnim;
		LocalAnim	m_LocalPolledAnim;

		unsigned int m_JointCount;
		mdl::TBoneJoint* m_pJoints;

		// skin
		GLuint m_SkinTextureIndex;

		bool m_SwitchToNavMesh;
		bool m_NavMeshEnable;
		bool m_DoorCollision;

		// b2d body
		b2Body* m_pBody;

		// player search
		bool m_CanSeePlayer;
		math::Vec3 m_PlayerPosition;
		b2Vec2 m_RayContact;
		b2Vec2 m_RayNormal;
		float m_SearchTime;
		float m_CurrentSearchTime;

		// player touch
		bool m_ContactWithPlayer;

		// used for culling
		bool m_InFrustum;
		collision::Sphere m_BoundSphere;
		math::Vec2 m_UnProjectedPoint;

		bool m_NewWaypoint;
		float m_MaxSpeed;
		float m_TargetRotation;
		float m_RotationAngle;

		float m_DeadFadeTimer;
		int m_DeadAlpha;

		// gibs
		bool m_SpawnNewGib;
		int m_NextGibIndex;
		Gib* m_Gibs[MAX_ENEMY_GIBS];

		int m_LostPartCount;
		int m_LostParts[MAX_ENEMY_LOST_PARTS];

		int m_PartLookup[10];

		// emitters
		efx::Emitter* m_pEmitters[MAX_ENEMY_EMITTERS];

		// audio
		snd::Sound m_SndLoad;
		ALuint m_SpottedPlayerAudio;
		ALuint m_HitPlayerAudio;
		ALuint m_MissPlayerAudio;
		ALuint m_DeathImpactPlayerAudio;
		bool m_HitMissAudioStarted;
		bool m_ImpactAudioStarted;
};

/////////////////////////////////////////////////////
/// Method: SetDrawState
/// Params: [in]state
///
/////////////////////////////////////////////////////
inline void Enemy::SetDrawState( bool state )
{
	m_DrawState = state;
}

/////////////////////////////////////////////////////
/// Method: SetDrawState
/// Params: [in]state
///
/////////////////////////////////////////////////////
inline bool Enemy::GetDrawState()
{
	return(m_DrawState);
}

/////////////////////////////////////////////////////
/// Method: SetDrawState
/// Params: [in]state
///
/////////////////////////////////////////////////////
inline void Enemy::SetState( EnemyState state )
{
	m_State = state;
}

/////////////////////////////////////////////////////
/// Method: GetState
/// Params: None
///
/////////////////////////////////////////////////////
inline Enemy::EnemyState Enemy::GetState()
{
	return m_State;
}

/////////////////////////////////////////////////////
/// Method: SetPosition
/// Params: [in]pos
///
/////////////////////////////////////////////////////
inline void Enemy::SetPosition( math::Vec3& pos )
{
	m_Pos = pos;
	m_PrevPos = pos;

	CalculateStartingCell( m_Pos );

	if( m_pBody != 0 )
	{
		m_pBody->SetTransform( b2Vec2(m_Pos.X, m_Pos.Z), 0.0f );	
	}

	m_BoundSphere.vCenterPoint = m_Pos;
	m_BoundSphere.vCenterPoint.Y = (m_Pos.Y+1.4f);
}

/////////////////////////////////////////////////////
/// Method: GetPosition
/// Params: None
///
/////////////////////////////////////////////////////
inline math::Vec3& Enemy::GetPosition()
{
	return m_Pos;
}

/////////////////////////////////////////////////////
/// Method: SetTargetRotation
/// Params: [in]angle
///
/////////////////////////////////////////////////////
inline void Enemy::SetTargetRotation( float angle, bool force )
{
	m_TargetRotation = angle;

	if( force )
		m_RotationAngle = angle;
}

/////////////////////////////////////////////////////
/// Method: NavigateToLocation
/// Params: [in]pos, [in]cell
///
/////////////////////////////////////////////////////
inline void Enemy::NavigateToLocation(const math::Vec3& pos, NavigationCell* cell)
{
	if (m_Parent)
	{
		if( cell == 0 )
			return;

		m_Movement = math::Vec3(0.0f,0.0f,0.0f);

		m_PathActive = m_Parent->BuildNavigationPath( m_Path, m_CurrentCell, m_Pos, cell, pos );

		if(m_PathActive)
		{
			m_NextWaypoint = m_Path.WaypointList().begin();
			m_NewWaypoint = true;
		}
	}
}

#endif // __ENEMY_H__

