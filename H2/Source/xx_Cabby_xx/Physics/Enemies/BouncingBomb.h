
/*===================================================================
	File: BouncingBomb.h
	Game: Cabby

	(C)Hidden Games
=====================================================================*/

#ifdef ALLOW_UNUSED_OBJECTS

#ifndef __BOUNCINGBOMB_H__
#define __BOUNCINGBOMB_H__

#include "BaseEnemy.h"

class BouncingBomb : public PhysicsIdentifier, public BaseEnemy
{
	public:
		BouncingBomb();
		virtual ~BouncingBomb();

		int Create( math::Vec3 vPos, math::Vec2 vAreaDims );

		virtual void Draw();
		virtual void Update( float deltaTime );

		virtual void DrawDebug();

	private:

};

#endif // __BOUNCINGBOMB_H__

#endif // ALLOW_UNUSED_OBJECTS
