
/*===================================================================
	File: Sentry.cpp
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
#include "Physics/Enemies/Sentry.h"

namespace
{
	GLfloat m[16] = 
	{ 1.0f, 0.0f, 0.0f, 0.0f,
	  0.0f, 1.0f, 0.0f, 0.0f,
	  0.0f, 0.0f, 1.0f, 0.0f,
	  0.0f, 0.0f, 0.0f, 1.0f };

	const float ROTATION_INCREMENT = 360.0f;
	const float ROTATION_INCREMENT_CONFORM = 0.1f;

	const float ANGULAR_VEL = 5.0f;
	const float HORIZONTAL_FORCE = 1000.0f;
	const float VERTICAL_FORCE = 1000.0f;

	const float MAX_VEL = 7.0f;
}

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
Sentry::Sentry()
	: PhysicsIdentifier( PHYSICSBASICID_ENEMY, PHYSICSCASTID_ENEMYSENTRY )
{
	m_pModel = res::LoadModel( 106 );
	DBG_ASSERT( m_pModel != 0 );

	m_TargetRotation = 0.0f;
	m_RotationAngle = 0.0f;

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
}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
Sentry::~Sentry()
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
int Sentry::Create( math::Vec3 vPos, math::Vec2 vAreaDims, int movementDirection, int startPoint, float moveSpeed, float rotateSpeed )
{
#ifdef _DEBUG
	if( script::LuaFunctionCheck( "SetupEnemySentry" ) == 0 )
#endif // _DEBUG
	{
		int result = 0;

		int errorFuncIndex;
		errorFuncIndex = script::GetErrorFuncIndex();

		// get the function
		lua_getglobal( script::LuaScripting::GetState(), "SetupEnemySentry" );

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
			DBGLOG( "YShape: *ERROR* Calling function '%s' failed\n", "SetupEnemySentry" );
			DBGLOG( "\tLUA_TRACEBACK: %s\n", lua_tostring( script::LuaScripting::GetState(), -1 ) );
			DBG_ASSERT(0);

			return(1);
		}

		// grab the return
		m_pBody = reinterpret_cast<b2Body *>( lua_touserdata(script::LuaScripting::GetState(), -1) );
		lua_pop( script::LuaScripting::GetState(), 1 );
		DBG_ASSERT( m_pBody != 0 );
		m_pBody->SetUserData( reinterpret_cast<void *>(this) );

		m_AreaCenter = vPos;
		m_AreaDims = vAreaDims;

		m_AreaMinExtends.X = m_AreaCenter.X - m_AreaDims.Width*0.5f;
		m_AreaMinExtends.Y = m_AreaCenter.Y - m_AreaDims.Height*0.5f;

		m_AreaMaxExtends.X = m_AreaCenter.X + m_AreaDims.Width*0.5f;
		m_AreaMaxExtends.Y = m_AreaCenter.Y + m_AreaDims.Height*0.5f;

		m_MovementDirection = movementDirection;
		m_StartPoint = startPoint;
		m_MoveSpeed = moveSpeed;
		m_RotateSpeed = rotateSpeed;

		if( m_MovementDirection == MOVEMENT_HORIZONTAL )
		{
			switch( m_StartPoint )
			{
				case STARTPOINT_CENTER:
				break;
				case STARTPOINT_LEFT_OR_TOP:
				{
					// spawn far left
					b2Vec2 pos( m_AreaMinExtends.X, vPos.Y );
					m_pBody->SetXForm( pos, math::DegToRad(0.0f) );
				}break;
				case STARTPOINT_RIGHT_OR_BOTTOM:
				{
						// spawn far right
					b2Vec2 pos( m_AreaMaxExtends.X, vPos.Y );
					m_pBody->SetXForm( pos, math::DegToRad(0.0f) );
				}break;

				default:
					DBG_ASSERT(0);
					break;
			}

			b2Vec2 vel( MAX_VEL, 0.0f );
			m_pBody->SetLinearVelocity( vel );
			m_pBody->SetAngularVelocity( ANGULAR_VEL );
		}
		else
		{
			switch( m_StartPoint )
			{
				case STARTPOINT_CENTER:
				break;
				case STARTPOINT_LEFT_OR_TOP:
				{
					// spawn top
					b2Vec2 pos( vPos.X, m_AreaMaxExtends.Y );
					m_pBody->SetXForm( pos, math::DegToRad(0.0f) );
				}break;
				case STARTPOINT_RIGHT_OR_BOTTOM:
				{
					// spawn bottom
					b2Vec2 pos( vPos.X, m_AreaMinExtends.Y );
					m_pBody->SetXForm( pos, math::DegToRad(0.0f) );
				}break;

				default:
					DBG_ASSERT(0);
					break;
			}

			b2Vec2 vel( 0.0f, -MAX_VEL );
			m_pBody->SetLinearVelocity( vel );
			m_pBody->SetAngularVelocity( ANGULAR_VEL );

			b2Vec2 bodyPos = m_pBody->GetWorldCenter();
			m_BoundingSphere.vCenterPoint.X = bodyPos.x;
			m_BoundingSphere.vCenterPoint.Y = bodyPos.y;
	
			m_BoundingSphere.fRadius = m_pModel->modelSphere.fRadius;

		}

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
void Sentry::Draw()
{
	if( renderer::OpenGL::GetInstance()->SphereInFrustum( m_BoundingSphere.vCenterPoint.X, m_BoundingSphere.vCenterPoint.Y, m_BoundingSphere.vCenterPoint.Z, m_BoundingSphere.fRadius ) )
	{
		renderer::OpenGL::GetInstance()->SetColour4ub( 225, 225, 225, 255 );

		glPushMatrix();
			glTranslatef( m_pBody->GetWorldCenter().x, m_pBody->GetWorldCenter().y, 0.0f );

			const b2XForm xfm = m_pBody->GetXForm();

			m[0] = xfm.R.col1.x;	m[4] = xfm.R.col2.x;
			m[1] = xfm.R.col1.y;	m[5] = xfm.R.col2.y;

			glMultMatrixf( m );

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
void Sentry::Update( float deltaTime )
{
	b2Vec2 linVel = m_pBody->GetLinearVelocity();
	b2Vec2 bodyPos = m_pBody->GetWorldCenter();

	if( m_MovementDirection == MOVEMENT_HORIZONTAL )
	{
		if( bodyPos.x <= m_AreaMinExtends.X ||
			bodyPos.x >= m_AreaMaxExtends.X )
		{
			
			linVel.y = 0.0f;
		
			if( bodyPos.x <= m_AreaMinExtends.X  )
			{
				linVel.x = MAX_VEL;
				m_pBody->SetLinearVelocity( linVel );
				m_pBody->SetAngularVelocity( ANGULAR_VEL );
			}
			else
			{
				linVel.x = -MAX_VEL;
				m_pBody->SetLinearVelocity( linVel );
				m_pBody->SetAngularVelocity( -ANGULAR_VEL );
			}
		}

		linVel = m_pBody->GetLinearVelocity();
		linVel.y = 0.0f;
		if( linVel.x > 0.0f )
			linVel.x = MAX_VEL;
		else if( linVel.x < 0.0f )
			linVel.x = -MAX_VEL;
	}
	else if( m_MovementDirection == MOVEMENT_VERTICAL )
	{
		if( bodyPos.y <= m_AreaMinExtends.Y ||
			bodyPos.y >= m_AreaMaxExtends.Y )
		{
			linVel.x = 0.0f;
			
			if( bodyPos.y <= m_AreaMinExtends.Y )
			{
				linVel.y = MAX_VEL;
				m_pBody->SetLinearVelocity( linVel );
				m_pBody->SetAngularVelocity( ANGULAR_VEL );
			}
			else
			{
				linVel.y = -MAX_VEL;
				m_pBody->SetLinearVelocity( linVel );
				m_pBody->SetAngularVelocity( -ANGULAR_VEL );
			}
		}

		linVel = m_pBody->GetLinearVelocity();
		linVel.x = 0.0f;
		if( linVel.y > 0.0f )
			linVel.y = MAX_VEL;
		else if( linVel.y < 0.0f )
			linVel.y = -MAX_VEL;
	}

	// keep the linear velocity sane
	m_pBody->SetLinearVelocity( linVel );

	// update the bounding sphere
	m_BoundingSphere.vCenterPoint.X = bodyPos.x;
	m_BoundingSphere.vCenterPoint.Y = bodyPos.y;
}

/////////////////////////////////////////////////////
/// Method: Remove
/// Params: None
///
/////////////////////////////////////////////////////
void Sentry::Remove()
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
void Sentry::Reset()
{
	Create( m_SpawnLocation, m_AreaDims, m_MovementDirection, m_StartPoint, m_MoveSpeed, m_RotateSpeed );
}

/////////////////////////////////////////////////////
/// Method: DrawDebug
/// Params: None
///
/////////////////////////////////////////////////////
void Sentry::DrawDebug()
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
}
