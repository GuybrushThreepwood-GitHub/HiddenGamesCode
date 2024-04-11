
/*===================================================================
	File: PhysicsContact.cpp
	Game: H4

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"
#include "MathBase.h"
#include "RenderBase.h"
#include "CollisionBase.h"
#include "ModelBase.h"
#include "PhysicsBase.h"

#include "AppConsts.h"

#include "Physics/PhysicsIds.h"
#include "Player/Player.h"
#include "Enemy/Enemy.h"

#include "GameSystems.h"

#include "PhysicsContact.h"

/////////////////////////////////////////////////////
/// Method: BeginContact
/// Params: [in]point
///
/////////////////////////////////////////////////////
void ContactListener::BeginContact(b2Contact* contact)
{
	const b2Body* pBody1 = 0;
	const b2Body* pBody2 = 0;

	physics::PhysicsIdentifier* pData1 = 0;
	physics::PhysicsIdentifier* pData2 = 0;

	pBody1 = contact->GetFixtureA()->GetBody();
	pBody2 = contact->GetFixtureB()->GetBody();

	if( pBody1 != 0 )
		pData1 = reinterpret_cast<physics::PhysicsIdentifier*>( pBody1->GetUserData() );
	if( pBody2 != 0 )
		pData2 = reinterpret_cast<physics::PhysicsIdentifier*>( pBody2->GetUserData() );

	// all physics data should have the PhysicsIdentifier inherited
	DBG_ASSERT_MSG( (pData1 != 0), "Physics contact Body1 has no PhysicsIdentifier" );
	DBG_ASSERT_MSG( (pData2 != 0), "Physics contact Body2 has no PhysicsIdentifier" );

	m_PointCount = 0;

	const b2Manifold* manifold = contact->GetManifold();

	if (manifold->pointCount == 0)
	{
		return;
	}

	b2Fixture* fixtureA = contact->GetFixtureA();
	b2Fixture* fixtureB = contact->GetFixtureB();

	//b2PointState state1[b2_maxManifoldPoints], state2[b2_maxManifoldPoints];
	//b2GetPointStates(state1, state2, oldManifold, manifold);

	b2WorldManifold worldManifold;
	contact->GetWorldManifold(&worldManifold);

	for (int32 i = 0; i < manifold->pointCount && m_PointCount < MAX_CONTACT_POINTS; ++i)
	{
		ContactPoint* cp = m_Points + m_PointCount;
		cp->fixtureA = fixtureA;
		cp->fixtureB = fixtureB;
		cp->position = worldManifold.points[i];
		cp->normal = worldManifold.normal;
		//cp->state = state2[i];
		++m_PointCount;
	}

	Player* pPlayer = &GameSystems::GetInstance()->GetPlayer();

	// PLAYER CONTACT
	if( pData1->GetBaseId() == PHYSICSBASICID_PLAYER )
	{
		// data 1 is the player
		pPlayer->HandleContact( pData2->GetBaseId(), pData2->GetCastingId(), &m_Points[0], contact->GetFixtureB(), pBody2 );
	}
	else 
	if( pData2->GetBaseId() == PHYSICSBASICID_PLAYER )
	{
		// data 2 is the player
		pPlayer->HandleContact( pData1->GetBaseId(), pData1->GetCastingId(), &m_Points[0], contact->GetFixtureA(), pBody1 );
	}

	// BULLET CONTACT
	if( pData1->GetBaseId() == PHYSICSBASICID_BULLET )
	{
		Bullet* pBullet = 0;
		pBullet = reinterpret_cast<Bullet*>( pBody1->GetUserData() );
		DBG_ASSERT_MSG( (pBullet != 0), "Could not cast bullet class in physics contact" );

		// data 1 is the bullet
		pBullet->HandleContact( pData2->GetBaseId(), pData2->GetCastingId(), &m_Points[0], contact->GetFixtureB(), pBody2 );
	}
	else 
	if( pData2->GetBaseId() == PHYSICSBASICID_BULLET )
	{
		Bullet* pBullet = 0;
		pBullet = reinterpret_cast<Bullet*>( pBody2->GetUserData() );
		DBG_ASSERT_MSG( (pBullet != 0), "Could not cast bullet class in physics contact" );

		// data 2 is the bullet
		pBullet->HandleContact( pData1->GetBaseId(), pData1->GetCastingId(), &m_Points[0], contact->GetFixtureA(), pBody1 );
	}

	// ENEMY CONTACT
	if( pData1->GetBaseId() == PHYSICSBASICID_ENEMY )
	{
		Enemy* pEnemy = 0;
		pEnemy = reinterpret_cast<Enemy*>( pBody1->GetUserData() );
		DBG_ASSERT_MSG( (pEnemy != 0), "Could not cast enemy class in physics contact" );

		// data 1 is the enemy
		pEnemy->HandleContact( pData2->GetBaseId(), pData2->GetCastingId(), &m_Points[0], contact->GetFixtureB(), pBody2 );
	}
	else 
	if( pData2->GetBaseId() == PHYSICSBASICID_ENEMY )
	{
		Enemy* pEnemy = 0;
		pEnemy = reinterpret_cast<Enemy*>( pBody2->GetUserData() );
		DBG_ASSERT_MSG( (pEnemy != 0), "Could not cast enemy class in physics contact" );

		// data 2 is the enemy
		pEnemy->HandleContact( pData1->GetBaseId(), pData1->GetCastingId(), &m_Points[0], contact->GetFixtureA(), pBody1 );
	}

	// FLOATING HEAD CONTACT
	if( pData1->GetBaseId() == PHYSICSBASICID_FLOATINGHEAD )
	{
		FloatingHead* pHead = 0;
		pHead = reinterpret_cast<FloatingHead*>( pBody1->GetUserData() );
		DBG_ASSERT_MSG( (pHead != 0), "Could not cast floating head class in physics contact" );

		// data 1 is the enemy
		pHead->HandleContact( pData2->GetBaseId(), pData2->GetCastingId(), &m_Points[0], contact->GetFixtureB(), pBody2 );
	}
	else 
	if( pData2->GetBaseId() == PHYSICSBASICID_FLOATINGHEAD )
	{
		FloatingHead* pHead = 0;
		pHead = reinterpret_cast<FloatingHead*>( pBody2->GetUserData() );
		DBG_ASSERT_MSG( (pHead != 0), "Could not cast floating head class in physics contact" );

		// data 2 is the enemy
		pHead->HandleContact( pData1->GetBaseId(), pData1->GetCastingId(), &m_Points[0], contact->GetFixtureA(), pBody1 );
	}

	// CRAWLING SPIDER CONTACT
	if( pData1->GetBaseId() == PHYSICSBASICID_CRAWLINGSPIDER )
	{
		CrawlingSpider* pSpider = 0;
		pSpider = reinterpret_cast<CrawlingSpider*>( pBody1->GetUserData() );
		DBG_ASSERT_MSG( (pSpider != 0), "Could not cast crawling spider class in physics contact" );

		// data 1 is the enemy
		pSpider->HandleContact( pData2->GetBaseId(), pData2->GetCastingId(), &m_Points[0], contact->GetFixtureB(), pBody2 );
	}
	else 
	if( pData2->GetBaseId() == PHYSICSBASICID_CRAWLINGSPIDER )
	{
		CrawlingSpider* pSpider = 0;
		pSpider = reinterpret_cast<CrawlingSpider*>( pBody2->GetUserData() );
		DBG_ASSERT_MSG( (pSpider != 0), "Could not cast crawling spider class in physics contact" );

		// data 2 is the enemy
		pSpider->HandleContact( pData1->GetBaseId(), pData1->GetCastingId(), &m_Points[0], contact->GetFixtureA(), pBody1 );
	}

	// GIB CONTACT
/*	if( pData1->GetBaseId() == PHYSICSBASICID_GIB )
	{
		Gib* pGib = 0;
		pGib = reinterpret_cast<Gib*>( pBody1->GetUserData() );
		DBG_ASSERT_MSG( (pGib != 0), "Could not cast gib class in physics contact" );

		// data 1 is the gib
		pGib->HandleContact( pData2->GetBaseId(), pData2->GetCastingId(), &m_Points[0], contact->GetFixtureB(), pBody2 );
	}
	else */
	if( pData2->GetBaseId() == PHYSICSBASICID_GIB )
	{
		Gib* pGib = 0;
		pGib = reinterpret_cast<Gib*>( pBody2->GetUserData() );
		DBG_ASSERT_MSG( (pGib != 0), "Could not cast gib class in physics contact" );

		// data 2 is the gib
		pGib->HandleContact( pData1->GetBaseId(), pData1->GetCastingId(), &m_Points[0], contact->GetFixtureA(), pBody1 );
	}

}

