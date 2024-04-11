
/*===================================================================
	File: DoorSlideLeft.h
	Game: Cabby

	(C)Hidden Games
=====================================================================*/

#ifndef __DOORSLIDELEFT_H__
#define __DOORSLIDELEFT_H__

#include "BaseDoor.h"

class DoorSlideLeft : public physics::PhysicsIdentifier, public BaseDoor
{
	public:
		DoorSlideLeft();
		virtual ~DoorSlideLeft();

		int Create( math::Vec3 vPos, float extendMotorSpeed=0.5f, float retractMotorSpeed=-0.5f, float extendPauseTime=0.0f, float retractPauseTime=0.0f );

		virtual void Draw();
		virtual void Update( float deltaTime );
		virtual void Remove();
		virtual void Reset();

	private:
		ALuint m_AudioId;

};

#endif // __DOORSLIDELEFT_H__
