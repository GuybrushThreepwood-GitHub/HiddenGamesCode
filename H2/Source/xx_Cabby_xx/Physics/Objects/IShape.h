
/*===================================================================
	File: IShape.h
	Game: Cabby

	(C)Hidden Games
=====================================================================*/

#ifdef ALLOW_UNUSED_OBJECTS

#ifndef __ISHAPE_H__
#define __ISHAPE_H__

#include "BaseShape.h"

class IShape : public PhysicsIdentifier, public BaseShape
{
	public:
		IShape();
		virtual ~IShape();

		int Create( math::Vec3 vPos, eRotationType rotationType = SHAPE_360_STOP_CW, float motorSpeed=1.0f, float rotationPauseTime=2.0f );

		virtual void Draw();
		virtual void Update( float deltaTime );
		virtual void Remove();
		virtual void Reset();

	private:

};

#endif // __ISHAPE_H__

#endif // ALLOW_UNUSED_OBJECTS