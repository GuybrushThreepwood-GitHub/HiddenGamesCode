
/*===================================================================
	File: PhysicsAccessB2D.cpp
	Library: Script

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"
#include "MathBase.h"
#include "PhysicsBase.h"
#include "ScriptBase.h"

#ifdef BASE_SUPPORT_BOX2D

#include "Script/box2d/PhysicsAccessB2D.h"

script::LuaGlobal box2DGlobals[] = 
{
	// core languages
	{ "b2_staticBody",				static_cast<double>(b2_staticBody),				LUA_TNUMBER },
	{ "b2_kinematicBody",			static_cast<double>(b2_kinematicBody),			LUA_TNUMBER },
	{ "b2_dynamicBody",				static_cast<double>(b2_dynamicBody),			LUA_TNUMBER },

};

/////////////////////////////////////////////////////
/// Function: RegisterPhysicsB2DFunctions
/// Params: None
///
/////////////////////////////////////////////////////
void script::RegisterPhysicsB2DFunctions( )
{
	int i=0;

	// setup globals
	for( i=0; i < sizeof(box2DGlobals)/sizeof(script::LuaGlobal); ++i )
	{
		lua_pushnumber( script::LuaScripting::GetState(), box2DGlobals[i].nConstantValue );
		lua_setglobal( script::LuaScripting::GetState(), box2DGlobals[i].szConstantName );
	}

	script::LuaScripting::GetInstance()->RegisterFunction( "CreateBody",			script::ScriptCreateBody );
	script::LuaScripting::GetInstance()->RegisterFunction( "GetBodyWorldCenter",	script::ScriptGetBodyWorldCenter );

	script::LuaScripting::GetInstance()->RegisterFunction( "CreateBoxShape",		script::ScriptCreateBoxShape );
	script::LuaScripting::GetInstance()->RegisterFunction( "CreateCircleShape",	script::ScriptCreateCircleShape );
	script::LuaScripting::GetInstance()->RegisterFunction( "CreatePolygonShape",	script::ScriptCreatePolygonShape );
	
	script::LuaScripting::GetInstance()->RegisterFunction( "DestroyFixture",		script::ScriptDestroyFixture );
	
	script::LuaScripting::GetInstance()->RegisterFunction( "CreateDistanceJoint",	script::ScriptCreateDistanceJoint );
	script::LuaScripting::GetInstance()->RegisterFunction( "CreateGearJoint",		script::ScriptCreateGearJoint );
	script::LuaScripting::GetInstance()->RegisterFunction( "CreateMouseJoint",		script::ScriptCreateMouseJoint );
	script::LuaScripting::GetInstance()->RegisterFunction( "CreatePrismaticJoint",	script::ScriptCreatePrismaticJoint );
	script::LuaScripting::GetInstance()->RegisterFunction( "CreatePulleyJoint",	script::ScriptCreatePulleyJoint );
	script::LuaScripting::GetInstance()->RegisterFunction( "CreateRevoluteJoint",	script::ScriptCreateRevoluteJoint );
	script::LuaScripting::GetInstance()->RegisterFunction( "DestroyJoint",			script::ScriptDestroyJoint );
	
	script::LuaScripting::GetInstance()->RegisterFunction( "SetMassFromShapes",	script::ScriptSetMassFromShapes );
	script::LuaScripting::GetInstance()->RegisterFunction( "SetMeshToBody",		script::ScriptSetMeshToBody );

	script::LuaScripting::GetInstance()->RegisterFunction( "SetBodyXForm",		script::ScriptSetBodyXForm );
	script::LuaScripting::GetInstance()->RegisterFunction( "SetBodyMass",		script::ScriptSetBodyMass );
}

/////////////////////////////////////////////////////
/// Function: ScriptCreateBody
/// Params: [in]pState
///
/////////////////////////////////////////////////////
int script::ScriptCreateBody( lua_State* pState )
{		
	b2World* pWorld = physics::PhysicsWorldB2D::GetWorld();
	b2Body* pNewBody = 0;
	physics::PhysicsIdentifier* pPhysId = 0;

	b2BodyDef bd;
	
	if( lua_istable( pState, 1 ) )
	{	
		if( script::DoesTableItemExist( "positionX", 1, LUA_TNUMBER ) )
			bd.position.x		= static_cast<float32>( script::LuaGetNumberFromTableItem( "positionX", 1, 0.0 ) ); 
		if( script::DoesTableItemExist( "positionY", 1, LUA_TNUMBER ) )
			bd.position.y		= static_cast<float32>( script::LuaGetNumberFromTableItem( "positionY", 1, 0.0 ) ); 

		if( script::DoesTableItemExist( "angle", 1, LUA_TNUMBER ) )
			bd.angle			= static_cast<float32>( script::LuaGetNumberFromTableItem( "angle", 1, 0.0 ) ); 
		if( script::DoesTableItemExist( "linearDamping", 1, LUA_TNUMBER ) )
			bd.linearDamping	= static_cast<float32>( script::LuaGetNumberFromTableItem( "linearDamping", 1, 0.0 ) ); 
		if( script::DoesTableItemExist( "angularDamping", 1, LUA_TNUMBER ) )
			bd.angularDamping	= static_cast<float32>( script::LuaGetNumberFromTableItem( "angularDamping", 1, 0.0 ) ); 

		if( script::DoesTableItemExist( "allowSleep", 1, LUA_TBOOLEAN ) )
			bd.allowSleep		= script::LuaGetBoolFromTableItem( "allowSleep", 1, true );
		if( script::DoesTableItemExist( "awake", 1, LUA_TBOOLEAN ) )
			bd.awake			= !script::LuaGetBoolFromTableItem( "awake", 1, false ); 
		if( script::DoesTableItemExist( "fixedRotation", 1, LUA_TBOOLEAN ) )
			bd.fixedRotation	= script::LuaGetBoolFromTableItem( "fixedRotation", 1, false );
		if( script::DoesTableItemExist( "bullet", 1, LUA_TBOOLEAN ) )
			bd.bullet			= script::LuaGetBoolFromTableItem( "bullet", 1, false ); 
		if( script::DoesTableItemExist( "type", 1, LUA_TNUMBER ) )
			bd.type				= static_cast<b2BodyType>( (int)script::LuaGetNumberFromTableItem( "type", 1, 0.0 ) ); 
		if( script::DoesTableItemExist( "gravityScale", 1, LUA_TNUMBER ) )
			bd.gravityScale		= static_cast<float32>( script::LuaGetNumberFromTableItem( "gravityScale", 1, 1.0f ) ); 

		int numeric1 = -9999;
		int numeric2 = -9999;
		int numeric3 = -9999;
		int numeric4 = -9999;
		int numeric5 = -9999;

		if( script::DoesTableItemExist( "numeric1", 1, LUA_TNUMBER ) )
			numeric1	= static_cast<int>( script::LuaGetNumberFromTableItem( "numeric1", 1, -9999.0f ) ); 
		if( script::DoesTableItemExist( "numeric2", 1, LUA_TNUMBER ) )
			numeric2	= static_cast<int>( script::LuaGetNumberFromTableItem( "numeric2", 1, -9999.0f ) ); 
		if( script::DoesTableItemExist( "numeric3", 1, LUA_TNUMBER ) )
			numeric3	= static_cast<int>( script::LuaGetNumberFromTableItem( "numeric3", 1, -9999.0f ) ); 
		if( script::DoesTableItemExist( "numeric4", 1, LUA_TNUMBER ) )
			numeric4	= static_cast<int>( script::LuaGetNumberFromTableItem( "numeric4", 1, -9999.0f ) ); 
		if( script::DoesTableItemExist( "numeric5", 1, LUA_TNUMBER ) )
			numeric5	= static_cast<int>( script::LuaGetNumberFromTableItem( "numeric5", 1, -9999.0f ) ); 

		if( numeric1 != -9999.0f ||
			numeric2 != -9999.0f ||
			numeric3 != -9999.0f ||
			numeric4 != -9999.0f ||
			numeric5 != -9999.0f )
		{
			pPhysId = new physics::PhysicsIdentifier;
			DBG_ASSERT( pPhysId != 0 );

			pPhysId->SetNumeric1( numeric1 );
			pPhysId->SetNumeric2( numeric2 );
			pPhysId->SetNumeric3( numeric3 );
			pPhysId->SetNumeric4( numeric4 );
			pPhysId->SetNumeric5( numeric5 );
		}
	}
		
	pNewBody = pWorld->CreateBody(&bd);
	DBG_ASSERT( pNewBody != 0 );

	if( pPhysId != 0 )
		pNewBody->SetUserData( reinterpret_cast<void*>(pPhysId) );
	else
		pNewBody->SetUserData(0);

	lua_pushlightuserdata( pState, reinterpret_cast<void *>(pNewBody) );
	return(1);
}

/////////////////////////////////////////////////////
/// Function: ScriptGetBodyWorldCenter
/// Params: [in]pState
///
/////////////////////////////////////////////////////
int script::ScriptGetBodyWorldCenter( lua_State* pState )
{	
	b2Body* pBody = 0;
	b2Vec2 center( 0.0f, 0.0f );

	if( lua_islightuserdata( pState, 1 ) )
	{
		pBody = reinterpret_cast<b2Body *>( lua_touserdata(pState,1) );
		DBG_ASSERT( pBody != 0 );

		center = pBody->GetWorldCenter();
	}
	
	lua_pushnumber( pState, center.x );
	lua_pushnumber( pState, center.y );
	return(2);
}

/////////////////////////////////////////////////////
/// Function: ScriptCreateBoxShape
/// Params: [in]pState
///
/////////////////////////////////////////////////////
int script::ScriptCreateBoxShape( lua_State* pState )
{
	b2PolygonShape boxShape;
	b2FixtureDef fd;
	b2Body* pBody = 0;
	b2Fixture* pFixture = 0;
	physics::PhysicsIdentifier* pPhysId = 0;

	if( lua_islightuserdata( pState, 1 ) )
	{
		pBody = reinterpret_cast<b2Body *>( lua_touserdata(pState,1) );
		DBG_ASSERT( pBody != 0 );

		if( lua_istable( pState, 2 ) )
		{
			int paramIndex = 2;

			if( script::DoesTableItemExist( "friction", paramIndex, LUA_TNUMBER ) )
				fd.friction		= static_cast<float32>( script::LuaGetNumberFromTableItem( "friction", paramIndex, 0.2 ) );
			if( script::DoesTableItemExist( "restitution", paramIndex, LUA_TNUMBER ) )
				fd.restitution	= static_cast<float32>( script::LuaGetNumberFromTableItem( "restitution", paramIndex, 0.0 ) );
			if( script::DoesTableItemExist( "density", paramIndex, LUA_TNUMBER ) )
				fd.density		= static_cast<float32>( script::LuaGetNumberFromTableItem( "density", paramIndex, 0.0 ) );

			if( script::DoesTableItemExist( "categoryBits", paramIndex, LUA_TNUMBER ) )
				fd.filter.categoryBits	= static_cast<int16>( script::LuaGetNumberFromTableItem( "categoryBits", paramIndex, 0x0001 ) );
			if( script::DoesTableItemExist( "groupIndex", paramIndex, LUA_TNUMBER ) )
				fd.filter.groupIndex		= static_cast<int16>( script::LuaGetNumberFromTableItem( "groupIndex", paramIndex, 0 ) );
			if( script::DoesTableItemExist( "maskBits", paramIndex, LUA_TNUMBER ) )
				fd.filter.maskBits		= static_cast<int16>( script::LuaGetNumberFromTableItem( "maskBits", paramIndex, 0xFFFF ) );

			b2Vec2 center( 0.0f, 0.0f );
			if( script::DoesTableItemExist( "centerX", paramIndex, LUA_TNUMBER ) )
				center.x				= static_cast<float32>( script::LuaGetNumberFromTableItem( "centerX", paramIndex ) );
			if( script::DoesTableItemExist( "centerY", paramIndex, LUA_TNUMBER ) )
				center.y				= static_cast<float32>( script::LuaGetNumberFromTableItem( "centerY", paramIndex ) );

			float32 rotation		= 0.0f;
			if( script::DoesTableItemExist( "rotation", paramIndex, LUA_TNUMBER ) )
				rotation				= static_cast<float32>( script::LuaGetNumberFromTableItem( "rotation", paramIndex ) );
			float32 hx				= 1.0f;
			if( script::DoesTableItemExist( "hx", paramIndex, LUA_TNUMBER ) )
				hx = static_cast<float32>( script::LuaGetNumberFromTableItem( "hx", paramIndex ) );
			float32 hy				= 1.0f;
			if( script::DoesTableItemExist( "hy", paramIndex, LUA_TNUMBER ) )
				hy = static_cast<float32>( script::LuaGetNumberFromTableItem( "hy", paramIndex ) );

			int numeric1 = -9999;
			int numeric2 = -9999;
			int numeric3 = -9999;
			int numeric4 = -9999;
			int numeric5 = -9999;

			if( script::DoesTableItemExist( "numeric1", paramIndex, LUA_TNUMBER ) )
				numeric1	= static_cast<int>( script::LuaGetNumberFromTableItem( "numeric1", paramIndex, -9999.0f ) ); 
			if( script::DoesTableItemExist( "numeric2", paramIndex, LUA_TNUMBER ) )
				numeric2	= static_cast<int>( script::LuaGetNumberFromTableItem( "numeric2", paramIndex, -9999.0f ) ); 
			if( script::DoesTableItemExist( "numeric3", paramIndex, LUA_TNUMBER ) )
				numeric3	= static_cast<int>( script::LuaGetNumberFromTableItem( "numeric3", paramIndex, -9999.0f ) ); 
			if( script::DoesTableItemExist( "numeric4", paramIndex, LUA_TNUMBER ) )
				numeric4	= static_cast<int>( script::LuaGetNumberFromTableItem( "numeric4", paramIndex, -9999.0f ) ); 
			if( script::DoesTableItemExist( "numeric5", paramIndex, LUA_TNUMBER ) )
				numeric5	= static_cast<int>( script::LuaGetNumberFromTableItem( "numeric5", paramIndex, -9999.0f ) ); 

			if( numeric1 != -9999.0f ||
				numeric2 != -9999.0f ||
				numeric3 != -9999.0f ||
				numeric4 != -9999.0f ||
				numeric5 != -9999.0f )
			{
				pPhysId = new physics::PhysicsIdentifier;
				DBG_ASSERT( pPhysId != 0 );

				pPhysId->SetNumeric1( numeric1 );
				pPhysId->SetNumeric2( numeric2 );
				pPhysId->SetNumeric3( numeric3 );
				pPhysId->SetNumeric4( numeric4 );
				pPhysId->SetNumeric5( numeric5 );
			}

			boxShape.SetAsBox( hx, hy, center, rotation );	
		}			

		fd.shape = &boxShape;
		
		pFixture = pBody->CreateFixture( &fd );
		DBG_ASSERT( pFixture != 0 );

		if( pPhysId != 0 )
			pFixture->SetUserData( reinterpret_cast<void*>(pPhysId) );
		else
			pFixture->SetUserData( 0 );
	}
		
	lua_pushlightuserdata( pState, reinterpret_cast<void *>(pFixture) );
	return(1);
}

/////////////////////////////////////////////////////
/// Function: ScriptCreateCircleShape
/// Params: [in]pState
///
/////////////////////////////////////////////////////
int script::ScriptCreateCircleShape( lua_State* pState )
{
	b2CircleShape	circleShape;
	b2FixtureDef	fd;

	b2Body* pBody = 0;
	b2Fixture* pFixture = 0;
	physics::PhysicsIdentifier* pPhysId = 0;

	if( lua_islightuserdata( pState, 1 ) )
	{
		pBody = reinterpret_cast<b2Body *>( lua_touserdata(pState,1) );
		DBG_ASSERT( pBody != 0 );

		if( lua_istable( pState, 2 ) )
		{
			int paramIndex = 2;

			if( script::DoesTableItemExist( "friction", paramIndex, LUA_TNUMBER ) )
				fd.friction		= static_cast<float32>( script::LuaGetNumberFromTableItem( "friction", paramIndex, 0.2 ) );
			if( script::DoesTableItemExist( "restitution", paramIndex, LUA_TNUMBER ) )
				fd.restitution		= static_cast<float32>( script::LuaGetNumberFromTableItem( "restitution", paramIndex, 0.0 ) );
			if( script::DoesTableItemExist( "density", paramIndex, LUA_TNUMBER ) )
				fd.density			= static_cast<float32>( script::LuaGetNumberFromTableItem( "density", paramIndex, 0.0 ) );

			if( script::DoesTableItemExist( "localPositionX", paramIndex, LUA_TNUMBER ) )
				circleShape.m_p.x = static_cast<float32>( script::LuaGetNumberFromTableItem( "localPositionX", paramIndex, 0.0 ) );
			if( script::DoesTableItemExist( "localPositionY", paramIndex, LUA_TNUMBER ) )
				circleShape.m_p.y = static_cast<float32>( script::LuaGetNumberFromTableItem( "localPositionY", paramIndex, 0.0 ) );

			if( script::DoesTableItemExist( "radius", paramIndex, LUA_TNUMBER ) )
				circleShape.m_radius			= static_cast<float32>( script::LuaGetNumberFromTableItem( "radius", paramIndex, 1.0 ) );

			int numeric1 = -9999;
			int numeric2 = -9999;
			int numeric3 = -9999;
			int numeric4 = -9999;
			int numeric5 = -9999;

			if( script::DoesTableItemExist( "numeric1", paramIndex, LUA_TNUMBER ) )
				numeric1	= static_cast<int>( script::LuaGetNumberFromTableItem( "numeric1", paramIndex, -9999.0f ) ); 
			if( script::DoesTableItemExist( "numeric2", paramIndex, LUA_TNUMBER ) )
				numeric2	= static_cast<int>( script::LuaGetNumberFromTableItem( "numeric2", paramIndex, -9999.0f ) ); 
			if( script::DoesTableItemExist( "numeric3", paramIndex, LUA_TNUMBER ) )
				numeric3	= static_cast<int>( script::LuaGetNumberFromTableItem( "numeric3", paramIndex, -9999.0f ) ); 
			if( script::DoesTableItemExist( "numeric4", paramIndex, LUA_TNUMBER ) )
				numeric4	= static_cast<int>( script::LuaGetNumberFromTableItem( "numeric4", paramIndex, -9999.0f ) ); 
			if( script::DoesTableItemExist( "numeric5", paramIndex, LUA_TNUMBER ) )
				numeric5	= static_cast<int>( script::LuaGetNumberFromTableItem( "numeric5", paramIndex, -9999.0f ) ); 

			if( numeric1 != -9999.0f ||
				numeric2 != -9999.0f ||
				numeric3 != -9999.0f ||
				numeric4 != -9999.0f ||
				numeric5 != -9999.0f )
			{
				pPhysId = new physics::PhysicsIdentifier;
				DBG_ASSERT( pPhysId != 0 );

				pPhysId->SetNumeric1( numeric1 );
				pPhysId->SetNumeric2( numeric2 );
				pPhysId->SetNumeric3( numeric3 );
				pPhysId->SetNumeric4( numeric4 );
				pPhysId->SetNumeric5( numeric5 );
			}
		}			
		
		fd.shape = &circleShape;

		pFixture = pBody->CreateFixture( &fd );
		DBG_ASSERT( pFixture != 0 );

		if( pPhysId != 0 )
			pFixture->SetUserData( reinterpret_cast<void*>(pPhysId) );
		else
			pFixture->SetUserData( 0 );
	}

	lua_pushlightuserdata( pState, reinterpret_cast<void *>(pFixture) );
	return(1);
}

/////////////////////////////////////////////////////
/// Function: ScriptCreatePolygonShape
/// Params: [in]pState
///
/////////////////////////////////////////////////////
int script::ScriptCreatePolygonShape( lua_State* pState )
{
	int i=0, j=0, k=0, componentIndex=0, vertIndex=0;
	b2PolygonShape polyShape;
	b2FixtureDef fd;

	b2Body* pBody = 0;
	b2Fixture* pFixture = 0;
	physics::PhysicsIdentifier* pPhysId = 0;

	if( lua_islightuserdata( pState, 1 ) )
	{
		pBody = reinterpret_cast<b2Body *>( lua_touserdata(pState,1) );
		DBG_ASSERT( pBody != 0 );

		if( lua_istable( pState, 2 ) )
		{
			int paramIndex = 2;

			if( script::DoesTableItemExist( "friction", paramIndex, LUA_TNUMBER ) )
				fd.friction		= static_cast<float32>( script::LuaGetNumberFromTableItem( "friction", paramIndex, 0.2 ) );
			if( script::DoesTableItemExist( "restitution", paramIndex, LUA_TNUMBER ) )
				fd.restitution	= static_cast<float32>( script::LuaGetNumberFromTableItem( "restitution", paramIndex, 0.0 ) );
			if( script::DoesTableItemExist( "density", paramIndex, LUA_TNUMBER ) )
				fd.density		= static_cast<float32>( script::LuaGetNumberFromTableItem( "density", paramIndex, 0.0 ) );

			if( script::DoesTableItemExist( "categoryBits", paramIndex, LUA_TNUMBER ) )
				fd.filter.categoryBits	= static_cast<int16>( script::LuaGetNumberFromTableItem( "categoryBits", paramIndex, 0x0001 ) );
			if( script::DoesTableItemExist( "groupIndex", paramIndex, LUA_TNUMBER ) )
				fd.filter.groupIndex		= static_cast<int16>( script::LuaGetNumberFromTableItem( "groupIndex", paramIndex, 0 ) );
			if( script::DoesTableItemExist( "maskBits", paramIndex, LUA_TNUMBER ) )
				fd.filter.maskBits		= static_cast<int16>( script::LuaGetNumberFromTableItem( "maskBits", paramIndex, 0xFFFF ) );

			int numeric1 = -9999;
			int numeric2 = -9999;
			int numeric3 = -9999;
			int numeric4 = -9999;
			int numeric5 = -9999;

			if( script::DoesTableItemExist( "numeric1", paramIndex, LUA_TNUMBER ) )
				numeric1	= static_cast<int>( script::LuaGetNumberFromTableItem( "numeric1", paramIndex, -9999.0f ) ); 
			if( script::DoesTableItemExist( "numeric2", paramIndex, LUA_TNUMBER ) )
				numeric2	= static_cast<int>( script::LuaGetNumberFromTableItem( "numeric2", paramIndex, -9999.0f ) ); 
			if( script::DoesTableItemExist( "numeric3", paramIndex, LUA_TNUMBER ) )
				numeric3	= static_cast<int>( script::LuaGetNumberFromTableItem( "numeric3", paramIndex, -9999.0f ) ); 
			if( script::DoesTableItemExist( "numeric4", paramIndex, LUA_TNUMBER ) )
				numeric4	= static_cast<int>( script::LuaGetNumberFromTableItem( "numeric4", paramIndex, -9999.0f ) ); 
			if( script::DoesTableItemExist( "numeric5", paramIndex, LUA_TNUMBER ) )
				numeric5	= static_cast<int>( script::LuaGetNumberFromTableItem( "numeric5", paramIndex, -9999.0f ) ); 

			int triCount				= 0;
			if( script::DoesTableItemExist( "tris", paramIndex, LUA_TNUMBER ) )
				triCount = static_cast<int>( script::LuaGetNumberFromTableItem( "tris", paramIndex ) );

			int meshPolyListCount		= 0;
			if( script::DoesTableItemExist( "meshPolyListCount", paramIndex, LUA_TNUMBER ) )
				meshPolyListCount = static_cast<int>( script::LuaGetNumberFromTableItem( "meshPolyListCount", paramIndex ) );

			for( i=0; i < meshPolyListCount; ++i )
			{
				char polyListTableName[32];
				std::memset( polyListTableName, 0, sizeof(char)*32 );

				snprintf( polyListTableName, 32, "polyList%d", i );

				// grab table
				lua_pushstring( pState, polyListTableName );
				lua_gettable( pState, -2 ); // key

				// new table is top
				if( lua_istable( pState, -1 ) )
				{
					int polyCount		= 0;
					if( script::DoesTableItemExist( "polyCount", 3, LUA_TNUMBER ) )
						polyCount = static_cast<int>( script::LuaGetNumberFromTableItem( "polyCount", 3 ) );

					for( j=0; j < polyCount; ++j )
					{
						char polyTableName[32];
						std::memset( polyTableName, 0, sizeof(char)*32 );

						snprintf( polyTableName, 32, "poly%d", j );

						// grab table
						lua_pushstring( pState, polyTableName );
						lua_gettable( pState, -2 ); // key

						if( lua_istable( pState, -1 ) )
						{
							int polyVertCount		= 0;
							if( script::DoesTableItemExist( "polyVertCount", 4, LUA_TNUMBER ) )
								polyVertCount = static_cast<int>( script::LuaGetNumberFromTableItem( "polyVertCount", 4 ) );

							polyShape.m_count = polyVertCount;
							DBG_ASSERT( polyVertCount < b2_maxPolygonVertices );

							b2Vec2 vertices[b2_maxPolygonVertices];

							// grab table
							/////////////
							{
								lua_pushstring( pState, "v" );
								lua_gettable( pState, -2 ); // key

								if( lua_istable( pState, -1 ) )
								{
									for( k=1, componentIndex=0, vertIndex=0; k <= polyVertCount*2; ++k, ++componentIndex)
									{
										lua_rawgeti( pState, -1, k );

										if( componentIndex == 0 )
											vertices[vertIndex].x = static_cast<float32>( lua_tonumber( pState, -1 ) );
										else
										{
											vertices[vertIndex].y = static_cast<float32>( lua_tonumber( pState, -1 ) );

											componentIndex = -1;
											vertIndex++;
										}

										lua_pop( pState, 1 );
									}

									polyShape.Set( &vertices[0], polyVertCount );
									fd.shape = &polyShape;

									// poly complete
									pFixture = pBody->CreateFixture( &fd );
									DBG_ASSERT( pFixture != 0 );

									if( numeric1 != -9999.0f ||
										numeric2 != -9999.0f ||
										numeric3 != -9999.0f ||
										numeric4 != -9999.0f ||
										numeric5 != -9999.0f )
									{
										pPhysId = new physics::PhysicsIdentifier;
										DBG_ASSERT( pPhysId != 0 );

										pPhysId->SetNumeric1( numeric1 );
										pPhysId->SetNumeric2( numeric2 );
										pPhysId->SetNumeric3( numeric3 );
										pPhysId->SetNumeric4( numeric4 );
										pPhysId->SetNumeric5( numeric5 );
									}

									if( pPhysId != 0 )
										pFixture->SetUserData( reinterpret_cast<void*>(pPhysId) );
									else
										pFixture->SetUserData( 0 );
								}
								// remove poly table
								lua_pop( pState, 1 );
							}
							///////////////
						}

						// remove poly table
						lua_pop( pState, 1 );
					}
				}

				// remove polyList table
				lua_pop( pState, 1 );
			}			
		}			
		
	}
		
	//lua_pushlightuserdata( pState, reinterpret_cast<void *>(pShape) );
	return(0);
}

/////////////////////////////////////////////////////
/// Function: ScriptDestroyFixture
/// Params: [in]pState
///
/////////////////////////////////////////////////////
int script::ScriptDestroyFixture( lua_State* pState )
{
	b2Body* pBody = 0;
	b2Fixture* pFixture = 0;

	if( lua_islightuserdata( pState, 1 ) &&
		lua_islightuserdata( pState, 2 ) )
	{
		pBody = reinterpret_cast<b2Body *>( lua_touserdata(pState,1) );
		DBG_ASSERT( pBody != 0 );
		pFixture = reinterpret_cast<b2Fixture *>( lua_touserdata(pState,2) );
		DBG_ASSERT( pFixture != 0 );

		pBody->DestroyFixture(pFixture);
	}

	return(0);
}

/////////////////////////////////////////////////////
/// Function: ScriptCreateDistanceJoint
/// Params: [in]pState
///
/////////////////////////////////////////////////////
int script::ScriptCreateDistanceJoint( lua_State* pState )
{
	//Initialize(pBody1, pBody2, anchor1X, anchor1Y, anchor2X, anchor2Y)

	b2DistanceJointDef jd;
	b2DistanceJoint* pJoint = 0;

	b2Body* pBody1 = 0;
	b2Body* pBody2 = 0;
	b2Vec2 anchor1( 0.0f, 0.0f );
	b2Vec2 anchor2( 0.0f, 0.0f );

	// prepare the joint
	if( lua_islightuserdata( pState, 1 ) && // pBody1
		lua_islightuserdata( pState, 2 ) && // pBody2
		lua_isnumber( pState, 3 ) && // anchor1X
		lua_isnumber( pState, 4 ) && // anchor1Y
		lua_isnumber( pState, 5 ) && // anchor2X
		lua_isnumber( pState, 6 ) ) // anchor2Y
	{

		pBody1 = reinterpret_cast<b2Body*>( lua_touserdata( pState, 1 ) );
		DBG_ASSERT( pBody1 != 0 );

		pBody2 = reinterpret_cast<b2Body*>( lua_touserdata( pState, 2 ) );
		DBG_ASSERT( pBody2 != 0 );

		anchor1.x = static_cast<float32>( lua_tonumber( pState, 3 ) );
		anchor1.y = static_cast<float32>( lua_tonumber( pState, 4 ) );

		anchor2.x = static_cast<float32>( lua_tonumber( pState, 5 ) );
		anchor2.y = static_cast<float32>( lua_tonumber( pState, 6 ) );

		jd.Initialize( pBody1, pBody1, anchor1, anchor2 );

		//
		if( lua_istable( pState, 7 ) )
		{
			if( script::DoesTableItemExist( "localAnchor1X", 7, LUA_TNUMBER ) )
				jd.localAnchorA.x		= static_cast<float32>( script::LuaGetNumberFromTableItem( "localAnchor1X", 7, 0.0 ) );
			if( script::DoesTableItemExist( "localAnchor1Y", 7, LUA_TNUMBER ) )
				jd.localAnchorA.y		= static_cast<float32>( script::LuaGetNumberFromTableItem( "localAnchor1Y", 7, 0.0 ) );
			if( script::DoesTableItemExist( "localAnchor2X", 7, LUA_TNUMBER ) )
				jd.localAnchorB.x		= static_cast<float32>( script::LuaGetNumberFromTableItem( "localAnchor2X", 7, 0.0 ) );
			if( script::DoesTableItemExist( "localAnchor2Y", 7, LUA_TNUMBER ) )
				jd.localAnchorB.y		= static_cast<float32>( script::LuaGetNumberFromTableItem( "localAnchor2Y", 7, 0.0 ) );
			if( script::DoesTableItemExist( "length", 7, LUA_TNUMBER ) )
				jd.length				= static_cast<float32>( script::LuaGetNumberFromTableItem( "length", 7, 1.0 ) );
			if( script::DoesTableItemExist( "frequencyHz", 7, LUA_TNUMBER ) )
				jd.frequencyHz			= static_cast<float32>( script::LuaGetNumberFromTableItem( "frequencyHz", 7, 0.0 ) );
			if( script::DoesTableItemExist( "dampingRatio", 7, LUA_TNUMBER ) )
				jd.dampingRatio			= static_cast<float32>( script::LuaGetNumberFromTableItem( "dampingRatio", 7, 0.0 ) );
		}

		pJoint = reinterpret_cast<b2DistanceJoint *>( physics::PhysicsWorldB2D::GetWorld()->CreateJoint( &jd ) );
		DBG_ASSERT( pJoint != 0 );
	}

	lua_pushlightuserdata( pState, reinterpret_cast<void *>(pJoint) );
	return(1);
}

/////////////////////////////////////////////////////
/// Function: ScriptCreateGearJoint
/// Params: [in]pState
///
/////////////////////////////////////////////////////
int script::ScriptCreateGearJoint( lua_State* pState )
{
	b2GearJointDef jd;
	b2GearJoint* pJoint = 0;

	if( lua_istable( pState, 1 ) )
	{
		if( script::DoesTableItemExist( "joint1", 1, LUA_TLIGHTUSERDATA ) )
			jd.joint1		= reinterpret_cast<b2Joint*>( script::LuaGetUserDataFromTableItem( "joint1", 1, 0 ) );
		if( script::DoesTableItemExist( "joint2", 1, LUA_TLIGHTUSERDATA ) )
			jd.joint2		= reinterpret_cast<b2Joint*>( script::LuaGetUserDataFromTableItem( "joint2", 1, 0 ) );
		if( script::DoesTableItemExist( "ratio", 1, LUA_TNUMBER ) )
			jd.ratio		= static_cast<float32>( script::LuaGetNumberFromTableItem( "ratio", 1, 1.0 ) );

		pJoint = reinterpret_cast<b2GearJoint *>( physics::PhysicsWorldB2D::GetWorld()->CreateJoint( &jd ) );
		DBG_ASSERT( pJoint != 0 );
	}

	lua_pushlightuserdata( pState, reinterpret_cast<void *>(pJoint) );
	return(1);
}

/////////////////////////////////////////////////////
/// Function: ScriptCreateMouseJoint
/// Params: [in]pState
///
/////////////////////////////////////////////////////
int script::ScriptCreateMouseJoint( lua_State* pState )
{
	b2MouseJointDef jd;
	b2MouseJoint* pJoint = 0;

	if( lua_istable( pState, 1 ) )
	{
		if( script::DoesTableItemExist( "targetX", 1, LUA_TNUMBER ) )
			jd.target.x		= static_cast<float32>( script::LuaGetNumberFromTableItem( "targetX", 1, 0.0 ) );
		if( script::DoesTableItemExist( "targetY", 1, LUA_TNUMBER ) )
			jd.target.y		= static_cast<float32>( script::LuaGetNumberFromTableItem( "targetY", 1, 0.0 ) );
		if( script::DoesTableItemExist( "maxForce", 1, LUA_TNUMBER ) )
			jd.maxForce		= static_cast<float32>( script::LuaGetNumberFromTableItem( "maxForce", 1, 0.0 ) );
		if( script::DoesTableItemExist( "frequencyHz", 1, LUA_TNUMBER ) )
			jd.frequencyHz	= static_cast<float32>( script::LuaGetNumberFromTableItem( "frequencyHz", 1, 5.0 ) );
		if( script::DoesTableItemExist( "dampingRatio", 1, LUA_TNUMBER ) )
			jd.dampingRatio	= static_cast<float32>( script::LuaGetNumberFromTableItem( "dampingRatio", 1, 0.7 ) );
		//if( script::DoesTableItemExist( "timeStep", 1, LUA_TNUMBER ) )
		//	jd.timeStep		= static_cast<float32>( script::LuaGetNumberFromTableItem( "timeStep", 1, 1.0/60.0 ) );
		
		pJoint = reinterpret_cast<b2MouseJoint *>( physics::PhysicsWorldB2D::GetWorld()->CreateJoint( &jd ) );
		DBG_ASSERT( pJoint != 0 );
	}

	lua_pushlightuserdata( pState, reinterpret_cast<void *>(pJoint) );
	return(1);
}

/////////////////////////////////////////////////////
/// Function: ScriptCreatePrismaticJoint
/// Params: [in]pState
///
/////////////////////////////////////////////////////
int script::ScriptCreatePrismaticJoint( lua_State* pState )
{
	//void Initialize(pBody1, pBody2, anchorX, anchorY, axisX, axisY)

	b2PrismaticJointDef jd;
	b2PrismaticJoint* pJoint = 0;

	b2Body* pBody1 = 0;
	b2Body* pBody2 = 0;
	b2Vec2 anchor( 0.0f, 0.0f );
	b2Vec2 axis( 1.0f, 0.0f );

	// prepare the joint
	if( lua_islightuserdata( pState, 1 ) && // pBody1
		lua_islightuserdata( pState, 2 ) && // pBody2
		lua_isnumber( pState, 3 ) && // anchor1X
		lua_isnumber( pState, 4 ) && // anchor1Y
		lua_isnumber( pState, 5 ) && // axisX
		lua_isnumber( pState, 6 ) ) // axisY
	{

		pBody1 = reinterpret_cast<b2Body*>( lua_touserdata( pState, 1 ) );
		DBG_ASSERT( pBody1 != 0 );

		pBody2 = reinterpret_cast<b2Body*>( lua_touserdata( pState, 2 ) );
		DBG_ASSERT( pBody2 != 0 );

		anchor.x = static_cast<float32>( lua_tonumber( pState, 3 ) );
		anchor.y = static_cast<float32>( lua_tonumber( pState, 4 ) );

		axis.x = static_cast<float32>( lua_tonumber( pState, 5 ) );
		axis.y = static_cast<float32>( lua_tonumber( pState, 6 ) );

		jd.Initialize( pBody1, pBody2, anchor, axis );

		//
		if( lua_istable( pState, 7 ) )
		{
			if( script::DoesTableItemExist( "localAnchor1X", 7, LUA_TNUMBER ) )
				jd.localAnchorA.x		= static_cast<float32>( script::LuaGetNumberFromTableItem( "localAnchor1X", 7, 0.0 ) );
			if( script::DoesTableItemExist( "localAnchor1Y", 7, LUA_TNUMBER ) )
				jd.localAnchorA.y		= static_cast<float32>( script::LuaGetNumberFromTableItem( "localAnchor1Y", 7, 0.0 ) );
			if( script::DoesTableItemExist( "localAnchor2X", 7, LUA_TNUMBER ) )
				jd.localAnchorB.x		= static_cast<float32>( script::LuaGetNumberFromTableItem( "localAnchor2X", 7, 0.0 ) );
			if( script::DoesTableItemExist( "localAnchor2Y", 7, LUA_TNUMBER ) )
				jd.localAnchorB.y		= static_cast<float32>( script::LuaGetNumberFromTableItem( "localAnchor2Y", 7, 0.0 ) );

			if( script::DoesTableItemExist( "localAxisAX", 7, LUA_TNUMBER ) )
				jd.localAxisA.x			= static_cast<float32>( script::LuaGetNumberFromTableItem( "localAxis1X", 7, 1.0 ) );
			if( script::DoesTableItemExist( "localAxisAY", 7, LUA_TNUMBER ) )
				jd.localAxisA.y			= static_cast<float32>( script::LuaGetNumberFromTableItem( "localAxis1Y", 7, 0.0 ) );
			if( script::DoesTableItemExist( "referenceAngle", 7, LUA_TNUMBER ) )
				jd.referenceAngle		= math::DegToRad( static_cast<float32>( script::LuaGetNumberFromTableItem( "referenceAngle", 7, 0.0 ) ) );
			if( script::DoesTableItemExist( "enableLimit", 7, LUA_TBOOLEAN) )
				jd.enableLimit			= script::LuaGetBoolFromTableItem( "enableLimit", 7, false );

			if( script::DoesTableItemExist( "lowerTranslation", 7, LUA_TNUMBER ) )
				jd.lowerTranslation		= static_cast<float32>( script::LuaGetNumberFromTableItem( "lowerTranslation", 7, 0.0 ) );
			if( script::DoesTableItemExist( "upperTranslation", 7, LUA_TNUMBER ) )
				jd.upperTranslation		= static_cast<float32>( script::LuaGetNumberFromTableItem( "upperTranslation", 7, 0.0 ) );
			if( script::DoesTableItemExist( "enableMotor", 7, LUA_TBOOLEAN ) )
				jd.enableMotor			= script::LuaGetBoolFromTableItem( "enableMotor", 7, false );
			if( script::DoesTableItemExist( "maxMotorForce", 7, LUA_TNUMBER ) )
				jd.maxMotorForce		= static_cast<float32>( script::LuaGetNumberFromTableItem( "maxMotorForce", 7, 0.0 ) );
			if( script::DoesTableItemExist( "motorSpeed", 7, LUA_TNUMBER ) )
				jd.motorSpeed			= static_cast<float32>( script::LuaGetNumberFromTableItem( "motorSpeed", 7, 0.0 ) );
		}

		pJoint = reinterpret_cast<b2PrismaticJoint *>( physics::PhysicsWorldB2D::GetWorld()->CreateJoint( &jd ) );
		DBG_ASSERT( pJoint != 0 );
	}

	lua_pushlightuserdata( pState, reinterpret_cast<void *>(pJoint) );
	return(1);
}

/////////////////////////////////////////////////////
/// Function: ScriptCreatePulleyJoint
/// Params: [in]pState
///
/////////////////////////////////////////////////////
int script::ScriptCreatePulleyJoint( lua_State* pState )
{
	//void Initialize(pBody1, pBody2, groundAnchor1X, groundAnchor1Y, groundAnchor2X, groundAnchor2Y,
	//				anchor1X, anchor1Y, anchor2X, anchor2Y, ratio );

	b2PulleyJointDef jd;
	b2PulleyJoint* pJoint = 0;

	b2Body* pBody1 = 0;
	b2Body* pBody2 = 0;
	b2Vec2 groundAnchor1( -1.0f, 1.0f );
	b2Vec2 groundAnchor2( 1.0f, 1.0f );
	b2Vec2 anchor1( -1.0f, 0.0f );
	b2Vec2 anchor2( 1.0f, 0.0f );
	float32 ratio = 1.0f;

	// prepare the joint
	if( lua_islightuserdata( pState, 1 ) && // pBody1
		lua_islightuserdata( pState, 2 ) && // pBody2
		lua_isnumber( pState, 3 ) && // groundAnchor1X
		lua_isnumber( pState, 4 ) && // groundAnchor1Y
		lua_isnumber( pState, 5 ) && // groundAnchor2X
		lua_isnumber( pState, 6 ) && // groundAnchor2Y
		lua_isnumber( pState, 7 ) && // anchor1X
		lua_isnumber( pState, 8 ) && // anchor1Y
		lua_isnumber( pState, 9 ) && // anchor2X
		lua_isnumber( pState, 10 ) && // anchor2Y
		lua_isnumber( pState, 11 ) ) // ratio 
	{

		pBody1 = reinterpret_cast<b2Body*>( lua_touserdata( pState, 1 ) );
		DBG_ASSERT( pBody1 != 0 );

		pBody2 = reinterpret_cast<b2Body*>( lua_touserdata( pState, 2 ) );
		DBG_ASSERT( pBody2 != 0 );

		groundAnchor1.x = static_cast<float32>( lua_tonumber( pState, 3 ) );
		groundAnchor1.y = static_cast<float32>( lua_tonumber( pState, 4 ) );

		groundAnchor2.x = static_cast<float32>( lua_tonumber( pState, 5 ) );
		groundAnchor2.y = static_cast<float32>( lua_tonumber( pState, 6 ) );

		anchor1.x = static_cast<float32>( lua_tonumber( pState, 7 ) );
		anchor1.y = static_cast<float32>( lua_tonumber( pState, 8 ) );

		anchor2.x = static_cast<float32>( lua_tonumber( pState, 9 ) );
		anchor2.y = static_cast<float32>( lua_tonumber( pState, 10 ) );

		ratio = static_cast<float32>( lua_tonumber( pState, 11 ) );

		jd.Initialize( pBody1, pBody2, groundAnchor1, groundAnchor2, anchor1, anchor2, ratio );

		//
		if( lua_istable( pState, 12 ) )
		{
			if( script::DoesTableItemExist( "groundAnchor1X", 12, LUA_TNUMBER ) )
				jd.groundAnchorA.x		= static_cast<float32>( script::LuaGetNumberFromTableItem( "groundAnchor1X", 12, -1.0 ) );
			if( script::DoesTableItemExist( "groundAnchor1Y", 12, LUA_TNUMBER ) )
				jd.groundAnchorA.y		= static_cast<float32>( script::LuaGetNumberFromTableItem( "groundAnchor1Y", 12, 1.0 ) );
			if( script::DoesTableItemExist( "groundAnchor2X", 12, LUA_TNUMBER ) )
				jd.groundAnchorB.x		= static_cast<float32>( script::LuaGetNumberFromTableItem( "groundAnchor2X", 12, 1.0 ) );
			if( script::DoesTableItemExist( "groundAnchor2Y", 12, LUA_TNUMBER ) )
				jd.groundAnchorB.y		= static_cast<float32>( script::LuaGetNumberFromTableItem( "groundAnchor2Y", 12, 1.0 ) );

			if( script::DoesTableItemExist( "localAnchor1X", 12, LUA_TNUMBER ) )
				jd.localAnchorA.x		= static_cast<float32>( script::LuaGetNumberFromTableItem( "localAnchor1X", 12, -1.0 ) );
			if( script::DoesTableItemExist( "localAnchor1Y", 12, LUA_TNUMBER ) )
				jd.localAnchorA.y		= static_cast<float32>( script::LuaGetNumberFromTableItem( "localAnchor1Y", 12, 0.0 ) );
			if( script::DoesTableItemExist( "localAnchor2X", 12, LUA_TNUMBER ) )
				jd.localAnchorB.x		= static_cast<float32>( script::LuaGetNumberFromTableItem( "localAnchor2X", 12, 1.0 ) );
			if( script::DoesTableItemExist( "localAnchor2Y", 12, LUA_TNUMBER ) )
				jd.localAnchorB.y		= static_cast<float32>( script::LuaGetNumberFromTableItem( "localAnchor2Y", 12, 0.0 ) );

			if( script::DoesTableItemExist( "length1", 12, LUA_TNUMBER ) )
				jd.lengthA				= static_cast<float32>( script::LuaGetNumberFromTableItem( "length1", 12, 0.0 ) );

			if( script::DoesTableItemExist( "length2", 12, LUA_TNUMBER ) )
				jd.lengthB				= static_cast<float32>( script::LuaGetNumberFromTableItem( "length2", 12, 0.0 ) );

			if( script::DoesTableItemExist( "ratio", 12, LUA_TNUMBER ) )
				jd.ratio				= static_cast<float32>( script::LuaGetNumberFromTableItem( "ratio", 12, 1.0 ) );

			if( script::DoesTableItemExist( "collideConnected", 12, LUA_TBOOLEAN ) )
				jd.collideConnected		= script::LuaGetBoolFromTableItem( "collideConnected", 12, true );
		}

		pJoint = reinterpret_cast<b2PulleyJoint *>( physics::PhysicsWorldB2D::GetWorld()->CreateJoint( &jd ) );
		DBG_ASSERT( pJoint != 0 );
	}

	lua_pushlightuserdata( pState, reinterpret_cast<void *>(pJoint) );
	return(1);
}

/////////////////////////////////////////////////////
/// Function: ScriptCreateRevoluteJoint
/// Params: [in]pState
///
/////////////////////////////////////////////////////
int script::ScriptCreateRevoluteJoint( lua_State* pState )
{
	// CreateRevoluteJoint( pBody1, pBody2, anchorX, anchorY, jointData )

	b2RevoluteJointDef jd;
	b2RevoluteJoint* pJoint = 0;

	b2Body* pBody1 = 0;
	b2Body* pBody2 = 0;
	b2Vec2 anchor( 0.0f, 0.0f );

	// prepare the joint
	if( lua_islightuserdata( pState, 1 ) && // pBody1
		lua_islightuserdata( pState, 2 ) && // pBody2
		lua_isnumber( pState, 3 ) && // anchorX
		lua_isnumber( pState, 4 ) ) // anchorY
	{
		pBody1 = reinterpret_cast<b2Body*>( lua_touserdata( pState, 1 ) );
		DBG_ASSERT( pBody1 != 0 );

		pBody2 = reinterpret_cast<b2Body*>( lua_touserdata( pState, 2 ) );
		DBG_ASSERT( pBody2 != 0 );

		anchor.x = static_cast<float32>( lua_tonumber( pState, 3 ) );
		anchor.y = static_cast<float32>( lua_tonumber( pState, 4 ) );
		
		jd.Initialize( pBody1, pBody2, anchor );

		// 
		if( lua_istable( pState, 5 ) )
		{
			if( script::DoesTableItemExist( "localAnchor1X", 5, LUA_TNUMBER ) )
				jd.localAnchorA.x		= static_cast<float32>( script::LuaGetNumberFromTableItem( "localAnchor1X", 5, 0.0 ) );
			if( script::DoesTableItemExist( "localAnchor1Y", 5, LUA_TNUMBER ) )
				jd.localAnchorA.y		= static_cast<float32>( script::LuaGetNumberFromTableItem( "localAnchor1Y", 5, 0.0 ) );
			if( script::DoesTableItemExist( "localAnchor2Y", 5, LUA_TNUMBER ) )
				jd.localAnchorB.x		= static_cast<float32>( script::LuaGetNumberFromTableItem( "localAnchor2X", 5, 0.0 ) );
			if( script::DoesTableItemExist( "localAnchor2Y", 5, LUA_TNUMBER ) )
				jd.localAnchorB.y		= static_cast<float32>( script::LuaGetNumberFromTableItem( "localAnchor2Y", 5, 0.0 ) );

			if( script::DoesTableItemExist( "referenceAngle", 5, LUA_TNUMBER ) )
				jd.referenceAngle		= math::DegToRad( static_cast<float32>( script::LuaGetNumberFromTableItem( "referenceAngle", 5, 0.0 ) ) );
			if( script::DoesTableItemExist( "lowerAngle", 5, LUA_TNUMBER ) )
				jd.lowerAngle			= math::DegToRad( static_cast<float32>( script::LuaGetNumberFromTableItem( "lowerAngle", 5, 0.0 ) ) );
			if( script::DoesTableItemExist( "upperAngle", 5, LUA_TNUMBER ) )
				jd.upperAngle			= math::DegToRad( static_cast<float32>( script::LuaGetNumberFromTableItem( "upperAngle", 5, 0.0 ) ) );
			if( script::DoesTableItemExist( "maxMotorTorque", 5, LUA_TNUMBER ) )
				jd.maxMotorTorque		= static_cast<float32>( script::LuaGetNumberFromTableItem( "maxMotorTorque", 5, 0.0 ) );
			if( script::DoesTableItemExist( "motorSpeed", 5, LUA_TNUMBER ) )
				jd.motorSpeed			= static_cast<float32>( script::LuaGetNumberFromTableItem( "motorSpeed", 5, 0.0 ) );

			if( script::DoesTableItemExist( "enableLimit", 5, LUA_TBOOLEAN ) )
				jd.enableLimit			= script::LuaGetBoolFromTableItem( "enableLimit", 5, false );
			if( script::DoesTableItemExist( "enableMotor", 5, LUA_TBOOLEAN ) )
				jd.enableMotor			= script::LuaGetBoolFromTableItem( "enableMotor", 5, false );
		}

		pJoint = reinterpret_cast<b2RevoluteJoint *>( physics::PhysicsWorldB2D::GetWorld()->CreateJoint( &jd ) );
		DBG_ASSERT( pJoint != 0 );
	}

	lua_pushlightuserdata( pState, reinterpret_cast<void *>(pJoint) );
	return(1);
}

/////////////////////////////////////////////////////
/// Function: ScriptDestroyJoint
/// Params: [in]pState
///
/////////////////////////////////////////////////////
int script::ScriptDestroyJoint( lua_State* pState )
{
	b2Joint* pJoint = 0;

	if( lua_islightuserdata( pState, 1 ) )
	{
		pJoint = reinterpret_cast<b2Joint *>( lua_touserdata(pState,1) );
		DBG_ASSERT( pJoint != 0 );

		physics::PhysicsWorldB2D::GetWorld()->DestroyJoint(pJoint);
	}

	return(0);
	
}

/////////////////////////////////////////////////////
/// Function: ScriptSetMassFromShapes
/// Params: [in]pState
///
/////////////////////////////////////////////////////
int script::ScriptSetMassFromShapes( lua_State* pState )
{
	b2Body* pBody = 0;

	if( lua_islightuserdata( pState, 1 ) )
	{
		pBody = reinterpret_cast<b2Body *>( lua_touserdata(pState,1) );
		DBG_ASSERT( pBody != 0 );

		//pBody->SSetMassFromShapes();
	}

	return(0);
}

/////////////////////////////////////////////////////
/// Function: ScriptSetMeshToBody
/// Params: [in]pState
///
/////////////////////////////////////////////////////
int script::ScriptSetMeshToBody( lua_State* pState )
{
	DBG_ASSERT(0);
	return(0);
}

/////////////////////////////////////////////////////
/// Function: ScriptSetBodyXForm
/// Params: [in]pState
///
/////////////////////////////////////////////////////
int script::ScriptSetBodyXForm( lua_State* pState )
{
	b2Body* pBody = 0;
	b2Vec2 pos( 0.0f, 0.0f );
	float32 angle = 0.0f;

	if( lua_islightuserdata( pState, 1 ) && // body
		lua_isnumber( pState, 2 ) && // positionX
		lua_isnumber( pState, 3 ) && // positionY
		lua_isnumber( pState, 4 ) ) // angle
	{
		pBody = reinterpret_cast<b2Body *>( lua_touserdata(pState,1) );
		DBG_ASSERT( pBody != 0 );

		pos.x = static_cast<float32>( lua_tonumber(pState,2) );
		pos.y = static_cast<float32>( lua_tonumber(pState,3) );

		angle = math::DegToRad( static_cast<float32>( lua_tonumber(pState,4) ) );

		pBody->SetTransform( pos, angle );
	}

	return(0);
}

/////////////////////////////////////////////////////
/// Function: ScriptSetBodyMass
/// Params: [in]pState
///
/////////////////////////////////////////////////////
int script::ScriptSetBodyMass( lua_State* pState )
{
	b2Body* pBody = 0;
	b2MassData massData;
	std::memset( &massData, 0, sizeof(b2MassData) );

	if( lua_islightuserdata( pState, 1 ) ) // body
	{
		pBody = reinterpret_cast<b2Body *>( lua_touserdata(pState,1) );
		DBG_ASSERT( pBody != 0 );

		if( lua_istable( pState, 2 ) )
		{
			if( script::DoesTableItemExist( "mass", 2, LUA_TNUMBER ) )
				massData.mass		= static_cast<float32>( script::LuaGetNumberFromTableItem( "mass", 2, 0.0 ) );
			if( script::DoesTableItemExist( "centerX", 2, LUA_TNUMBER ) )
				massData.center.x	= static_cast<float32>( script::LuaGetNumberFromTableItem( "centerX", 2, 0.0 ) );
			if( script::DoesTableItemExist( "centerY", 2, LUA_TNUMBER ) )
				massData.center.y	= static_cast<float32>( script::LuaGetNumberFromTableItem( "centerY", 2, 0.0 ) );
			if( script::DoesTableItemExist( "I", 2, LUA_TNUMBER ) )
				massData.I			= static_cast<float32>( script::LuaGetNumberFromTableItem( "I", 2, 0.0 ) );
			else
			{
				if( !pBody->IsFixedRotation() )
					massData.I = massData.mass * b2Dot(massData.center, massData.center);
			}
		}

		pBody->SetMassData( &massData );
	}

	return(0);
}

#endif // BASE_SUPPORT_BOX2D

