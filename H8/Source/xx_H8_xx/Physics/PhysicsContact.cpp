
/*===================================================================
	File: PhysicsContact.cpp
	Game: H8

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"
#include "MathBase.h"
#include "RenderBase.h"
#include "CollisionBase.h"
#include "ModelBase.h"
#include "PhysicsBase.h"

#include "Physics/PhysicsIds.h"
#include "Player/Player.h"
#include "Enemy/Enemy.h"
#include "Core/Core.h"

#include "GameSystems.h"

#include "PhysicsContact.h"

namespace
{
	math::Vec3 zeroVec(0.0f,0.0f,0.0f);

	std::vector<b2Fixture*> m_EnemyContacts;
}

///
bool EnemyContactHandled( b2Fixture* fixture );

/////////////////////////////////////////////////////
/// Method: ClearContacts
/// Params: None
///
/////////////////////////////////////////////////////
void ClearContacts()
{
	m_EnemyContacts.clear();
}

/////////////////////////////////////////////////////
/// Method: BeginContact
/// Params: [in]contact
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

	Enemy* pEnemy = 0;
	Player* pShield = 0;
	Core* pCore = 0;

	if( pData1->GetBaseId() == PHYSICSBASICID_ENEMY )
	{
		// get the enemy first
		pEnemy = reinterpret_cast<Enemy*>( pBody1->GetUserData() );
		
		if( pData2->GetBaseId() == PHYSICSBASICID_CORE )
		{
			pCore = reinterpret_cast<Core*>( pBody2->GetUserData() );
			HandleEnemyToCore( pEnemy, fixtureA, pCore, fixtureB, m_Points[0].position, contact );
		}
		else if( pData2->GetBaseId() == PHYSICSBASICID_SHIELD )
		{
			pShield = reinterpret_cast<Player*>( pBody2->GetUserData() );
			HandleEnemyToShield( pEnemy, fixtureA, pShield, fixtureB, m_Points[0].position, contact );
		}
		else
			DBG_ASSERT(0);
	}
	else 
	if( pData2->GetBaseId() == PHYSICSBASICID_ENEMY )
	{
		pEnemy = reinterpret_cast<Enemy*>( pBody2->GetUserData() );

		if( pData1->GetBaseId() == PHYSICSBASICID_CORE )
		{
			pCore = reinterpret_cast<Core*>( pBody1->GetUserData() );
			HandleEnemyToCore( pEnemy, fixtureB, pCore, fixtureA, m_Points[0].position, contact );
		}
		else if( pData1->GetBaseId() == PHYSICSBASICID_SHIELD )
		{
			pShield = reinterpret_cast<Player*>( pBody1->GetUserData() );
			HandleEnemyToShield( pEnemy, fixtureB, pShield, fixtureA, m_Points[0].position, contact );
		}
		else
			DBG_ASSERT(0);
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
}

/////////////////////////////////////////////////////
/// Method: PreSolve
/// Params: [in]contact, [in]oldManifold
///
/////////////////////////////////////////////////////
void ContactListener::PreSolve(b2Contact* contact, const b2Manifold* oldManifold)
{

}

/////////////////////////////////////////////////////
/// Method: PostSolve
/// Params: [in]contact, [in]impulse
///
/////////////////////////////////////////////////////
void ContactListener::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse)
{

}

/////////////////////////////////////////////////////
/// Method: HandleEnemyToShield
/// Params:
///
/////////////////////////////////////////////////////
void ContactListener::HandleEnemyToShield( Enemy* pEnemy, b2Fixture* enemyFixture, Player* pShield, b2Fixture* shieldFixture, b2Vec2 position, b2Contact* contact )
{
	DBG_ASSERT( pEnemy != 0 );
	DBG_ASSERT( pShield != 0 );

	// state already changed
	if( pEnemy->GetFlags() != 0 )
	{
		contact->SetEnabled(false);
		return;
	}

	physics::PhysicsIdentifier* pEnemyData = 0;
	physics::PhysicsIdentifier* pShieldData = 0;

	if( !EnemyContactHandled(enemyFixture) )
	{
		pEnemyData = reinterpret_cast<physics::PhysicsIdentifier*>( enemyFixture->GetUserData() );
		DBG_ASSERT( pEnemyData != 0 );

		pShieldData = reinterpret_cast<physics::PhysicsIdentifier*>( shieldFixture->GetUserData() );
		DBG_ASSERT( pShieldData != 0 );

		int enemyColourIndex = pEnemyData->GetNumeric1();
		int shieldColourIndex = pShieldData->GetNumeric2();

		if( enemyColourIndex != shieldColourIndex )
		{
			// badness - pass through
			pEnemy->SetFlags( Enemy::ENEMYFLAG_BAD_MATCH );
			pShield->PlayNegativeChime();
			contact->SetEnabled(false);

			// bonus lost
			pShield->ResetConsecutiveCount();

			// rebound
			/*pEnemy->SetFlags( Enemy::ENEMYFLAG_REBOUND_BAD );
			pEnemy->Rebound();

			EnemyToShieldCollision newCollision;
			newCollision.colourIndex = shieldColourIndex;
			newCollision.meshIndex = -9999;
			newCollision.whichFixture = shieldFixture;
			newCollision.timeDisabled = 1.0f;

			pShield->AddShieldCollision( newCollision );

			pShield->AddShieldDeflect( shieldColourIndex );

			pShield->PlayNegativeChime();

			// mismatch, set core to angry
			GameSystems::GetInstance()->GetCore()->SetPupilState( Core::PupilState_Angry_ShieldHit, 0.0f );*/
		}
		else
		{
			// goodness
			pEnemy->SetFlags( Enemy::ENEMYFLAG_REBOUND_GOOD );
			pEnemy->Rebound();

			pShield->AddShieldAbsorb( shieldColourIndex );

			pShield->PlayPositiveChime();

			GameSystems::GetInstance()->AddToScore( pEnemy->GetPoints() );

			GameSystems::GetInstance()->IncrementLevelUpCounter();

			pShield->IncreaseConsecutiveCount();

			//math::Vec3 col = GameSystems::GetInstance()->GetColourForIndex( enemyColourIndex );
			//GameSystems::GetInstance()->SpawnCircle( zeroVec, col );
		}
	}
	else
	{
		contact->SetEnabled(false);
	}
}

