
/*===================================================================
	File: PhysicsWorld.h
	Game: Cabby

	(C)Hidden Games
=====================================================================*/

#ifndef __PHYSICSWORLD_H__
#define __PHYSICSWORLD_H__

#include "Box2D.h"
#include "Box2DDebugDraw.h"

class Player;
namespace mdl { class ModelHGM; }

class ContactListener : public b2ContactListener
{
	public:
		void Add(const b2ContactPoint* point);
		void Persist(const b2ContactPoint* point);
		void Remove(const b2ContactPoint* point);
};

class PhysicsWorld
{
	public:
		struct PhysicsObject
		{
			int type;
			b2Vec2 storedPos;
			float storedRot;
			b2Body *pBody;
			bool isComplex;
			bool fixedRotation;
			mdl::ModelHGM* pModel;
		};

	public:
		PhysicsWorld();
		~PhysicsWorld();
 
		static void Create( b2Vec2& minBounds, b2Vec2& maxBounds, b2Vec2& gravity, bool enableDebugDraw = false );
		static void Destroy();

		static b2World* GetWorld();
		static void SetPlayer( Player* player )	{ ms_Player = player; }
		static Player* GetPlayer()					{ return ms_Player; }

	public:
		static b2Vec2 ContactNormal;

	private:
		static b2World* ms_World;
		static Box2DDebugDraw ms_DebugDraw;
		static ContactListener ms_ContactListener;

		static Player* ms_Player;
};



#endif // __PHYSICSWORLD_H__
