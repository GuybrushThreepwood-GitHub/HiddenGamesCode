
/*===================================================================
	File: Fish.cpp
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

#include "Resources/ModelResources.h"

#include "Physics/PhysicsWorld.h"
#include "Physics/Enemies/Fish.h"

namespace
{
	const float ROTATION_INCREMENT = 180.0f;
	const float ROTATION_INCREMENT_CONFORM = 0.1f;
	const float ROTATION_TOLERANCE = 10.0f;

	const float TIME_TO_NEXT_MOVE = 3.0f;

	const float TOLERANCE = 0.5f;
	const float VELX = 2.0f;
	const float VELY = 3.0f;
	const float FORCE_SCALE = 1000.0f;
}

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
Fish::Fish()
	: PhysicsIdentifier( PHYSICSBASICID_ENEMY, PHYSICSCASTID_ENEMYFISH )
{
	m_pModel = res::LoadModel( 104 );
	DBG_ASSERT( m_pModel != 0 );

	/*for( i = 0; i < m_pModel->GetNumMaterials(); i++ )
	{
		// read in materials
		for( j = 0; j < LITE_MAX_USEABLE_TEXUNITS; j++ )
		{
			if( m_pModel->GetMaterialsPtr()[i].texUnit[j].nTextureID != renderer::INVALID_OBJECT )
			{
				renderer::OpenGL::GetInstance()->BindTexture( m_pModel->GetMaterialsPtr()[i].texUnit[j].nTextureID );
				glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
				glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
			}
		}
	}*/

	m_TargetRotation = 0.0f;
	m_RotationAngle = 0.0f;

	m_MoveTimer = 0.0f;
	m_NextMovePoint = math::Vec2( 0.0f, 0.0f );
}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
Fish::~Fish()
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
int Fish::Create( math::Vec3 vPos, math::Vec2 vAreaDims )
{
#ifdef _DEBUG
	if( script::LuaFunctionCheck( "SetupEnemyFish" ) == 0 )
#endif // _DEBUG
	{
		int result = 0;

		int errorFuncIndex;
		errorFuncIndex = script::GetErrorFuncIndex();

		// get the function
		lua_getglobal( script::LuaScripting::GetState(), "SetupEnemyFish" );

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
			DBGLOG( "YShape: *ERROR* Calling function '%s' failed\n", "SetupEnemyFish" );
			DBGLOG( "\tLUA_TRACEBACK: %s\n", lua_tostring( script::LuaScripting::GetState(), -1 ) );
			DBG_ASSERT(0);

			return(1);
		}

		// grab the return
		m_pBody = reinterpret_cast<b2Body *>( lua_touserdata(script::LuaScripting::GetState(), -1) );
		lua_pop( script::LuaScripting::GetState(), 1 );
		DBG_ASSERT( m_pBody != 0 );
		m_pBody->SetUserData( reinterpret_cast<void *>(this) );

		m_State = FishState_Idle;

		m_AreaCenter = vPos;
		m_AreaDims = vAreaDims;

		m_AreaMinExtends.X = m_AreaCenter.X - m_AreaDims.Width*0.5f;
		m_AreaMinExtends.Y = m_AreaCenter.Y - m_AreaDims.Height*0.5f;

		m_AreaMaxExtends.X = m_AreaCenter.X + m_AreaDims.Width*0.5f;
		m_AreaMaxExtends.Y = m_AreaCenter.Y + m_AreaDims.Height*0.5f;

		m_MoveTimer = 0.0f;
		m_NextMovePoint = math::Vec2( m_pBody->GetWorldCenter().x, m_pBody->GetWorldCenter().y );

		m_ModelChangeTimer = 0.0f;
		m_RandomModelChangeTime = math::RandomNumber( 5.0f, 15.0f );

		m_ModelSwap = false;
		m_pModel->SetMeshDrawState( 0, false );
		m_pModel->SetMeshDrawState( 1, true );
		m_pModel->SetupAndFree();

		m_ModelChangeTimer = m_RandomModelChangeTime;

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
void Fish::Draw()
{
	if( renderer::OpenGL::GetInstance()->SphereInFrustum( m_BoundingSphere.vCenterPoint.X, m_BoundingSphere.vCenterPoint.Y, m_BoundingSphere.vCenterPoint.Z, m_BoundingSphere.fRadius ) )
	{
		renderer::OpenGL::GetInstance()->SetColour4ub( 225, 225, 225, 255 );

		glPushMatrix();
			glTranslatef( m_pBody->GetWorldCenter().x, m_pBody->GetWorldCenter().y, 0.0f );
			glRotatef( m_RotationAngle, 0.0f, 1.0f, 0.0f );

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
void Fish::Update( float deltaTime )
{
	b2Vec2 linVel = m_pBody->GetLinearVelocity();
	b2Vec2 bodyPos = m_pBody->GetWorldCenter();

	if( bodyPos.x <= m_AreaMinExtends.X ||
		bodyPos.x >= m_AreaMaxExtends.X )
	{
		linVel.x = -linVel.x;
		m_pBody->SetLinearVelocity( linVel );
	}
	if( bodyPos.y <= m_AreaMinExtends.Y ||
		bodyPos.y >= m_AreaMaxExtends.Y )
	{
		linVel.y = -linVel.y;
		m_pBody->SetLinearVelocity( linVel );
	}


	math::Vec2 currentPosition = math::Vec2( m_pBody->GetWorldCenter().x, m_pBody->GetWorldCenter().y );
	if( m_State == FishState_Idle )
	{
		const float SLOW_DOWN = 0.99f;
		b2Vec2 v( linVel.x*SLOW_DOWN, linVel.y*SLOW_DOWN );
		m_pBody->SetLinearVelocity( v );

		if( m_MoveTimer <= TIME_TO_NEXT_MOVE )
		{
			m_MoveTimer += deltaTime;
		}
		else
		{
			m_State = FishState_GoToPoint;

			m_NextMovePoint.X = math::RandomNumber( m_AreaMinExtends.X, m_AreaMaxExtends.X );
			m_NextMovePoint.Y = math::RandomNumber( m_AreaMinExtends.Y, m_AreaMaxExtends.Y );

			m_DirectionToMove = m_NextMovePoint - math::Vec2( m_pBody->GetWorldCenter().x, m_pBody->GetWorldCenter().y );
			m_DirectionToMove.normalise();

			m_MoveTimer = 0.0f;
		}
	}
	else if( m_State == FishState_GoToPoint )
	{
		if( (currentPosition.X-m_NextMovePoint.X > -TOLERANCE &&
			currentPosition.X-m_NextMovePoint.X < TOLERANCE)  &&
			(currentPosition.Y-m_NextMovePoint.Y > -TOLERANCE &&
			currentPosition.Y-m_NextMovePoint.Y < TOLERANCE) )
		{
			m_State = FishState_Idle;
		}
		else
		{
			m_DirectionToMove = m_NextMovePoint - math::Vec2( m_pBody->GetWorldCenter().x, m_pBody->GetWorldCenter().y );
			m_DirectionToMove.normalise();

			if( linVel.x < VELX &&
				linVel.x > -VELX )
			{
				b2Vec2 force( m_DirectionToMove.X*FORCE_SCALE, 0.0f );
				m_pBody->ApplyForce( force, m_pBody->GetWorldCenter() );
			}

			if( linVel.y < VELY &&
				linVel.y > -VELY )
			{
				b2Vec2 force( 0.0f, m_DirectionToMove.Y*FORCE_SCALE );
				m_pBody->ApplyForce( force, m_pBody->GetWorldCenter() );
			}

			linVel = m_pBody->GetLinearVelocity();
		}
	}

	if( linVel.x > 0.05f )
		m_TargetRotation = 180.0f;
	else if( linVel.x < -0.05f )
		m_TargetRotation = 0.0f;

	if( m_RotationAngle != m_TargetRotation )
	{
		if( m_TargetRotation < m_RotationAngle )
			m_RotationAngle -= ROTATION_INCREMENT*deltaTime;
		else if( m_TargetRotation > m_RotationAngle )
			m_RotationAngle += ROTATION_INCREMENT*deltaTime;

		if( (m_RotationAngle-m_TargetRotation) < ROTATION_TOLERANCE &&
			(m_RotationAngle-m_TargetRotation) > -ROTATION_TOLERANCE )
				m_RotationAngle = m_TargetRotation;
	}

	m_ModelChangeTimer += deltaTime;

	if( m_ModelChangeTimer >= m_RandomModelChangeTime )
	{
		if( m_ModelSwap )
		{
			m_pModel->SetMeshDrawState( 0, false );
			m_pModel->SetMeshDrawState( 1, true );

			m_ModelSwap = false;
		}
		else
		{
			m_ModelSwap = true;

			m_pModel->SetMeshDrawState( 0, true );
			m_pModel->SetMeshDrawState( 1, false );
		}

		m_RandomModelChangeTime = math::RandomNumber( 5.0f, 15.0f );
		m_ModelChangeTimer = 0.0f;
	}

	// update bounding sphere
	m_BoundingSphere.vCenterPoint.X = bodyPos.x;
	m_BoundingSphere.vCenterPoint.Y = bodyPos.y;
}

/////////////////////////////////////////////////////
/// Method: Remove
/// Params: None
///
/////////////////////////////////////////////////////
void Fish::Remove()
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
void Fish::Reset()
{
	Create( m_SpawnLocation, m_AreaDims );
}

/////////////////////////////////////////////////////
/// Method: DrawDebug
/// Params: None
///
/////////////////////////////////////////////////////
void Fish::DrawDebug()
{
	math::Vec2 vaSegmentPoints[4];

	renderer::OpenGL::GetInstance()->SetColour4ub( 255,0,0,255 );

	math::Vec2 pos( m_pBody->GetWorldCenter().x, m_pBody->GetWorldCenter().y );

	vaSegmentPoints[0] = math::Vec2( m_AreaCenter.X - m_AreaDims.Width*0.5f, m_AreaCenter.Y - m_AreaDims.Height*0.5f );
	vaSegmentPoints[1] = math::Vec2( m_AreaCenter.X + m_AreaDims.Width*0.5f, m_AreaCenter.Y - m_AreaDims.Height*0.5f );
	vaSegmentPoints[2] = math::Vec2( m_AreaCenter.X + m_AreaDims.Width*0.5f, m_AreaCenter.Y + m_AreaDims.Height*0.5f );
	vaSegmentPoints[3] = math::Vec2( m_AreaCenter.X - m_AreaDims.Width*0.5f, m_AreaCenter.Y + m_AreaDims.Height*0.5f );

	glVertexPointer(2, GL_FLOAT, sizeof(math::Vec2), vaSegmentPoints);
	glDrawArrays(GL_LINE_LOOP, 0, 4 );

	renderer::OpenGL::GetInstance()->SetColour4ub( 0,255,0,255 );

	glPointSize( 2.0f );

	glVertexPointer(2, GL_FLOAT, sizeof(math::Vec2), &m_NextMovePoint);
	glDrawArrays( GL_POINTS, 0, 1);

	glPointSize( 1.0f );
}
