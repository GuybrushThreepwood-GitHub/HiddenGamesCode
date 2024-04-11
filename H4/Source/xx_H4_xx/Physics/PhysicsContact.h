
/*===================================================================
	File: PhysicsContact.h
	Game: H4

	(C)Hidden Games
=====================================================================*/

#ifndef __PHYSICSCONTACT_H__
#define __PHYSICSCONTACT_H__

#include "AppConsts.h"
#include "PhysicsBase.h"

const short WORLDGROUP = -1; 
const short PLAYER_AND_BULLET_GROUP = -2;
const short GIB_GROUP = -3; 
const short ENEMY_GROUP = 1;

const uint16 WORLDHIGH_CATEGORY = 0x0001;
const uint16 WORLDLOW_CATEGORY = 0x0002;
const uint16 PLAYER_CATEGORY = 0x0004;
const uint16 ENEMY_CATEGORY = 0x0008;
const uint16 GIB_CATEGORY = 0x0010;

const uint16 BULLETLOW_CATEGORY = 0x1000;
const uint16 BULLETHIGH_CATEGORY = 0x2000;

// collision attribs
const int NUMERIC1_WORLD_HIGHWALL	= 1;
const int NUMERIC1_WORLD_LOWWALL	= 2;
const int NUMERIC1_WORLD_DOOR		= 3;

// sound and emitter for bullets
const int NUMERIC2_WORLD_METAL		= 1;
const int NUMERIC2_WORLD_WOOD		= 2;
const int NUMERIC2_WORLD_PLASTER	= 3;


const int32 MAX_CONTACT_POINTS = 2048;

struct ContactPoint
{
	b2Fixture* fixtureA;
	b2Fixture* fixtureB;
	b2Vec2 normal;
	b2Vec2 position;
	b2PointState state;
};

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

	protected:
		ContactPoint m_Points[MAX_CONTACT_POINTS];
		int32 m_PointCount;
};

#endif // __PHYSICSCONTACT_H__
