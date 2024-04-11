
/*===================================================================
	File: HShape.h
	Game: Cabby

	(C)Hidden Games
=====================================================================*/

#ifndef __HSHAPE_H__
#define __HSHAPE_H__

#include "BaseShape.h"

class HShape : public physics::PhysicsIdentifier, public BaseShape
{
	public:
		HShape();
		virtual ~HShape();

		int Create( math::Vec3 vPos, eRotationType rotationType = SHAPE_360_CONSTANT_CW, float motorSpeed=1.0f, float rotationPauseTime=2.0f );

		virtual void Draw();
		virtual void Update( float deltaTime );
		virtual void Remove();
		virtual void Reset();

	private:

};

#endif // __HSHAPE_H__
