
/*===================================================================
	File: PhysicsAccess.cpp
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

#include "Audio/AudioSystem.h"

#include "Physics/PhysicsWorld.h"
#include "Level/Level.h"
#include "Player/Player.h"

#include "ScriptAccess/ScriptDataHolder.h"
#include "ScriptAccess/PhysicsAccess.h"

namespace taxiScript
{
	ScriptDataHolder* pScriptPhysData = 0;
}

/////////////////////////////////////////////////////
/// Function: RegisterScriptFunctions
/// Params: [in]dataHolder
///
/////////////////////////////////////////////////////
void RegisterPhysicsFunctions( ScriptDataHolder& dataHolder )
{
	taxiScript::pScriptPhysData = &dataHolder;

	script::LuaScripting::GetInstance()->RegisterFunction( "CreateBody",			ScriptCreateBody );
	script::LuaScripting::GetInstance()->RegisterFunction( "GetBodyWorldCenter",	ScriptGetBodyWorldCenter );

	script::LuaScripting::GetInstance()->RegisterFunction( "CreateBoxShape",		ScriptCreateBoxShape );
	script::LuaScripting::GetInstance()->RegisterFunction( "CreateCircleShape",	ScriptCreateCircleShape );
	script::LuaScripting::GetInstance()->RegisterFunction( "CreatePolygonShape",	ScriptCreatePolygonShape );
	
	script::LuaScripting::GetInstance()->RegisterFunction( "DestroyShape",			ScriptDestroyShape );
	
	script::LuaScripting::GetInstance()->RegisterFunction( "CreateDistanceJoint",		ScriptCreateDistanceJoint );
	script::LuaScripting::GetInstance()->RegisterFunction( "CreateGearJoint",			ScriptCreateGearJoint );
	script::LuaScripting::GetInstance()->RegisterFunction( "CreateMouseJoint",			ScriptCreateMouseJoint );
	script::LuaScripting::GetInstance()->RegisterFunction( "CreatePrismaticJoint",		ScriptCreatePrismaticJoint );
	script::LuaScripting::GetInstance()->RegisterFunction( "CreatePulleyJoint",		ScriptCreatePulleyJoint );
	script::LuaScripting::GetInstance()->RegisterFunction( "CreateRevoluteJoint",		ScriptCreateRevoluteJoint );
	script::LuaScripting::GetInstance()->RegisterFunction( "DestroyJoint",				ScriptDestroyJoint );
	
	script::LuaScripting::GetInstance()->RegisterFunction( "SetMassFromShapes",	ScriptSetMassFromShapes );
	script::LuaScripting::GetInstance()->RegisterFunction( "SetMeshToBody",		ScriptSetMeshToBody );

	script::LuaScripting::GetInstance()->RegisterFunction( "SetBodyXForm",		ScriptSetBodyXForm );
	script::LuaScripting::GetInstance()->RegisterFunction( "SetBodyMass",		ScriptSetBodyMass );
}

/////////////////////////////////////////////////////
/// Function: ScriptCreateBody
/// Params: [in]pState
///
/////////////////////////////////////////////////////
int ScriptCreateBody( lua_State* pState )
{		
	b2World* pWorld = PhysicsWorld::GetWorld();
	b2Body* pNewBody = 0;

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
		if( script::DoesTableItemExist( "isSleeping", 1, LUA_TBOOLEAN ) )
			bd.isSleeping		= script::LuaGetBoolFromTableItem( "isSleeping", 1, false ); 
		if( script::DoesTableItemExist( "fixedRotation", 1, LUA_TBOOLEAN ) )
			bd.fixedRotation	= script::LuaGetBoolFromTableItem( "fixedRotation", 1, false );
		if( script::DoesTableItemExist( "isBullet", 1, LUA_TBOOLEAN ) )
			bd.isBullet			= script::LuaGetBoolFromTableItem( "isBullet", 1, false ); 
		if( script::DoesTableItemExist( "applyGravity", 1, LUA_TBOOLEAN ) )
			bd.applyGravity		= script::LuaGetBoolFromTableItem( "applyGravity", 1, true ); 
	}
		
	pNewBody = pWorld->CreateBody(&bd);
	DBG_ASSERT( pNewBody != 0 );

	lua_pushlightuserdata( pState, reinterpret_cast<void *>(pNewBody) );
	return(1);
}

/////////////////////////////////////////////////////
/// Function: ScriptGetBodyWorldCenter
/// Params: [in]pState
///
/////////////////////////////////////////////////////
int ScriptGetBodyWorldCenter( lua_State* pState )
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
int ScriptCreateBoxShape( lua_State* pState )
{
	b2PolygonDef boxShape;
	b2Body* pBody = 0;
	b2Shape* pShape = 0;

	if( lua_islightuserdata( pState, 1 ) )
	{
		pBody = reinterpret_cast<b2Body *>( lua_touserdata(pState,1) );
		DBG_ASSERT( pBody != 0 );

		if( lua_istable( pState, 2 ) )
		{
			if( script::DoesTableItemExist( "friction", 2, LUA_TNUMBER ) )
				boxShape.friction		= static_cast<float32>( script::LuaGetNumberFromTableItem( "friction", 2, 0.2 ) );
			if( script::DoesTableItemExist( "restitution", 2, LUA_TNUMBER ) )
				boxShape.restitution	= static_cast<float32>( script::LuaGetNumberFromTableItem( "restitution", 2, 0.0 ) );
			if( script::DoesTableItemExist( "density", 2, LUA_TNUMBER ) )
				boxShape.density		= static_cast<float32>( script::LuaGetNumberFromTableItem( "density", 2, 0.0 ) );
				
			b2Vec2 center( 0.0f, 0.0f );
			if( script::DoesTableItemExist( "centerX", 2, LUA_TNUMBER ) )
				center.x				= static_cast<float32>( script::LuaGetNumberFromTableItem( "centerX", 2 ) );
			if( script::DoesTableItemExist( "centerY", 2, LUA_TNUMBER ) )
				center.y				= static_cast<float32>( script::LuaGetNumberFromTableItem( "centerY", 2 ) );

			float32 rotation		= 0.0f;
			if( script::DoesTableItemExist( "rotation", 2, LUA_TNUMBER ) )
				rotation				= static_cast<float32>( script::LuaGetNumberFromTableItem( "rotation", 2 ) );
			float32 hx				= 1.0f;
			if( script::DoesTableItemExist( "hx", 2, LUA_TNUMBER ) )
				hx = static_cast<float32>( script::LuaGetNumberFromTableItem( "hx", 2 ) );
			float32 hy				= 1.0f;
			if( script::DoesTableItemExist( "hy", 2, LUA_TNUMBER ) )
				hy = static_cast<float32>( script::LuaGetNumberFromTableItem( "hy", 2 ) );

			boxShape.SetAsBox( hx, hy, center, rotation );
		}			
		
		pShape = pBody->CreateShape( &boxShape );
		DBG_ASSERT( pShape != 0 );
	}
		
	lua_pushlightuserdata( pState, reinterpret_cast<void *>(pShape) );
	return(1);
}

/////////////////////////////////////////////////////
/// Function: ScriptCreateCircleShape
/// Params: [in]pState
///
/////////////////////////////////////////////////////
int ScriptCreateCircleShape( lua_State* pState )
{
	b2CircleDef	circleShape;
	b2Body* pBody = 0;
	b2Shape* pShape = 0;

	if( lua_islightuserdata( pState, 1 ) )
	{
		pBody = reinterpret_cast<b2Body *>( lua_touserdata(pState,1) );
		DBG_ASSERT( pBody != 0 );

		if( lua_istable( pState, 2 ) )
		{
			if( script::DoesTableItemExist( "friction", 2, LUA_TNUMBER ) )
				circleShape.friction		= static_cast<float32>( script::LuaGetNumberFromTableItem( "friction", 2, 0.2 ) );
			if( script::DoesTableItemExist( "restitution", 2, LUA_TNUMBER ) )
				circleShape.restitution		= static_cast<float32>( script::LuaGetNumberFromTableItem( "restitution", 2, 0.0 ) );
			if( script::DoesTableItemExist( "density", 2, LUA_TNUMBER ) )
				circleShape.density			= static_cast<float32>( script::LuaGetNumberFromTableItem( "density", 2, 0.0 ) );

			if( script::DoesTableItemExist( "localPositionX", 2, LUA_TNUMBER ) )
				circleShape.localPosition.x = static_cast<float32>( script::LuaGetNumberFromTableItem( "localPositionX", 2, 0.0 ) );
			if( script::DoesTableItemExist( "localPositionY", 2, LUA_TNUMBER ) )
				circleShape.localPosition.y = static_cast<float32>( script::LuaGetNumberFromTableItem( "localPositionY", 2, 0.0 ) );

			if( script::DoesTableItemExist( "radius", 2, LUA_TNUMBER ) )
				circleShape.radius			= static_cast<float32>( script::LuaGetNumberFromTableItem( "radius", 2, 1.0 ) );
		}			
		
		pShape = pBody->CreateShape( &circleShape );
		DBG_ASSERT( pShape != 0 );
	}

	lua_pushlightuserdata( pState, reinterpret_cast<void *>(pShape) );
	return(1);
}

/////////////////////////////////////////////////////
/// Function: ScriptCreatePolygonShape
/// Params: [in]pState
///
/////////////////////////////////////////////////////
int ScriptCreatePolygonShape( lua_State* pState )
{
	int i=0, j=0, k=0, componentIndex=0, vertIndex=0;
	b2PolygonDef polyShape;
	b2Body* pBody = 0;
	b2Shape* pShape = 0;

	if( lua_islightuserdata( pState, 1 ) )
	{
		pBody = reinterpret_cast<b2Body *>( lua_touserdata(pState,1) );
		DBG_ASSERT( pBody != 0 );

		if( lua_istable( pState, 2 ) )
		{
			if( script::DoesTableItemExist( "friction", 2, LUA_TNUMBER ) )
				polyShape.friction		= static_cast<float32>( script::LuaGetNumberFromTableItem( "friction", 2, 0.2 ) );
			if( script::DoesTableItemExist( "restitution", 2, LUA_TNUMBER ) )
				polyShape.restitution	= static_cast<float32>( script::LuaGetNumberFromTableItem( "restitution", 2, 0.0 ) );
			if( script::DoesTableItemExist( "density", 2, LUA_TNUMBER ) )
				polyShape.density		= static_cast<float32>( script::LuaGetNumberFromTableItem( "density", 2, 0.0 ) );

			int triCount				= 0;
			if( script::DoesTableItemExist( "tris", 2, LUA_TNUMBER ) )
				triCount = static_cast<int>( script::LuaGetNumberFromTableItem( "tris", 2 ) );

			int meshPolyListCount		= 0;
			if( script::DoesTableItemExist( "meshPolyListCount", 2, LUA_TNUMBER ) )
				meshPolyListCount = static_cast<int>( script::LuaGetNumberFromTableItem( "meshPolyListCount", 2 ) );

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

							polyShape.vertexCount = polyVertCount;

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
											polyShape.vertices[vertIndex].x = static_cast<float32>( lua_tonumber( pState, -1 ) );
										else
										{
											polyShape.vertices[vertIndex].y = static_cast<float32>( lua_tonumber( pState, -1 ) );

											componentIndex = -1;
											vertIndex++;
										}

										lua_pop( pState, 1 );
									}

									// poly complete
									pShape = pBody->CreateShape( &polyShape );
									DBG_ASSERT( pShape != 0 );
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

/*			{
				// grab table
				lua_pushstring( pState, "p" );
				lua_gettable( pState, 2 ); // key

				// new table is top
				if( lua_istable( pState, -1 ) )
				{
					int pointCount = luaL_getn( pState, -1 );

					polyShape.vertexCount = 3;

					int vertIndex = 0;

					for( i=1, componentIndex=0, triPointIndex=0; i <= pointCount; ++i, ++componentIndex, ++triPointIndex )
					{
						lua_rawgeti( pState, -1, i );

						if( componentIndex == 0 )
							polyShape.vertices[vertIndex].x = static_cast<float32>( lua_tonumber( pState, -1 ) );
						else
						{
							polyShape.vertices[vertIndex].y = static_cast<float32>( lua_tonumber( pState, -1 ) );

							componentIndex = -1;
							vertIndex++;
						}

						// end of a triangle
						if( triPointIndex == 5 )
						{
							pShape = pBody->CreateShape( &polyShape );
							DBG_ASSERT( pShape != 0 );

							vertIndex = 0;
							triPointIndex = -1;
						}

						lua_pop( pState, 1 );
					}
				}
				lua_pop( pState, 1 );
			}
*/			
		}			
		
	}
		
	//lua_pushlightuserdata( pState, reinterpret_cast<void *>(pShape) );
	return(0);
}

