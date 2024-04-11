
/*===================================================================
	File: BaseDoor.h
	Game: Cabby

	(C)Hidden Games
=====================================================================*/

#ifndef __BASEDOOR_H__
#define __BASEDOOR_H__

#include "Physics/PhysicsIds.h"

class BaseDoor
{
	public:
		BaseDoor()
		{
			m_pBody = 0;
			m_pJoint = 0;
			m_SpawnLocation = math::Vec2( 0.0f, 0.0f );

			m_pModel = 0;

			m_BoundingSphere.vCenterPoint = math::Vec3( 0.0f, 0.0f, 0.0f );
			m_BoundingSphere.fRadius = 1.0f;

			m_BoundingAABB.vBoxMin = math::Vec3( -1.0f, -1.0f, -1.0f );
			m_BoundingAABB.vBoxMax = math::Vec3( 1.0f, 1.0f, 1.0f );

			m_ExtendMotorSpeed = 0.5f;
			m_RetractMotorSpeed = -0.5f;
			m_ExtendPauseTime = 0.0f;
			m_RetractPauseTime = 0.0f;

			m_TimeToSwap = -1.0f;
		}
		virtual ~BaseDoor()
		{
		}

		virtual void Draw() {}
		virtual void Update( float deltaTime ) {}
		virtual void Remove() {}
		virtual void Reset() {}

	protected:
		b2Body* m_pBody;
		b2PrismaticJoint* m_pJoint;
		math::Vec3 m_SpawnLocation;

		mdl::ModelHGM* m_pModel;

		collision::Sphere m_BoundingSphere;
		collision::AABB m_BoundingAABB;

		float m_ExtendMotorSpeed;
		float m_RetractMotorSpeed;

		float m_ExtendPauseTime;
		float m_RetractPauseTime;

		float m_TimeToSwap;
		float m_NewMotorSpeed;

	private:

};

#endif // __BASEDOOR_H__
