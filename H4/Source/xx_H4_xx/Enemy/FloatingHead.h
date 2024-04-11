
/*===================================================================
	File: FloatingHead.h
	Game: H4

	(C)Hidden Games
=====================================================================*/

#ifndef __FLOATINGHEAD_H__
#define __FLOATINGHEAD_H__

#include "Physics/PhysicsIds.h"

#include "NavigationMesh/NavObject.h"

#include "GameConsts.h"

const int MAX_FLOATINGHEAD_EMITTERS = 2;
const float FLOATINGHEAD_HURTTIMER = 5.0f;

class FloatingHeadRayCastCallback : public b2RayCastCallback
{
	public:
		FloatingHeadRayCastCallback()
		{
			hit = false;
		}

		float32 ReportFixture(	b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float32 fraction)
		{
			const b2Body* pBody = 0;
			physics::PhysicsIdentifier* pData = 0;
			int numeric1 = -9999;
			int numeric2 = -9999;
			int numeric3 = -9999;
			int numeric4 = -9999;
			int numeric5 = -9999;

			pBody = fixture->GetBody();

			if( pBody != 0 )
			{
				pData = reinterpret_cast<physics::PhysicsIdentifier*>( fixture->GetUserData() );
						
				DBG_ASSERT_MSG( (pData != 0), "Could not get physics user data on floating head ray world contact" );
						
				numeric1 = pData->GetNumeric1();
				numeric2 = pData->GetNumeric2();
				numeric3 = pData->GetNumeric3();
				numeric4 = pData->GetNumeric4();
				numeric5 = pData->GetNumeric5();
			}

			switch( pData->GetBaseId() )
			{
				case PHYSICSBASICID_WORLD:
				{
					if( numeric1 == NUMERIC1_WORLD_HIGHWALL || numeric1 == -9999 )
					{
						// is this a low/high/blocking part of the world
						hit = true;
						hitId = PHYSICSBASICID_WORLD;

						hitPoint = point;
						hitNormal = normal;

						return fraction;
					}
					else
						return(-1.0f);
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

class FloatingHead : public physics::PhysicsIdentifier
{
	public:
		enum FloatingHeadState
		{
			FloatingHeadState_InActive=0,

			FloatingHeadState_Idle,

			FloatingHeadState_Move,

			FloatingHeadState_Attack,
			FloatingHeadState_Dead
		};

	public:
		FloatingHead( );
		virtual ~FloatingHead();

		void SetupFloatingHead( mdl::ModelHGA* model, int skinIndex );

		void PreDrawSetup( bool pauseFlag );
		void Draw();

		void DrawDebug();
		void Update( float deltaTime );
		void UpdateJoints( float deltaTime );
		void UpdateSkins();
		
		void SetAnimation( unsigned int nAnimId, int nFrameOffset = -1, bool bForceChange = true );
		void HandleContact( int idNum, int castId, const ContactPoint* contact, const b2Fixture* pOtherFixture, const b2Body* pOtherBody );
		void ClearContact( int idNum, int castId, const ContactPoint* contact, const b2Fixture* pOtherFixture, const b2Body* pOtherBody );

		void SetDrawState( bool state );
		bool GetDrawState();

		void SetState( FloatingHeadState state );
		FloatingHeadState GetState();

		void SetPosition( math::Vec3& pos );
		math::Vec3& GetPosition();

		float GetRotation()									{ return m_RotationAngle; }
		void SetTargetRotation( float angle, bool force=false );
		float GetTargetRotation()							{ return m_TargetRotation;}

		const collision::Sphere& GetBoundingSphere()		{ return m_BoundSphere; }
		math::Vec2& GetUnprojection()						{ return m_UnProjectedPoint; }

		void SetPlayerPosition( const math::Vec3& pos )		{ m_PlayerPosition = pos; }
		void SetInFrustum( bool state )						{ m_InFrustum = state; }
		bool InFrustum()									{ return m_InFrustum; }
	
		void SetLightZoneIndex( int index )					{ m_LightZoneIndex = index; }
		int GetLightZoneIndex()								{ return m_LightZoneIndex; }

		bool CanHurtPlayer()								{ return(m_HurtPlayer); }
		void HurtPlayer()									{ m_HurtPlayer = false; m_HurtPlayerTimer = FLOATINGHEAD_HURTTIMER; }

	private:
		void UpdateDirect( float deltaTime );
		void UpdateIdle( float deltaTime );

		void ProcessState();
		
	private:
		FloatingHeadState m_State;
		FloatingHeadState m_PrevState;

		bool m_DrawState;
		math::Vec3 m_Pos;
		math::Vec3 m_PrevPos;

		math::Vec3 m_Dir;

		math::Vec3 m_Movement;

		float m_LastDeltaTime;
		int m_LightZoneIndex;

		struct LocalAnim
		{
			float animTimer;

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

		/// current primary animation
		mdl::BoneAnimation *m_pPrimaryAnim;
		LocalAnim	m_LocalPrimaryAnim;

		/// current polled animation
		mdl::BoneAnimation *m_pPolledAnim;
		LocalAnim	m_LocalPolledAnim;

		unsigned int m_JointCount;
		mdl::TBoneJoint* m_pJoints;

		// skin
		int m_SkinTextureIndex;
		int m_ShadowIndex;

		bool m_NavMeshEnable;

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

		float m_MaxSpeed;
		float m_TargetRotation;
		float m_RotationAngle;

		float m_DeadTimer;

		// damage player
		bool m_HurtPlayer;
		float m_HurtPlayerTimer;

		// audio
		snd::Sound m_SndLoad;
		ALuint m_SpottedPlayerAudio;
		ALuint m_DamagePlayerAudio;

		ALuint m_SoundLoopSource;

		// drips
		efx::Emitter* m_pEmitters[MAX_FLOATINGHEAD_EMITTERS];
};

/////////////////////////////////////////////////////
/// Method: SetDrawState
/// Params: [in]state
///
/////////////////////////////////////////////////////
inline void FloatingHead::SetDrawState( bool state )
{
	m_DrawState = state;
}

/////////////////////////////////////////////////////
/// Method: SetDrawState
/// Params: [in]state
///
/////////////////////////////////////////////////////
inline bool FloatingHead::GetDrawState()
{
	return(m_DrawState);
}

/////////////////////////////////////////////////////
/// Method: SetDrawState
/// Params: [in]state
///
/////////////////////////////////////////////////////
inline void FloatingHead::SetState( FloatingHeadState state )
{
	m_State = state;
}

/////////////////////////////////////////////////////
/// Method: GetState
/// Params: None
///
/////////////////////////////////////////////////////
inline FloatingHead::FloatingHeadState FloatingHead::GetState()
{
	return m_State;
}

/////////////////////////////////////////////////////
/// Method: SetPosition
/// Params: [in]pos
///
/////////////////////////////////////////////////////
inline void FloatingHead::SetPosition( math::Vec3& pos )
{
	m_Pos = pos;
	m_PrevPos = pos;

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
inline math::Vec3& FloatingHead::GetPosition()
{
	return m_Pos;
}

/////////////////////////////////////////////////////
/// Method: SetTargetRotation
/// Params: [in]angle
///
/////////////////////////////////////////////////////
inline void FloatingHead::SetTargetRotation( float angle, bool force )
{
	m_TargetRotation = angle;

	if( force )
		m_RotationAngle = angle;
}

#endif // __FLOATINGHEAD_H__