/////////////////////////////////////////////////////
/// Method: EndContact
/// Params: [in]point
///
/////////////////////////////////////////////////////
void ContactListener::EndContact(b2Contact* contact)
{
	const b2Body* pBody1 = 0;
	const b2Body* pBody2 = 0;

	physics::PhysicsIdentifier* pData1 = 0;
	physics::PhysicsIdentifier* pData2 = 0;

	pBody1 = contact->GetFixtureA()->GetBody();
	pBody2 = contact->GetFixtureB()->GetBody();

	if( pBody1 != 0 )
		pData1 = reinterpret_cast<physics::PhysicsIdentifier*>( pBody1->GetUserData() );
	if( pBody2 != 0 )
		pData2 = reinterpret_cast<physics::PhysicsIdentifier*>( pBody2->GetUserData() );

	// all physics data should have the PhysicsIdentifier inherited
	DBG_ASSERT_MSG( (pData1 != 0), "Physics remove contact Body1 has no PhysicsIdentifier" );
	DBG_ASSERT_MSG( (pData2 != 0), "Physics remove contact Body2 has no PhysicsIdentifier" );

	Player* pPlayer = &GameSystems::GetInstance()->GetPlayer();

	if( pData1->GetBaseId() == PHYSICSBASICID_PLAYER )
	{
		// data 1 is the player
		pPlayer->ClearContact( pData2->GetBaseId(), pData2->GetCastingId(), 0, contact->GetFixtureB(), pBody2 );
	}
	else 
	if( pData2->GetBaseId() == PHYSICSBASICID_PLAYER )
	{
		// data 2 is the player
		pPlayer->ClearContact( pData1->GetBaseId(), pData1->GetCastingId(), 0, contact->GetFixtureA(), pBody1 );
	}

	// ENEMY CONTACT
	if( pData1->GetBaseId() == PHYSICSBASICID_ENEMY )
	{
		Enemy* pEnemy = 0;
		pEnemy = reinterpret_cast<Enemy*>( pBody1->GetUserData() );
		DBG_ASSERT_MSG( (pEnemy != 0), "Could not cast enemy class in physics remove contact" );

		// data 1 is the enemy
		pEnemy->ClearContact( pData2->GetBaseId(), pData2->GetCastingId(), 0, contact->GetFixtureB(), pBody2 );
	}
	else 
	if( pData2->GetBaseId() == PHYSICSBASICID_ENEMY )
	{
		Enemy* pEnemy = 0;
		pEnemy = reinterpret_cast<Enemy*>( pBody2->GetUserData() );
		DBG_ASSERT_MSG( (pEnemy != 0), "Could not cast enemy class in physics remove contact" );

		// data 2 is the enemy
		pEnemy->ClearContact( pData1->GetBaseId(), pData1->GetCastingId(), 0, contact->GetFixtureA(), pBody1 );
	}

	// FLOATING HEAD CONTACT
	if( pData1->GetBaseId() == PHYSICSBASICID_FLOATINGHEAD )
	{
		FloatingHead* pHead = 0;
		pHead = reinterpret_cast<FloatingHead*>( pBody1->GetUserData() );
		DBG_ASSERT_MSG( (pHead != 0), "Could not cast enemy class in physics remove contact" );

		// data 1 is the enemy
		pHead->ClearContact( pData2->GetBaseId(), pData2->GetCastingId(), 0, contact->GetFixtureB(), pBody2 );
	}
	else 
	if( pData2->GetBaseId() == PHYSICSBASICID_FLOATINGHEAD )
	{
		FloatingHead* pHead = 0;
		pHead = reinterpret_cast<FloatingHead*>( pBody2->GetUserData() );
		DBG_ASSERT_MSG( (pHead != 0), "Could not cast enemy class in physics remove contact" );

		// data 2 is the enemy
		pHead->ClearContact( pData1->GetBaseId(), pData1->GetCastingId(), 0, contact->GetFixtureA(), pBody1 );
	}

	// CRAWLING SPIDER CONTACT
	if( pData1->GetBaseId() == PHYSICSBASICID_CRAWLINGSPIDER )
	{
		CrawlingSpider* pSpider = 0;
		pSpider = reinterpret_cast<CrawlingSpider*>( pBody1->GetUserData() );
		DBG_ASSERT_MSG( (pSpider != 0), "Could not cast enemy class in physics remove contact" );

		// data 1 is the enemy
		pSpider->ClearContact( pData2->GetBaseId(), pData2->GetCastingId(), 0, contact->GetFixtureB(), pBody2 );
	}
	else 
	if( pData2->GetBaseId() == PHYSICSBASICID_CRAWLINGSPIDER )
	{
		CrawlingSpider* pSpider = 0;
		pSpider = reinterpret_cast<CrawlingSpider*>( pBody2->GetUserData() );
		DBG_ASSERT_MSG( (pSpider != 0), "Could not cast enemy class in physics remove contact" );

		// data 2 is the enemy
		pSpider->ClearContact( pData1->GetBaseId(), pData1->GetCastingId(), 0, contact->GetFixtureA(), pBody1 );
	}
}

