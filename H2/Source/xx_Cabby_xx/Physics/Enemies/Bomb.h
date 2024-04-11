
/*===================================================================
	File: Bomb.h
	Game: Cabby

	(C)Hidden Games
=====================================================================*/

#ifdef ALLOW_UNUSED_OBJECTS

#ifndef __BOMB_H__
#define __BOMB_H__

#include "BaseEnemy.h"

class Bomb : public PhysicsIdentifier, public BaseEnemy
{
	public:
		Bomb();
		virtual ~Bomb();

		int Create( math::Vec3 vPos, math::Vec2 vAreaDims );

		virtual void Draw();
		virtual void Update( float deltaTime );

		virtual void DrawDebug();

	private:

};

#endif // __BOMB_H__

#endif // ALLOW_UNUSED_OBJECTS