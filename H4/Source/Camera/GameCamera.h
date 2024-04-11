
/*===================================================================
	File: GameCamera.h
	Game: H4

	(C)Hidden Games
=====================================================================*/

#ifndef __GAMECAMERA_H__
#define __GAMECAMERA_H__

#include "Level/Level.h"
#include "Player/Player.h"
#include "Camera/Camera.h"

class GameCamera : public Camera
{
	public:
		enum GameCamMode
		{
			FOLLOW_PLAYER=0,
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
		GameCamera( Player& player );
		~GameCamera();

		virtual void SetPosition( math::Vec3& posVector );
		virtual void SetTarget( math::Vec3& targetVector );

		void SetupCamera();
		void Update( float deltaTime );
		void SetZoom( float zoom );

		//void GoToPoint( CameraPoint* pPoint, const char* callFunc );
		void GoToObject( BaseObject* pObject, const char* callFunc );

		void FollowPlayer( const char* callFunc );
		void FollowObject( BaseObject* pObject, float followTime );

		void SetLerp( bool state );
		void SetLerpSpeed( float lerpSpeed );

	private:
		bool m_LerpEnabled;
		float m_LerpSpeed;
		char m_CameraFunc[LUAFUNC_STRBUFFER];
		GameCamMode m_CameraMode;

		ScriptDataHolder::CameraSetup m_CameraData;

		float m_CurrentZoom;

		//CameraPoint* m_pCamPoint;
		BaseObject* m_pFollowObject;
		float m_FollowObjectTime;
		float m_FollowTime;

		Player& m_Player;

		math::Vec3 m_LerpPos;
		math::Vec3 m_LerpTarget;
};

/////////////////////////////////////////////////////
/// Method: SetPosition
/// 
///
/////////////////////////////////////////////////////
inline void GameCamera::SetPosition( math::Vec3& posVector )
{
	m_LerpPos = posVector;
}

/////////////////////////////////////////////////////
/// Method: SetTarget
/// 
///
/////////////////////////////////////////////////////
inline void GameCamera::SetTarget( math::Vec3& targetVector )
{
	m_LerpTarget = targetVector;
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