/////////////////////////////////////////////////////
/// Method: PreSolve
/// Params: [in]contact, [in]oldManifold
///
/////////////////////////////////////////////////////
void ContactListener::PreSolve(b2Contact* contact, const b2Manifold* oldManifold)
{
/*	m_PointCount = 0;

	const b2Manifold* manifold = contact->GetManifold();

	if (manifold->pointCount == 0)
	{
		return;
	}

	b2Fixture* fixtureA = contact->GetFixtureA();
	b2Fixture* fixtureB = contact->GetFixtureB();

	b2PointState state1[b2_maxManifoldPoints], state2[b2_maxManifoldPoints];
	b2GetPointStates(state1, state2, oldManifold, manifold);

	b2WorldManifold worldManifold;
	contact->GetWorldManifold(&worldManifold);

	for (int32 i = 0; i < manifold->pointCount && m_PointCount < MAX_CONTACT_POINTS; ++i)
	{
		ContactPoint* cp = m_Points + m_PointCount;
		cp->fixtureA = fixtureA;
		cp->fixtureB = fixtureB;
		cp->position = worldManifold.points[i];
		cp->normal = worldManifold.normal;
		cp->state = state2[i];
		++m_PointCount;
	}
*/
}

/////////////////////////////////////////////////////
/// Method: PostSolve
/// Params: [in]contact, [in]impulse
///
/////////////////////////////////////////////////////
void ContactListener::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse)
{

}
