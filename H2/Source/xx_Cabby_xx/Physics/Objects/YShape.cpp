
/*===================================================================
	File: YShape.cpp
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
#include "Physics/Objects/YShape.h"

namespace
{
	GLfloat m[16] = 
	{ 1.0f, 0.0f, 0.0f, 0.0f,
	  0.0f, 1.0f, 0.0f, 0.0f,
	  0.0f, 0.0f, 1.0f, 0.0f,
	  0.0f, 0.0f, 0.0f, 1.0f };
}

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
YShape::YShape()
	: PhysicsIdentifier( PHYSICSBASICID_SHAPE, PHYSICSCASTID_SHAPEY )
{
	m_pModel = res::LoadModel( 120 );
	DBG_ASSERT( m_pModel != 0 );
}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
YShape::~YShape()
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
int YShape::Create( math::Vec3 vPos, eRotationType rotationType, float motorSpeed, float rotationPauseTime )
{
#ifdef _DEBUG
	if( script::LuaFunctionCheck( "SetupYShape" ) == 0 )
#endif // _DEBUG
	{
		int result = 0;

		int errorFuncIndex;
		errorFuncIndex = script::GetErrorFuncIndex();

		// get the function
		lua_getglobal( script::LuaScripting::GetState(), "SetupYShape" );

		// push the parameters
		m_SpawnLocation = vPos;

		lua_pushnumber( script::LuaScripting::GetState(), vPos.X ); // posX
		lua_pushnumber( script::LuaScripting::GetState(), vPos.Y ); // posY
		lua_pushnumber( script::LuaScripting::GetState(), motorSpeed );

		result = lua_pcall( script::LuaScripting::GetState(), 3, 2, errorFuncIndex );

		// LUA_ERRRUN --- a runtime error. 
		// LUA_ERRMEM --- memory allocation error. For such errors, Lua does not call the error handler function. 
		// LUA_ERRERR --- error while running the error handler function. 

		if( result == LUA_ERRRUN || result == LUA_ERRMEM || result == LUA_ERRERR )
		{
			DBGLOG( "YShape: *ERROR* Calling function '%s' failed\n", "SetupYShape" );
			DBGLOG( "\tLUA_TRACEBACK: %s\n", lua_tostring( script::LuaScripting::GetState(), -1 ) );
			DBG_ASSERT(0);

			return(1);
		}

		// grab the return
		m_pJoint = reinterpret_cast<b2RevoluteJoint *>( lua_touserdata(script::LuaScripting::GetState(), -1) );
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

		m_RotationType = rotationType;
		m_MotorSpeed = motorSpeed;
		m_RotationPauseTime = rotationPauseTime;

		switch(m_RotationType)
		{
			case SHAPE_360_CONSTANT_CW:
			{
				m_pJoint->EnableLimit(false);
				m_ClockWise = true;
				
				m_Angle = m_pJoint->GetJointAngle();

				m_LowerAngleLimit = 0.0f;
				m_UpperAngleLimit = 0.0f;

			}break;
			case SHAPE_360_STOP_CW:
			{
				m_pJoint->EnableLimit(true);
				m_ClockWise = true;

				m_Angle = m_pJoint->GetJointAngle();

				m_LowerAngleLimit = -90.0f;
				m_UpperAngleLimit = 0.0f;

				m_pJoint->SetLimits( math::DegToRad(m_LowerAngleLimit), math::DegToRad(m_UpperAngleLimit) );

			}break;
			case SHAPE_360_CONSTANT_CCW:
			{
				m_pJoint->EnableLimit(false);
				m_ClockWise = false;

				m_Angle = m_pJoint->GetJointAngle();

				m_LowerAngleLimit = 0.0f;
				m_UpperAngleLimit = 0.0f;

			}break;
			case SHAPE_360_STOP_CCW:
			{
				m_pJoint->EnableLimit(true);
				m_ClockWise = false;

				m_Angle = m_pJoint->GetJointAngle();

				m_LowerAngleLimit = 0.0f;
				m_UpperAngleLimit = 90.0f;

				m_pJoint->SetLimits( math::DegToRad(m_LowerAngleLimit), math::DegToRad(m_UpperAngleLimit) );

			}break;
			case SHAPE_360_SWAP:
			{
				m_pJoint->EnableLimit(true);
				m_ClockWise = false;

				m_Angle = m_pJoint->GetJointAngle();

				m_LowerAngleLimit = -180.0f;
				m_UpperAngleLimit = 180.0f;

				m_pJoint->SetLimits( math::DegToRad(m_LowerAngleLimit), math::DegToRad(m_UpperAngleLimit) );

			}break;
			case SHAPE_180_STOP:
			{
				m_pJoint->EnableLimit(true);
				m_ClockWise = true;

				m_Angle = m_pJoint->GetJointAngle();

				m_LowerAngleLimit = -90.0f;
				m_UpperAngleLimit = 90.0f;

				m_pJoint->SetLimits( math::DegToRad(m_LowerAngleLimit), math::DegToRad(m_UpperAngleLimit) );

			}break;
			default:
				DBG_ASSERT(0);
				break;
		}

		// make sure it's going the correct way
		if( m_ClockWise )
		{
			m_Speed = m_pJoint->GetMotorSpeed();
			if( m_Speed > 0.0f )
				m_pJoint->SetMotorSpeed( -m_Speed );
		}
		else
		{
			m_Speed = m_pJoint->GetMotorSpeed();
			if( m_Speed < 0.0f )
				m_pJoint->SetMotorSpeed( -m_Speed );
		}

		b2Vec2 bodyPos = m_pBody->GetWorldCenter();
		m_BoundingSphere.vCenterPoint.X = bodyPos.x;
		m_BoundingSphere.vCenterPoint.Y = bodyPos.y;
	
		m_BoundingSphere.fRadius = m_pModel->modelSphere.fRadius;

		return(0);
	}

	return(1);
}

/////////////////////////////////////////////////////
/// Method: Draw
/// Params: None
///
/////////////////////////////////////////////////////
void YShape::Draw()
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
void YShape::Update( float deltaTime )
{
	m_Angle = m_pJoint->GetJointAngle();
	m_pBody->WakeUp();
	
	switch(m_RotationType)
	{
		case SHAPE_360_CONSTANT_CW:
		{

		}break;
		case SHAPE_360_STOP_CW:
		{
			if( m_TimeToSwap <= 0.0f ) 
			{
				if( m_ClockWise == true && 
					m_Angle <= math::DegToRad(m_LowerAngleLimit) )
				{
					m_TimeToSwap = m_RotationPauseTime;
				}
			}

			if( m_TimeToSwap > 0.0f )
			{
				m_TimeToSwap -= deltaTime;

				if( m_TimeToSwap <= 0.0f )
				{
					m_LowerAngleLimit -= 90.0f;

					m_pJoint->SetLimits( math::DegToRad(m_LowerAngleLimit), math::DegToRad(m_UpperAngleLimit) );
					m_pBody->WakeUp();
				}
			}
		}break;
		case SHAPE_360_CONSTANT_CCW:
		{

		}break;
		case SHAPE_360_STOP_CCW:
		{
			if( m_TimeToSwap <= 0.0f ) 
			{
				if( m_ClockWise == false && 
					m_Angle >= math::DegToRad(m_UpperAngleLimit) )
				{
					m_TimeToSwap = m_RotationPauseTime;
				}
			}

			if( m_TimeToSwap > 0.0f )
			{
				m_TimeToSwap -= deltaTime;

				if( m_TimeToSwap <= 0.0f )
				{
					m_UpperAngleLimit += 90.0f;

					m_pJoint->SetLimits( math::DegToRad(m_LowerAngleLimit), math::DegToRad(m_UpperAngleLimit) );
					m_pBody->WakeUp();
				}
			}
		}break;
		case SHAPE_360_SWAP:
		{
			if( m_ClockWise == false &&
				m_Angle >= math::DegToRad(m_UpperAngleLimit) )
			{
				m_ClockWise = true;
				m_TimeToSwap = m_RotationPauseTime;
				m_Speed = -m_pJoint->GetMotorSpeed();
			}
			else if( m_ClockWise == true && 
					m_Angle <= math::DegToRad(m_LowerAngleLimit) )
			{
				m_ClockWise = false;
				m_TimeToSwap = m_RotationPauseTime;
				m_Speed = -m_pJoint->GetMotorSpeed();
			}

			if( m_TimeToSwap > 0.0f )
			{
				m_TimeToSwap -= deltaTime;

				if( m_TimeToSwap <= 0.0f )
				{
					m_pBody->WakeUp();
					m_pJoint->SetMotorSpeed( m_Speed );
				}
			}
		}break;
		case SHAPE_180_STOP:
		{
			if( m_ClockWise == false &&
				m_Angle >= math::DegToRad(m_UpperAngleLimit) )
			{
				m_ClockWise = true;
				m_TimeToSwap = m_RotationPauseTime;
				m_Speed = -m_pJoint->GetMotorSpeed();
			}
			else if( m_ClockWise == true && 
					m_Angle <= math::DegToRad(m_LowerAngleLimit) )
			{
				m_ClockWise = false;
				m_TimeToSwap = m_RotationPauseTime;
				m_Speed = -m_pJoint->GetMotorSpeed();
			}

			if( m_TimeToSwap > 0.0f )
			{
				m_TimeToSwap -= deltaTime;

				if( m_TimeToSwap <= 0.0f )
				{
					m_pBody->WakeUp();
					m_pJoint->SetMotorSpeed( m_Speed );
				}
			}
		}break;
		default:
			DBG_ASSERT(0);
			break;
	}
}

/////////////////////////////////////////////////////
/// Method: Remove
/// Params: None
///
/////////////////////////////////////////////////////
void YShape::Remove()
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
void YShape::Reset()
{
	Create( m_SpawnLocation, m_RotationType, m_MotorSpeed, m_RotationPauseTime );
}
