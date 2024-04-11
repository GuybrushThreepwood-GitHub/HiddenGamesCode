
/*===================================================================
	File: CDoorSlideRightRight.cpp
	Game: Cabby

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

#include "CabbyConsts.h"
#include "Cabby.h"

#include "Audio/AudioSystem.h"

#include "Resources/ModelResources.h"

#include "Physics/PhysicsWorld.h"
#include "Physics/Objects/DoorSlideRight.h"

const float AUDIO_GAIN = 1.5f;

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
DoorSlideRight::DoorSlideRight()
	: PhysicsIdentifier( PHYSICSBASICID_DOOR, PHYSICSCASTID_DOORSLIDERIGHT )
{
	m_pModel = res::LoadModel( 102 );
	DBG_ASSERT( m_pModel != 0 );

	m_AudioId = snd::INVALID_SOUNDBUFFER;
	m_AudioId = AudioSystem::GetInstance()->AddAudioFile( 500/*"slide_door.wav"*/ );
}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
DoorSlideRight::~DoorSlideRight()
{
	if( m_pModel != 0 )
	{
		res::RemoveModel( m_pModel );
		m_pModel = 0;
	}
}

/////////////////////////////////////////////////////
/// Method: Create
/// Params: None
///
/////////////////////////////////////////////////////
int DoorSlideRight::Create( math::Vec3 vPos, float extendMotorSpeed, float retractMotorSpeed, float extendPauseTime, float retractPauseTime )
{
#ifdef _DEBUG
	if( script::LuaFunctionCheck( "SetupDoorSlideRight" ) == 0 )
#endif // _DEBUG
	{
		int result = 0;

		int errorFuncIndex;
		errorFuncIndex = script::GetErrorFuncIndex();

		// get the function
		lua_getglobal( script::LuaScripting::GetState(), "SetupDoorSlideRight" );

		// push the parameters
		m_SpawnLocation = vPos;

		lua_pushnumber( script::LuaScripting::GetState(), vPos.X ); // posX
		lua_pushnumber( script::LuaScripting::GetState(), vPos.Y ); // posY
		lua_pushnumber( script::LuaScripting::GetState(), extendMotorSpeed );

		result = lua_pcall( script::LuaScripting::GetState(), 3, 2, errorFuncIndex );

		// LUA_ERRRUN --- a runtime error. 
		// LUA_ERRMEM --- memory allocation error. For such errors, Lua does not call the error handler function. 
		// LUA_ERRERR --- error while running the error handler function. 

		if( result == LUA_ERRRUN || result == LUA_ERRMEM || result == LUA_ERRERR )
		{
			DBGLOG( "DoorSlideRight: *ERROR* Calling function '%s' failed\n", "SetupDoorSlideRight" );
			DBGLOG( "\tLUA_TRACEBACK: %s\n", lua_tostring( script::LuaScripting::GetState(), -1 ) );
			DBG_ASSERT(0);

			return(1);
		}

		// grab the return
		m_pJoint = reinterpret_cast<b2PrismaticJoint *>( lua_touserdata(script::LuaScripting::GetState(), -1) );
		lua_pop( script::LuaScripting::GetState(), 1 );
		DBG_ASSERT( m_pJoint != 0 );

		m_pBody = reinterpret_cast<b2Body *>( lua_touserdata(script::LuaScripting::GetState(), -1) );
		lua_pop( script::LuaScripting::GetState(), 1 );
		DBG_ASSERT( m_pBody != 0 );
		m_pBody->SetUserData( reinterpret_cast<void *>(this) );

		b2FilterData fd;

		fd.categoryBits = WORLD_CATEGORY;
		fd.maskBits = PLAYER_CATEGORY;
		fd.groupIndex = WORLD_GROUP;
		b2Shape* shapeList = m_pBody->GetShapeList();
		while( shapeList != 0 )
		{
			shapeList->SetFilterData(fd);

			shapeList = shapeList->GetNext();
		}

		m_ExtendMotorSpeed = extendMotorSpeed;
		m_RetractMotorSpeed = retractMotorSpeed;
		m_NewMotorSpeed = m_RetractMotorSpeed;
		m_ExtendPauseTime = extendPauseTime;
		m_RetractPauseTime = retractPauseTime;

		b2Vec2 bodyPos = m_pBody->GetWorldCenter();
		m_BoundingSphere.vCenterPoint.X = bodyPos.x;
		m_BoundingSphere.vCenterPoint.Y = bodyPos.y;
	
		m_BoundingSphere.fRadius = m_pModel->modelSphere.fRadius;

		m_TimeToSwap = -1.0f;

		return(0);
	}
#ifdef _DEBUG
	else
	{
		DBG_ASSERT(0);
	}
#endif // _DEBUG

	return(1);
}

