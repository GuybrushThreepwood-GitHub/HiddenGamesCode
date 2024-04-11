
/*===================================================================
	File: XShape.h
	Game: Cabby

	(C)Hidden Games
=====================================================================*/

#ifdef ALLOW_UNUSED_OBJECTS

#ifndef __XSHAPE_H__
#define __XSHAPE_H__

#include "BaseShape.h"

class XShape : public PhysicsIdentifier, public BaseShape
{
	public:
		XShape();
		virtual ~XShape();

		int Create( math::Vec3 vPos, eRotationType rotationType = SHAPE_360_CONSTANT_CCW, float motorSpeed=1.0f, float rotationPauseTime=2.0f );

		virtual void Draw();
		virtual void Update( float deltaTime );
		virtual void Remove();
		virtual void Reset();

	private:

};

#endif // __XSHAPE_H__

#endif // ALLOW_UNUSED_OBJECTS