
/*===================================================================
	File: PhysicsContact.h
	Game: H8

	(C)Hidden Games
=====================================================================*/

#ifndef __PHYSICSCONTACT_H__
#define __PHYSICSCONTACT_H__

#include "PhysicsBase.h"

const int32 MAX_CONTACT_POINTS = 1;

struct ContactPoint
{
	b2Fixture* fixtureA;
	b2Fixture* fixtureB;
	b2Vec2 normal;
	b2Vec2 position;
	b2PointState state;
};

struct EnemyToShieldCollision
{
	int type;

	b2Fixture* whichFixture;
	int colourIndex;
	int meshIndex;
	float timeDisabled;
};

// forward declare
class Enemy;
class Player;
class Core;

void ClearContacts();

class ContactListener : public b2ContactListener
{
	public:
		ContactListener() 
		{
			m_PointCount = 0;
		}
		virtual ~ContactListener() {}

		virtual void BeginContact(b2Contact* contact);
		//void Persist(const b2ContactPoint* point);
		virtual void EndContact(b2Contact* contact);

		virtual void PreSolve(b2Contact* contact, const b2Manifold* oldManifold);

		virtual void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse);

		void HandleEnemyToShield( Enemy* pEnemy, b2Fixture* enemyFixture, Player* pShield, b2Fixture* shieldFixture, b2Vec2 position, b2Contact* contact );

		void HandleEnemyToCore( Enemy* pEnemy, b2Fixture* enemyFixture, Core* coreBody, b2Fixture* coreFixture, b2Vec2 position, b2Contact* contact );

	protected:
		ContactPoint m_Points[MAX_CONTACT_POINTS];
		int32 m_PointCount;
};

#endif // __PHYSICSCONTACT_H__