/////////////////////////////////////////////////////
/// Method: Draw
/// Params: None
///
/////////////////////////////////////////////////////
void DoorSlideRight::Draw()
{
	if( renderer::OpenGL::GetInstance()->SphereInFrustum( m_BoundingSphere.vCenterPoint.X, m_BoundingSphere.vCenterPoint.Y, m_BoundingSphere.vCenterPoint.Z, m_BoundingSphere.fRadius ) )
	{
		renderer::OpenGL::GetInstance()->SetColour4ub( 225, 225, 225, 255 );

		glPushMatrix();
			glTranslatef( m_pBody->GetWorldCenter().x, m_pBody->GetWorldCenter().y, 0.0f );

			if( m_pModel != 0 )
				m_pModel->Draw();
		glPopMatrix();
	}
}

/////////////////////////////////////////////////////
/// Method: Update
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
void DoorSlideRight::Update( float deltaTime )
{
	b2Vec2 bodyPos = m_pBody->GetWorldCenter();
	float translation = m_pJoint->GetJointTranslation();

	const float TOLERANCE = 0.001f;
	
	if( m_TimeToSwap > 0.0f )
	{
		m_TimeToSwap -= deltaTime;

		if( m_TimeToSwap <= 0.0f )
		{
			m_pBody->WakeUp();
			m_pJoint->SetMotorSpeed( m_NewMotorSpeed );

			m_TimeToSwap = -1.0f;
		}
	}
	else
	{
		if( translation >= m_pJoint->GetUpperLimit()-TOLERANCE )
		{				
			if( m_ExtendPauseTime > 0.0f )
			{
				m_TimeToSwap = m_ExtendPauseTime;
				m_NewMotorSpeed = m_RetractMotorSpeed;

				// stop the motor
				m_pJoint->SetMotorSpeed( 0.0f );
			}
			else
			{
				m_pBody->WakeUp();
				m_NewMotorSpeed = m_RetractMotorSpeed;
				m_pJoint->SetMotorSpeed( m_NewMotorSpeed );

				m_TimeToSwap = -1.0f;
			}

			if( renderer::OpenGL::GetInstance()->SphereInFrustum( m_BoundingSphere.vCenterPoint.X, m_BoundingSphere.vCenterPoint.Y, m_BoundingSphere.vCenterPoint.Z, m_BoundingSphere.fRadius ) )
			{
				ALuint sourceId = snd::SoundManager::GetInstance()->GetFreeSoundSource();

				if( sourceId != snd::INVALID_SOUNDSOURCE )
				{
					AudioSystem::GetInstance()->PlayAudio( sourceId, m_AudioId, m_BoundingSphere.vCenterPoint, AL_FALSE, AL_FALSE, 1.0f, AUDIO_GAIN );
				}
			}
		}
		else if( translation <= m_pJoint->GetLowerLimit()+TOLERANCE )
		{
			if( m_RetractPauseTime > 0.0f )
			{
				m_TimeToSwap = m_RetractPauseTime;
				m_NewMotorSpeed = m_ExtendMotorSpeed;

				m_pJoint->SetMotorSpeed( 0.0f );
			}
			else
			{
				m_pBody->WakeUp();
				m_NewMotorSpeed = m_ExtendMotorSpeed;
				m_pJoint->SetMotorSpeed( m_NewMotorSpeed );

				m_TimeToSwap = -1.0f;
			}

			if( renderer::OpenGL::GetInstance()->SphereInFrustum( m_BoundingSphere.vCenterPoint.X, m_BoundingSphere.vCenterPoint.Y, m_BoundingSphere.vCenterPoint.Z, m_BoundingSphere.fRadius ) )
			{
				ALuint sourceId = snd::SoundManager::GetInstance()->GetFreeSoundSource();

				if( sourceId != snd::INVALID_SOUNDSOURCE )
				{
					AudioSystem::GetInstance()->PlayAudio( sourceId, m_AudioId, m_BoundingSphere.vCenterPoint, AL_FALSE, AL_FALSE, 1.0f, AUDIO_GAIN );
				}
			}
		}
	}

	// update the bounding sphere
	m_BoundingSphere.vCenterPoint.X = bodyPos.x;
	m_BoundingSphere.vCenterPoint.Y = bodyPos.y;
}

/////////////////////////////////////////////////////
/// Method: Remove
/// Params: None
///
/////////////////////////////////////////////////////
void DoorSlideRight::Remove()
{
	if( m_pJoint != 0 )
	{
		PhysicsWorld::GetWorld()->DestroyJoint( m_pJoint );
		m_pJoint = 0;
	}
	if( m_pBody != 0 )
	{
		PhysicsWorld::GetWorld()->DestroyBody( m_pBody );
		m_pBody = 0;
	}
}

/////////////////////////////////////////////////////
/// Method: Reset
/// Params: None
///
/////////////////////////////////////////////////////
void DoorSlideRight::Reset()
{
	Create( m_SpawnLocation, m_ExtendMotorSpeed, m_RetractMotorSpeed, m_ExtendPauseTime, m_RetractPauseTime );
}
