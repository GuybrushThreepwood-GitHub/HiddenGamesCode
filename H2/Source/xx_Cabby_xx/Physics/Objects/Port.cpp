
/*===================================================================
	File: Port.cpp
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
#include "Physics/Objects/Port.h"

namespace
{

}

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
Port::Port()
	: PhysicsIdentifier( PHYSICSBASICID_PORT, PHYSICSCASTID_PORT )
{

}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
Port::~Port()
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
int Port::Create( math::Vec3 vPos, ePortTypes portType, int portNumber, int portSize )
{
	const char* functionName = 0;

	functionName = GetPortFunction( portType, portNumber, portSize );
	DBG_ASSERT( functionName != 0 );

	m_pModel = GetModel( portType, portNumber, portSize );

#ifdef _DEBUG
	if( script::LuaFunctionCheck( functionName ) == 0 )
#endif // _DEBUG
	{
		int result = 0;

		int errorFuncIndex;
		errorFuncIndex = script::GetErrorFuncIndex();

		// get the function
		lua_getglobal( script::LuaScripting::GetState(), functionName );

		// push the parameters
		lua_pushnumber( script::LuaScripting::GetState(), vPos.X ); // posX
		lua_pushnumber( script::LuaScripting::GetState(), vPos.Y ); // posY

		result = lua_pcall( script::LuaScripting::GetState(), 2, 1, errorFuncIndex );

		// LUA_ERRRUN --- a runtime error. 
		// LUA_ERRMEM --- memory allocation error. For such errors, Lua does not call the error handler function. 
		// LUA_ERRERR --- error while running the error handler function. 

		if( result == LUA_ERRRUN || result == LUA_ERRMEM || result == LUA_ERRERR )
		{
			DBGLOG( "Port: *ERROR* Calling function '%s' failed\n", functionName );
			DBGLOG( "\tLUA_TRACEBACK: %s\n", lua_tostring( script::LuaScripting::GetState(), -1 ) );
			DBG_ASSERT(0);

			return(1);
		}

		// grab the return
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

		m_PortType = portType;
		m_PortNumber = portNumber;
		m_PortSize = portSize;

		// create a bounding area for culling
		m_BoundingAABB.vCenter = math::Vec3( vPos );
		m_BoundingAABB.vBoxMin = m_BoundingAABB.vCenter + m_pModel->modelAABB.vBoxMin;
		m_BoundingAABB.vBoxMax = m_BoundingAABB.vCenter + m_pModel->modelAABB.vBoxMax;

		return(0);
	}

	return(1);
}

/////////////////////////////////////////////////////
/// Method: Draw
/// Params: None
///
/////////////////////////////////////////////////////
void Port::Draw()
{
	unsigned int i=0;
	if( !renderer::OpenGL::GetInstance()->AABBInFrustum( m_BoundingAABB ) )
		return;

	switch( m_PortType )
	{
		// possible shared resource, set the correct light submesh
		case PORTTYPE_NUMBERED:
		{
			if( m_pModel != 0 )
			{
				for( i=1; i <= 9; ++i )
				{
					if( i == m_PortNumber )
						m_pModel->SetMeshDrawState( i, true );
					else
						m_pModel->SetMeshDrawState( i, false );
				}
			}
		}break;
			
		case PORTTYPE_HQ:
		case PORTTYPE_FUEL:
		case PORTTYPE_TOLL:
		default:
			break;
	}

	glPushMatrix();
		glTranslatef( m_pBody->GetWorldCenter().x, m_pBody->GetWorldCenter().y, 0.0f );

		//const b2XForm xfm = m_pBody->GetXForm();
		//m[0] = xfm.R.col1.x;	m[4] = xfm.R.col2.x;
		//m[1] = xfm.R.col1.y;	m[5] = xfm.R.col2.y;
		//glMultMatrixf( m );

		if( m_pModel != 0 )
			m_pModel->Draw();
	glPopMatrix();
}

/////////////////////////////////////////////////////
/// Method: Draw
/// Params: None
///
/////////////////////////////////////////////////////
void Port::Draw( int fromIndex, int toIndex )
{
	unsigned int i=0;
	if( !renderer::OpenGL::GetInstance()->AABBInFrustum( m_BoundingAABB ) )
		return;

	switch( m_PortType )
	{
		// possible shared resource, set the correct light submesh
		case PORTTYPE_NUMBERED:
		{
			if( m_pModel != 0 )
			{
				for( i=1; i <= 9; ++i )
				{
					if( i == m_PortNumber )
						m_pModel->SetMeshDrawState( i, true );
					else
						m_pModel->SetMeshDrawState( i, false );
				}
			}
		}break;
		case PORTTYPE_HQ:
		case PORTTYPE_FUEL:
		case PORTTYPE_TOLL:
		default:
			break;			
	}

	renderer::OpenGL::GetInstance()->SetColour4ub( 225, 225, 225, 255 );

	glPushMatrix();
		glTranslatef( m_pBody->GetWorldCenter().x, m_pBody->GetWorldCenter().y, 0.0f );

		if( m_pModel != 0 )
			m_pModel->Draw( fromIndex, toIndex );
	glPopMatrix();
}

/////////////////////////////////////////////////////
/// Method: Update
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
void Port::Update( float deltaTime )
{

}

/////////////////////////////////////////////////////
/// Method: GetModel
/// Params: None
///
/////////////////////////////////////////////////////
mdl::ModelHGM* Port::GetModel( ePortTypes portType, int portNumber, int portSize )
{
	switch( portType )
	{
		case PORTTYPE_NUMBERED:
		{
			switch( portSize )
			{
				case 0:
					return res::LoadModel( 113 );
				break;
				case 1:
					return res::LoadModel( 114 );
				break;
				case 2:
					return res::LoadModel( 115 );
				break;
				default:
					DBG_ASSERT(0);
					break;
			}
		}break;
		case PORTTYPE_HQ:
		{
			switch( portSize )
			{
				case 0:
					return res::LoadModel( 110 );
				break;
				case 1:
					return res::LoadModel( 111 );
				break;
				case 2:
					return res::LoadModel( 112 );
				break;
				default:
					DBG_ASSERT(0);
					break;
			}
		}break;
		case PORTTYPE_FUEL:
		{
			switch( portSize )
			{
				case 0:
					return res::LoadModel( 107 );
				break;
				case 1:
					return res::LoadModel( 108 );
				break;
				case 2:
					return res::LoadModel( 109 );
				break;
				default:
					DBG_ASSERT(0);
					break;
			}
		}break;
		case PORTTYPE_TOLL:
		{
			switch( portSize )
			{
				case 0:
					return 0;//"Physics/Toll_x1.hgm";
				break;
				case 1:
					return 0;//"Physics/Toll_x2.hgm";
				break;
				case 2:
					return 0;//"Physics/Toll_x3.hgm";
				break;
				default:
					DBG_ASSERT(0);
					break;
			}
		}break;

		default:
			DBG_ASSERT(0);
			break;
	}

	return 0;
}

/////////////////////////////////////////////////////
/// Method: GetPortFunction
/// Params: None
///
/////////////////////////////////////////////////////
const char* Port::GetPortFunction( ePortTypes portType, int portNumber, int portSize )
{
	switch( portType )
	{
		case PORTTYPE_NUMBERED:
		{
			switch( portSize )
			{
				case 0:
					return "SetupTaxix1";
				break;
				case 1:
					return "SetupTaxix2";
				break;
				case 2:
					return "SetupTaxix3";
				break;
				default:
					DBG_ASSERT(0);
					break;
			}

		}break;
		case PORTTYPE_HQ:
		{
			switch( portSize )
			{
				case 0:
					return "SetupHomex1";
				break;
				case 1:
					return "SetupHomex2";
				break;
				case 2:
					return "SetupHomex3";
				break;
				default:
					DBG_ASSERT(0);
					break;
			}
		}break;
		case PORTTYPE_FUEL:
		{
			switch( portSize )
			{
				case 0:
					return "SetupFuelx1";
				break;
				case 1:
					return "SetupFuelx2";
				break;
				case 2:
					return "SetupFuelx3";
				break;
				default:
					DBG_ASSERT(0);
					break;
			}
		}break;
		case PORTTYPE_TOLL:
		{
			switch( portSize )
			{
				case 0:
					return "SetupTollx1";
				break;
				case 1:
					return "SetupTollx2";
				break;
				case 2:
					return "SetupTollx3";
				break;
				default:
					DBG_ASSERT(0);
					break;
			}
		}break;

		default:
			DBG_ASSERT(0);
			break;
	}

	return 0;
}

