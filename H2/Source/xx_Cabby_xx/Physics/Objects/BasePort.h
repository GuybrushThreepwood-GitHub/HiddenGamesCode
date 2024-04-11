
/*===================================================================
	File: BasePort.h
	Game: Cabby

	(C)Hidden Games
=====================================================================*/

#ifndef __BASEPORT_H__
#define __BASEPORT_H__

#include "Physics/PhysicsIds.h"

class BasePort
{
	public:
		enum ePortTypes
		{
			PORTTYPE_NUMBERED=0,
			PORTTYPE_HQ,
			PORTTYPE_FUEL,
			PORTTYPE_TOLL
		};

	public:
		BasePort()
		{
			m_pBody = 0;
			m_SpawnLocation = math::Vec2( 0.0f, 0.0f );

			m_pModel = 0;

			m_PortType = PORTTYPE_NUMBERED;
			m_PortNumber = -1;
			m_PortSize = -1;
		}
		virtual ~BasePort()
		{
		}

		virtual void Draw() {}
		virtual void Draw( int fromIndex, int toIndex ) {}
		virtual void Update( float deltaTime ) {}

	protected:
		b2Body* m_pBody;
		math::Vec3 m_SpawnLocation;

		mdl::ModelHGM* m_pModel;

		ePortTypes m_PortType;
		int m_PortNumber;
		int m_PortSize;

	private:

};

#endif // __BASEPORT_H__
