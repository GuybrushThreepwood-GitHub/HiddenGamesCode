
/*===================================================================
	File: Mine.cpp
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
#include "Resources/EmitterResources.h"

#include "Player/Player.h"
#include "Physics/PhysicsWorld.h"
#include "Physics/Enemies/Mine.h"

namespace
{
	const float ROTATION_INCREMENT = 360.0f;
	const float ROTATION_INCREMENT_CONFORM = 0.1f;
}

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
Mine::Mine()
	: PhysicsIdentifier( PHYSICSBASICID_ENEMY, PHYSICSCASTID_ENEMYMINE )
{
	m_pModel = res::LoadModel( 105 );
	DBG_ASSERT( m_pModel != 0 );

	m_TargetRotation = 0.0f;
	m_RotationAngle = 0.0f;
	m_Scale = 1.0f;

	m_IsActive = true;

	m_ShouldRespawn = true;
	m_RespawnTime = 5.0f;
	m_CurrentRespawnTime = 0.0f;

	m_ExplodeAudioId = snd::INVALID_SOUNDBUFFER;
	m_ExplodeAudioId = AudioSystem::GetInstance()->AddAudioFile( 100/*"bomb_air.wav"*/ ); 
	m_Explosion.Create( *res::GetEmitterResource( 100 )->block );
	res::SetupTexturesOnEmitter( &m_Explosion );
	m_Explosion.Disable();
}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
Mine::~Mine()
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
int Mine::Create( math::Vec3 vPos, math::Vec2 vAreaDims, bool respawnFlag, float respawnTime )
{
#ifdef _DEBUG
	if( script::LuaFunctionCheck( "SetupEnemyMine" ) == 0 )
#endif // _DEBUG
	{
		int result = 0;

		int errorFuncIndex;
		errorFuncIndex = script::GetErrorFuncIndex();

		// get the function
		lua_getglobal( script::LuaScripting::GetState(), "SetupEnemyMine" );

		// push the parameters
		m_SpawnLocation = vPos;

		lua_pushnumber( script::LuaScripting::GetState(), vPos.X ); // posX
		lua_pushnumber( script::LuaScripting::GetState(), vPos.Y ); // posY

		result = lua_pcall( script::LuaScripting::GetState(), 2, 1, errorFuncIndex );

		// LUA_ERRRUN --- a runtime error. 
		// LUA_ERRMEM --- memory allocation error. For such errors, Lua does not call the error handler function. 
		// LUA_ERRERR --- error while running the error handler function. 

		if( result == LUA_ERRRUN || result == LUA_ERRMEM || result == LUA_ERRERR )
		{
			DBGLOG( "YShape: *ERROR* Calling function '%s' failed\n", "SetupEnemyMine" );
			DBGLOG( "\tLUA_TRACEBACK: %s\n", lua_tostring( script::LuaScripting::GetState(), -1 ) );
			DBG_ASSERT(0);

			return(1);
		}

		// grab the return
		m_pBody = reinterpret_cast<b2Body *>( lua_touserdata(script::LuaScripting::GetState(), -1) );
		lua_pop( script::LuaScripting::GetState(), 1 );
		DBG_ASSERT( m_pBody != 0 );
		m_pBody->SetUserData( reinterpret_cast<void *>(this) );

		m_IsActive = true;
		m_AreaCenter = vPos;
		m_AreaDims = vAreaDims;

		m_AreaMinExtends.X = m_AreaCenter.X - m_AreaDims.Width*0.5f;
		m_AreaMinExtends.Y = m_AreaCenter.Y - m_AreaDims.Height*0.5f;

		m_AreaMaxExtends.X = m_AreaCenter.X + m_AreaDims.Width*0.5f;
		m_AreaMaxExtends.Y = m_AreaCenter.Y + m_AreaDims.Height*0.5f;

		m_ShouldRespawn = respawnFlag;
		m_RespawnTime = respawnTime;

		b2Vec2 bodyPos = m_pBody->GetWorldCenter();
		m_BoundingSphere.vCenterPoint.X = bodyPos.x;
		m_BoundingSphere.vCenterPoint.Y = bodyPos.y;
	
		m_BoundingSphere.fRadius = m_pModel->modelSphere.fRadius;

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
void Mine::Draw()
{
	m_Explosion.Draw();

	if( !m_IsActive )
		return;

	if( renderer::OpenGL::GetInstance()->SphereInFrustum( m_BoundingSphere.vCenterPoint.X, m_BoundingSphere.vCenterPoint.Y, m_BoundingSphere.vCenterPoint.Z, m_BoundingSphere.fRadius ) )
	{
		renderer::OpenGL::GetInstance()->SetColour4ub( 225, 225, 225, 255 );

		glPushMatrix();
			glTranslatef( m_pBody->GetWorldCenter().x, m_pBody->GetWorldCenter().y, 0.0f );
			glRotatef( m_RotationAngle, 0.0f, 1.0f, 0.0f );
			glScalef( m_Scale, m_Scale, m_Scale );
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
void Mine::Update( float deltaTime )
{
	m_Explosion.Update(deltaTime);
	
	if( !m_IsActive )
	{
		if( m_pBody != 0 )
		{
			PhysicsWorld::GetWorld()->DestroyBody( m_pBody );
			m_pBody = 0;
		}

		if( m_CurrentRespawnTime > 0.0f )
		{
			m_CurrentRespawnTime -= deltaTime;
			if( m_CurrentRespawnTime < 0.0f )
			{
				Create( m_SpawnLocation, m_AreaDims, m_ShouldRespawn, m_RespawnTime );

				m_CurrentRespawnTime = 0.0f;
				m_IsActive = true;
			}
		}
	}

	if( m_IsActive )
	{
		if( m_Scale != 1.0f )
		{
			const float GROW_FACTOR = 0.5f*deltaTime;
			m_Scale += GROW_FACTOR;

			if( m_Scale > 1.0f )
				m_Scale = 1.0f;
		}
	}
}

/////////////////////////////////////////////////////
/// Method: HandleContact
/// Params: None
///
/////////////////////////////////////////////////////
void Mine::HandleContact( const b2ContactPoint* contact )
{
	if( m_IsActive )
	{
		b2Body* pPlayerBody = PhysicsWorld::GetPlayer()->GetBody();
		b2Vec2 vel = pPlayerBody->GetLinearVelocity();

		b2Vec2 dir(-vel.x, -vel.y);

		const float FORCEMULTIPLY = 15.0f;
		pPlayerBody->ApplyImpulse( FORCEMULTIPLY*dir, pPlayerBody->GetWorldCenter() );
		Explode();

		m_Scale = 0.1f;
	}
}

/////////////////////////////////////////////////////
/// Method: Remove
/// Params: None
///
/////////////////////////////////////////////////////
void Mine::Remove()
{
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
void Mine::Reset()
{
	Create( m_SpawnLocation, m_AreaDims, m_ShouldRespawn, m_RespawnTime );
}

/////////////////////////////////////////////////////
/// Method: Explode
/// Params: None
///
/////////////////////////////////////////////////////
void Mine::Explode()
{
	m_IsActive = false;
	m_CurrentRespawnTime = m_RespawnTime;

	ALuint soundId = snd::SoundManager::GetInstance()->GetFreeSoundSource();
	AudioSystem::GetInstance()->PlayAudio( soundId, m_ExplodeAudioId, math::Vec3(0.0f,0.0f,0.0f), AL_TRUE, AL_FALSE );

	m_Explosion.SetPos( math::Vec3(m_SpawnLocation.X,m_SpawnLocation.Y,0.0f) );
	m_Explosion.Enable();
	m_Explosion.Disable();
}

/////////////////////////////////////////////////////
/// Method: DrawDebug
/// Params: None
///
/////////////////////////////////////////////////////
void Mine::DrawDebug()
{
	if( m_pBody==0 )
		return;

	math::Vec2 vaSegmentPoints[4];

	renderer::OpenGL::GetInstance()->SetColour4ub( 255,0,0,255 );

	math::Vec2 pos( m_pBody->GetWorldCenter().x, m_pBody->GetWorldCenter().y );

	vaSegmentPoints[0] = math::Vec2( m_AreaCenter.X - m_AreaDims.Width*0.5f, m_AreaCenter.Y - m_AreaDims.Height*0.5f );
	vaSegmentPoints[1] = math::Vec2( m_AreaCenter.X + m_AreaDims.Width*0.5f, m_AreaCenter.Y - m_AreaDims.Height*0.5f );
	vaSegmentPoints[2] = math::Vec2( m_AreaCenter.X + m_AreaDims.Width*0.5f, m_AreaCenter.Y + m_AreaDims.Height*0.5f );
	vaSegmentPoints[3] = math::Vec2( m_AreaCenter.X - m_AreaDims.Width*0.5f, m_AreaCenter.Y + m_AreaDims.Height*0.5f );

	glVertexPointer(2, GL_FLOAT, sizeof(math::Vec2), vaSegmentPoints);
	glDrawArrays(GL_LINE_LOOP, 0, 4 );
}
