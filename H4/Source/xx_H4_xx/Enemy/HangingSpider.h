
/*===================================================================
	File: HangingSpider.h
	Game: H4

	(C)Hidden Games
=====================================================================*/

#ifndef __HANGINGSPIDER_H__
#define __HANGINGSPIDER_H__

#include "Physics/PhysicsIds.h"

#include "GameConsts.h"

const int MAX_HANGINGSPIDER_EMITTERS = 1;
const float HANGINGSPIDER_HURTTIMER = 3.0f;

class HangingSpider
{
	public:
		enum HangingSpiderState
		{
			HangingSpiderState_InActive=0,

			HangingSpiderState_IdleUp,
			HangingSpiderState_IdleDown,

			HangingSpiderState_Drop,

			HangingSpiderState_Swing,
			HangingSpiderState_Climb,

			HangingSpiderState_Dead
		};

	public:
		HangingSpider( );
		virtual ~HangingSpider();

		void SetupHangingSpider( mdl::ModelHGA* model );

		void PreDrawSetup( bool pauseFlag );
		void Draw();

		void DrawDebug();
		void Update( float deltaTime );
		void UpdateJoints( float deltaTime );
		void UpdateSkins();
		
		void SetAnimation( unsigned int nAnimId, int nFrameOffset = -1, bool bForceChange = true );

		void SetDrawState( bool state );
		bool GetDrawState();

		void SetState( HangingSpiderState state );
		HangingSpiderState GetState();

		void SetPosition( math::Vec3& pos );
		math::Vec3& GetPosition();
		void SetRotation( float angle );

		collision::Sphere& GetBoundingSphere()				{ return m_BoundSphere; }
		math::Vec2& GetUnprojection()						{ return m_UnProjectedPoint; }

		void SetPlayerPosition( const math::Vec3& pos )		{ m_PlayerPosition = pos; }
		void SetInFrustum( bool state )						{ m_InFrustum = state; }
		bool InFrustum()									{ return m_InFrustum; }
	
		bool CanHurtPlayer()								{ return(m_HurtPlayer); }
		void HurtPlayer()									{ m_HurtPlayer = false; m_HurtPlayerTimer = HANGINGSPIDER_HURTTIMER; }
		
		void SetHealth( int val )							{ m_Health = val; }
		int GetHealth()										{ return m_Health; }

		void SpawnBlood();

	private:
		void ProcessState();
		
	private:
		HangingSpiderState m_State;
		HangingSpiderState m_PrevState;

		int m_Health;

		bool m_DrawState;
		math::Vec3 m_Pos;
		math::Vec3 m_PrevPos;
		float m_RotationAngle;

		float m_LastDeltaTime;

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

		// player search
		math::Vec3 m_PlayerPosition;

		// player touch
		bool m_ContactWithPlayer;

		float m_DropTimer;
		float m_ResetTimer;

		int m_NumSwings;
		int m_CompleteSwings;

		efx::Emitter* m_pEmitters[MAX_HANGINGSPIDER_EMITTERS];

		// used for culling
		bool m_InFrustum;
		collision::Sphere m_BoundSphere;
		math::Vec2 m_UnProjectedPoint;

		// death
		float m_DeadTimer;

		// damage player
		bool m_HurtPlayer;
		float m_HurtPlayerTimer;

		// audio
		snd::Sound m_SndLoad;
		ALuint m_DropAudio;
		ALuint m_ClimbAudio;
		ALuint m_DeathAudio;
};

/////////////////////////////////////////////////////
/// Method: SetDrawState
/// Params: [in]state
///
/////////////////////////////////////////////////////
inline void HangingSpider::SetDrawState( bool state )
{
	m_DrawState = state;
}

/////////////////////////////////////////////////////
/// Method: SetDrawState
/// Params: [in]state
///
/////////////////////////////////////////////////////
inline bool HangingSpider::GetDrawState()
{
	return(m_DrawState);
}

/////////////////////////////////////////////////////
/// Method: SetDrawState
/// Params: [in]state
///
/////////////////////////////////////////////////////
inline void HangingSpider::SetState( HangingSpiderState state )
{
	m_State = state;
}

/////////////////////////////////////////////////////
/// Method: GetState
/// Params: None
///
/////////////////////////////////////////////////////
inline HangingSpider::HangingSpiderState HangingSpider::GetState()
{
	return m_State;
}

/////////////////////////////////////////////////////
/// Method: SetPosition
/// Params: [in]pos
///
/////////////////////////////////////////////////////
inline void HangingSpider::SetPosition( math::Vec3& pos )
{
	m_Pos = pos;
	m_PrevPos = pos;

	m_BoundSphere.vCenterPoint = m_Pos;
}

/////////////////////////////////////////////////////
/// Method: GetPosition
/// Params: None
///
/////////////////////////////////////////////////////
inline math::Vec3& HangingSpider::GetPosition()
{
	return m_Pos;
}

/////////////////////////////////////////////////////
/// Method: SetRotation
/// Params: [in]angle
///
/////////////////////////////////////////////////////
inline void HangingSpider::SetRotation( float angle )
{
	m_RotationAngle = angle;
}

#endif // __HANGINGSPIDER_H__

