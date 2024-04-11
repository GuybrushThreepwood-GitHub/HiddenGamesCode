
/*===================================================================
	File: YShape.h
	Game: Cabby

	(C)Hidden Games
=====================================================================*/

#ifndef __YSHAPE_H__
#define __YSHAPE_H__

#include "BaseShape.h"

class YShape : public physics::PhysicsIdentifier, public BaseShape
{
	public:
		YShape();
		virtual ~YShape();

		int Create( math::Vec3 vPos, eRotationType rotationType = SHAPE_180_STOP, float motorSpeed=1.0f, float rotationPauseTime=2.0f );

		virtual void Draw();
		virtual void Update( float deltaTime );
		virtual void Remove();
		virtual void Reset();

	private:

};

#endif // __YSHAPE_H__
