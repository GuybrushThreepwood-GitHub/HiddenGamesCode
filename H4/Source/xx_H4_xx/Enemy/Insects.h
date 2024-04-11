
/*===================================================================
	File: Insects.h
	Game: H4

	(C)Hidden Games
=====================================================================*/

#ifndef __INSECTS_H__
#define __INSECTS_H__

#include "Physics/PhysicsIds.h"

#include "GameConsts.h"

class Insects
{
	public:
		Insects( );
		virtual ~Insects();

		void Draw();

		void DrawDebug();
		void Update( float deltaTime );

		void SetPosition( math::Vec3& pos );
		math::Vec3& GetPosition();
		void SetRotation( float angle );

		collision::Sphere& GetBoundingSphere()				{ return m_BoundSphere; }

		void SetInFrustum( bool state )						{ m_InFrustum = state; }
		bool InFrustum()									{ return m_InFrustum; }
	
	private:
		math::Vec3 m_Pos;
		float m_RotationAngle;

		float m_LastDeltaTime;

		// used for culling
		bool m_InFrustum;
		collision::Sphere m_BoundSphere;

		float m_TimeInArea;
};

/////////////////////////////////////////////////////
/// Method: SetPosition
/// Params: [in]pos
///
/////////////////////////////////////////////////////
inline void Insects::SetPosition( math::Vec3& pos )
{
	m_Pos = pos;
	m_BoundSphere.vCenterPoint = m_Pos;
}

/////////////////////////////////////////////////////
/// Method: SetRotation
/// Params: [in]angle
///
/////////////////////////////////////////////////////
inline void Insects::SetRotation( float angle )
{
	m_RotationAngle = angle;
}

#endif // __INSECTS_H__

