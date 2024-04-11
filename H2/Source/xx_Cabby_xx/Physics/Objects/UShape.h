
/*===================================================================
	File: UShape.h
	Game: Cabby

	(C)Hidden Games
=====================================================================*/

#ifdef ALLOW_UNUSED_OBJECTS

#ifndef __USHAPE_H__
#define __USHAPE_H__

#include "BaseShape.h"

class UShape : public PhysicsIdentifier, public BaseShape
{
	public:
		UShape();
		virtual ~UShape();

		int Create( math::Vec3 vPos, eRotationType rotationType = SHAPE_360_CONSTANT_CW, float motorSpeed=1.0f, float rotationPauseTime=2.0f );

		virtual void Draw();
		virtual void Update( float deltaTime );
		virtual void Remove();
		virtual void Reset();

	private:

};

#endif // __USHAPE_H__

#endif // ALLOW_UNUSED_OBJECTS