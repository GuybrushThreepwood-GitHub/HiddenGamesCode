
/*===================================================================
	File: DustDevil.h
	Game: Cabby

	(C)Hidden Games
=====================================================================*/

#ifdef ALLOW_UNUSED_OBJECTS

#ifndef __DUSTDEVIL_H__
#define __DUSTDEVIL_H__

#include "BaseEnemy.h"

class DustDevil : public PhysicsIdentifier, public BaseEnemy
{
	public:
		DustDevil();
		virtual ~DustDevil();

		int Create( math::Vec3 vPos, math::Vec2 vAreaDims );

		virtual void Draw();
		virtual void Update( float deltaTime );

		virtual void DrawDebug();

	private:

};

#endif // __DUSTDEVIL_H__

#endif // ALLOW_UNUSED_OBJECTS