/////////////////////////////////////////////////////
/// Method: HandleEnemyToCore
/// Params: 
///
/////////////////////////////////////////////////////
void ContactListener::HandleEnemyToCore( Enemy* pEnemy, b2Fixture* enemyFixture, Core* pCore, b2Fixture* coreFixture, b2Vec2 position, b2Contact* contact )
{
	DBG_ASSERT( pEnemy != 0 );
	DBG_ASSERT( pCore != 0 );

	physics::PhysicsIdentifier* pEnemyData = 0;
	physics::PhysicsIdentifier* pCoreData = 0;

	pEnemyData = reinterpret_cast<physics::PhysicsIdentifier*>( enemyFixture->GetUserData() );
	DBG_ASSERT( pEnemyData != 0 );

	pCoreData = reinterpret_cast<physics::PhysicsIdentifier*>( coreFixture->GetUserData() );
	DBG_ASSERT( pCore != 0 );

	// badness
	pEnemy->SetFlags( Enemy::ENEMYFLAG_DESTROY );
	pCore->TakeDamage();
	pCore->PlayCoreHitAudio();

	// core hit
	pCore->SetPupilState( Core::PupilState_Wince_CoreHit, 0.0f );

	contact->SetEnabled(false);
}

/////////////////////////////////////////////////////
/// Method: EnemyContactHandled
/// Params: 
///
/////////////////////////////////////////////////////
bool EnemyContactHandled( b2Fixture* fixture )
{
	std::vector<b2Fixture *>::iterator it = m_EnemyContacts.begin();

	while( it != m_EnemyContacts.end() )
	{
		if( (*it) == fixture )
			return true;

		// next
		it++;
	}

	// add it
	m_EnemyContacts.push_back(fixture);

	return false;
}