/////////////////////////////////////////////////////
/// Function: ScriptDestroyShape
/// Params: [in]pState
///
/////////////////////////////////////////////////////
int ScriptDestroyShape( lua_State* pState )
{
	b2Body* pBody = 0;
	b2Shape* pShape = 0;

	if( lua_islightuserdata( pState, 1 ) &&
		lua_islightuserdata( pState, 2 ) )
	{
		pBody = reinterpret_cast<b2Body *>( lua_touserdata(pState,1) );
		DBG_ASSERT( pBody != 0 );
		pShape = reinterpret_cast<b2Shape *>( lua_touserdata(pState,2) );
		DBG_ASSERT( pShape != 0 );

		pBody->DestroyShape(pShape);
	}

	return(0);
}

/////////////////////////////////////////////////////
/// Function: ScriptCreateDistanceJoint
/// Params: [in]pState
///
/////////////////////////////////////////////////////
int ScriptCreateDistanceJoint( lua_State* pState )
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
				jd.localAnchor1.x		= static_cast<float32>( script::LuaGetNumberFromTableItem( "localAnchor1X", 7, 0.0 ) );
			if( script::DoesTableItemExist( "localAnchor1Y", 7, LUA_TNUMBER ) )
				jd.localAnchor1.y		= static_cast<float32>( script::LuaGetNumberFromTableItem( "localAnchor1Y", 7, 0.0 ) );
			if( script::DoesTableItemExist( "localAnchor2X", 7, LUA_TNUMBER ) )
				jd.localAnchor2.x		= static_cast<float32>( script::LuaGetNumberFromTableItem( "localAnchor2X", 7, 0.0 ) );
			if( script::DoesTableItemExist( "localAnchor2Y", 7, LUA_TNUMBER ) )
				jd.localAnchor2.y		= static_cast<float32>( script::LuaGetNumberFromTableItem( "localAnchor2Y", 7, 0.0 ) );
			if( script::DoesTableItemExist( "length", 7, LUA_TNUMBER ) )
				jd.length				= static_cast<float32>( script::LuaGetNumberFromTableItem( "length", 7, 1.0 ) );
			if( script::DoesTableItemExist( "frequencyHz", 7, LUA_TNUMBER ) )
				jd.frequencyHz			= static_cast<float32>( script::LuaGetNumberFromTableItem( "frequencyHz", 7, 0.0 ) );
			if( script::DoesTableItemExist( "dampingRatio", 7, LUA_TNUMBER ) )
				jd.dampingRatio			= static_cast<float32>( script::LuaGetNumberFromTableItem( "dampingRatio", 7, 0.0 ) );
		}

		pJoint = reinterpret_cast<b2DistanceJoint *>( PhysicsWorld::GetWorld()->CreateJoint( &jd ) );
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
int ScriptCreateGearJoint( lua_State* pState )
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

		pJoint = reinterpret_cast<b2GearJoint *>( PhysicsWorld::GetWorld()->CreateJoint( &jd ) );
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
int ScriptCreateMouseJoint( lua_State* pState )
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
		if( script::DoesTableItemExist( "timeStep", 1, LUA_TNUMBER ) )
			jd.timeStep		= static_cast<float32>( script::LuaGetNumberFromTableItem( "timeStep", 1, 1.0/60.0 ) );
		
		pJoint = reinterpret_cast<b2MouseJoint *>( PhysicsWorld::GetWorld()->CreateJoint( &jd ) );
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
int ScriptCreatePrismaticJoint( lua_State* pState )
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
				jd.localAnchor1.x		= static_cast<float32>( script::LuaGetNumberFromTableItem( "localAnchor1X", 7, 0.0 ) );
			if( script::DoesTableItemExist( "localAnchor1Y", 7, LUA_TNUMBER ) )
				jd.localAnchor1.y		= static_cast<float32>( script::LuaGetNumberFromTableItem( "localAnchor1Y", 7, 0.0 ) );
			if( script::DoesTableItemExist( "localAnchor2X", 7, LUA_TNUMBER ) )
				jd.localAnchor2.x		= static_cast<float32>( script::LuaGetNumberFromTableItem( "localAnchor2X", 7, 0.0 ) );
			if( script::DoesTableItemExist( "localAnchor2Y", 7, LUA_TNUMBER ) )
				jd.localAnchor2.y		= static_cast<float32>( script::LuaGetNumberFromTableItem( "localAnchor2Y", 7, 0.0 ) );

			if( script::DoesTableItemExist( "localAxisAX", 7, LUA_TNUMBER ) )
				jd.localAxis1.x			= static_cast<float32>( script::LuaGetNumberFromTableItem( "localAxisAX", 7, 1.0 ) );
			if( script::DoesTableItemExist( "localAxisAY", 7, LUA_TNUMBER ) )
				jd.localAxis1.y			= static_cast<float32>( script::LuaGetNumberFromTableItem( "localAxisAY", 7, 0.0 ) );
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

		pJoint = reinterpret_cast<b2PrismaticJoint *>( PhysicsWorld::GetWorld()->CreateJoint( &jd ) );
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
int ScriptCreatePulleyJoint( lua_State* pState )
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
				jd.groundAnchor1.x		= static_cast<float32>( script::LuaGetNumberFromTableItem( "groundAnchor1X", 12, -1.0 ) );
			if( script::DoesTableItemExist( "groundAnchor1Y", 12, LUA_TNUMBER ) )
				jd.groundAnchor1.y		= static_cast<float32>( script::LuaGetNumberFromTableItem( "groundAnchor1Y", 12, 1.0 ) );
			if( script::DoesTableItemExist( "groundAnchor2X", 12, LUA_TNUMBER ) )
				jd.groundAnchor2.x		= static_cast<float32>( script::LuaGetNumberFromTableItem( "groundAnchor2X", 12, 1.0 ) );
			if( script::DoesTableItemExist( "groundAnchor2Y", 12, LUA_TNUMBER ) )
				jd.groundAnchor2.y		= static_cast<float32>( script::LuaGetNumberFromTableItem( "groundAnchor2Y", 12, 1.0 ) );

			if( script::DoesTableItemExist( "localAnchor1X", 12, LUA_TNUMBER ) )
				jd.localAnchor1.x		= static_cast<float32>( script::LuaGetNumberFromTableItem( "localAnchor1X", 12, -1.0 ) );
			if( script::DoesTableItemExist( "localAnchor1Y", 12, LUA_TNUMBER ) )
				jd.localAnchor1.y		= static_cast<float32>( script::LuaGetNumberFromTableItem( "localAnchor1Y", 12, 0.0 ) );
			if( script::DoesTableItemExist( "localAnchor2X", 12, LUA_TNUMBER ) )
				jd.localAnchor2.x		= static_cast<float32>( script::LuaGetNumberFromTableItem( "localAnchor2X", 12, 1.0 ) );
			if( script::DoesTableItemExist( "localAnchor2Y", 12, LUA_TNUMBER ) )
				jd.localAnchor2.y		= static_cast<float32>( script::LuaGetNumberFromTableItem( "localAnchor2Y", 12, 0.0 ) );

			if( script::DoesTableItemExist( "length1", 12, LUA_TNUMBER ) )
				jd.length1				= static_cast<float32>( script::LuaGetNumberFromTableItem( "length1", 12, 0.0 ) );
			if( script::DoesTableItemExist( "maxLength1", 12, LUA_TNUMBER ) )
				jd.maxLength1			= static_cast<float32>( script::LuaGetNumberFromTableItem( "maxLength1", 12, 0.0 ) );
			if( script::DoesTableItemExist( "length2", 12, LUA_TNUMBER ) )
				jd.length2				= static_cast<float32>( script::LuaGetNumberFromTableItem( "length2", 12, 0.0 ) );
			if( script::DoesTableItemExist( "maxLength2", 12, LUA_TNUMBER ) )
				jd.maxLength2			= static_cast<float32>( script::LuaGetNumberFromTableItem( "maxLength2", 12, 0.0 ) );
			if( script::DoesTableItemExist( "ratio", 12, LUA_TNUMBER ) )
				jd.ratio				= static_cast<float32>( script::LuaGetNumberFromTableItem( "ratio", 12, 1.0 ) );

			if( script::DoesTableItemExist( "collideConnected", 12, LUA_TBOOLEAN ) )
				jd.collideConnected		= script::LuaGetBoolFromTableItem( "collideConnected", 12, true );
		}

		pJoint = reinterpret_cast<b2PulleyJoint *>( PhysicsWorld::GetWorld()->CreateJoint( &jd ) );
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
int ScriptCreateRevoluteJoint( lua_State* pState )
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
				jd.localAnchor1.x		= static_cast<float32>( script::LuaGetNumberFromTableItem( "localAnchor1X", 5, 0.0 ) );
			if( script::DoesTableItemExist( "localAnchor1Y", 5, LUA_TNUMBER ) )
				jd.localAnchor1.y		= static_cast<float32>( script::LuaGetNumberFromTableItem( "localAnchor1Y", 5, 0.0 ) );
			if( script::DoesTableItemExist( "localAnchor2Y", 5, LUA_TNUMBER ) )
				jd.localAnchor2.x		= static_cast<float32>( script::LuaGetNumberFromTableItem( "localAnchor2X", 5, 0.0 ) );
			if( script::DoesTableItemExist( "localAnchor2Y", 5, LUA_TNUMBER ) )
				jd.localAnchor2.y		= static_cast<float32>( script::LuaGetNumberFromTableItem( "localAnchor2Y", 5, 0.0 ) );

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

		pJoint = reinterpret_cast<b2RevoluteJoint *>( PhysicsWorld::GetWorld()->CreateJoint( &jd ) );
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
int ScriptDestroyJoint( lua_State* pState )
{
	b2Joint* pJoint = 0;

	if( lua_islightuserdata( pState, 1 ) )
	{
		pJoint = reinterpret_cast<b2Joint *>( lua_touserdata(pState,1) );
		DBG_ASSERT( pJoint != 0 );

		PhysicsWorld::GetWorld()->DestroyJoint(pJoint);
	}

	return(0);
	
}

