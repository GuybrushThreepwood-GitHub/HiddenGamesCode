
/*===================================================================
	File: Fish.h
	Game: Cabby

	(C)Hidden Games
=====================================================================*/

#ifndef __FISH_H__
#define __FISH_H__

#include "BaseEnemy.h"

class Fish : public physics::PhysicsIdentifier, public BaseEnemy
{
	public:
		enum eFishState
		{
			FishState_Idle=0,
			FishState_GoToPoint
		};

	public:
		Fish();
		virtual ~Fish();

		int Create( math::Vec3 vPos, math::Vec2 vAreaDims );

		virtual void Draw();
		virtual void Update( float deltaTime );
		virtual void Remove();
		virtual void Reset();
		virtual void DrawDebug();

	private:
		eFishState m_State;
		
		float m_TargetRotation;
		float m_RotationAngle;

		float m_MoveTimer;
		math::Vec2 m_NextMovePoint;
		math::Vec2 m_DirectionToMove;

		bool m_ModelSwap;
		float m_ModelChangeTimer;
		float m_RandomModelChangeTime;
};

#endif // __FISH_H__
