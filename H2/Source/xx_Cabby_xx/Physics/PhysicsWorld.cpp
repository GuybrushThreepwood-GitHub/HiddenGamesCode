
/*===================================================================
	File: PhysicsWorld.cpp
	Game: Cabby

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"
#include "MathBase.h"
#include "RenderBase.h"
#include "CollisionBase.h"
#include "Model/ModelCommon.h"
#include "ModelBase.h"

#include "Box2D.h"
#include "Box2DDebugDraw.h"

#include "Level/Level.h"
#include "Player/Player.h"
#include "PhysicsWorld.h"

#include "Physics/PhysicsIds.h"

b2World* PhysicsWorld::ms_World = 0;
Box2DDebugDraw PhysicsWorld::ms_DebugDraw;
ContactListener PhysicsWorld::ms_ContactListener;
Player* PhysicsWorld::ms_Player;

b2Vec2 PhysicsWorld::ContactNormal = b2Vec2( 0.0f, 1.0f );

namespace
{
	b2AABB WorldAABB;
	b2Vec2 Gravity;
}

void ContactListener::Add(const b2ContactPoint* point)
{
	b2Body* pBody1 = 0;
	b2Body* pBody2 = 0;

	physics::PhysicsIdentifier* pData1 = 0;
	physics::PhysicsIdentifier* pData2 = 0;

	pBody1 = point->shape1->GetBody();
	pBody2 = point->shape2->GetBody();

	if( pBody1 != 0 )
		pData1 = reinterpret_cast<physics::PhysicsIdentifier*>( pBody1->GetUserData() );
	if( pBody2 != 0 )
		pData2 = reinterpret_cast<physics::PhysicsIdentifier*>( pBody2->GetUserData() );

	// all physics data should have the PhysicsIdentifier inherited
	DBG_ASSERT( pData1 != 0 );
	DBG_ASSERT( pData2 != 0 );

	Player* pPlayer = PhysicsWorld::GetPlayer();

	if( pData1->GetBaseId() == PHYSICSBASICID_PLAYER )
	{
		// data 1 is the player
		PhysicsWorld::ContactNormal = point->normal;

		pPlayer->HandleContact( pData2->GetBaseId(), pData2->GetCastingId(), point, point->shape2, pBody2 );
	}
	else 
	if( pData2->GetBaseId() == PHYSICSBASICID_PLAYER )
	{
		// data 2 is the player
		PhysicsWorld::ContactNormal = point->normal;

		pPlayer->HandleContact( pData1->GetBaseId(), pData1->GetCastingId(), point, point->shape1, pBody1 );
		
	}
}

void ContactListener::Persist(const b2ContactPoint* point)
{
	b2Body* pBody1 = 0;
	b2Body* pBody2 = 0;

	physics::PhysicsIdentifier* pData1 = 0;
	physics::PhysicsIdentifier* pData2 = 0;

	pBody1 = point->shape1->GetBody();
	pBody2 = point->shape2->GetBody();

	if( pBody1 != 0 )
		pData1 = reinterpret_cast<physics::PhysicsIdentifier*>( pBody1->GetUserData() );
	if( pBody2 != 0 )
		pData2 = reinterpret_cast<physics::PhysicsIdentifier*>( pBody2->GetUserData() );

	// all physics data should have the PhysicsIdentifier inherited
	DBG_ASSERT( pData1 != 0 );
	DBG_ASSERT( pData2 != 0 );

	Player* pPlayer = PhysicsWorld::GetPlayer();

	if( pData1->GetBaseId() == PHYSICSBASICID_PLAYER )
	{
		PhysicsWorld::ContactNormal = point->normal;

		// data 1 is the player
		pPlayer->HandlePersistantContact( pData2->GetBaseId(), pData2->GetCastingId(), point, point->shape2, pBody2 );
	}
	else 
	if( pData2->GetBaseId() == PHYSICSBASICID_PLAYER )
	{
		PhysicsWorld::ContactNormal = point->normal;

		// data 2 is the player
		pPlayer->HandlePersistantContact( pData1->GetBaseId(), pData1->GetCastingId(), point, point->shape1, pBody1 );
	}
}

void ContactListener::Remove(const b2ContactPoint* point)
{
	b2Body* pBody1 = 0;
	b2Body* pBody2 = 0;

	physics::PhysicsIdentifier* pData1 = 0;
	physics::PhysicsIdentifier* pData2 = 0;

	pBody1 = point->shape1->GetBody();
	pBody2 = point->shape2->GetBody();

	if( pBody1 != 0 )
		pData1 = reinterpret_cast<physics::PhysicsIdentifier*>( pBody1->GetUserData() );
	if( pBody2 != 0 )
		pData2 = reinterpret_cast<physics::PhysicsIdentifier*>( pBody2->GetUserData() );

	// all physics data should have the PhysicsIdentifier inherited
	DBG_ASSERT( pData1 != 0 );
	DBG_ASSERT( pData2 != 0 );

	Player* pPlayer = PhysicsWorld::GetPlayer();

	if( pData1->GetBaseId() == PHYSICSBASICID_PLAYER )
	{		
		PhysicsWorld::ContactNormal = b2Vec2( 0.0f, 0.0f );

		pPlayer->SetContactState( false );

		// data 1 is the player
		pPlayer->ClearContact( pData2->GetBaseId() );
	}
	else 
	if( pData2->GetBaseId() == PHYSICSBASICID_PLAYER )
	{
		PhysicsWorld::ContactNormal = b2Vec2( 0.0f, 0.0f );

		pPlayer->SetContactState( false );

		// data 2 is the player
		pPlayer->ClearContact( pData1->GetBaseId() );
	}

}

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
PhysicsWorld::PhysicsWorld()
{

}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
PhysicsWorld::~PhysicsWorld()
{

}

/////////////////////////////////////////////////////
/// Static Method: Create
/// Params: [in]minBounds, [in]maxBounds, [in]gravity, [in]enableDebugDraw
///
/////////////////////////////////////////////////////
void PhysicsWorld::Create( b2Vec2& minBounds, b2Vec2& maxBounds, b2Vec2& gravity, bool enableDebugDraw )
{
	WorldAABB.lowerBound = minBounds;
	WorldAABB.upperBound = maxBounds;

	Gravity = gravity;
	bool doSleep = true;

	PhysicsWorld::ms_World = new b2World( WorldAABB, Gravity, doSleep );
	DBG_ASSERT( PhysicsWorld::ms_World != 0 );

	if( enableDebugDraw )
	{
		PhysicsWorld::ms_World->SetDebugDraw(&ms_DebugDraw);

		unsigned int flags = 0;
		flags += b2DebugDraw::e_shapeBit;
		flags += b2DebugDraw::e_centerOfMassBit;
		flags += b2DebugDraw::e_jointBit;
		//flags += b2DebugDraw::e_aabbBit;
		ms_DebugDraw.SetFlags(flags);
	}

	PhysicsWorld::ms_World->SetContactListener(&ms_ContactListener);
}

/////////////////////////////////////////////////////
/// Static  Method: Destroy
/// Params: None
///
/////////////////////////////////////////////////////
void PhysicsWorld::Destroy()
{	
	if( PhysicsWorld::ms_World != 0 )
	{
		delete PhysicsWorld::ms_World;
		PhysicsWorld::ms_World = 0;
	}
}

/////////////////////////////////////////////////////
/// Static Method: GetWorld
/// Params: None
///
/////////////////////////////////////////////////////
b2World* PhysicsWorld::GetWorld()
{
	return PhysicsWorld::ms_World;
}
