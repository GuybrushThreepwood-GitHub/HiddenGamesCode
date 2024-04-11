
/*===================================================================
	File: Piranha.h
	Game: Cabby

	(C)Hidden Games
=====================================================================*/

#ifdef ALLOW_UNUSED_OBJECTS

#ifndef __PIRANHA_H__
#define __PIRANHA_H__

#include "BaseEnemy.h"

class Piranha : public PhysicsIdentifier, public BaseEnemy
{
	public:
		Piranha();
		virtual ~Piranha();

		int Create( math::Vec3 vPos, math::Vec2 vAreaDims );

		virtual void Draw();
		virtual void Update( float deltaTime );

		virtual void DrawDebug();

	private:
		float m_TargetRotation;
		float m_RotationAngle;

};

#endif // __PIRANHA_H__

#endif // ALLOW_UNUSED_OBJECTS
