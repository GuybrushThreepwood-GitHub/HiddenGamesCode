
/*===================================================================
	File: DoorSlideUp.h
	Game: Cabby

	(C)Hidden Games
=====================================================================*/

#ifndef __DOORSLIDEUP_H__
#define __DOORSLIDEUP_H__

#include "BaseDoor.h"

class DoorSlideUp : public physics::PhysicsIdentifier, public BaseDoor
{
	public:
		DoorSlideUp();
		virtual ~DoorSlideUp();

		int Create( math::Vec3 vPos, float extendMotorSpeed=0.5f, float retractMotorSpeed=-0.5f, float extendPauseTime=0.0f, float retractPauseTime=0.0f );

		virtual void Draw();
		virtual void Update( float deltaTime );
		virtual void Remove();
		virtual void Reset();

	private:
		ALuint m_AudioId;
};

#endif // __DOORSLIDEUP_H__
