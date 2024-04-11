
/*===================================================================
	File: Door.cpp
	Game: H4

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"
#include "MathBase.h"
#include "CollisionBase.h"
#include "RenderBase.h"
#include "ModelBase.h"
#include "SoundBase.h"

#include "Audio/AudioSystem.h"

#include "Level/LevelTypes.h"
#include "Physics/PhysicsContact.h"

#include "Resources/ModelResources.h"
#include "Resources/SoundResources.h"
#include "Door.h"

namespace
{
	char SMALL_DOOR_CALL[] = "GetSmallDoor";
	char LARGE_DOOR_CALL[] = "GetLargeDoor";
	char ELEVATOR_DOOR_CALL[] = "GetElevatorDoor";

	const float DOOR_SPEED = 1.5f;
	const float DOOR_BLOCK_HEIGHT = 1.850f;
	const float MAX_DOOR_HEIGHT = 2.125f;

	const int SMALL_DOOR_TYPE = 0;
	const int LARGE_DOOR_TYPE = 1;
	const int ELEVATOR_DOOR_TYPE = 2;
}

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
Door::Door( int objectId, int meshId, int type, bool hasReflection, RoomInstance* roomInstance, lua_State* state )
	: BaseObject( PHYSICSBASICID_WORLD, PHYSICSCASTID_WORLD, objectId, DOOR, state )
	, m_LuaState(state)
	, m_pBody(0)
	, m_pModel(0)
	, m_RoomInstance(roomInstance)
	, m_CurrentHeight(0.0f)
	, m_CurrentXExtend(0.0f)
	, m_Type(type)
	, m_Reflect(hasReflection)
	, m_State(DoorState_Closed)
	, m_ElevatorHalfWidth(0.0f)
{
	std::memset( m_FunctionBuffer, 0, sizeof(char)*LUAFUNC_STRBUFFER );

	m_pModel = res::LoadModel( meshId, true );

	m_DoorOpenAudio = snd::INVALID_SOUNDBUFFER;
	m_DoorCloseAudio = snd::INVALID_SOUNDBUFFER;

	snd::Sound sndLoad;
	const char* sr = 0;
	if( m_Type == 0 )
	{
		sr = res::GetSoundResource( 10 );
		m_DoorOpenAudio = snd::SoundLoad( sr, sndLoad );
		DBG_ASSERT( m_DoorOpenAudio != snd::INVALID_SOUNDBUFFER );

		sr = res::GetSoundResource( 11 );
		m_DoorCloseAudio = snd::SoundLoad( sr, sndLoad );
		DBG_ASSERT( m_DoorCloseAudio != snd::INVALID_SOUNDBUFFER );
	}
	else if( m_Type == 1 )
	{
		sr = res::GetSoundResource( 12 );
		m_DoorOpenAudio = snd::SoundLoad( sr, sndLoad );
		DBG_ASSERT( m_DoorOpenAudio != snd::INVALID_SOUNDBUFFER );

		sr = res::GetSoundResource( 13 );
		m_DoorCloseAudio = snd::SoundLoad( sr, sndLoad );
		DBG_ASSERT( m_DoorCloseAudio != snd::INVALID_SOUNDBUFFER );
	}
	else if( m_Type == 2 )
	{
		sr = res::GetSoundResource( 14 );
		m_DoorOpenAudio = snd::SoundLoad( sr, sndLoad );
		DBG_ASSERT( m_DoorOpenAudio != snd::INVALID_SOUNDBUFFER );

		sr = res::GetSoundResource( 15 );
		m_DoorCloseAudio = snd::SoundLoad( sr, sndLoad );
		DBG_ASSERT( m_DoorCloseAudio != snd::INVALID_SOUNDBUFFER );
	}

}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
Door::~Door()
{
	b2World* pWorld = physics::PhysicsWorldB2D::GetWorld();
	if( m_pBody != 0 )
	{
		if( pWorld )
			pWorld->DestroyBody(m_pBody);
		m_pBody = 0;
	}

	res::RemoveModel(m_pModel);
	m_pModel = 0;

	snd::RemoveSound( m_DoorOpenAudio );
	snd::RemoveSound( m_DoorCloseAudio );
}

/////////////////////////////////////////////////////
/// Method: PostConstruction
/// Params: None
///
/////////////////////////////////////////////////////
int Door::PostConstruction()
{
	b2World* pWorld = physics::PhysicsWorldB2D::GetWorld();
	const math::Vec3& pos = GetPos();
	float angle = GetRot();

	char* funcName = 0;

	if( m_Type == SMALL_DOOR_TYPE )
		funcName = SMALL_DOOR_CALL;
	if( m_Type == LARGE_DOOR_TYPE )
		funcName = LARGE_DOOR_CALL;
	if( m_Type == ELEVATOR_DOOR_TYPE )
		funcName = ELEVATOR_DOOR_CALL;
	else
		DBG_ASSERT_MSG( (funcName != 0), "DOOR: *ERROR* Unrecognised door type" );

#ifdef _DEBUG
	if( script::LuaFunctionCheck( funcName ) == 0 )
#endif // _DEBUG
	{
		int result = 0;

		int errorFuncIndex;
		errorFuncIndex = script::GetErrorFuncIndex();

		// get the function
		lua_getglobal( GetScriptState(), funcName );

		result = lua_pcall( GetScriptState(), 0, 1, errorFuncIndex );

		// LUA_ERRRUN --- a runtime error. 
		// LUA_ERRMEM --- memory allocation error. For such errors, Lua does not call the error handler function. 
		// LUA_ERRERR --- error while running the error handler function. 

		if( result == LUA_ERRRUN || result == LUA_ERRMEM || result == LUA_ERRERR )
		{
			DBGLOG( "DOOR: *ERROR* Calling function '%s' failed\n", funcName );
			DBGLOG( "\tLUA_TRACEBACK: %s\n", lua_tostring( GetScriptState(), -1 ) );

			DBG_ASSERT_MSG( 0, "DOOR: *ERROR* Calling function '%s' failed", funcName );

			return(1);
		}

		// grab the return
		const char* tableName = 0;
		tableName = lua_tostring(GetScriptState(), -1);

		b2BodyDef bd;
		bd.type = b2_staticBody;
		bd.position.x = pos.X;
		bd.position.y = pos.Z;

		// create the body
		m_pBody = pWorld->CreateBody(&bd);
		DBG_ASSERT_MSG( (m_pBody != 0), "Could not create door physics body" );

		m_pBody->SetUserData( reinterpret_cast<void *>(this) );

		if( tableName )
		{
			b2Fixture* pFixture = 0;

			if( script::DoesTableItemExist( tableName, "friction", LUA_TNUMBER ) )
				m_FixDef.friction		= static_cast<float32>( script::LuaGetNumberFromTableItem( tableName, "friction", 0.2 ) );
			if( script::DoesTableItemExist( tableName, "restitution", LUA_TNUMBER ) )
				m_FixDef.restitution	= static_cast<float32>( script::LuaGetNumberFromTableItem( tableName, "restitution", 0.0 ) );
			if( script::DoesTableItemExist( tableName, "density",  LUA_TNUMBER ) )
				m_FixDef.density		= static_cast<float32>( script::LuaGetNumberFromTableItem( tableName, "density",  0.0 ) );
			
			int val = -9999;
			if( script::DoesTableItemExist( tableName, "numeric1",  LUA_TNUMBER ) )
				val		= static_cast<int>( script::LuaGetNumberFromTableItem( tableName, "numeric1",  -9999.0f ) );
			SetNumeric1( val );

			if( script::DoesTableItemExist( tableName, "numeric2",  LUA_TNUMBER ) )
				val		= static_cast<int>( script::LuaGetNumberFromTableItem( tableName, "numeric2",  -9999.0f ) );
			SetNumeric2( val );

			if( script::DoesTableItemExist( tableName, "numeric3",  LUA_TNUMBER ) )
				val		= static_cast<int>( script::LuaGetNumberFromTableItem( tableName, "numeric3",  -9999.0f ) );
			SetNumeric3( val );

			if( script::DoesTableItemExist( tableName, "numeric4",  LUA_TNUMBER ) )
				val		= static_cast<int>( script::LuaGetNumberFromTableItem( tableName, "numeric4",  -9999.0f ) );
			SetNumeric4( val );

			if( script::DoesTableItemExist( tableName, "numeric5",  LUA_TNUMBER ) )
				val		= static_cast<int>( script::LuaGetNumberFromTableItem( tableName, "numeric5",  -9999.0f ) );
			SetNumeric5( val );

			m_FixDef.filter.categoryBits	= WORLDHIGH_CATEGORY;
			m_FixDef.filter.groupIndex		= WORLDGROUP;
			m_FixDef.filter.maskBits		= 0xFFFF;

			b2Vec2 center( 0.0f, 0.0f );
			float32 rotation = math::DegToRad(angle);

			float32 hx				= 1.0f;
			if( script::DoesTableItemExist( tableName, "hx", LUA_TNUMBER ) )
				hx = static_cast<float32>( script::LuaGetNumberFromTableItem( tableName, "hx" ) );
			float32 hy				= 1.0f;
			if( script::DoesTableItemExist( tableName, "hy", LUA_TNUMBER ) )
				hy = static_cast<float32>( script::LuaGetNumberFromTableItem( tableName, "hy" ) );

			m_PolyDef.SetAsBox( hx, hy, center, rotation );

			m_FixDef.shape = &m_PolyDef;

			// create the shape
			pFixture = m_pBody->CreateFixture( &m_FixDef );
			DBG_ASSERT_MSG( (pFixture != 0), "Could not create door physics shape" );

			pFixture->SetUserData( reinterpret_cast<void *>(this) );
		}
		
		// setup the bounding area
		m_AABB = m_pModel->modelAABB;

		// update AABB with correct world position
		if( m_Type == SMALL_DOOR_TYPE ||
			m_Type == LARGE_DOOR_TYPE )
		{
			m_AABB.vBoxMin = math::RotateAroundPoint( m_AABB.vBoxMin, m_AABB.vCenter, math::DegToRad(angle) );
			m_AABB.vBoxMax = math::RotateAroundPoint( m_AABB.vBoxMax, m_AABB.vCenter, math::DegToRad(angle) );

			m_AABB.vCenter = pos;
			m_MinExtend = m_AABB.vBoxMin;
			m_MaxExtend = m_AABB.vBoxMax;
			m_AABB.vBoxMin = m_AABB.vCenter + m_MinExtend;
			m_AABB.vBoxMax = m_AABB.vCenter + m_MaxExtend;
		}
		else if( m_Type == ELEVATOR_DOOR_TYPE )
		{
			m_ElevatorHalfWidth = (m_AABB.vBoxMax.X - m_AABB.vBoxMin.X)-0.004f;

			// elevator doors have to be manually extended to create both doors (2x to cover both doors at full extend)
			m_AABB.vBoxMax.X = m_AABB.vBoxMax.X * 4.0f;
			m_AABB.vBoxMin.X = m_AABB.vBoxMin.X * 4.0f;

			m_AABB.vBoxMin = math::RotateAroundPoint( m_AABB.vBoxMin, m_AABB.vCenter, math::DegToRad(angle) );
			m_AABB.vBoxMax = math::RotateAroundPoint( m_AABB.vBoxMax, m_AABB.vCenter, math::DegToRad(angle) );

			m_AABB.vCenter = pos;
			m_MinExtend = m_AABB.vBoxMin;
			m_MaxExtend = m_AABB.vBoxMax;
			m_AABB.vBoxMin = m_AABB.vCenter + m_MinExtend;
			m_AABB.vBoxMax = m_AABB.vCenter + m_MaxExtend;
		}

		lua_pop( GetScriptState(), 1 );

		return(0);
	}
#ifdef _DEBUG
	else
	{
		DBG_ASSERT_MSG( 0, "Script Function '%s' does not exist, cannot create door type", funcName );
	}
#endif // _DEBUG

	return(1);
}

/////////////////////////////////////////////////////
/// Method: OnCreate
/// Params: None
///
/////////////////////////////////////////////////////
void Door::OnCreate()
{
	snprintf( m_FunctionBuffer, LUAFUNC_STRBUFFER, "Door%d_OnCreate", GetId() );
	if( script::LuaFunctionCheck( m_FunctionBuffer ) == 0 )
	{
		// function exists call it
		//void* pObject = reinterpret_cast<void *>(this);
		int err = 0;

		int errorFuncIndex;
		errorFuncIndex = script::GetErrorFuncIndex();

		lua_getglobal( GetScriptState(), m_FunctionBuffer );

		//lua_pushlightuserdata( GetScriptState(), pObject );
		err = lua_pcall( GetScriptState(), 0, 0, errorFuncIndex );

		// LUA_ERRRUN --- a runtime error. 
		// LUA_ERRMEM --- memory allocation error. For such errors, Lua does not call the error handler function. 
		// LUA_ERRERR --- error while running the error handler function. 

		if( err == LUA_ERRRUN || err == LUA_ERRMEM || err == LUA_ERRERR )
		{
			DBGLOG( "LUASCRIPTING: *ERROR* Calling script '%s' failed\n", m_FunctionBuffer );
			DBGLOG( "\tLUA_TRACEBACK: %s\n", lua_tostring( script::LuaScripting::GetState(), -1 ) );

			script::StackDump(script::LuaScripting::GetState());

			DBG_ASSERT_MSG( 0, "LUASCRIPTING: *ERROR* Calling script '%s' failed", m_FunctionBuffer );
		}
	}
}

/////////////////////////////////////////////////////
/// Method: Toggle
/// Params: None
///
/////////////////////////////////////////////////////
void Door::Toggle()
{
	math::Vec3 zeroVec( 0.0f, 0.0f, 0.0f );

	switch(m_State)
	{
		case DoorState_Closed:
		{
			m_State = DoorState_Opening;
			AudioSystem::GetInstance()->PlayAudio( m_DoorOpenAudio, zeroVec, true );
		}break;
		case DoorState_Open:
		{
			m_State = DoorState_Closing;
			AudioSystem::GetInstance()->PlayAudio( m_DoorCloseAudio, zeroVec, true );
		}break;

		case DoorState_Closing:
		case DoorState_Opening:
		break;

		default:
			DBG_ASSERT_MSG( 0, "Invalid door state" );
			break;
	}
}

/////////////////////////////////////////////////////
/// Method: Update
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
void Door::Update( float deltaTime )
{
	const math::Vec3& pos = GetPos();

	switch( m_State )
	{
		case DoorState_Opening:
		{
			if( m_Type == SMALL_DOOR_TYPE ||
				m_Type == LARGE_DOOR_TYPE )
			{
				if( m_CurrentHeight < MAX_DOOR_HEIGHT )
				{
					m_CurrentHeight += DOOR_SPEED*deltaTime;

					// remove the physics body at the head height
					if( m_CurrentHeight > DOOR_BLOCK_HEIGHT && m_pBody != 0 )
					{
						b2World* pWorld = physics::PhysicsWorldB2D::GetWorld();
						if( m_pBody != 0 )
						{
							pWorld->DestroyBody(m_pBody);
							m_pBody = 0;
						}
					}
				}
				else
				{
					m_CurrentHeight = MAX_DOOR_HEIGHT;
					m_State = DoorState_Open;
				}
			}
			else
			{
				if( m_CurrentXExtend < m_ElevatorHalfWidth )
				{
					m_CurrentXExtend += DOOR_SPEED*deltaTime;

					// remove the physics body at the head height
					if( m_CurrentXExtend > m_ElevatorHalfWidth && m_pBody != 0 )
					{
						b2World* pWorld = physics::PhysicsWorldB2D::GetWorld();
						if( m_pBody != 0 )
						{
							pWorld->DestroyBody(m_pBody);
							m_pBody = 0;
						}
					}
				}
				else
				{
					m_CurrentXExtend = m_ElevatorHalfWidth;
					m_State = DoorState_Open;
				}
			}

		}break;
		case DoorState_Closing:
		{
			if( m_Type == SMALL_DOOR_TYPE ||
				m_Type == LARGE_DOOR_TYPE )
			{
				if( m_CurrentHeight > 0.0f )
				{
					m_CurrentHeight -= DOOR_SPEED*deltaTime;

					if( m_CurrentHeight < DOOR_BLOCK_HEIGHT && m_pBody == 0 )
					{
						if( m_pBody == 0 )
						{
							b2World* pWorld = physics::PhysicsWorldB2D::GetWorld();
							const math::Vec3& pos = GetPos();
							//float angle = GetRot();

							b2BodyDef bd;
							bd.position.x = pos.X;
							bd.position.y = pos.Z;

							// create the body
							m_pBody = pWorld->CreateBody(&bd);
							DBG_ASSERT_MSG( ( m_pBody != 0 ), "Could not create closed door physics body" );
							m_pBody->SetUserData( reinterpret_cast<void *>(this) );

							// create the shape
							b2Fixture* pFixture = 0;
							pFixture = m_pBody->CreateFixture( &m_FixDef );
							DBG_ASSERT_MSG( ( pFixture != 0 ), "Could not create closed door physics fixture" );
							pFixture->SetUserData( reinterpret_cast<void *>(this) );
						}
					}
				}
				else
				{
					m_CurrentHeight = 0.0f;
					m_State = DoorState_Closed;
				}
			}
			else
			{
				if( m_CurrentXExtend > 0.0f )
				{
					m_CurrentXExtend -= DOOR_SPEED*deltaTime;

					if( m_CurrentXExtend < m_ElevatorHalfWidth && m_pBody == 0 )
					{
						if( m_pBody == 0 )
						{
							b2World* pWorld = physics::PhysicsWorldB2D::GetWorld();
							const math::Vec3& pos = GetPos();
							//float angle = GetRot();

							b2BodyDef bd;
							bd.position.x = pos.X;
							bd.position.y = pos.Z;

							// create the body
							m_pBody = pWorld->CreateBody(&bd);
							DBG_ASSERT_MSG( ( m_pBody != 0 ), "Could not create closed door physics body" );
							m_pBody->SetUserData( reinterpret_cast<void *>(this) );

							// create the shape
							b2Fixture* pFixture = 0;
							pFixture = m_pBody->CreateFixture( &m_FixDef );
							DBG_ASSERT_MSG( ( pFixture != 0 ), "Could not create closed door physics fixture" );
							pFixture->SetUserData( reinterpret_cast<void *>(this) );
						}
					}
				}
				else
				{
					m_CurrentXExtend = 0.0f;
					m_State = DoorState_Closed;
				}
			}

		}break;

		case DoorState_Closed:
		case DoorState_Open:
		break;

		default:
			DBG_ASSERT_MSG( 0, "Invalid door state" );
			break;
	}

	// update AABB
	if( m_Type == SMALL_DOOR_TYPE ||
		m_Type == LARGE_DOOR_TYPE )
	{
		m_AABB.vCenter.X = pos.X;
		m_AABB.vCenter.Y = m_CurrentHeight;
		m_AABB.vCenter.Z = pos.Z;

		m_AABB.vBoxMin = (m_AABB.vCenter + m_MinExtend);
		m_AABB.vBoxMax = (m_AABB.vCenter + m_MaxExtend);
	}

}

/////////////////////////////////////////////////////
/// Method: Draw
/// Params: None
///
/////////////////////////////////////////////////////
void Door::Draw()
{
	//glPushMatrix();
	//	renderer::DrawAABB( m_AABB.vBoxMin, m_AABB.vBoxMax);
	//	renderer::OpenGL::GetInstance()->SetColour4ub( 255,255,255,255 );
	//glPopMatrix();

	if( !renderer::OpenGL::GetInstance()->AABBInFrustum(m_AABB) )
		return;

	const math::Vec3& pos = GetPos();
	float angle = GetRot();

	if( m_Type == SMALL_DOOR_TYPE ||
		m_Type == LARGE_DOOR_TYPE )
	{
		glPushMatrix();
			glTranslatef( pos.X, m_CurrentHeight, pos.Z );
			glRotatef( angle, 0.0f, 1.0f, 0.0f );
			if( m_pModel != 0 )
				m_pModel->Draw();
		glPopMatrix();

		if(m_Reflect)
		{
			glFrontFace( GL_CW );

			glPushMatrix();
				glTranslatef( pos.X, -m_CurrentHeight, pos.Z );
				glRotatef( angle, 0.0f, 1.0f, 0.0f );
				glScalef( 1.0f, -1.0f, 1.0f );
				if( m_pModel != 0 )
					m_pModel->Draw();
			glPopMatrix();

			glFrontFace( GL_CCW );
		}
	}
	else
	{
		float hhw = m_ElevatorHalfWidth*0.5f;

		// left door
		glPushMatrix();
			glTranslatef( pos.X, pos.Y, pos.Z );
			glRotatef( angle, 0.0f, 1.0f, 0.0f );
			glTranslatef( -hhw-m_CurrentXExtend, 0.0f, 0.0f );
			if( m_pModel != 0 )
				m_pModel->Draw();
		glPopMatrix();

		// right door
		glPushMatrix();
			glTranslatef( pos.X, pos.Y, pos.Z );
			glRotatef( angle, 0.0f, 1.0f, 0.0f );
			glTranslatef( hhw+m_CurrentXExtend, 0.0f, 0.0f );
			glScalef( -1.0f, 1.0f, -1.0f );
			if( m_pModel != 0 )
				m_pModel->Draw();
		glPopMatrix();

		if(m_Reflect)
		{
			glFrontFace( GL_CW );

			// left door
			glPushMatrix();
				glTranslatef( pos.X, pos.Y, pos.Z );
				glRotatef( angle, 0.0f, 1.0f, 0.0f );
				glTranslatef( -hhw-m_CurrentXExtend, 0.0f, 0.0f );
				glScalef( 1.0f, -1.0f, 1.0f );
				if( m_pModel != 0 )
					m_pModel->Draw();
			glPopMatrix();

			// right door
			glPushMatrix();
				glTranslatef( pos.X, pos.Y, pos.Z );
				glRotatef( angle, 0.0f, 1.0f, 0.0f );
				glTranslatef( hhw+m_CurrentXExtend, 0.0f, 0.0f );
				glScalef( -1.0f, -1.0f, -1.0f );
				if( m_pModel != 0 )
					m_pModel->Draw();
			glPopMatrix();

			glFrontFace( GL_CCW );
		}
	}
}

