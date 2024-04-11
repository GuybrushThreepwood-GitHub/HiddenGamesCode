
/*===================================================================
	File: GameCamera.h
	Game: Cabby

	(C)Hidden Games
=====================================================================*/

#ifndef __GAMECAMERA_H__
#define __GAMECAMERA_H__

#include "Level/Level.h"
#include "Player/Player.h"
#include "Camera/Camera.h"

#include "CabbyConsts.h"

class GameCamera : public Camera
{
	public:
		enum GameCamMode
		{
			NONE=0,
			SNAP,
			FOLLOW_PLAYER,
			FOLLOW_OBJECT,
			
			GOTO_POINT,
			GOTO_OBJECT,

			TOTAL_CAMMODES
		};

		enum GameCamProps
		{
			CHANGE_ZOOM = TOTAL_CAMMODES,
			RESET_ZOOM
		};

	public:
		GameCamera();
		GameCamera( Player& player );
		~GameCamera();

		virtual void SetPosition( math::Vec3& posVector );
		virtual void SetTarget( math::Vec3& targetVector );
		virtual void RotateAroundPoint( math::Vec3& vCenter, float x, float y, float z);

		void SetupCamera();
		void Update( float deltaTime );
		void SetZoom( float zoom );

		void SetLerp( bool state );
		void SetLerpSpeed( float lerpSpeed );

	private:
		bool m_LerpEnabled;
		float m_LerpSpeed;
		GameCamMode m_CameraMode;

		//ScriptDataHolder::CameraSetup m_CameraData;

		float m_CurrentZoom;

		float m_FollowObjectTime;
		float m_FollowTime;

		Player* m_Player;

		math::Vec3 m_LerpPos;
		math::Vec3 m_LerpTarget;

		//math::Vec3 m_CurrentPos;
		//math::Vec3 m_CurrentTarget;
};

/////////////////////////////////////////////////////
/// Method: SetPosition
/// 
///
/////////////////////////////////////////////////////
inline void GameCamera::SetPosition( math::Vec3& posVector )
{
	m_LerpPos = posVector;
	if( !m_LerpEnabled )
		m_Pos = posVector;
}

/////////////////////////////////////////////////////
/// Method: SetTarget
/// 
///
/////////////////////////////////////////////////////
inline void GameCamera::SetTarget( math::Vec3& targetVector )
{
	m_LerpTarget = targetVector;
	if( !m_LerpEnabled )
		m_Target = targetVector;

}

/////////////////////////////////////////////////////
/// Method: SetZoom
/// 
///
/////////////////////////////////////////////////////
inline void GameCamera::SetZoom( float zoom )
{
	m_CurrentZoom = zoom;
}

/////////////////////////////////////////////////////
/// Method: SetLerp
/// 
///
/////////////////////////////////////////////////////
inline void GameCamera::SetLerp( bool state )
{
	m_LerpEnabled = state;
}

/////////////////////////////////////////////////////
/// Method: SetLerpSpeed
/// 
///
/////////////////////////////////////////////////////
inline void GameCamera::SetLerpSpeed( float lerpSpeed )
{
	m_LerpSpeed = lerpSpeed;
}

#endif // __GAMECAMERA_H__
