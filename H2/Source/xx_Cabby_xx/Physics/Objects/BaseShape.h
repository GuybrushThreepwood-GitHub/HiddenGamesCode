
/*===================================================================
	File: BaseShape.h
	Game: Cabby

	(C)Hidden Games
=====================================================================*/

#ifndef __BASESHAPE_H__
#define __BASESHAPE_H__

#include "Physics/PhysicsIds.h"

class BaseShape
{
	public:
		enum eRotationType
		{
			SHAPE_360_CONSTANT_CW=0,
			SHAPE_360_STOP_CW,

			SHAPE_360_CONSTANT_CCW,
			SHAPE_360_STOP_CCW,

			SHAPE_360_SWAP,

			SHAPE_180_STOP
		};
	
	public:
		BaseShape()
		{
			m_pBody = 0;
			m_pJoint = 0;
			m_SpawnLocation = math::Vec2( 0.0f, 0.0f );

			m_RotationType = SHAPE_360_CONSTANT_CW;
			m_MotorSpeed = 1.0f;
			m_RotationPauseTime = 2.0f;

			m_ClockWise = true;
			m_TimeToSwap = -1.0f;
			m_Speed = 0.0f;
			m_Angle = 0.0f;

			m_LowerAngleLimit = 0.0f;
			m_UpperAngleLimit = 0.0f;

			m_pModel = 0;

			m_BoundingSphere.vCenterPoint = math::Vec3( 0.0f, 0.0f, 0.0f );
			m_BoundingSphere.fRadius = 1.0f;

			m_BoundingAABB.vBoxMin = math::Vec3( -1.0f, -1.0f, -1.0f );
			m_BoundingAABB.vBoxMax = math::Vec3( 1.0f, 1.0f, 1.0f );
		}
		virtual ~BaseShape()
		{
		}

		virtual void Draw() {}
		virtual void Update( float deltaTime ) {}
		virtual void Remove() {}
		virtual void Reset() {}

	protected:
		b2Body* m_pBody;
		b2RevoluteJoint* m_pJoint;
		math::Vec3 m_SpawnLocation;

		collision::Sphere m_BoundingSphere;
		collision::AABB m_BoundingAABB;

		eRotationType m_RotationType;
		float m_MotorSpeed;
		float m_RotationPauseTime;

		bool m_ClockWise;
		float m_TimeToSwap;
		float m_Speed;
		float m_Angle;

		float m_LowerAngleLimit;
		float m_UpperAngleLimit;

		mdl::ModelHGM* m_pModel;

	private:

};

#endif // __BASESHAPE_H__