/////////////////////////////////////////////////////
/// Function: ScriptSetMassFromShapes
/// Params: [in]pState
///
/////////////////////////////////////////////////////
int ScriptSetMassFromShapes( lua_State* pState )
{
	b2Body* pBody = 0;

	if( lua_islightuserdata( pState, 1 ) )
	{
		pBody = reinterpret_cast<b2Body *>( lua_touserdata(pState,1) );
		DBG_ASSERT( pBody != 0 );

		pBody->SetMassFromShapes();
	}

	return(0);
}

/////////////////////////////////////////////////////
/// Function: ScriptSetMeshToBody
/// Params: [in]pState
///
/////////////////////////////////////////////////////
int ScriptSetMeshToBody( lua_State* pState )
{
/*	unsigned int i=0; 
	b2Body* pBody = 0;
	const char* meshName = 0;

	if( lua_islightuserdata( pState, 1 ) )
	{
		pBody = reinterpret_cast<b2Body *>( lua_touserdata(pState,1) );
		DBG_ASSERT( pBody != 0 );

		meshName = lua_tostring(pState,2);
		DBG_ASSERT( meshName != 0 );

		std::vector<PhysicsWorld::PhysicsObject>& objList = taxiScript::pScriptPhysData->GetObjectList();

		for( i=0; i < objList.size(); ++i )
		{
			if( objList[i].pBody == pBody )
			{
				mdl::ModelHGM* pModel = 0;
				pModel = new mdl::ModelHGM;
				DBG_ASSERT( pModel != 0 );

				pModel->Load( meshName );
				pModel->SetModelGeneralFlags( mdl::MODELFLAG_GENERAL_AUTOCULLING | mdl::MODELFLAG_GENERAL_SPHERECULL | mdl::MODELFLAG_GENERAL_MODELCULL );

				objList[i].pModel = pModel;
			}
		}
	}
*/
	DBG_ASSERT(0);
	return(0);
}

/////////////////////////////////////////////////////
/// Function: ScriptSetBodyXForm
/// Params: [in]pState
///
/////////////////////////////////////////////////////
int ScriptSetBodyXForm( lua_State* pState )
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

		pBody->SetXForm( pos, angle );
	}

	return(0);
}

/////////////////////////////////////////////////////
/// Function: ScriptSetBodyMass
/// Params: [in]pState
///
/////////////////////////////////////////////////////
int ScriptSetBodyMass( lua_State* pState )
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

		pBody->SetMass( &massData );
	}

	return(0);
}