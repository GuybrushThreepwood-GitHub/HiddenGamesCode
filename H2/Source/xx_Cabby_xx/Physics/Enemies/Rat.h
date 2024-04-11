
/*===================================================================
	File: Rat.h
	Game: Cabby

	(C)Hidden Games
=====================================================================*/

#ifdef ALLOW_UNUSED_OBJECTS

#ifndef __RAT_H__
#define __RAT_H__

#include "BaseEnemy.h"

class Rat : public PhysicsIdentifier, public BaseEnemy
{
	public:
		Rat();
		virtual ~Rat();

		int Create( math::Vec3 vPos, math::Vec2 vAreaDims );

		virtual void Draw();
		virtual void Update( float deltaTime );

		virtual void DrawDebug();

	private:

};

#endif // __RAT_H__

#endif // ALLOW_UNUSED_OBJECTS
