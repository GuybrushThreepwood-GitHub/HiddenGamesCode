
/*===================================================================
	File: Port.h
	Game: Cabby

	(C)Hidden Games
=====================================================================*/

#ifndef __PORT_H__
#define __PORT_H__

#include "PhysicsBase.h"
#include "BasePort.h"

class Port : public physics::PhysicsIdentifier, public BasePort
{
	public:
		Port();
		virtual ~Port();

		int Create( math::Vec3 vPos, ePortTypes portType, int portNumber, int portSize );

		virtual void Draw();
		virtual void Draw( int fromIndex, int toIndex );
		virtual void Update( float deltaTime );

	private:
		mdl::ModelHGM* GetModel( ePortTypes portType, int portNumber, int portSize );
		const char* GetPortFunction( ePortTypes portType, int portNumber, int portSize );

	private:
		collision::AABB m_BoundingAABB;
};

#endif // __PORT_H__
