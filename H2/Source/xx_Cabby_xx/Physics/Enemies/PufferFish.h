
/*===================================================================
	File: PufferFish.h
	Game: Cabby

	(C)Hidden Games
=====================================================================*/

#ifndef __PUFFERFISH_H__
#define __PUFFERFISH_H__

#include "BaseEnemy.h"

class PufferFish : public physics::PhysicsIdentifier, public BaseEnemy
{
	public:
		enum eFishState
		{
			FishState_Idle=0,
			FishState_GoToPoint
		};

	public:
		PufferFish();
		virtual ~PufferFish();

		int Create( math::Vec3 vPos, math::Vec2 vAreaDims );

		virtual void Draw();
		virtual void Update( float deltaTime );

		virtual void DrawDebug();

	private:
		eFishState m_State;

		float m_TargetRotation;
		float m_RotationAngle;

		float m_MoveTimer;
		math::Vec2 m_NextMovePoint;
		math::Vec2 m_DirectionToMove;
};

#endif // __PUFFERFISH_H__
