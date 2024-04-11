
/*===================================================================
	File: DustDevil.cpp
	Game: Cabby

	(C)Hidden Games
=====================================================================*/

#ifdef ALLOW_UNUSED_OBJECTS

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
#include "Physics/Enemies/DustDevil.h"

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
DustDevil::DustDevil()
	: PhysicsIdentifier( PHYSICSBASICID_ENEMY, PHYSICSCASTID_ENEMYDUSTDEVIL )
{
	//m_ResourceIndex = Resources::LoadModelResource( "physics/Enemy_DustDevil.hgm", false );
	//m_pModel = Resources::GetModel( m_ResourceIndex );
	DBG_ASSERT( m_pModel != 0 );
}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
DustDevil::~DustDevil()
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
int DustDevil::Create( math::Vec3 vPos, math::Vec2 vAreaDims )
{
#ifdef _DEBUG
	if( script::LuaFunctionCheck( "SetupEnemyDustDevil" ) == 0 )
#endif // _DEBUG
	{
		int result = 0;

		int errorFuncIndex;
		lua_pushliteral( script::LuaScripting::GetState(), "_TRACEBACK" );
		lua_rawget( script::LuaScripting::GetState(), LUA_GLOBALSINDEX );  // get traceback function
		errorFuncIndex = lua_gettop( script::LuaScripting::GetState() );

		lua_pop( script::LuaScripting::GetState(), 1 );

		// get the function
		lua_getglobal( script::LuaScripting::GetState(), "SetupEnemyDustDevil" );

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
			DBGLOG( "YShape: *ERROR* Calling function '%s' failed\n", "SetupEnemyDustDevil" );
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

		m_AreaMinExtends.X = m_AreaCenter.X - m_AreaDims.fWidth*0.5f;
		m_AreaMinExtends.Y = m_AreaCenter.Y - m_AreaDims.fHeight*0.5f;

		m_AreaMaxExtends.X = m_AreaCenter.X + m_AreaDims.fWidth*0.5f;
		m_AreaMaxExtends.Y = m_AreaCenter.Y + m_AreaDims.fHeight*0.5f;

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
void DustDevil::Draw()
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

/////////////////////////////////////////////////////
/// Method: Update
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
void DustDevil::Update( float deltaTime )
{
	b2Vec2 linVel = m_pBody->GetLinearVelocity();
	float32 angVel = m_pBody->GetAngularVelocity();
	b2Vec2 bodyPos = m_pBody->GetWorldCenter();

	if( bodyPos.x <= m_AreaMinExtends.X ||
		bodyPos.x >= m_AreaMaxExtends.X )
	{
		linVel.x = -linVel.x;
		m_pBody->SetLinearVelocity( linVel );

		angVel = -angVel;
		m_pBody->SetAngularVelocity( angVel );
	}
}

/////////////////////////////////////////////////////
/// Method: DrawDebug
/// Params: None
///
/////////////////////////////////////////////////////
void DustDevil::DrawDebug()
{
	math::Vec2 vaSegmentPoints[4];

	renderer::OpenGL::GetInstance()->SetColour4ub( 255,0,0,255 );

	math::Vec2 pos( m_pBody->GetWorldCenter().x, m_pBody->GetWorldCenter().y );

	vaSegmentPoints[0] = math::Vec2( m_AreaCenter.X - m_AreaDims.fWidth*0.5f, m_AreaCenter.Y - m_AreaDims.fHeight*0.5f );
	vaSegmentPoints[1] = math::Vec2( m_AreaCenter.X + m_AreaDims.fWidth*0.5f, m_AreaCenter.Y - m_AreaDims.fHeight*0.5f );
	vaSegmentPoints[2] = math::Vec2( m_AreaCenter.X + m_AreaDims.fWidth*0.5f, m_AreaCenter.Y + m_AreaDims.fHeight*0.5f );
	vaSegmentPoints[3] = math::Vec2( m_AreaCenter.X - m_AreaDims.fWidth*0.5f, m_AreaCenter.Y + m_AreaDims.fHeight*0.5f );

	glVertexPointer(2, GL_FLOAT, sizeof(math::Vec2), vaSegmentPoints);
	glDrawArrays(GL_LINE_LOOP, 0, 4 );
}

#endif // ALLOW_UNUSED_OBJECTS