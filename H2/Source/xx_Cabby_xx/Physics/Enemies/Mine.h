
/*===================================================================
	File: Mine.h
	Game: Cabby

	(C)Hidden Games
=====================================================================*/

#ifndef __MINE_H__
#define __MINE_H__

#include "Effects/Emitter.h"
#include "BaseEnemy.h"

class Mine : public physics::PhysicsIdentifier, public BaseEnemy
{
	public:
		Mine();
		virtual ~Mine();

		int Create( math::Vec3 vPos, math::Vec2 vAreaDims, bool respawnFlag=true, float respawnTime=5.0f );

		virtual void Draw();
		virtual void Update( float deltaTime );
		virtual void HandleContact( const b2ContactPoint* contact );
		void Explode();
		virtual void Remove();
		virtual void Reset();
		virtual void DrawDebug();
		
	private:
		efx::Emitter m_Explosion;
		ALuint m_ExplodeAudioId;

		bool m_IsActive;

		float m_TargetRotation;
		float m_RotationAngle;

		bool m_ShouldRespawn;
		float m_RespawnTime;

		float m_CurrentRespawnTime;

		float m_Scale;

};

#endif // __MINE_H__
