
/*===================================================================
	File: DoorSlideRight.h
	Game: Cabby

	(C)Hidden Games
=====================================================================*/

#ifndef __DOORSLIDERIGHT_H__
#define __DOORSLIDERIGHT_H__

#include "BaseDoor.h"

class DoorSlideRight : public physics::PhysicsIdentifier, public BaseDoor
{
	public:
		DoorSlideRight();
		virtual ~DoorSlideRight();

		int Create( math::Vec3 vPos, float extendMotorSpeed=0.5f, float retractMotorSpeed=-0.5f, float extendPauseTime=0.0f, float retractPauseTime=0.0f );

		virtual void Draw();
		virtual void Update( float deltaTime );
		virtual void Remove();
		virtual void Reset();

	private:
		ALuint m_AudioId;

};

#endif // __DOORSLIDERIGHT_H__
