
/*===================================================================
	File: Gib.h
	Game: H4

	(C)Hidden Games
=====================================================================*/

#ifndef __GIB_H__
#define __GIB_H__

#include "Physics/PhysicsIds.h"

#include "GameConsts.h"

class Gib : public physics::PhysicsIdentifier
{
	public:
		Gib();
		~Gib();

		void SpawnGib( mdl::ModelHGM* pModel, const math::Vec3& pos, float radius );

		void Draw();
		void Update( float deltaTime );

		void HandleContact( int idNum, int castId, const ContactPoint* contact, const b2Fixture* pOtherFixture, const b2Body* pOtherBody );

	private:
		bool m_IsActive;
		math::Vec2 m_Rotation;
		b2Body* m_Body;
		math::Vec3 m_Pos;
		math::Vec3 m_Vel;
		float m_FakeGrav;
		bool m_FirstFloorHit;

		int m_NextSplatIndex;
		mdl::ModelHGM* m_pModel;
		collision::Sphere m_BoundSphere;
};

#endif // __GIB_H__

