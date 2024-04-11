
/*===================================================================
	File: Sentry.h
	Game: Cabby

	(C)Hidden Games
=====================================================================*/

#ifndef __SENTRY_H__
#define __SENTRY_H__

#include "BaseEnemy.h"

class Sentry : public physics::PhysicsIdentifier, public BaseEnemy
{
	public:
		Sentry();
		virtual ~Sentry();

		int Create( math::Vec3 vPos, math::Vec2 vAreaDims, int movementDirection, int startPoint, float moveSpeed, float rotateSpeed );

		virtual void Draw();
		virtual void Update( float deltaTime );
		virtual void Remove();
		virtual void Reset();
		virtual void DrawDebug();

	private:
		float m_TargetRotation;
		float m_RotationAngle;

		int m_MovementDirection;
		int m_StartPoint;
		float m_MoveSpeed;
		float m_RotateSpeed;
};

#endif // __SENTRY_H__
