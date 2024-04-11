
/*===================================================================
	File: PhysicsWorld.cpp
	Library: Physics (BOX2D)

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"
#include "MathBase.h"

#include "RenderBase.h"
#include "PhysicsBase.h"

#ifdef BASE_SUPPORT_BOX2D

b2World* physics::PhysicsWorldB2D::ms_World = 0;
physics::Box2DDebugDraw physics::PhysicsWorldB2D::ms_DebugDraw;

namespace physics
{
	b2AABB WorldAABB;
	b2Vec2 Gravity;
}

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
physics::PhysicsWorldB2D::PhysicsWorldB2D()
{

}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
physics::PhysicsWorldB2D::~PhysicsWorldB2D()
{

}

/////////////////////////////////////////////////////
/// Static Method: Create
/// Params: [in]minBounds, [in]maxBounds, [in]gravity, [in]enableDebugDraw
///
/////////////////////////////////////////////////////
void physics::PhysicsWorldB2D::Create( b2Vec2& minBounds, b2Vec2& maxBounds, b2Vec2& gravity, bool enableDebugDraw )
{
	WorldAABB.lowerBound = minBounds;
	WorldAABB.upperBound = maxBounds;

	Gravity = gravity;
	//bool doSleep = true;

	physics::PhysicsWorldB2D::ms_World = new b2World( Gravity );
	DBG_ASSERT( physics::PhysicsWorldB2D::ms_World != 0 );

	if( enableDebugDraw )
	{
		physics::PhysicsWorldB2D::ms_World->SetDebugDraw(&ms_DebugDraw);

		unsigned int flags = 0;
		flags += b2Draw::e_shapeBit;
		flags += b2Draw::e_centerOfMassBit;
		flags += b2Draw::e_jointBit;
		//flags += b2Draw::e_aabbBit;
		ms_DebugDraw.SetFlags(flags);
	}

	//physics::PhysicsWorld::ms_World->SetContactListener(&ms_ContactListener);
}

/////////////////////////////////////////////////////
/// Static  Method: Destroy
/// Params: None
///
/////////////////////////////////////////////////////
void physics::PhysicsWorldB2D::Destroy()
{	
	if( physics::PhysicsWorldB2D::ms_World != 0 )
	{
		delete physics::PhysicsWorldB2D::ms_World;
		physics::PhysicsWorldB2D::ms_World = 0;
	}
}

/////////////////////////////////////////////////////
/// Static Method: DrawDebugData
/// Params: None
///
/////////////////////////////////////////////////////
void physics::PhysicsWorldB2D::DrawDebugData()
{
	if( physics::PhysicsWorldB2D::ms_World )
	{
		physics::Box2DPreDebugDraw();

		physics::PhysicsWorldB2D::ms_World->DrawDebugData();

		physics::Box2DPostDebugDraw();
	}
}

/////////////////////////////////////////////////////
/// Static Method: GetWorld
/// Params: None
///
/////////////////////////////////////////////////////
b2World* physics::PhysicsWorldB2D::GetWorld()
{
	return physics::PhysicsWorldB2D::ms_World;
}

/////////////////////////////////////////////////////
/// Static Method: GetDrawer
/// Params: None
///
/////////////////////////////////////////////////////
physics::Box2DDebugDraw* physics::PhysicsWorldB2D::GetDrawer()
{
	return &physics::PhysicsWorldB2D::ms_DebugDraw;
}

#endif // BASE_SUPPORT_BOX2D

