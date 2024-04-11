
/*===================================================================
	File: GameCamera.cpp
	Game: H4

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
GameCamera::GameCamera( Player& player )
	: m_Player(player)
	, m_CameraMode(FOLLOW_PLAYER)
	//, m_pCamPoint(0)
	, m_pFollowObject(0)
	, m_LerpEnabled(true)
	, m_LerpSpeed(CAM_CATCHUP)
{
	ScriptDataHolder* pScriptData = GetScriptDataHolder();
	m_CameraData = pScriptData->GetCameraData();

	m_CurrentZoom = 50.0f;//m_CameraData.cam_zdistance;

	math::Vec3 playerPos = m_Player.GetPosition();
	m_LerpPos = math::Vec3( playerPos.X, playerPos.Y, playerPos.Z );
	m_LerpTarget = math::Vec3( playerPos.X, playerPos.Y, playerPos.Z-1.0f );

	std::memset( m_CameraFunc, 0, sizeof(char)*LUAFUNC_STRBUFFER );
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
	renderer::OpenGL::GetInstance()->SetLookAt( m_LerpPos.X, m_LerpPos.Y, m_LerpPos.Z, 
													m_LerpTarget.X, m_LerpTarget.Y, m_LerpTarget.Z );
}

/////////////////////////////////////////////////////
/// Method: Update
/// Params: deltaTime
///
/////////////////////////////////////////////////////
void GameCamera::Update( float deltaTime )
{
	int err = 0;
	math::Vec3 playerPos = m_Player.GetPosition();

	math::Vec3 camPos = m_Player.GetCamPos();
	math::Vec3 camTarget = m_Player.GetCamTarget();

	/*if( m_Player.IsRunning() )
	{
		m_LerpSpeed = CAM_CATCHUP*2.25f;
	}
	else*/
		m_LerpSpeed = CAM_CATCHUP*2.0f;

	math::Vec3 newPos( 0.0f, 0.0f, 0.0f );
	math::Vec3 newLookAt( 0.0f, 0.0f, 0.0f );

	const float LERP_TOLERANCE = 0.01f;

	if( m_LerpEnabled )
	{
		if( m_CameraMode == FOLLOW_PLAYER )
		{
			newPos = camPos;

			bool posArrive = math::WithinTolerance( m_LerpPos, newPos, LERP_TOLERANCE );

			//if( !posArrive )
				m_LerpPos = math::Lerp( m_LerpPos, newPos, deltaTime*m_LerpSpeed );

			newLookAt = camTarget;

			bool targetArrive =math::WithinTolerance( m_LerpTarget, newLookAt, LERP_TOLERANCE );
			//if( !targetArrive )
				m_LerpTarget = math::Lerp( m_LerpTarget, newLookAt, deltaTime*m_LerpSpeed );

			// call arrival function
			if( posArrive && targetArrive )
			{
				if( m_CameraFunc[0] != '\0' )
				{
					if( script::LuaFunctionCheck( m_CameraFunc ) == 0 )
					{
						int errorFuncIndex;
						errorFuncIndex = script::GetErrorFuncIndex();

						// function exists call it
						lua_getglobal( script::LuaScripting::GetState(), m_CameraFunc );
						err = lua_pcall( script::LuaScripting::GetState(), 0, 0, errorFuncIndex );

						// LUA_ERRRUN --- a runtime error. 
						// LUA_ERRMEM --- memory allocation error. For such errors, Lua does not call the error handler function. 
						// LUA_ERRERR --- error while running the error handler function. 

						if( err == LUA_ERRRUN || err == LUA_ERRMEM || err == LUA_ERRERR )
						{
							DBGLOG( "LUASCRIPTING: *ERROR* Calling script '%s' failed\n", m_CameraFunc );
							DBGLOG( "\tLUA_TRACEBACK: %s\n", lua_tostring(script::LuaScripting::GetState(), -1 ) );

							script::StackDump(script::LuaScripting::GetState());

							DBG_ASSERT_MSG( 0, "LUASCRIPTING: *ERROR* Calling script '%s' failed", m_CameraFunc );
						}

						// clear the func
						std::memset( m_CameraFunc, 0, sizeof(char)*LUAFUNC_STRBUFFER );
					}
				}
			}
		}
		else if( m_CameraMode == FOLLOW_OBJECT )
		{
			if( m_FollowObjectTime > 0.0f )
			{
				if( m_FollowTime < m_FollowObjectTime )
				{
					m_FollowTime += deltaTime;

					math::Vec3 camPoint = m_pFollowObject->GetPos();

					newPos = math::Vec3( camPoint.X, camPoint.Y, m_CurrentZoom );

					bool posArrive = math::WithinTolerance( m_LerpPos, newPos, LERP_TOLERANCE );
					if( !posArrive )
						m_LerpPos = math::Lerp( m_LerpPos, newPos, deltaTime*CAM_CATCHUP );

					newLookAt = math::Vec3( camPoint.X, camPoint.Y, -500.0f );

					bool targetArrive = math::WithinTolerance( m_LerpTarget, newLookAt, LERP_TOLERANCE );
					if( !targetArrive )
						m_LerpTarget = math::Lerp( m_LerpTarget, newLookAt, deltaTime*CAM_CATCHUP );
				}
				else
				{
					m_FollowTime = 0.0f;
					m_FollowObjectTime = 0.0f;

					m_CameraMode = FOLLOW_PLAYER;
					//m_pCamPoint = 0;
					m_pFollowObject = 0;

					std::memset( m_CameraFunc, 0, sizeof(char)*LUAFUNC_STRBUFFER );
				}
			}
			else
			{
				math::Vec3 camPoint = m_pFollowObject->GetPos();

				newPos = math::Vec3( camPoint.X, camPoint.Y, m_CurrentZoom );

				bool posArrive = math::WithinTolerance( m_LerpPos, newPos, LERP_TOLERANCE );
				if( !posArrive )
					m_LerpPos = math::Lerp( m_LerpPos, newPos, deltaTime*CAM_CATCHUP );

				newLookAt = math::Vec3( camPoint.X, camPoint.Y, -500.0f );

				bool targetArrive = math::WithinTolerance( m_LerpTarget, newLookAt, LERP_TOLERANCE );
				if( !targetArrive )
					m_LerpTarget = math::Lerp( m_LerpTarget, newLookAt, deltaTime*CAM_CATCHUP );
			}
		}
		/*else if( m_CameraMode == GOTO_POINT )
		{
			math::Vec3 camPoint = m_pCamPoint->GetPos();

			newPos = math::Vec3( camPoint.X, camPoint.Y, m_CurrentZoom );

			bool posArrive = math::WithinTolerance( m_LerpPos, newPos, LERP_TOLERANCE );
			if( !posArrive )
				m_LerpPos = math::Lerp( m_LerpPos, newPos, deltaTime*CAM_CATCHUP );

			newLookAt = math::Vec3( camPoint.X, camPoint.Y, -500.0f );

			bool targetArrive = math::WithinTolerance( m_LerpTarget, newLookAt, LERP_TOLERANCE );
			if( !targetArrive )
				m_LerpTarget = math::Lerp( m_LerpTarget, newLookAt, deltaTime*CAM_CATCHUP );

			// call arrival function
			if( posArrive && targetArrive )
			{
				if( m_CameraFunc[0] != '\0' )
				{
					if( script::LuaFunctionCheck( m_CameraFunc ) == 0 )
					{
						int errorFuncIndex;
						errorFuncIndex = script::GetErrorFuncIndex();

						// function exists call it
						lua_getglobal( script::LuaScripting::GetState(), m_CameraFunc );
						err = lua_pcall( script::LuaScripting::GetState(), 0, 0, errorFuncIndex );

						// LUA_ERRRUN --- a runtime error. 
						// LUA_ERRMEM --- memory allocation error. For such errors, Lua does not call the error handler function. 
						// LUA_ERRERR --- error while running the error handler function. 

						if( err == LUA_ERRRUN || err == LUA_ERRMEM || err == LUA_ERRERR )
						{
							DBGLOG( "LUASCRIPTING: *ERROR* Calling script '%s' failed\n", m_CameraFunc );
							DBGLOG( "\tLUA_TRACEBACK: %s\n", lua_tostring( script::LuaScripting::GetState(), -1 ) );

							script::StackDump(script::LuaScripting::GetState());

							DBG_ASSERT_MSG( 0, "LUASCRIPTING: *ERROR* Calling script '%s' failed", m_CameraFunc );
						}

						// clear the func
						std::memset( m_CameraFunc, 0, sizeof(char)*LUAFUNC_STRBUFFER );
					}
				}
			}
		}*/
		else if( m_CameraMode == GOTO_OBJECT )
		{
			math::Vec3 camPoint = m_pFollowObject->GetPos();

			newPos = math::Vec3( camPoint.X, camPoint.Y, m_CurrentZoom );

			bool posArrive = math::WithinTolerance( m_LerpPos, newPos, LERP_TOLERANCE );
			if( !posArrive )
				m_LerpPos = math::Lerp( m_LerpPos, newPos, deltaTime*CAM_CATCHUP );

			newLookAt = math::Vec3( camPoint.X, camPoint.Y, -500.0f );

			bool targetArrive = math::WithinTolerance( m_LerpTarget, newLookAt, LERP_TOLERANCE );
			if( !targetArrive )
				m_LerpTarget = math::Lerp( m_LerpTarget, newLookAt, deltaTime*CAM_CATCHUP );

			// call arrival function
			if( posArrive && targetArrive )
			{
				if( m_CameraFunc[0] != '\0' )
				{
					if( script::LuaFunctionCheck( m_CameraFunc ) == 0 )
					{
						int errorFuncIndex;
						errorFuncIndex = script::GetErrorFuncIndex();

						// function exists call it
						lua_getglobal( script::LuaScripting::GetState(), m_CameraFunc );
						err = lua_pcall( script::LuaScripting::GetState(), 0, 0, errorFuncIndex );

						// LUA_ERRRUN --- a runtime error. 
						// LUA_ERRMEM --- memory allocation error. For such errors, Lua does not call the error handler function. 
						// LUA_ERRERR --- error while running the error handler function. 

						if( err == LUA_ERRRUN || err == LUA_ERRMEM || err == LUA_ERRERR )
						{
							DBGLOG( "LUASCRIPTING: *ERROR* Calling script '%s' failed\n", m_CameraFunc );
							DBGLOG( "\tLUA_TRACEBACK: %s\n", lua_tostring( script::LuaScripting::GetState(), -1 ) );

							script::StackDump(script::LuaScripting::GetState());

							DBG_ASSERT_MSG( 0, "LUASCRIPTING: *ERROR* Calling script '%s' failed", m_CameraFunc );
						}

						// clear the func
						std::memset( m_CameraFunc, 0, sizeof(char)*LUAFUNC_STRBUFFER );
					}
				}
			}
		}
	}
	else
	{
		if( m_CameraMode == FOLLOW_PLAYER )
		{
			m_LerpPos = camPos;
			m_LerpTarget = camTarget;
		}
		else if( m_CameraMode == FOLLOW_OBJECT )
		{
			math::Vec3 camPoint = m_pFollowObject->GetPos();

			if( m_FollowObjectTime > 0.0f )
			{
				if( m_FollowTime < m_FollowObjectTime )
				{
					m_FollowTime += deltaTime;
			
					m_LerpPos = math::Vec3( camPoint.X, camPoint.Y, m_CurrentZoom );
					m_LerpTarget = math::Vec3( camPoint.X, camPoint.Y, -500.0f );
				}
				else
				{
					m_FollowTime = 0.0f;
					m_FollowObjectTime = 0.0f;

					m_CameraMode = FOLLOW_PLAYER;
					//m_pCamPoint = 0;
					m_pFollowObject = 0;

					std::memset( m_CameraFunc, 0, sizeof(char)*LUAFUNC_STRBUFFER );
				}
			}
			else
			{
				m_LerpPos = math::Vec3( camPoint.X, camPoint.Y, m_CurrentZoom );
				m_LerpTarget = math::Vec3( camPoint.X, camPoint.Y, -500.0f );
			}
		}
		/*else if( m_CameraMode == GOTO_POINT )
		{
			math::Vec3 camPoint = m_pCamPoint->GetPos();

			m_LerpPos = math::Vec3( camPoint.X, camPoint.Y, m_CurrentZoom );
			m_LerpTarget = math::Vec3( camPoint.X, camPoint.Y, -500.0f );
		}*/
		else if( m_CameraMode == GOTO_OBJECT )
		{
			math::Vec3 camPoint = m_pFollowObject->GetPos();

			m_LerpPos = math::Vec3( camPoint.X, camPoint.Y, m_CurrentZoom );
			m_LerpTarget = math::Vec3( camPoint.X, camPoint.Y, -500.0f );
		}
	}

	m_Pos = m_LerpPos;
	m_Target = m_LerpTarget;
}

/////////////////////////////////////////////////////
/// Method: GoToPoint
/// Params: pPoint
///
/////////////////////////////////////////////////////
/*void GameCamera::GoToPoint( CameraPoint* pPoint, const char* callFunc )
{
	m_CameraMode = GOTO_POINT;
	m_pCamPoint = pPoint;
	if( callFunc )
		snprintf( m_CameraFunc, LUAFUNC_STRBUFFER, callFunc );
}*/

/////////////////////////////////////////////////////
/// Method: GoToObject
/// Params: pObject
///
/////////////////////////////////////////////////////
void GameCamera::GoToObject( BaseObject* pObject, const char* callFunc )
{
	m_CameraMode = GOTO_OBJECT;
	//m_pCamPoint = 0;
	m_pFollowObject = pObject;

	if( callFunc )
		snprintf( m_CameraFunc, LUAFUNC_STRBUFFER, "%s", callFunc );
}

/////////////////////////////////////////////////////
/// Method: FollowPlayer
/// Params: None
///
/////////////////////////////////////////////////////
void GameCamera::FollowPlayer( const char* callFunc )
{
	m_CameraMode = FOLLOW_PLAYER;
	//m_pCamPoint = 0;

	if( callFunc )
		snprintf( m_CameraFunc, LUAFUNC_STRBUFFER, "%s", callFunc );
}

/////////////////////////////////////////////////////
/// Method: FollowObject
/// Params: None
///
/////////////////////////////////////////////////////
void GameCamera::FollowObject( BaseObject* pObject, float followTime )
{
	m_CameraMode = FOLLOW_OBJECT;
	//m_pCamPoint = 0;
	m_pFollowObject = pObject;

	m_FollowTime = 0.0f;
	m_FollowObjectTime = followTime;
}
